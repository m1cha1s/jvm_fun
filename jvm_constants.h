#ifndef _JVM_CONSTANTS_H
#define _JVM_CONSTANTS_H

#include "jvm_common.h"

typedef struct {
  u8 tag;
  u8 info[];
} Constant_Info;

typedef struct {
  u8 tag;
  u16 name_index;
} Constant_Class_Info;

typedef struct {
  u8 tag;
  u16 class_index;
  u16 name_and_type_index;
} Constant_Ref_Info;

typedef struct {
  u8 tag;
  u16 string_index;
} Constant_String_Info;

typedef struct {
  u8 tag;
  s32 bytes;
} Constant_Integer_Info;

typedef struct {
  u8 tag;
  f32 bytes;
} Constant_Float_Info;

typedef struct {
  u8 tag;
  s64 bytes;
} Constant_Long_Info;

typedef struct {
  u8 tag;
  f64 bytes;
} Constant_Double_Info;

typedef struct {
  u8 tag;
  u16 name_index;
  u16 descriptor_index;
} Constant_NameAndType_Info;

typedef struct {
  u8 tag;
  u16 length;
  u8 *bytes;
} Constant_Utf8_Info;

typedef struct {
  u8 tag;
  u8 reference_kind;
  u16 reference_index;
} Constant_MethodHandle_Info;

typedef struct {
  u8 tag;
  u16 descriptor_index;
} Constant_MethodType_Info;

typedef struct {
  u8 tag;
  u16 bootstrap_method_attr_index;
  u16 name_and_type_index;
} Constant_InvokeDynamic_Info;

#endif
