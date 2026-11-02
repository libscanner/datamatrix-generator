# Changelog

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
