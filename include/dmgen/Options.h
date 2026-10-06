// dmgen/Options.h — encoding options.
#pragma once

#include "dmgen/Gs1.h"

namespace dmgen {

/// Data encodation scheme (ISO/IEC 16022, 5.2).
enum class Encodation {
    Auto,     ///< schemes chosen by the look-ahead algorithm of ISO/IEC 16022 Annex P (as in zxing)
    Ascii,    ///< ASCII only (digit pairs are compacted) — the most compatible
    C40,
    Text,
    X12,
    Edifact,
    Base256,
    Minimal,  ///< the shorter of a segment search and Auto (slower to compute)
};

struct EncodeOptions {
    /// Symbol side: 0 — the smallest that fits, otherwise one of the 24 values
    /// 10…144 (squareSizes()). A value not in the table returns InvalidArgument;
    /// data that does not fit returns SizeTooSmall. Spare capacity is padded per
    /// the standard.
    int side = 0;
    Encodation encodation = Encodation::Auto;
    /// GS1 option: the string is validated first (gs1::validate) and a
    /// violation returns Gs1Invalid; then it is encoded with FNC1 as the first
    /// codeword.
    Gs1Mode gs1 = Gs1Mode::Off;
    /// Group separator inside the symbol; Fnc1 only in Gs1 mode.
    Gs1Separator separator = Gs1Separator::Gs29;
    /// Notation of the GS1 string.
    Gs1Input gs1Input = Gs1Input::Auto;
    /// Self-check: the assembled symbol is read back (finder pattern →
    /// codewords → ECC → scheme decoding) and compared with the input. A
    /// mismatch is a dmgen bug; InternalVerifyFailed is returned instead of the
    /// symbol.
    bool verify = true;
};

} // namespace dmgen
