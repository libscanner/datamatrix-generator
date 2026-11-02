// dmgen/Language.h — language of library messages (encoding and GS1
// validation errors, license state): "ru" or "en".
//
// By default it comes from the LIBSCANNER_LANG environment variable, otherwise
// from the system language: Russian system — Russian, any other — English.
// The choice applies to the whole process.
#pragma once

#include <string_view>

namespace dmgen {

/// "ru" or "en" (case-insensitive); otherwise false, the language is unchanged.
bool setMessageLanguage(std::string_view lang);

/// Current message language: "ru" or "en".
std::string_view messageLanguage();

}  // namespace dmgen
