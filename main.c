#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define MS_ARENA_IMPLEMENTATION
#include "ms_arena.h"

#include "jvm.h"

int main(int argc, char *argv[]) {
    assert(argc > 1);

    JVM jvm = {0};

    Sized_Buffer class_buf = load_file(argv[1]);

    JVM_Class_File cf = {0};
    if (class_file_load(&cf, class_buf)) {
        fprintf(stderr, "Failed to load class!\n");
    }

    free(class_buf.data);

    return 0;
}

Sized_Buffer load_file(char *path) {
    Sized_Buffer buf = {0};

    FILE *file = fopen(path, "rb");
    if (!file) return buf;

    fseek(file, 0L, SEEK_END);
    buf.size = ftell(file);
    rewind(file);

    buf.data = (u8*)malloc(buf.size);
    fread(buf.data, 1, buf.size, file);

    fclose(file);

    return buf;
}
