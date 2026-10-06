// SPDX-License-Identifier: MIT-0
// Minimal dmgen example: a Chestny ZNAK code -> PNG file, plus GS1 validation.
//
//   test_dmgen [data] [output.png]
//
// data is a Chestny ZNAK string, bracketed "(01)...(21)...(93)..." or raw with
// the GS byte written as <GS>. For arbitrary text leave EncodeOptions::gs1 at Off.

#include <dmgen/Dmgen.h>

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::string data = argc > 1 ? argv[1] : "(01)04601234567893(21)5Abc12Xyz!-(93)dGVz";
    const std::string out  = argc > 2 ? argv[2] : "code.png";

    std::cout << "dmgen " << dmgen::versionString() << " (headers " << DMGEN_VERSION_STRING << ")\n";

    // GS1 validation does not need a license; messages come from the library in Russian.
    const dmgen::gs1::Report rep = dmgen::gs1::validate(data, dmgen::Gs1Mode::ChestnyZnak);
    for (const auto& i : rep.issues)
        std::cout << (i.warning ? "warning: " : "error: ") << i.message << "\n";
    if (!rep.ok)
        return 1;

    dmgen::EncodeOptions o;
    o.gs1 = dmgen::Gs1Mode::ChestnyZnak;          // validation and FNC1 as the first codeword

    auto sym = dmgen::encode(data, o);
    if (!sym) {
        // ErrorCode::Unlicensed when the trial period is over and there is no license.
        std::cerr << sym.error().message << "\n";
        return 1;
    }
    std::cout << "symbol " << sym->info.side << "x" << sym->info.side << "\n";

    // 0.5 mm module at 300 dpi; the DPI is written to the file.
    const auto r = dmgen::RenderOptions::fromMm(0.5, 300);
    auto saved = dmgen::save(dmgen::render(sym->modules, r), out, dmgen::ImageFormat::Png);
    if (!saved) {
        std::cerr << saved.error().message << "\n";
        return 1;
    }
    std::cout << "saved " << out << "\n";
    return 0;
}
