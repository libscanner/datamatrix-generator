// dmgen/Symbol.h — symbol sizes and the assembled symbol.
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace dmgen {

/// Parameters of a square ECC 200 symbol per ISO/IEC 16022 (Table 7).
///
/// A symbol of side `side` is divided into regions × regions data regions,
/// each with its own finder pattern (solid left and bottom edges, alternating
/// top and right edges). The codeword stream is split into `blocks`
/// interleaved Reed–Solomon blocks: codeword i goes to block i mod blocks.
struct SymbolInfo {
    int side;           ///< modules per side, 10…144
    int regions;        ///< data regions per side: 1, 2, 4 or 6
    int regionSide;     ///< data region side without the finder pattern, modules
    int dataCodewords;  ///< total data codewords
    int eccCodewords;   ///< total error correction codewords
    int blocks;         ///< Reed–Solomon blocks

    /// Side of the mapping matrix (the symbol without region finder patterns).
    constexpr int mappingSide() const noexcept { return regions * regionSide; }
    /// Error correction codewords per block (equal for all blocks).
    constexpr int eccPerBlock() const noexcept { return eccCodewords / blocks; }
    /// Data codewords in block b. For 144×144 the blocks are unequal: the
    /// first eight have 156 codewords, the last two 155 — the formula handles it.
    constexpr int dataInBlock(int b) const noexcept {
        return dataCodewords / blocks + (b < dataCodewords % blocks ? 1 : 0);
    }
};

/// All 24 square sizes of the standard, by increasing side.
std::span<const SymbolInfo> squareSizes() noexcept;

/// The size with side `side`, if the standard defines it (10, 12, … 144).
std::optional<SymbolInfo> sizeInfo(int side) noexcept;

/// Square module matrix: 1 is dark. Row 0 is the top row, column 0 the left
/// column (the solid L edges are on the left and bottom).
class BitMatrix {
public:
    BitMatrix() = default;
    explicit BitMatrix(int side) : side_(side), bits_(static_cast<std::size_t>(side) * side, 0) {}

    int  side() const noexcept { return side_; }
    bool get(int x, int y) const { return bits_[static_cast<std::size_t>(y) * side_ + x] != 0; }
    void set(int x, int y, bool dark) { bits_[static_cast<std::size_t>(y) * side_ + x] = dark ? 1 : 0; }
    /// Modules row by row, 0/1.
    const std::vector<uint8_t>& data() const noexcept { return bits_; }

    bool operator==(const BitMatrix&) const = default;

private:
    int side_ = 0;
    std::vector<uint8_t> bits_;
};

} // namespace dmgen
