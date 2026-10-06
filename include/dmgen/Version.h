// dmgen/Version.h — library version.
#pragma once

#define DMGEN_VERSION_MAJOR 1
#define DMGEN_VERSION_MINOR 0
#define DMGEN_VERSION_PATCH 0
#define DMGEN_VERSION_STRING "1.0.0"

#ifdef __cplusplus
namespace dmgen {

/// The version the library was built as (not the version of the headers the
/// caller sees): a mismatch shows that the headers and the library come from
/// different builds.
const char* versionString() noexcept;

} // namespace dmgen
#endif
