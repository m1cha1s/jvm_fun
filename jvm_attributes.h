#ifndef _JVM_ATTRIBUTES_H
#define _JVM_ATTRIBUTES_H

typedef struct {
  u16 attribute_name_index;
  u32 attribute_length;
  u8 info[];
} Attribute_Info;

typedef struct {
  u16 attribute_name_index;
  u32 attribute_length;
  u16 constant_value_index;
} Attribute_ConstantValue_Info;

#endif
