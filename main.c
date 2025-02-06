#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

typedef struct {
    char *data;
    size_t size;
} Sized_Buffer;

typedef struct {

} JVM_Class;

typedef struct {

} JVM;

Sized_Buffer load_file(char *path);

int jvm_load_class(JVM *jvm, Sized_Buffer buf);

int main(int argc, char *argv[]) {
    assert(argc > 1);

    JVM jvm = {0};

    Sized_Buffer class_buf = load_file(argv[1]);

    jvm_load_class(&jvm, class_buf);

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

    buf.data = (char*)malloc(buf.size);
    fread(buf.data, 1, buf.size, file);

    fclose(file);

    return buf;
}

int jvm_load_class(JVM *jvm, Sized_Buffer buf) {
    /* Validate that the file is a class file */

    unsigned int magic = *(unsigned int*)buf.data;
    printf("%x\n", magic);

}
