# Changelog

## 1.0.0 — 2026-10-04

First public release.

- C++20 static library `libdmgen.a` (headers `include/dmgen/`), flat C ABI library `dmgen_c` (`dmgen/dmgen_c.h`).
- Python package `dmgen` (wheels for Windows x64 and Linux x86-64, ARM64, ARMv7; Python 3.8+), with the `dmgen` command.
- Command-line tool `dmgen`: single code, `--check-gs1`, `--all-sizes`, `--batch`, `--license`.
- All 24 square DataMatrix ECC 200 sizes (10×10 … 144×144), six encodation schemes, GS1 and Chestny ZNAK validation, PNG and JPEG output, self-check of every symbol.
- Platforms: Windows x64, Linux x86-64, Linux ARM64, Linux ARMv7 (Raspberry Pi).
- 30-day trial from the first run on a machine; license activation online or offline.
