// dmgen/Dmgen.h — umbrella header for the C++ API.
//
//   #include <dmgen/Dmgen.h>
//
//   dmgen::EncodeOptions o;
//   o.gs1 = dmgen::Gs1Mode::ChestnyZnak;
//   auto sym = dmgen::encode("0104601234567893215Abc12Xyz!-\x1D" "93dGVz", o);
//   if (!sym) { /* sym.error().code, sym.error().message */ }
//   auto png = dmgen::toPng(dmgen::render(sym->modules));
#pragma once

#include "dmgen/Encoder.h"
#include "dmgen/Gs1.h"
#include "dmgen/Image.h"
#include "dmgen/Options.h"
#include "dmgen/Result.h"
#include "dmgen/Symbol.h"
#include "dmgen/Version.h"
