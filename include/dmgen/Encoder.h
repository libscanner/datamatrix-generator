// dmgen/Encoder.h — string → DataMatrix symbol.
#pragma once

#include "dmgen/Options.h"
#include "dmgen/Result.h"
#include "dmgen/Symbol.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace dmgen {

/// A run of the stream encoded with one scheme: input characters
/// [inputBegin, inputEnd) and data codewords [wordBegin, wordEnd), including
/// latch codewords. For debugging.
struct Segment {
    Encodation scheme;
    int inputBegin, inputEnd;
    int wordBegin, wordEnd;
};

/// An assembled symbol.
struct Symbol {
    SymbolInfo           info{};         ///< selected size
    BitMatrix            modules;        ///< modules without the quiet zone
    std::vector<uint8_t> codewords;      ///< all codewords in placement order: data, then ECC
    int                  dataLength = 0; ///< data codewords before padding
    std::vector<Segment> segments;       ///< which scheme encodes which part
    /// The string a reader will return: the input as is or, in GS1 mode, the
    /// normalized string (with a GS byte where a separator is needed), without
    /// the leading FNC1.
    std::string text;
};

/// Encodes the bytes of data. Bytes 128–255 are encoded with Upper Shift; the
/// text is not transcoded.
Result<Symbol> encode(std::string_view data, const EncodeOptions& options = {});

} // namespace dmgen
