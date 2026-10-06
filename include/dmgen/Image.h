// dmgen/Image.h — symbol raster, PNG and JPEG.
#pragma once

#include "dmgen/Result.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace dmgen {

class BitMatrix;

/// How to draw the symbol.
struct RenderOptions {
    static constexpr int    kMaxModulePx  = 1000;    ///< upper limit of modulePx
    static constexpr int    kMaxQuietZone = 100;     ///< upper limit of quietZone, modules
    static constexpr double kMaxDpi       = 10000;   ///< upper limit of dpi

    int     modulePx  = 8;      ///< pixels per module, 1…kMaxModulePx
    int     quietZone = 2;      ///< quiet zone, modules, 0…kMaxQuietZone; the standard requires at least 1
    bool    invert    = false;  ///< light modules on a dark background
    uint8_t dark  = 0;          ///< brightness of a dark module
    uint8_t light = 255;        ///< brightness of a light module and of the quiet zone
    double  dpi   = 0;          ///< resolution stored in PNG/JPEG metadata, ≤ kMaxDpi; 0 — not written

    /// Module size in millimetres at a given print resolution: modulePx =
    /// round(moduleMm / 25.4 · dpi), at least 1; dpi is written to the file.
    /// A module larger than kMaxModulePx (or a non-numeric value) is rejected
    /// by render().
    static RenderOptions fromMm(double moduleMm, double dpi);

    /// The same picture, tuned for JPEG: the module is rounded up to a
    /// multiple of 8, so with a quiet zone of whole modules every 8×8 block is
    /// uniform and compression causes no ringing at module edges.
    RenderOptions jpegFriendly() const;
};

/// Single-channel 8-bit raster (Gray8), rows stored contiguously without padding.
struct Image {
    static constexpr int kMaxSide = 32768;   ///< max side, px (render, toPng, toJpeg, save)

    int width  = 0;
    int height = 0;
    std::vector<uint8_t> pixels;   ///< width·height bytes, row 0 is the top row
    double dpi = 0;                ///< from RenderOptions; 0 — not set

    uint8_t at(int x, int y) const { return pixels[static_cast<std::size_t>(y) * width + x]; }
};

/// Draws the symbol. modulePx < 1 and quietZone < 0 are clamped to the nearest
/// valid values (1 and 0). Options above their limits, a non-numeric dpi and a
/// side larger than Image::kMaxSide return InvalidArgument; out-of-memory is
/// also returned as an error, not thrown.
Result<Image> render(const BitMatrix& modules, const RenderOptions& options = {});

enum class ImageFormat { Png, Jpeg };

struct JpegOptions {
    int quality = 100;   ///< 1…100; lossy compression of a barcode — below 90 is not recommended
};

/// PNG, 1 channel, lossless — the recommended format.
Result<std::vector<uint8_t>> toPng(const Image& image);
/// JPEG (baseline). See RenderOptions::jpegFriendly.
Result<std::vector<uint8_t>> toJpeg(const Image& image, const JpegOptions& options = {});
/// Writes to a file; the path is a std::filesystem::path, Unicode works on Windows.
Result<void> save(const Image& image, const std::filesystem::path& path, ImageFormat format,
                  const JpegOptions& options = {});

/// The same functions taking the result of render(): a rendering error is
/// passed through unchanged, so the chain toPng(render(modules)) works.
Result<std::vector<uint8_t>> toPng(const Result<Image>& image);
Result<std::vector<uint8_t>> toJpeg(const Result<Image>& image, const JpegOptions& options = {});
Result<void> save(const Result<Image>& image, const std::filesystem::path& path, ImageFormat format,
                  const JpegOptions& options = {});

} // namespace dmgen
