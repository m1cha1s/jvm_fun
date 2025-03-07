#include <stdio.h>
#include <string.h>

#include "jvm.h"
#include "jvm_common.h"
#include "jvm_attributes.h"
#include "jvm_fields.h"
#include "ms_arena.h"

static inline u32 read_u32(u8 **data) {
    u32 d = (u32)(*data)[3]
        | ((u32)(*data)[2] << 8)
        | ((u32)(*data)[1] << 16)
        | ((u32)(*data)[0] << 24);

    *data += 4;

    return d;
}

static inline u16 read_u16(u8 **data) {
    u16 d = ((u16)(*data)[1])
        | ((u16)(*data)[0] << 8);

    *data += 2;

    return d;
}

static u8 read_u8(u8 **data) {
    u8 d = ((u8)(*data)[0]);

    *data += 1;

    return d;
}

Attribute_Info *class_file_load_attribute(JVM_Class_File *cf, u8 **rptr) {
    u16 attribute_name_index = read_u16(rptr);
    u32 attribute_length = read_u32(rptr);
    
    Attribute_Info *ai = arena_alloc(&cf->arena, sizeof(u16)+sizeof(Attribute_Type)+sizeof(u32)+attribute_length);

    ai->attribute_name_index = attribute_name_index;
    ai->attribute_length = attribute_length;
    ai->type = ATTR_Unknown;

    Constant_Utf8_Info *attr_name = (Constant_Utf8_Info*)(cf->cp_info[attribute_name_index]);
    if (strncmp("ConstantValue", attr_name->bytes, attr_name->length) == 0) {
        ai->type = ATTR_ConstantValue;
        printf("Constant\n");

        Attribute_ConstantValue_Info *cv = (Attribute_ConstantValue_Info*)ai;
        cv->constant_value_index = read_u16(rptr);
    }
    else if (strncmp("Code", attr_name->bytes, attr_name->length) == 0) {
        ai->type = ATTR_Code;
        printf("Code\n");

        Attribute_Code_Info *ci = (Attribute_Code_Info*)ai;

        ci->max_stack = read_u16(rptr);
        ci->max_locals = read_u16(rptr);
        ci->code_length = read_u32(rptr);
        ci->code = arena_alloc(&cf->arena, ci->code_length*sizeof(u8));
        for (u32 i = 0; i < ci->code_length; ++i) ci->code[i] = read_u8(rptr);
        ci->exception_table_length = read_u16(rptr);
        ci->exception_table = arena_alloc(&cf->arena, sizeof(Exception_Info)*ci->exception_table_length);
        for (u16 i = 0; i < ci->exception_table_length; ++i) ci->exception_table[i] = (Exception_Info){read_u16(rptr),read_u16(rptr),read_u16(rptr),read_u16(rptr)};
        ci->attributes_count = read_u16(rptr);
        ci->attributes = arena_alloc(&cf->arena, sizeof(Attribute_Info*)*ci->attributes_count);
        for (u16 i = 0; i < ci->attributes_count; ++i) ci->attributes[i] = class_file_load_attribute(cf, rptr);
    } else {
        // This is a stub to not break the reading of the class file;
        for (u32 i = 0; i < attribute_length; ++i) {
            (void)read_u8(rptr);
        }
    }

    return ai;
}

int class_file_load(JVM_Class_File *cf, Sized_Buffer buf) {
    cf->arena = (Arena){0};
    /* Validate that the file is a class file */
    u8 *rptr = buf.data;
    u8 *endptr = buf.data+buf.size;

    u32 magic = read_u32(&rptr);
    printf("0x%X\n", magic);
    if (magic != 0xCAFEBABE) return -1;

    cf->minor_version = read_u16(&rptr);
    cf->major_version = read_u16(&rptr);

    printf("Version: %u:%u\n", (u32)cf->major_version, (u32)cf->minor_version);

    /* Read constant pool */

    cf->constant_pool_count = read_u16(&rptr)-1; // -1 is according to the jvm 8 spec.

    cf->cp_info = arena_alloc(&cf->arena, sizeof(Constant_Info*)*(cf->constant_pool_count));

    printf("%u\n", cf->constant_pool_count);

    for (u16 i = 0; i < cf->constant_pool_count; ++i) {
        u8 cp_info_tag = read_u8(&rptr);
        switch (cp_info_tag) {
        case 7: { // CONSTANT_Class
            Constant_Class_Info *class_info = arena_alloc(&cf->arena, sizeof(Constant_Class_Info));
            cf->cp_info[i] = (Constant_Info*)class_info;

            class_info->tag = cp_info_tag;
            class_info->name_index = read_u16(&rptr);
        } break;
        case 9: 
        case 10:
        case 11: { // refs 
            Constant_Ref_Info *ref_info = arena_alloc(&cf->arena, sizeof(Constant_Ref_Info));
            cf->cp_info[i] = (Constant_Info*)ref_info;

            ref_info->tag = cp_info_tag;
            ref_info->class_index = read_u16(&rptr);
            ref_info->name_and_type_index = read_u16(&rptr);
        } break;
        case 8: { // String
            Constant_String_Info *string_info = arena_alloc(&cf->arena, sizeof(Constant_String_Info));
            cf->cp_info[i] = (Constant_Info*)string_info;

            string_info->tag = cp_info_tag;
            string_info->string_index = read_u16(&rptr);
        } break;
        case 3: { // Integer
            Constant_Integer_Info *int_info = arena_alloc(&cf->arena, sizeof(Constant_Integer_Info));
            cf->cp_info[i] = (Constant_Info*)int_info;

            int_info->tag = cp_info_tag;
            int_info->bytes = (s32)read_u32(&rptr);
        } break;
        case 4: { // Float
            Constant_Float_Info *float_info = arena_alloc(&cf->arena, sizeof(Constant_Float_Info));
            cf->cp_info[i] = (Constant_Info*)float_info;

            float_info->tag = cp_info_tag;
            float_info->bytes = (f32)read_u32(&rptr);
        } break;
        case 5: { // Long
            Constant_Long_Info *long_info = arena_alloc(&cf->arena, sizeof(Constant_Long_Info));
            cf->cp_info[i] = (Constant_Info*)long_info;

            long_info->tag = cp_info_tag;
            long_info->bytes = (s64)(((u64)read_u32(&rptr))<<32 | (u64)read_u32(&rptr));
        } break;
        case 6: { // Double
            Constant_Double_Info *double_info = arena_alloc(&cf->arena, sizeof(Constant_Double_Info));
            cf->cp_info[i] = (Constant_Info*)double_info;

            double_info->tag = cp_info_tag;
            double_info->bytes = (f64)(((u64)read_u32(&rptr))<<32 | (u64)read_u32(&rptr));
        } break;
        case 12: { // NameAndType
            Constant_NameAndType_Info *nat_info = arena_alloc(&cf->arena, sizeof(Constant_NameAndType_Info));
            cf->cp_info[i] = (Constant_Info*)nat_info;

            nat_info->tag = cp_info_tag;
            nat_info->name_index = read_u16(&rptr);
            nat_info->descriptor_index = read_u16(&rptr);
        } break;
        case 1: { // UTF-8
            Constant_Utf8_Info *utf8_info = arena_alloc(&cf->arena, sizeof(Constant_Utf8_Info));
            cf->cp_info[i] = (Constant_Info*)utf8_info;

            utf8_info->tag = cp_info_tag;
            utf8_info->length = read_u16(&rptr);
            utf8_info->bytes = arena_alloc(&cf->arena, utf8_info->length);
            for (u16 j = 0; j < utf8_info->length; ++j) utf8_info->bytes[j] = read_u8(&rptr);
        } break;
        case 15: { // Method Handle
            Constant_MethodHandle_Info *mh_info = arena_alloc(&cf->arena, sizeof(Constant_MethodHandle_Info));
            cf->cp_info[i] = (Constant_Info*)mh_info;

            mh_info->tag = cp_info_tag;
            mh_info->reference_kind = read_u8(&rptr);
            mh_info->reference_index = read_u16(&rptr);
        } break;
        case 16: { // Method Type
            Constant_MethodType_Info *mt_info = arena_alloc(&cf->arena, sizeof(Constant_MethodType_Info));
            cf->cp_info[i] = (Constant_Info*)mt_info;

            mt_info->tag = cp_info_tag;
            mt_info->descriptor_index = read_u16(&rptr);
        } break;
        case 18: { // Invoke Dynamic
            Constant_InvokeDynamic_Info *id_info = arena_alloc(&cf->arena, sizeof(Constant_InvokeDynamic_Info));
            cf->cp_info[i] = (Constant_Info*)id_info;

            id_info->tag = cp_info_tag;
            id_info->bootstrap_method_attr_index = read_u16(&rptr);
            id_info->name_and_type_index = read_u16(&rptr);
        } break;
        default: printf("Unimplemented: %u\n", cp_info_tag);
        }
    }

    // for (int i = 0; i < constant_pool_count; ++i) {
    //     printf("%d\n", (int)(cp_info[i]->tag));
    // }

    cf->access_flags = read_u16(&rptr);
    cf->this_class = read_u16(&rptr);
    cf->super_class = read_u16(&rptr);

    cf->interfaces_count = read_u16(&rptr);
    cf->interfaces = arena_alloc(&cf->arena, sizeof(u16)*cf->interfaces_count);
    for (u16 i = 0; i < cf->interfaces_count; ++i) cf->interfaces[i] = read_u16(&rptr);

    cf->fields_count = read_u16(&rptr);
    cf->fields = arena_alloc(&cf->arena, sizeof(Field_Info*)*cf->fields_count);
    for (u16 i = 0; i < cf->fields_count; ++i) {
        Field_Info *f = arena_alloc(&cf->arena, sizeof(Field_Info));
        cf->fields[i] = f;

        f->access_flags = read_u16(&rptr);
        f->name_index = read_u16(&rptr);
        f->descriptor_index = read_u16(&rptr);
        
        f->attributes_count = read_u16(&rptr);
        f->attributes = arena_alloc(&cf->arena, sizeof(Attribute_Info*)*f->attributes_count);
        for (u16 j = 0; j < f->attributes_count; ++j) {
            printf("foo\n");
            f->attributes[j] = class_file_load_attribute(cf, &rptr);
        }
    }

    cf->methods_count = read_u16(&rptr);
    cf->methods = arena_alloc(&cf->arena, sizeof(Method_Info*)*(cf->methods_count));
    for (u16 i = 0; i < cf->methods_count; ++i) {
        Method_Info *m = arena_alloc(&cf->arena, sizeof(Method_Info));
        cf->methods[i] = m;

        m->access_flags = read_u16(&rptr);
        m->name_index = read_u16(&rptr);
        m->descriptor_index = read_u16(&rptr);
        m->attributes_count = read_u16(&rptr);
        m->attributes = arena_alloc(&cf->arena, sizeof(Attribute_Info*)*m->attributes_count);
        for (u16 j = 0; j < m->attributes_count; ++j) {
            m->attributes[j] = class_file_load_attribute(cf, &rptr);
        }
    }

    cf->attributes_count = read_u16(&rptr);
    cf->attributes = arena_alloc(&cf->arena, sizeof(Attribute_Info*)*cf->attributes_count);

    for (u16 i = 0; i < cf->attributes_count; ++i) {
        cf->attributes[i] = class_file_load_attribute(cf, &rptr);
    }

    return 0;
}

