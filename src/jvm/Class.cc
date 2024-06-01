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
    ASSERT_READ(minor)
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
        SESE_DEBUG("tag %d", tag);
        if (tag == utf8_info) {
            int16_t length;
            char bytes[UINT16_MAX]{};
            ASSERT_READ(length)
            length = FromBigEndian16(length);
            SESE_DEBUG("length %d", length);
            if (length != file->read(bytes, length)) {
                throw sese::Exception("failed to parse bytes");
            }
            SESE_DEBUG("bytes %s", bytes);
        } else if (tag == class_info) {
            int16_t index;
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            SESE_DEBUG("index %d", index);
        } else if (tag == method_ref_info) {
            int16_t index;
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            SESE_DEBUG("index %d", index);
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            SESE_DEBUG("index %d", index);
        } else if (tag == name_and_type_info) {
            int16_t index;
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            SESE_DEBUG("index %d", index);
            ASSERT_READ(index)
            index = FromBigEndian16(index);
            SESE_DEBUG("index %d", index);
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

    for (int i = 0; i< methods_count; ++i) {
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

