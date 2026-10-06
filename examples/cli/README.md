# Command-line tool `dmgen`

The `dmgen` tool ships in every archive (`bin/dmgen`, `bin\dmgen.exe`), in the CLI-only archives (`…-cli.zip` /
`…-cli.tar.gz`) and in the Python wheel (after `pip install` the `dmgen` command is on the environment's `PATH`;
`python -m dmgen` works too). Full option list: `dmgen --help` (in Russian).

## Usage

```text
dmgen [options] <data> -o <file.png|.jpg>
dmgen [options] -o <file.png|.jpg> -- <data>
dmgen --check-gs1 [--gs1=cz] <data>
dmgen --all-sizes [options] <data> --out-dir <directory>
dmgen --batch <file> --out-dir <directory> [options]
```

## Examples

```sh
# Chestny ZNAK code, raw form with the GS separator written as <GS>
dmgen "0104601234567893215Abc12Xyz!-<GS>93dGVz" --gs1=cz -o code.png

# validate a string against GS1 and the Chestny ZNAK profile (no license needed)
dmgen --check-gs1 --gs1=cz "(01)04601234567893(21)5Abc12Xyz!-(93)dGVz"

# the same data in every size that fits
dmgen --all-sizes "Hello" --out-dir sizes/

# one code per line of codes.txt, 0.5 mm module at 300 dpi
dmgen --batch codes.txt --out-dir out/ --gs1=cz --mm=0.5 --dpi=300

# JPEG: module a multiple of 8 px, so JPEG blocks stay uniform
dmgen "Hello" --jpeg-friendly --quality=100 -o hello.jpg

# data that starts with "-": options go before "--"
dmgen --px=10 -o dash.png -- "-123"
```

## Options

| Option | Meaning |
|---|---|
| `--gs1[=gs1\|cz]` | validate as GS1 (`cz` — plus the Chestny ZNAK profile) and encode as GS1 DataMatrix |
| `--separator=gs\|fnc1` | group separator in the symbol (default `gs`) |
| `--input=auto\|raw\|bracketed` | GS1 input form; raw separator: byte 0x1D, `<GS>`, `{GS}`, `\x1D` |
| `--scheme=auto\|minimal\|ascii\|c40\|text\|x12\|edifact\|base256` | encodation scheme |
| `--size=N` | symbol side 10…144 (default: the smallest that fits) |
| `--px=N` | pixels per module (8) |
| `--quiet=N` | quiet zone, modules (2; the standard requires at least 1) |
| `--mm=X --dpi=N` | module size in millimeters at the print resolution |
| `--quality=N` | JPEG quality (100) |
| `--jpeg-friendly` | module size a multiple of 8 px |
| `--invert` | light modules on a dark background |
| `--no-verify` | skip the read-back self-check |
| `--print` | draw the symbol in the console |

Without a license (after the trial period) the tool exits with code 4.

## License

```sh
dmgen --license status                   # trial period, term, offline mode
dmgen --license activate XXXXX-XXXXX-XXXXX-XXXXX-XXXXX
dmgen --license refresh                  # check with the server now
dmgen --license request request.json     # offline activation: upload in your account, get license.lic
dmgen --license import license.lic       # accept a license file (also renewals)
dmgen --license deactivate               # release this machine before moving the license
dmgen --license fingerprint              # machine fingerprint, for support requests
dmgen --license status --json            # machine-readable output
```

Instead of activating by hand, put a file `dmgen.key` with the key next to `dmgen` / `dmgen.exe`
(or `dmgen_c.dll`): the library activates it on its own.

Documentation: <https://libscanner.com/en/docs/datamatrix-generator/#cli>
