#include "Class.h"

#include <sese/Log.h>
#include <sese/util/Endian.h>
#include <sese/util/Exception.h>

jvm::Class::Class(const std::string &path) {
    SESE_DEBUG("open %s", path.c_str());
    file = sese::io::File::create(path, sese::io::File::B_READ);
    if (!file) throw sese::Exception("cannot open file");
    SESE_DEBUG("open success");
}

void jvm::Class::parse() {
    parseMagicNumber();
    parseVersion();
    parseConstantPool();
    parseClass();
    parseFields();
    parseMethods();
    parseAttributes();
}

#define IF_READ(m) if (sizeof(m) != file->read(&m, sizeof(m)))
#define ASSERT_READ(m) IF_READ(m) throw sese::Exception("failed to parse " #m);

void jvm::Class::parseMagicNumber() {
    ASSERT_READ(magic)
    magic = ToBigEndian32(magic);
    SESE_DEBUG("magic number 0x%x", magic);
}

void jvm::Class::parseVersion() {
    ASSERT_READ(minor)
    minor = FromBigEndian16(minor);
    SESE_DEBUG("minor version %d", minor);
    ASSERT_READ(major)
    major = FromBigEndian16(major);
    SESE_DEBUG("major version %d", major);
}

void jvm::Class::parseConstantPool() {
    ASSERT_READ(constant_pool_count)
    constant_pool_count = FromBigEndian16(constant_pool_count);
    SESE_DEBUG("constant pool count %d", constant_pool_count);

    for (int i = 1; i < constant_pool_count; i++) {
        int8_t tag;
        ASSERT_READ(tag)
        if (tag == utf8_info) {
            int16_t length;
            char bytes[UINT16_MAX]{};
            ASSERT_READ(length)
            length = FromBigEndian16(length);
            if (length != file->read(bytes, length)) {
                throw sese::Exception("failed to parse bytes");
            }
            auto item = std::make_unique<ConstantInfo_Utf8>();
            item->tag = tag;
            item->bytes = std::string(bytes, length);
            constant_infos.emplace_back(std::move(item));
        } else if (tag == integer_info) {
            int32_t bytes;
            ASSERT_READ(bytes)
            bytes = FromBigEndian32(bytes);
            auto item = std::make_unique<ConstantInfo_Integer>();
            item->bytes = bytes;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == float_info) {
            float_t bytes;
            ASSERT_READ(bytes)
            // bytes = FromBigEndian32(bytes);
            auto item = std::make_unique<ConstantInfo_Float>();
            item->bytes = bytes;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == long_info) {
            int64_t bytes;
            ASSERT_READ(bytes)
            bytes = FromBigEndian32(bytes);
            auto item = std::make_unique<ConstantInfo_Long>();
            item->bytes = bytes;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == double_info) {
            double bytes;
            ASSERT_READ(bytes)
            // bytes = FromBigEndian32(bytes);
            auto item = std::make_unique<ConstantInfo_Double>();
            item->bytes = bytes;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == class_info) {
            int16_t index;
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            auto item = std::make_unique<ConstantInfo_Class>();
            item->index = index;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == string_info) {
            int16_t index;
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            auto item = std::make_unique<ConstantInfo_String>();
            item->index = index;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == field_ref_info) {
            int16_t index1, index2;
            ASSERT_READ(index1)
            index1 = FromBigEndian16(index1);
            ASSERT_READ(index2)
            index2 = FromBigEndian16(index2);
            auto item = std::make_unique<ConstantInfo_FieldRef>();
            item->class_info_index = index1;
            item->name_and_type_index = index2;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == method_ref_info) {
            int16_t index1, index2;
            ASSERT_READ(index1)
            index1 = FromBigEndian16(index1);
            ASSERT_READ(index2)
            index2 = FromBigEndian16(index2);
            auto item = std::make_unique<ConstantInfo_MethodRef>();
            item->class_info_index = index1;
            item->name_and_type_index = index2;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == interface_method_ref_info) {
            int16_t index1, index2;
            ASSERT_READ(index1)
            index1 = FromBigEndian16(index1);
            ASSERT_READ(index2)
            index2 = FromBigEndian16(index2);
            auto item = std::make_unique<ConstantInfo_InterfaceMethodRef>();
            item->class_info_index = index1;
            item->name_and_type_index = index2;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == name_and_type_info) {
            int16_t index1, index2;
            ASSERT_READ(index1)
            index1 = FromBigEndian16(index1);
            ASSERT_READ(index2)
            index2 = FromBigEndian16(index2);
            auto item = std::make_unique<ConstantInfo_NameAndType>();
            item->name_index = index1;
            item->descriptor_index = index2;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == method_handle_info) {
            int8_t kind;
            int16_t index;
            ASSERT_READ(kind)
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            auto item = std::make_unique<ConstantInfo_MethodHandle>();
            item->refrence_kind = kind;
            item->refrence_index = index;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == method_type_info) {
            int16_t index;
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            auto item = std::make_unique<ConstantInfo_MethodType>();
            item->descriptor_index = index;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == invoke_dynamic_info) {
            int16_t index1, index2;
            ASSERT_READ(index1)
            index1 = FromBigEndian16(index1);
            ASSERT_READ(index2)
            index2 = FromBigEndian16(index2);
            auto item = std::make_unique<ConstantInfo_InvokeDynamic>();
            item->bootstrap_method_attr_index = index1;
            item->name_and_type_index = index2;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == module_info) {
            int16_t index;
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            auto item = std::make_unique<ConstantInfo_Module>();
            item->name_index = index;
            constant_infos.emplace_back(std::move(item));
        } else if (tag == package_info) {
            int16_t index;
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            auto item = std::make_unique<ConstantInfo_Package>();
            item->name_index = index;
            constant_infos.emplace_back(std::move(item));
        } else {
            throw sese::Exception("unknow tag type");
        }
    }
}

void jvm::Class::parseClass() {
    ASSERT_READ(access_flags)
    access_flags = FromBigEndian16(access_flags);
    SESE_DEBUG("access flags 0x%x", access_flags);

    ASSERT_READ(this_class)
    this_class = FromBigEndian16(this_class);
    SESE_DEBUG("this class %d", this_class);

    ASSERT_READ(super_class)
    super_class = FromBigEndian16(super_class);
    SESE_DEBUG("super class %d", super_class);

    ASSERT_READ(interface_count)
    interface_count = FromBigEndian16(interface_count);
    SESE_DEBUG("interface count %d", interface_count);

    interfaces.reserve(interface_count);
    for (int i = 0; i < interface_count; i++) {
        uint16_t iface;
        ASSERT_READ(iface)
        iface = FromBigEndian16(iface);
        SESE_DEBUG("interface %d", iface);
        interfaces.push_back(iface);
    }
}

void jvm::Class::parseFields() {
    ASSERT_READ(fields_count)
    fields_count = FromBigEndian16(fields_count);
    SESE_DEBUG("fields count %d", fields_count);

    field_infos.reserve(fields_count);
    for (int i = 0; i < fields_count; ++i) {
        FieldInfo field_info;
        ASSERT_READ(field_info.access_flags)
        field_info.access_flags = FromBigEndian16(field_info.access_flags);
        ASSERT_READ(field_info.name_index)
        field_info.name_index = FromBigEndian16(field_info.name_index);
        ASSERT_READ(field_info.descriptor_index)
        field_info.descriptor_index = FromBigEndian16(field_info.descriptor_index);
        ASSERT_READ(field_info.attributes_count)
        field_info.attributes_count = FromBigEndian16(field_info.attributes_count);
        field_info.attribute_infos.reserve(field_info.attributes_count);
        for (int j = 0; i < field_info.attributes_count; j++) {
            AttributeInfo attribute_info;
            ASSERT_READ(attribute_info.name_index)
            ASSERT_READ(attribute_info.length)
            attribute_info.length = FromBigEndian32(attribute_info.length);
            attribute_info.info.reserve(attribute_info.length);
            for (int k = 0; k < attribute_info.length; k++) {
                uint8_t byte;
                ASSERT_READ(byte)
                attribute_info.info.push_back(byte);
            }
            field_info.attribute_infos.push_back(attribute_info);
        }
        field_infos.push_back(field_info);
    }
}

void jvm::Class::parseMethods() {
    ASSERT_READ(methods_count)
    methods_count = FromBigEndian16(methods_count);
    SESE_DEBUG("methods count %d", methods_count);

    for (int i = 0; i < methods_count; ++i) {
        MethodInfo method_info;
        ASSERT_READ(method_info.access_flags)
        method_info.access_flags = FromBigEndian16(method_info.access_flags);
        ASSERT_READ(method_info.name_index)
        method_info.name_index = FromBigEndian16(method_info.name_index);
        ASSERT_READ(method_info.descriptor_index)
        method_info.descriptor_index = FromBigEndian16(method_info.descriptor_index);
        ASSERT_READ(method_info.attributes_count)
        method_info.attributes_count = FromBigEndian16(method_info.attributes_count);
        method_info.attribute_infos.reserve(method_info.attributes_count);
        for (int j = 0; j < method_info.attributes_count; ++j) {
            AttributeInfo attribute_info;
            ASSERT_READ(attribute_info.name_index)
            ASSERT_READ(attribute_info.length)
            attribute_info.length = FromBigEndian32(attribute_info.length);
            attribute_info.info.reserve(attribute_info.length);
            for (int k = 0; k < attribute_info.length; k++) {
                uint8_t byte;
                ASSERT_READ(byte)
                attribute_info.info.push_back(byte);
            }
            method_info.attribute_infos.push_back(attribute_info);
        }
        method_infos.push_back(method_info);
    }
}

void jvm::Class::parseAttributes() {
    ASSERT_READ(attributes_count)
    attributes_count = FromBigEndian16(attributes_count);
    SESE_DEBUG("attributes count %d", attributes_count);
    attribute_infos.reserve(attributes_count);
    for (int j = 0; j < attributes_count; ++j) {
        AttributeInfo attribute_info;
        ASSERT_READ(attribute_info.name_index)
        ASSERT_READ(attribute_info.length)
        attribute_info.length = FromBigEndian32(attribute_info.length);
        attribute_info.info.reserve(attribute_info.length);
        for (int k = 0; k < attribute_info.length; k++) {
            uint8_t byte;
            ASSERT_READ(byte)
            attribute_info.info.push_back(byte);
        }
        attribute_infos.push_back(attribute_info);
    }
}
