# SPDX-License-Identifier: MIT-0
"""Generate DataMatrix codes with the dmgen Python package.

Install the wheel for your platform from GitHub Releases first:

    pip install dmgen-1.0.0-py3-none-win_amd64.whl

Usage:

    python example.py
"""

import sys

import dmgen
import dmgen.license as lic

CODE = "(01)04601234567893(21)5Abc12Xyz!-(93)dGVz"


def main():
    print("dmgen", dmgen.version())

    # GS1 validation does not need a license; messages come from the library in Russian.
    rep = dmgen.validate_gs1("(01)04601234567894(21)ABC", mode="cz")
    print("valid:", rep.ok)
    for i in rep.issues:
        print("  warning:" if i.warning else "  error:", i.message)

    # Encoding needs the trial period (30 days from the first run) or a license.
    s = lic.status()
    if not s["can_encode"]:
        print(s["message"])
        return 3

    try:
        # Chestny ZNAK code -> file; the format follows the extension.
        dmgen.save(CODE, "code.png", gs1="cz", module_px=10)

        # JPEG, 8 px per module, 300 dpi written into the file metadata.
        dmgen.save(CODE, "code.jpg", gs1="cz", module_px=8, dpi=300, quality=100)

        # Image in memory, plain (non-GS1) DataMatrix of a fixed size.
        png = dmgen.make_image("Hello", side=26)
        print("PNG in memory:", len(png), "bytes")

        # Module matrix: rows top to bottom, 1 = dark module.
        m = dmgen.encode_matrix("ABC")
        print(f"matrix {len(m)}x{len(m[0])}; available sizes: {dmgen.SIZES}")
    except dmgen.Gs1Error as e:
        for i in e.report.issues:
            print(i.message)
        return 1
    except dmgen.UnlicensedError as e:
        print(e.message)  # the trial period has ended, no license
        return 3
    except dmgen.EncodeError as e:
        print(e.message)  # size, invalid character, file write...
        return 1

    print("saved code.png and code.jpg")
    return 0


if __name__ == "__main__":
    sys.exit(main())
