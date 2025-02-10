#ifndef _JVM_H
#define _JVM_H

#include <stddef.h>

#include "ms_arena.h"

#include "jvm_common.h"
#include "jvm_constants.h"
#include "jvm_fields.h"
#include "jvm_attributes.h"
#include "jvm_methods.h"

typedef struct {
    u8 *data;
    size_t size;
} Sized_Buffer;

Sized_Buffer load_file(char *path);

typedef struct {
    Arena arena;
    u16 minor_version;
    u16 major_version;
    u16 constant_pool_count;
    Constant_Info **cp_info;
    u16 access_flags;
    u16 this_class;
    u16 super_class;
    u16 interfaces_count;
    u16 *interfaces;
    u16 fields_count;
    Field_Info **fields;
    u16 methods_count;
    Method_Info **methods;
    u16 attributes_count;
    Attribute_Info **attributes;
} JVM_Class_File;

typedef struct {

} JVM;

int class_file_load(JVM_Class_File *cf, Sized_Buffer buf);
#endif
