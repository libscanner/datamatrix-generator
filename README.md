**English** | [Русский](https://github.com/libscanner/datamatrix-generator/blob/main/README.ru.md)

# DataMatrix Generator library (dmgen)

Generates DataMatrix ECC 200 symbols per ISO/IEC 16022 (GOST R ISO/IEC 16022) with GS1 and Chestny ZNAK profile validation — PNG and JPEG for marking print.

`dmgen` is a commercial, closed-source C++20 library with a flat C ABI, a Python package and a command-line tool. It supports all 24 square sizes, six encodation schemes (ASCII, C40, Text, X12, EDIFACT, Base256) and a GS1 mode that validates the string against the GS1 General Specifications and the Chestny ZNAK profile (Russia's national product marking system). Every symbol is read back by the library itself before it is returned — a symbol that fails the self-check is never output. The public API does not throw: anything that can fail returns `Result<T>`.

This repository contains the public headers, examples, documentation links and license texts. **It contains no library source code.** Prebuilt binaries are attached to [GitHub Releases](https://github.com/libscanner/datamatrix-generator/releases/latest).

## Key numbers

| | |
|---|---|
| **24 sizes** | all square symbols of the standard, from 10×10 to 144×144 |
| **0.26 ms** | code and PNG of a 10×10 symbol; 144×144 takes 7.3 ms |

**[Create a DataMatrix code online](https://libscanner.com/en/generate/)** — free, in the browser, with GS1 and Chestny ZNAK validation.

## Platforms

| Platform | C++ static library + C ABI + CLI | Python wheel |
|---|---|---|
| Windows x64 | `dmgen-1.0.0-windows-x64.zip` | `dmgen-1.0.0-py3-none-win_amd64.whl` |
| Linux x86-64 | `dmgen-1.0.0-linux-x64.tar.gz` | `dmgen-1.0.0-py3-none-manylinux_2_36_x86_64.whl` |
| Linux ARM64 (Raspberry Pi 3/4/5, 64-bit) | `dmgen-1.0.0-linux-arm64.tar.gz` | `dmgen-1.0.0-py3-none-manylinux_2_36_aarch64.whl` |
| Linux ARMv7 (Raspberry Pi 2/3/4/5, 32-bit) | `dmgen-1.0.0-linux-armhf.tar.gz` | `dmgen-1.0.0-py3-none-manylinux_2_36_armv7l.whl` |

Each platform also has a CLI-only archive (`…-cli.zip` / `…-cli.tar.gz`) with just the `dmgen` tool.

- Windows packages are built with MinGW-w64 GCC (MSYS2 MINGW64). MSVC and other compilers use the C ABI (`dmgen_c.dll`).
- Linux packages are built with GCC 12 on Debian 12 (glibc 2.36): GCC 12 or newer is required for the C++ API; wheels need glibc 2.36+ (Debian 12 / Raspberry Pi OS Bookworm or newer).
- Python 3.8 or newer, no dependencies. Third-party code is bundled; there are no external runtime dependencies.

## Download

- **Binaries:** [GitHub Releases → latest](https://github.com/libscanner/datamatrix-generator/releases/latest) — archives for each platform and Python wheels.
- **Python:** [PyPI → libscanner-dmgen](https://pypi.org/project/libscanner-dmgen/) — `pip install libscanner-dmgen`, imported as `dmgen`.
- **Licenses:** buy a plan at <https://libscanner.com/en/> — 1 year or perpetual (<https://libscanner.com/en/products/datamatrix-generator-yearly>, <https://libscanner.com/en/products/datamatrix-generator-perpetual>). The activation key appears in your account after purchase.

### Trial

Every build works for **30 days from the first run on a machine** — no registration, no key, no network access during the trial. After that a license key from <https://libscanner.com> is required; until then `encode()` returns `ErrorCode::Unlicensed`. GS1 validation and rendering an existing matrix do not require a license.

## Package contents

```text
include/dmgen/{Dmgen,Encoder,Gs1,Image,License,Options,Result,Symbol,Version}.h   C++ API
include/dmgen/dmgen_c.h                     C ABI
lib/libdmgen.a                              C++ static library
lib/pkgconfig/dmgen.pc
lib/libdmgen_c.so.1 | bin/dmgen_c.dll       C ABI library (+ lib/dmgen_c.dll.a on Windows)
bin/dmgen                                   command-line tool
share/doc/dmgen/licenses/                   third-party licenses
```

The headers in [`include/`](https://github.com/libscanner/datamatrix-generator/tree/main/include/dmgen) of this repository are byte-for-byte the ones shipped in the 1.0.0 packages (their comments are in Russian; the English documentation is on the site).

## Quick start

### C++

```cpp
#include <dmgen/Dmgen.h>
#include <iostream>

int main() {
    dmgen::EncodeOptions o;
    o.gs1 = dmgen::Gs1Mode::ChestnyZnak;          // validation and FNC1 as the first codeword

    auto sym = dmgen::encode("(01)04601234567893(21)5Abc12Xyz!-(93)dGVz", o);
    if (!sym) {
        std::cerr << sym.error().message << "\n";   // in Russian
        return 1;
    }

    dmgen::RenderOptions r;                        // 8 px per module, quiet zone 2
    auto saved = dmgen::save(dmgen::render(sym->modules, r), "code.png", dmgen::ImageFormat::Png);
    return saved ? 0 : 1;
}
```

Link statically through pkg-config — the query **must** be `--static`:

```sh
PKG_CONFIG_PATH=/path/to/dmgen-1.0.0-linux-x64/lib/pkgconfig pkg-config --static --cflags --libs dmgen
```

In Meson:

```meson
dmgen_dep = dependency('dmgen', required: true, static: true)
executable('myprogram', 'main.cpp', dependencies: [dmgen_dep],
  link_args: ['-static'])   # Windows: MinGW runtime inside the program
```

Complete example: [`examples/cpp`](https://github.com/libscanner/datamatrix-generator/tree/main/examples/cpp). C ABI example (MSVC, other languages): [`examples/c`](https://github.com/libscanner/datamatrix-generator/tree/main/examples/c).

### Python

```sh
pip install libscanner-dmgen     # PyPI: Windows x64, Linux x86-64 / ARM64 / ARMv7 (glibc 2.36+)
```

```python
import dmgen

# Chestny ZNAK code -> file; the format follows the extension
dmgen.save("(01)04601234567893(21)5Abc12Xyz!-(93)dGVz", "code.png", gs1="cz")

# image in memory
png = dmgen.make_image("Hello", module_px=8)
jpg = dmgen.make_image("Hello", fmt="jpeg", quality=100)
```

The wheel also installs the `dmgen` command. Example: [`examples/python`](https://github.com/libscanner/datamatrix-generator/tree/main/examples/python).

### Command line

```sh
dmgen "0104601234567893215Abc12Xyz!-<GS>93dGVz" --gs1=cz -o code.png
dmgen --check-gs1 --gs1=cz "(01)04601234567893(21)5Abc12Xyz!-(93)dGVz"
```

More commands: [`examples/cli`](https://github.com/libscanner/datamatrix-generator/tree/main/examples/cli).

## Documentation

- C++ / C ABI / CLI: <https://libscanner.com/en/docs/datamatrix-generator/> (Russian: <https://libscanner.com/docs/datamatrix-generator/>)
- Python: <https://libscanner.com/en/docs/datamatrix-generator/?lang=python> (Russian: <https://libscanner.com/docs/datamatrix-generator/?lang=python>)

## License

Proprietary — see [LICENSE.txt](https://github.com/libscanner/datamatrix-generator/blob/main/LICENSE.txt) (End User License Agreement; the Russian text is binding, [LICENSE.en.txt](https://github.com/libscanner/datamatrix-generator/blob/main/LICENSE.en.txt) is an English translation for information). Starting to use the software means accepting the agreement.

Third-party components: [THIRD-PARTY-NOTICES.txt](https://github.com/libscanner/datamatrix-generator/blob/main/THIRD-PARTY-NOTICES.txt) and the license texts in [`licenses/`](https://github.com/libscanner/datamatrix-generator/tree/main/licenses).

Code in [`examples/`](https://github.com/libscanner/datamatrix-generator/tree/main/examples) is licensed under MIT-0 ([examples/LICENSE](https://github.com/libscanner/datamatrix-generator/blob/main/examples/LICENSE)) — copy it into your projects freely; the library itself is proprietary (LICENSE.txt).

## Contact

- Email: [libscanner@yandex.com](mailto:libscanner@yandex.com)
- Feedback form: <https://libscanner.com/en/feedback>
- Security issues: see [SECURITY.md](https://github.com/libscanner/datamatrix-generator/blob/main/SECURITY.md)

“GS1” is a trademark of GS1 AISBL. “Chestny ZNAK” («Честный знак») is a trademark of Operator-CRPT LLC (ООО «Оператор-ЦРПТ»). This product is not affiliated with or certified by the owners of these marks; the names are used only to describe the supported format.
