# C++ example — static linking with libdmgen.a

Requirements:

- Windows: MinGW-w64 GCC from [MSYS2](https://www.msys2.org/), **MINGW64** shell (not UCRT64):
  `pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-pkgconf mingw-w64-x86_64-meson`.
  MSVC cannot link `libdmgen.a` — use the [C ABI example](https://github.com/libscanner/datamatrix-generator/tree/main/examples/c) instead.
- Linux (x86-64, ARM64, ARMv7): GCC 12 or newer (the package is built with GCC 12 on Debian 12), pkg-config, Meson.

Download and unpack the archive for your platform from [Releases](https://github.com/libscanner/datamatrix-generator/releases/latest), then:

```sh
meson setup build --pkg-config-path=/path/to/dmgen-1.0.0-linux-x64/lib/pkgconfig
meson compile -C build
./build/test_dmgen                                             # default Chestny ZNAK sample -> code.png
./build/test_dmgen "(01)04601234567893(21)5Abc12Xyz!-(93)dGVz" label.png
```

Without Meson — the pkg-config query must be `--static`:

```sh
export PKG_CONFIG_PATH=/path/to/dmgen-1.0.0-linux-x64/lib/pkgconfig
g++ -std=c++20 -O2 main.cpp -o test_dmgen $(pkg-config --static --cflags --libs dmgen)
# Windows (MINGW64): add -static to link the compiler runtime into the .exe
```

After purchase, put the key file `dmgen.key` next to the program
(see the [License section of the documentation](https://libscanner.com/en/docs/datamatrix-generator/#license)).
