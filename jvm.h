#ifndef _JVM_H
#define _JVM_H

#include "jvm_common.h"
#include "jvm_constants.h"

typedef struct {
    u8 *data;
    size_t size;
} Sized_Buffer;

Sized_Buffer load_file(char *path);

typedef struct {
    
} JVM;

int jvm_load_class(JVM *jvm, Sized_Buffer buf);

#endif
