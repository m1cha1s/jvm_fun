#ifndef _JVM_METHODS_H
#define _JVM_METHODS_H

#include "jvm_common.h"
#include "jvm_attributes.h"

typedef struct {
  u16 access_flags;
  u16 name_index;
  u16 descriptor_index;
  u16 attributes_count;
  Attribute_Info **attributes;
} Method_Info;

#endif
