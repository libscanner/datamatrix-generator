// dmgen/Gs1.h — validation of a string against the GS1 standard and the
// Chestny Znak (Russian product marking) profile.
#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace dmgen {

/// GS1 option of the encoder.
enum class Gs1Mode {
    Off,          ///< plain DataMatrix, no validation
    Gs1,          ///< strict GS1 General Specifications validation, FNC1 as the first codeword
    ChestnyZnak,  ///< Gs1 plus the Russian marking profile: 01 → 21 → 91+92 or 93
};

/// How the group separator is encoded inside the symbol.
enum class Gs1Separator {
    Gs29,  ///< GS byte (29), codeword 30, as used by Chestny Znak
    Fnc1,  ///< FNC1, codeword 232, strictly per GS1 (Gs1 mode only)
};

/// The form of the input string.
enum class Gs1Input {
    Auto,       ///< "(" at the start means bracketed, otherwise raw
    Raw,        ///< 0104601234567893215Abc…, separator is byte 0x1D or <GS>, {GS}, \x1D, \u001d
    Bracketed,  ///< (01)04601234567893(21)5Abc… — separators are inserted automatically
};

namespace gs1 {

enum class IssueCode {
    Empty = 1,          ///< the string is empty
    UnknownAi,          ///< no known AI at this position
    BadBracket,         ///< malformed bracketed notation
    TooShort,           ///< value shorter than the AI minimum
    TooLong,            ///< value longer than the AI maximum (often a missing separator)
    MissingSeparator,   ///< no separator after an AI without a predefined length
    TrailingSeparator,  ///< separator at the end of the string
    ExtraSeparator,     ///< redundant separator (warning)
    BadChar,            ///< character not in the AI's allowed set
    BadCheckDigit,      ///< check digit mismatch
    BadDate,            ///< invalid date or time
    DuplicateAi,        ///< AI repeated
    MissingAssociation, ///< AI requires another AI (e.g. 21 requires GTIN) (warning)
    ProfileOrder,       ///< Chestny Znak: wrong element order
    ProfileMissing,     ///< Chestny Znak: a mandatory element is missing
    ProfileLength,      ///< Chestny Znak: wrong element length
    ProfileUnusual,     ///< Chestny Znak: unusual element or length (warning)
    Prefix,             ///< a ]d2 prefix or a leading GS was stripped (warning)
};

struct Element {
    std::string ai;
    std::string value;
};

struct Issue {
    IssueCode   code;
    bool        warning = false;  ///< a warning does not make the string invalid
    std::size_t position = 0;     ///< position in the input (after unescaping)
    std::string ai;               ///< the AI it refers to, if any
    std::string message;          ///< in Russian
};

struct Report {
    bool ok = false;                 ///< no errors (warnings are allowed)
    std::vector<Element> elements;
    std::vector<Issue>   issues;
    /// The element string with a GS byte (0x1D) where a separator is needed —
    /// exactly what a reader returns after reading the symbol. No leading FNC1.
    std::string normalized;
};

/// Parses and validates the string. mode == Off is treated as Gs1.
Report validate(std::string_view data, Gs1Mode mode = Gs1Mode::Gs1,
                Gs1Input input = Gs1Input::Auto);

/// The report's errors as one line (used for the Gs1Invalid message).
std::string describe(const Report& report);

} // namespace gs1
} // namespace dmgen
