// dmgen/License.h — library licensing.
//
// The library encodes while a trial period or a license is in effect.
// Otherwise dmgen::encode() immediately returns ErrorCode::Unlicensed and does
// not encode the string (DMGEN_E_UNLICENSED in the C ABI).
//
//     dmgen::LicenseStatus s = dmgen::license::status();
//     if (!s.canEncode()) show(s.message);
//
// Works the same way as the dmr reader and uses the same license server, but
// keeps its own state: the trial period and license of the two products on
// one machine are independent.
//
// TRIAL PERIOD — one month from the first run on this machine, with no network
// access and no registration.
//
// A LICENSE is bound to the hardware, not to the OS installation (fingerprint:
// motherboard, CPU, system disk). Reinstalling Windows or replacing the disk
// or CPU keeps it; replacing the motherboard does not. Activation is done in
// one of three ways:
//
//   key       PRIMARY: a text file dmgen.key containing the key is placed next
//   file      to the library (dmgen_c.dll / libdmgen_c.so) or to the program
//             (dmgen.exe; a program statically linked with libdmgen), and the
//             library activates the key over the network by itself — no calls
//             needed: on the first encode() or the first license:: function
//             (that call waits for the server's response, once). On failure the
//             reason is in status().message; without network it retries every
//             hour, after a server refusal once a day, also across restarts. A
//             file placed while the program is running is picked up within an
//             hour. A file with a different key does not replace a working
//             license; a key released with deactivate() is not activated
//             again. The file name differs from the reader's license.key
//             because both libraries may sit in one directory with different
//             keys;
//   online    activate("XXXXX-XXXXX-XXXXX-XXXXX-...") — the machine contacts
//             the license server itself;
//   offline   writeActivationRequest("request.json") → the file is moved to a
//             machine with internet access and uploaded to the customer
//             account → the account issues license.lic →
//             importLicenseFile("license.lic").
//
// A license activated over the network checks in with the server ONCE A DAY BY
// ITSELF: a background library thread renews the token (the limited and
// extended modes need this to keep working) and picks up revocation and term
// extension. No calls are needed — a program can run for months. The check
// runs on its own thread and never inside encode(): encoding never waits for
// the network. If the server is unreachable, it retries in an hour; the
// license keeps working until its offline period runs out.
//
// Moving to another machine — call deactivate() on the old one: the seat is
// released on the server with a receipt signed by this machine's secret.
// Without network — deactivateToFile(): the receipt is written to a file and
// can be sent from any machine.
//
// All functions are thread-safe. Network functions block the caller until a
// response arrives or LicenseOptions::timeoutMs expires.
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace dmgen {

enum class LicenseState {
    Trial           = 0,  ///< trial period in progress
    Active          = 1,  ///< license in effect
    RefreshDue      = 2,  ///< in effect, but no contact with the server for a
                          ///< long time; the automatic check retries hourly
    TrialExpired    = 3,  ///< trial period over, no license
    Expired         = 4,  ///< the paid term has ended; an extension made in
                          ///< the customer account is picked up automatically
    NetworkRequired = 5,  ///< cannot continue without contacting the server;
                          ///< once it responds, the automatic check restores
                          ///< the license
    WrongMachine    = 6,  ///< license issued to another machine
    Invalid         = 7,  ///< license record fails signature verification
};

struct LicenseStatus {
    LicenseState state = LicenseState::TrialExpired;

    /// Whether the library encodes right now.
    bool canEncode() const {
        return state == LicenseState::Trial || state == LicenseState::Active
            || state == LicenseState::RefreshDue;
    }

    /// End of the trial period or of the paid term, Unix seconds UTC.
    int64_t validUntil = 0;

    /// When the token becomes stale and the license moves to RefreshDue;
    /// 0 — never (trial period, full mode).
    int64_t refreshAfter = 0;

    /// The license stops working if there is no contact with the server
    /// beyond this time; 0 — no limit (full mode).
    int64_t offlineUntil = 0;

    /// Whole days until the nearest of the limits above; 0 — a limit has passed.
    int daysLeft = 0;

    std::string licenseId;
    std::string activationId;
    std::string offlineMode;    ///< limited | extended | full; empty without a license

    /// The system clock is behind the time this machine has already seen.
    /// License time is based on the latest seen time, so turning the clock back
    /// does not extend the term; the flag exists so that this can be reported.
    bool clockRollback = false;

    /// The state in words, for the user.
    std::string message;
};

struct LicenseResult {
    bool ok = false;

    /// Failure code: the server's response as is (LICENSE_NOT_FOUND,
    /// ACTIVATION_LIMIT_REACHED, LICENSE_REVOKED, SEAT_LOST, ...) or a local one:
    /// KEY_INVALID, NETWORK, NO_TRANSPORT, BAD_RESPONSE, NO_LICENSE, NO_KEY,
    /// NO_SECRET, LICENSE_CHANGED, FILE, FILE_INVALID, FILE_OUTDATED,
    /// FILE_FOREIGN, SERVER_CLOCK, WRONG_MACHINE, WRONG_PRODUCT.
    std::string error;

    /// What happened, in words.
    std::string message;

    /// The state AFTER the operation.
    LicenseStatus status;
};

struct LicenseOptions {
    /// License server URL; empty — the default built into the library.
    /// Overriding it is safe: server responses are verified by signature
    /// against keys built into the library.
    std::string serverUrl;

    /// The caller's application version, shown next to the machine in the
    /// customer account.
    std::string appVersion;

    /// Timeout of a single server request, ms.
    int timeoutMs = 15000;

    /// Daily automatic check with the server (see the header comment). Turn it
    /// off only if the program must not go online on its own by policy; then
    /// limited and extended licenses are renewed by the program calling
    /// refresh().
    bool autoCheck = true;
};

namespace license {

/// Configure before the first network call. Optional.
///
/// The first call into the library in the process — this function, any other
/// function here or the first encode() — activates the key from dmgen.key if
/// the file is present and not yet activated, and waits for the server's
/// response (see the header comment). The server and appVersion for that
/// activation come from configure(), so call it BEFORE the first encode().
void configure(const LicenseOptions& options);

/// Current state. Does not go online — except for the first call into the
/// library when dmgen.key is present (see configure).
LicenseStatus status();

/// Online activation with a key.
LicenseResult activate(const std::string& licenseKey);

/// Check with the server NOW, without waiting for the daily automatic check:
/// a "check license" button. Requires the key used for activation: a license
/// imported from a file has none — it is renewed with a new file.
LicenseResult refresh();

/// Release this machine: a receipt is sent to the server and the license is
/// removed here. Without network the license stays in place — see
/// deactivateToFile().
LicenseResult deactivate(const std::string& reason = {});

/// Release this machine offline. The license is removed IMMEDIATELY and the
/// receipt is written to a file — the ready-made body of
/// POST /api/v1/license/release, which can be sent to the server from any
/// machine.
LicenseResult deactivateToFile(const std::filesystem::path& receiptPath, const std::string& reason = {});

/// Offline activation request file: this machine's fingerprint. Upload it to
/// the customer account; license.lic is issued in return.
LicenseResult writeActivationRequest(const std::filesystem::path& path);

/// Import license.lic. Also used for renewal: the new file replaces the old one.
LicenseResult importLicenseFile(const std::filesystem::path& path);

/// Combined fingerprint of this machine, hex. For support: shows whether this
/// is the same machine as the one in the customer account.
std::string machineFingerprint();

/// Path to the dmgen.key the library takes its key from (see the header
/// comment); empty — there is no such file next to the library or the program.
std::filesystem::path keyFile();

} // namespace license
} // namespace dmgen
