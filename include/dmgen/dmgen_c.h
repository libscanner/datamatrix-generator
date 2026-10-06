/* dmgen_c.h — flat C ABI of the dmgen library (dmgen_c.dll / libdmgen_c.so).
 *
 * The single entry point for every language other than C++: Python (ctypes),
 * C# (P/Invoke), Java (FFM/JNA), Go (cgo), Node.js, Rust. Compatibility rules:
 *   - only fixed-size C types; enumerations are int32_t;
 *   - option structs start with struct_size; new fields are only appended,
 *     so existing callers keep working with a newer library;
 *   - memory allocated by the library is freed with dmgen_free /
 *     dmgen_gs1_report_free; a caller-buffer variant is also available;
 *   - C++ exceptions never cross the boundary; an error is reported as a
 *     return code plus text in dmgen_error (UTF-8);
 *   - no callbacks; all functions are thread-safe. The only global state is
 *     the process license (dmgen_license_*).
 */
#ifndef DMGEN_C_H
#define DMGEN_C_H

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32)
#  if defined(DMGEN_C_BUILDING)
#    define DMGEN_API __declspec(dllexport)
#  else
#    define DMGEN_API __declspec(dllimport)
#  endif
#elif defined(__GNUC__)
#  define DMGEN_API __attribute__((visibility("default")))
#else
#  define DMGEN_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DMGEN_ABI_VERSION 1

/* Return codes: 0 is success, the rest match dmgen::ErrorCode. */
typedef int32_t dmgen_status;
enum {
    DMGEN_OK                     = 0,
    DMGEN_E_INVALID_ARGUMENT     = 1,
    DMGEN_E_DATA_TOO_LONG        = 2,
    DMGEN_E_SIZE_TOO_SMALL       = 3,
    DMGEN_E_UNENCODABLE_CHAR     = 4,
    DMGEN_E_GS1_INVALID          = 5,
    DMGEN_E_IO                   = 6,
    DMGEN_E_INTERNAL_VERIFY      = 7,
    DMGEN_E_UNLICENSED           = 8,   /* trial period over and no license */
    DMGEN_E_BUFFER_TOO_SMALL     = 100  /* dmgen_make_image_into only */
};

enum { DMGEN_FORMAT_PNG = 0, DMGEN_FORMAT_JPEG = 1 };
enum { DMGEN_GS1_OFF = 0, DMGEN_GS1_GS1 = 1, DMGEN_GS1_CHESTNY_ZNAK = 2 };
enum { DMGEN_SEPARATOR_GS = 0, DMGEN_SEPARATOR_FNC1 = 1 };
enum { DMGEN_INPUT_AUTO = 0, DMGEN_INPUT_RAW = 1, DMGEN_INPUT_BRACKETED = 2 };
enum {
    DMGEN_SCHEME_AUTO = 0, DMGEN_SCHEME_ASCII, DMGEN_SCHEME_C40, DMGEN_SCHEME_TEXT,
    DMGEN_SCHEME_X12, DMGEN_SCHEME_EDIFACT, DMGEN_SCHEME_BASE256, DMGEN_SCHEME_MINIMAL
};

/* Encoding and rendering options. Initialize with dmgen_options_init. */
typedef struct dmgen_options {
    uint32_t struct_size;   /* sizeof(dmgen_options) as compiled by the caller */
    int32_t  gs1;           /* DMGEN_GS1_* */
    int32_t  separator;     /* DMGEN_SEPARATOR_* */
    int32_t  gs1_input;     /* DMGEN_INPUT_* */
    int32_t  scheme;        /* DMGEN_SCHEME_* */
    int32_t  side;          /* 0 — smallest that fits, otherwise a standard side 10…144 */
    int32_t  verify;        /* 1 — self-check (default) */
    int32_t  module_px;     /* pixels per module, default 8 */
    int32_t  quiet_zone;    /* modules, default 2 */
    int32_t  invert;        /* 0/1 */
    int32_t  dark;          /* 0…255, default 0 */
    int32_t  light;         /* 0…255, default 255 */
    int32_t  jpeg_quality;  /* 1…100, default 100 */
    int32_t  reserved;      /* alignment, 0 */
    double   dpi;           /* 0 — not written to the file */
} dmgen_options;

typedef struct dmgen_error {
    int32_t code;           /* dmgen_status */
    char    message[512];   /* UTF-8, null-terminated */
} dmgen_error;

typedef struct dmgen_gs1_element {
    char   ai[8];           /* null-terminated */
    char*  value;           /* UTF-8, null-terminated */
    size_t value_len;
} dmgen_gs1_element;

typedef struct dmgen_gs1_issue {
    int32_t code;           /* dmgen::gs1::IssueCode */
    int32_t warning;        /* 1 — warning */
    int32_t position;
    char    ai[8];
    char    message[256];
} dmgen_gs1_issue;

typedef struct dmgen_gs1_report {
    int32_t            ok;
    int32_t            element_count;
    dmgen_gs1_element* elements;
    int32_t            issue_count;
    dmgen_gs1_issue*   issues;
    char*              normalized;      /* with 0x1D bytes, as a reader returns it */
    size_t             normalized_len;
} dmgen_gs1_report;

DMGEN_API uint32_t    dmgen_abi_version(void);
DMGEN_API const char* dmgen_version_string(void);

DMGEN_API void dmgen_options_init(dmgen_options* options);

/* Data → PNG or JPEG in memory. Free *out with dmgen_free. */
DMGEN_API dmgen_status dmgen_make_image(const uint8_t* data, size_t len,
                                        const dmgen_options* options, int32_t format,
                                        uint8_t** out, size_t* out_len, dmgen_error* error);

/* Same, into a caller-provided buffer. If cap is too small, returns
 * DMGEN_E_BUFFER_TOO_SMALL and sets *needed to the required size; buf may be
 * NULL when cap = 0. */
DMGEN_API dmgen_status dmgen_make_image_into(const uint8_t* data, size_t len,
                                             const dmgen_options* options, int32_t format,
                                             uint8_t* buf, size_t cap, size_t* needed,
                                             dmgen_error* error);

/* Data → file (UTF-8 path; Unicode works on Windows). */
DMGEN_API dmgen_status dmgen_save_image(const uint8_t* data, size_t len,
                                        const dmgen_options* options, int32_t format,
                                        const char* path_utf8, dmgen_error* error);

/* Data → side×side module matrix (1 is dark, rows top to bottom).
 * Free *modules with dmgen_free. */
DMGEN_API dmgen_status dmgen_encode_matrix(const uint8_t* data, size_t len,
                                           const dmgen_options* options,
                                           uint8_t** modules, int32_t* side, dmgen_error* error);

/* GS1 validation. Free *report with dmgen_gs1_report_free. The return code
 * tells whether the call itself succeeded; whether the string is valid is the
 * report's ok field. */
DMGEN_API dmgen_status dmgen_gs1_validate(const uint8_t* data, size_t len, int32_t mode,
                                          int32_t input, dmgen_gs1_report** report,
                                          dmgen_error* error);
DMGEN_API void dmgen_gs1_report_free(dmgen_gs1_report* report);

/* Sides of the 24 standard square sizes in increasing order; returns their
 * count (24) and writes at most cap values. */
DMGEN_API int32_t dmgen_symbol_sizes(int32_t* sides, int32_t cap);

DMGEN_API void dmgen_free(void* p);

/* ── License (dmgen/License.h) ──────────────────────────────────────────────
 *
 * Symbol-producing functions return DMGEN_E_UNLICENSED while there is neither
 * a trial period nor a license. The primary activation method is a dmgen.key
 * file containing the key, placed next to dmgen_c.dll / libdmgen_c.so: no
 * calls are needed.
 *
 * Responses are UTF-8 JSON, the same as "dmgen --license ... --json":
 *   status     {"state","can_encode","valid_until","refresh_after",
 *               "offline_until","days_left","license_id","activation_id",
 *               "offline_mode","clock_rollback","message"}
 *   operation  {"ok","error","message","status":{...status...}}
 * state is one of trial, active, refresh_due, trial_expired, expired,
 * network_required, wrong_machine, invalid. Free the string with dmgen_free;
 * NULL means an internal error or NULL in a required argument. Paths are
 * UTF-8. Network functions wait for the server at most timeout_ms. */

/* Call before the first network call and the first encoding; optional.
 * server_url, app_version — NULL or "" for the default; timeout_ms <= 0 —
 * 15000; auto_check — daily background check with the server (1 — on). */
DMGEN_API void  dmgen_license_configure(const char* server_url, const char* app_version,
                                        int32_t timeout_ms, int32_t auto_check);
DMGEN_API char* dmgen_license_status_json(void);
DMGEN_API char* dmgen_license_activate_json(const char* key);
DMGEN_API char* dmgen_license_refresh_json(void);
DMGEN_API char* dmgen_license_deactivate_json(const char* reason);     /* reason may be NULL */
DMGEN_API char* dmgen_license_deactivate_to_file_json(const char* path_utf8, const char* reason);
DMGEN_API char* dmgen_license_write_activation_request_json(const char* path_utf8);
DMGEN_API char* dmgen_license_import_license_file_json(const char* path_utf8);
/* {"fingerprint":"<hex>"} */
DMGEN_API char* dmgen_license_fingerprint_json(void);

#ifdef __cplusplus
}
#endif

#endif /* DMGEN_C_H */
