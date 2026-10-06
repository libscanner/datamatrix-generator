# C example — dynamic linking with dmgen_c (C ABI)

`dmgen_c` is the same library behind a flat C interface (`dmgen/dmgen_c.h`): only `dmgen_*` symbols are exported,
fixed-size C types, options carry a `struct_size`, memory is freed with `dmgen_free`. The compiler runtime is
linked into it statically, so it works with MSVC, Clang, GCC and any language that can call C functions
(P/Invoke, JNA, cgo, ctypes). There is no `.pc` file for `dmgen_c`; paths are given directly.

## Linux (x86-64, ARM64, ARMv7)

```sh
cc main.c -o test_dmgen_c -I/path/to/dmgen-1.0.0-linux-x64/include -L/path/to/dmgen-1.0.0-linux-x64/lib -ldmgen_c -Wl,-rpath,'$ORIGIN'
cp -a /path/to/dmgen-1.0.0-linux-x64/lib/libdmgen_c.so.1* .
./test_dmgen_c
```

## Windows, MinGW-w64 GCC

```sh
gcc main.c -o test_dmgen_c.exe -I C:/path/to/dmgen-1.0.0-windows-x64/include -L C:/path/to/dmgen-1.0.0-windows-x64/lib -ldmgen_c
cp C:/path/to/dmgen-1.0.0-windows-x64/bin/dmgen_c.dll .
```

## Windows, MSVC

The package has no `.lib` file; generate the import library from the DLL. In an MSYS2 shell:

```sh
gendef dmgen_c.dll        # package mingw-w64-x86_64-tools; writes dmgen_c.def
```

Then in the Developer Command Prompt for VS (x64):

```bat
lib /def:dmgen_c.def /machine:x64 /out:dmgen_c.lib
cl /I C:\path\to\dmgen-1.0.0-windows-x64\include main.c dmgen_c.lib
```

At run time `dmgen_c.dll` must be next to the `.exe` (or on `PATH`); after purchase the key file `dmgen.key`
goes next to `dmgen_c.dll` / `libdmgen_c.so`.

Full C ABI reference: <https://libscanner.com/en/docs/datamatrix-generator/#cabi>
