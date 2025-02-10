#ifndef _JVM_ATTRIBUTES_H
#define _JVM_ATTRIBUTES_H

typedef enum {
  ATTR_Unknown,
  ATTR_ConstantValue,
  ATTR_Code,
} Attribute_Type;

typedef struct {
  u16 attribute_name_index;
  Attribute_Type type;
  u32 attribute_length;
  u8 info[];
} Attribute_Info;

typedef struct {
  u16 attribute_name_index;
  u32 attribute_length;
  u16 constant_value_index;
} Attribute_ConstantValue_Info;

typedef struct {
  u16 start_pc;
  u16 end_pc;
  u16 handler_pc;
  u16 catch_type;
} Exception_Info;

typedef struct {
  u16 attribute_name_index;
  u32 attribute_length;
  u16 max_stack;
  u16 max_locals;
  u32 code_length;
  u8 *code;
  u16 exception_table_length;
  Exception_Info *exception_table;
  u16 attributes_count;
  Attribute_Info **attributes;
} Attribute_Code_Info;

// typedef struct {
//   u16 attribute_name_index;
//   u32 attribute_length;
  // u16 number_of_entries;
  
// } Attribute_StackMapTable_Info;

#endif
