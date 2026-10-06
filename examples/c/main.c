// SPDX-License-Identifier: MIT-0
/*
 * dmgen through the flat C ABI (dmgen_c.dll / libdmgen_c.so).
 * Works with any C compiler, MSVC included, and is the model for bindings
 * from other languages (C#, Java, Go...).
 *
 *   test_dmgen_c [data] [output.png]
 *
 * data is a Chestny ZNAK string, bracketed "(01)...(21)...(93)..." or raw with
 * the GS byte written as <GS>; set o.gs1 = DMGEN_GS1_OFF for arbitrary text.
 */

#include <dmgen/dmgen_c.h>

#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    const char* data = argc > 1 ? argv[1] : "(01)04601234567893(21)5Abc12Xyz!-(93)dGVz";
    const char* path = argc > 2 ? argv[2] : "code.png";
    dmgen_options o;
    dmgen_error e;
    uint8_t* png = NULL;
    size_t len = 0;
    FILE* f;

    printf("dmgen %s\n", dmgen_version_string());

    dmgen_options_init(&o);                 /* defaults; also sets struct_size */
    o.gs1 = DMGEN_GS1_CHESTNY_ZNAK;         /* GS1 + Chestny ZNAK validation */
    o.module_px = 10;

    /* PNG in memory; the buffer is freed with dmgen_free. */
    if (dmgen_make_image((const uint8_t*)data, strlen(data), &o, DMGEN_FORMAT_PNG,
                         &png, &len, &e) != DMGEN_OK) {
        fprintf(stderr, "error %d: %s\n", (int)e.code, e.message);   /* message is UTF-8, in Russian */
        return 1;
    }

    f = fopen(path, "wb");
    if (!f || fwrite(png, 1, len, f) != len) {
        fprintf(stderr, "could not write %s\n", path);
        if (f)
            fclose(f);
        dmgen_free(png);
        return 1;
    }
    fclose(f);
    dmgen_free(png);

    printf("saved %s (%u bytes)\n", path, (unsigned)len);
    return 0;
}
