# Changelog

## 1.1.0 — 2026-11-06

Licensing for containers; generated symbols are unchanged.

- **Containers: instance license.** In Docker and Kubernetes the library now runs under an *instance* license: each running process takes a seat on the license server at start, reports in the background every 30 minutes and frees the seat on a normal exit; containers can be recreated freely. Pass the key in the `DMGEN_LICENSE_KEY` environment variable or as the `/run/secrets/dmgen.key` secret (checked in that order, then the key file next to the library). What counts is how many instances run at the same time; peaks up to twice the purchased number are allowed within a monthly allowance. Without a connection to the license server an instance keeps working for 7 days since its last report.
- **No trial without a key in a container.** Container state disappears on every restart, so a local trial would restart forever; instead get a container trial key (2 instances, 30 days) in your account. Outside containers the 30-day trial works as before. A machine license key is not activated in a container — the message says an instance license is needed.
- **Worker processes after fork** (gunicorn with `--preload`, `multiprocessing`, uWSGI without `lazy-apps`) are separate instances: each worker takes its own seat on its first encode, and a master process that does not encode itself frees its seat within 30 minutes.
- **Emergency extension.** If our license server is unavailable for a long time, the offline period can be extended by a signed notice published on independent mirrors (up to 30 days); the library fetches it by itself. `LicenseStatus::emergencyUntil` (`emergency_until` in Python and in `--license status --json`) shows the extended deadline.
- **Offline machines for the whole term.** A machine without internet can be activated with a file for the whole paid subscription term: tick "Machine without internet — work the whole term offline" when uploading the activation request in your account.
- Details: [license in Docker and Kubernetes](https://libscanner.com/en/articles/library-license-docker-kubernetes/).

## 1.0.3 — 2026-11-02

Messages in English; generated symbols are unchanged.

- The command-line tool, encoding and GS1 validation errors (including the names of GS1 application identifiers) and license messages now speak English on any non-Russian system and Russian on a Russian one. `--lang ru|en` or the `LIBSCANNER_LANG` environment variable set the language explicitly.
- `dmgen::setMessageLanguage()` / `dmgen::messageLanguage()` (new header `dmgen/Language.h`, included by `dmgen/Dmgen.h`) choose the language from a program; the Python package follows `LIBSCANNER_LANG` and the system language.

## 1.0.2 — 2026-10-19

- **New license signing key.** Our hosting provider reported unauthorised access to its virtualisation platform on 17 October 2026; the server's license signing key is considered compromised and has been replaced. Licenses are now signed with the new key, which only 1.0.2 trusts — **update to 1.0.2 before activating a license**. Earlier versions do not accept licenses issued from now on.
- Online activation sends the product of the build: a key for another product (for example, a reader key entered into the generator) is rejected by the server without taking a seat.
- Release packaging: Linux targets are built from the committed sources only (`git archive`), and the wheel contains exactly the package modules. Generated symbols are unchanged.

## 1.0.1 — 2026-10-15

Security fixes from an internal audit; generated symbols are unchanged.

- Windows: a shared `%ProgramData%\dmgen` copy of the license state owned by another non-admin user is ignored rather than read.
- Release packaging: a build without optimisation is no longer exempt from the license check, and wheels refuse build passports that are not for customers.

## 1.0.0 — 2026-10-04

First public release.

- C++20 static library `libdmgen.a` (headers `include/dmgen/`), flat C ABI library `dmgen_c` (`dmgen/dmgen_c.h`).
- Python package `dmgen` (wheels for Windows x64 and Linux x86-64, ARM64, ARMv7; Python 3.8+), with the `dmgen` command.
- Command-line tool `dmgen`: single code, `--check-gs1`, `--all-sizes`, `--batch`, `--license`.
- All 24 square DataMatrix ECC 200 sizes (10×10 … 144×144), six encodation schemes, GS1 and Chestny ZNAK validation, PNG and JPEG output, self-check of every symbol.
- Platforms: Windows x64, Linux x86-64, Linux ARM64, Linux ARMv7 (Raspberry Pi).
- 30-day trial from the first run on a machine; license activation online or offline.
