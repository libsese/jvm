#include "Class.h"

#include <sese/Log.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Endian.h>
#include <sese/util/Exception.h>

#undef SESE_DEBUG
#define SESE_DEBUG(...)

inline std::string getUtf8(const std::vector<std::unique_ptr<jvm::Class::ConstantInfo> > &constants, uint16_t index) {
    auto string_ptr = &constants[index];
    auto string_info = dynamic_cast<jvm::Class::ConstantInfo_Utf8 *>(string_ptr->get());
    return string_info->bytes;
}

void jvm::Class::parse(sese::io::InputStream *input_stream) {
    parseMagicNumber(input_stream);
    parseVersion(input_stream);
    parseConstantPool(input_stream);
    parseClass(input_stream);
    parseFields(input_stream);
    parseMethods(input_stream);
    parseAttributes(input_stream);
}

#define IF_READ(m) if (sizeof(m) != input_stream->read(&m, sizeof(m)))
#define ASSERT_READ(m) IF_READ(m) throw sese::Exception("failed to parse " #m);

void jvm::Class::parseMagicNumber(sese::io::InputStream *input_stream) {
    ASSERT_READ(magic)
    magic = ToBigEndian32(magic);
    SESE_DEBUG("magic number 0x%x", magic);
}

void jvm::Class::parseVersion(sese::io::InputStream *input_stream) {
    ASSERT_READ(minor)
    minor = FromBigEndian16(minor);
    SESE_DEBUG("minor version %d", minor);
    ASSERT_READ(major)
    major = FromBigEndian16(major);
    SESE_DEBUG("major version %d", major);
}

void jvm::Class::parseConstantPool(sese::io::InputStream *input_stream) {
    uint16_t constant_pool_count;
    ASSERT_READ(constant_pool_count)
    constant_pool_count = FromBigEndian16(constant_pool_count);
    SESE_DEBUG("constant pool count %d", constant_pool_count);
    constant_infos.reserve(constant_pool_count);

    constant_infos.emplace_back(std::make_unique<ConstantInfo>());
    for (int i = 1; i < constant_pool_count; i++) {
        int8_t tag;
        ASSERT_READ(tag)
        if (tag == utf8_info) {
            int16_t length;
            char bytes[UINT16_MAX]{};
            ASSERT_READ(length)
            length = FromBigEndian16(length);
            if (length != input_stream->read(bytes, length)) {
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
            bytes = FromBigEndian64(bytes);
            auto item = std::make_unique<ConstantInfo_Long>();
            item->bytes = bytes;
            constant_infos.emplace_back(std::move(item));

            constant_infos.emplace_back(std::make_unique<ConstantInfo>());
            ++i;
        } else if (tag == double_info) {
            double bytes;
            ASSERT_READ(bytes)
            // bytes = FromBigEndian64(bytes);
            auto item = std::make_unique<ConstantInfo_Double>();
            item->bytes = bytes;
            constant_infos.emplace_back(std::move(item));

            constant_infos.emplace_back(std::make_unique<ConstantInfo>());
            ++i;
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
            item->reference_kind = kind;
            item->reference_index = index;
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
            throw sese::Exception("unknown tag type");
        }
    }
}

void jvm::Class::parseClass(sese::io::InputStream *input_stream) {
    ASSERT_READ(access_flags)
    access_flags = FromBigEndian16(access_flags);
    SESE_DEBUG("access flags 0x%x", access_flags);

    ASSERT_READ(this_class)
    this_class = FromBigEndian16(this_class);
    SESE_DEBUG("this class %d", this_class);

    ASSERT_READ(super_class)
    super_class = FromBigEndian16(super_class);
    SESE_DEBUG("super class %d", super_class);

    uint16_t interface_count;
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

void jvm::Class::parseFields(sese::io::InputStream *input_stream) {
    int16_t fields_count;
    ASSERT_READ(fields_count)
    fields_count = FromBigEndian16(fields_count);
    SESE_DEBUG("fields count %d", fields_count);

    field_infos.reserve(fields_count);
    for (int i = 0; i < fields_count; ++i) {
        FieldInfo field_info;
        uint16_t name_index, descriptor_index, attributes_count;
        ASSERT_READ(field_info.access_flags)
        field_info.access_flags = FromBigEndian16(field_info.access_flags);
        ASSERT_READ(name_index)
        name_index = FromBigEndian16(name_index);
        field_info.name = getUtf8(constant_infos, name_index);
        ASSERT_READ(descriptor_index)
        descriptor_index = FromBigEndian16(descriptor_index);
        auto descriptor = getUtf8(constant_infos, descriptor_index);
        field_info.type.parse(descriptor);
        ASSERT_READ(attributes_count)
        attributes_count = FromBigEndian16(attributes_count);
        field_info.attribute_infos.reserve(attributes_count);
        for (int j = 0; i < attributes_count; j++) {
            AttributeInfo attribute_info;
            uint32_t length;
            ASSERT_READ(name_index)
            name_index = FromBigEndian16(name_index);
            attribute_info.name = getUtf8(constant_infos, name_index);
            ASSERT_READ(length)
            length = FromBigEndian32(length);
            attribute_info.info.reserve(length);
            attribute_info.info.reserve(length);
            if (length != input_stream->read(attribute_info.info.data(), length)) {
                throw sese::Exception("failed to parse attribute_info.info");
            }
            field_info.attribute_infos.push_back(attribute_info);
        }
        field_infos.push_back(std::move(field_info));
    }
}

void jvm::Class::parseMethods(sese::io::InputStream *input_stream) {
    uint16_t methods_count;
    ASSERT_READ(methods_count)
    methods_count = FromBigEndian16(methods_count);
    SESE_DEBUG("methods count %d", methods_count);

    for (int i = 0; i < methods_count; ++i) {
        MethodInfo method_info;
        uint16_t name_index, descriptor_index, attributes_count;
        ASSERT_READ(method_info.access_flags)
        method_info.access_flags = FromBigEndian16(method_info.access_flags);
        ASSERT_READ(name_index)
        name_index = FromBigEndian16(name_index);
        method_info.name = getUtf8(constant_infos, name_index);
        ASSERT_READ(descriptor_index)
        descriptor_index = FromBigEndian16(descriptor_index);
        auto descriptor = getUtf8(constant_infos, descriptor_index);
        method_info.descriptor = descriptor;
        descriptor = descriptor.substr(1, descriptor.length() - 1);
        auto pos1 = descriptor.find('(');
        auto pos2 = descriptor.find(')');
        auto params = descriptor.substr(pos1 + 1, pos2 - pos1 - 1);
        auto return_ = descriptor.substr(pos2 + 1);
        for (int l = 0; l < params.size();) {
            TypeInfo type_info;
            auto prefix = params.find_first_not_of('[');
            if (prefix == std::string::npos) prefix = 0;
            if (params[l + prefix] == 'L') {
                auto end = params.find_first_of(';');
                auto param = params.substr(l, end + 1);
                type_info.parse(param);
                l += static_cast<int>(end) + 1;
            } else {
                type_info.parse(params.substr(l, 1));
                l += 1;
            }
            method_info.args_type.emplace_back(std::move(type_info));
        }
        method_info.return_type.parse(return_);
        ASSERT_READ(attributes_count)
        attributes_count = FromBigEndian16(attributes_count);
        method_info.attribute_infos.reserve(attributes_count);
        for (int j = 0; j < attributes_count; ++j) {
            AttributeInfo attribute_info;
            uint32_t length;
            ASSERT_READ(name_index)
            name_index = FromBigEndian16(name_index);
            attribute_info.name = getUtf8(constant_infos, name_index);
            ASSERT_READ(length)
            length = FromBigEndian32(length);
            if (attribute_info.name == "Code") {
                method_info.code_info = std::make_unique<CodeInfo>();
                parseAttributeCode(input_stream, method_info.code_info.get());
            } else if (attribute_info.name == "Exceptions") {
                uint16_t exceptions_count;
                ASSERT_READ(exceptions_count)
                exceptions_count = FromBigEndian16(exceptions_count);
                for (int k = 0; k < exceptions_count; ++k) {
                    ExceptionInfo exception_info;
                    ASSERT_READ(exception_info.type)
                    exception_info.type = FromBigEndian16(exception_info.type);
                    method_info.exception_infos.emplace_back(exception_info);
                }
            } else {
                attribute_info.info.reserve(length);
                attribute_info.info.resize(length);
                if (length != input_stream->read(attribute_info.info.data(), length)) {
                    throw sese::Exception("failed to parse attribute_info.info");
                }
                method_info.attribute_infos.push_back(attribute_info);
            }
        }
        method_infos[method_info.name + method_info.descriptor] = std::move(method_info);
    }
}

void jvm::Class::parseAttributes(sese::io::InputStream *input_stream) {
    uint16_t attributes_count;
    ASSERT_READ(attributes_count)
    attributes_count = FromBigEndian16(attributes_count);
    SESE_DEBUG("attributes count %d", attributes_count);
    attribute_infos.reserve(attributes_count);
    for (int j = 0; j < attributes_count; ++j) {
        AttributeInfo attribute_info;
        uint16_t name_index;
        uint32_t length;
        ASSERT_READ(name_index)
        ASSERT_READ(length)
        length = FromBigEndian32(length);
        name_index = FromBigEndian16(name_index);
        attribute_info.name = getUtf8(constant_infos, name_index);
        if (attribute_info.name == "SourceFile") {
            ASSERT_READ(name_index)
            name_index = FromBigEndian16(name_index);
            source_file = getUtf8(constant_infos, name_index);
        } else {
            attribute_info.info.reserve(length);
            attribute_info.info.resize(length);
            if (length != input_stream->read(attribute_info.info.data(), length)) {
                throw sese::Exception("failed to parse attribute_info.info");
            }
            attribute_infos.push_back(attribute_info);
        }
    }
}

void jvm::Class::parseAttributeCode(sese::io::InputStream *input_stream, CodeInfo *code_info) const {
    uint32_t code_length;
    uint16_t exceptions_count;
    uint16_t attributes_count;
    ASSERT_READ(code_info->max_stack)
    code_info->max_stack = FromBigEndian16(code_info->max_stack);
    ASSERT_READ(code_info->max_locals)
    code_info->max_locals = FromBigEndian16(code_info->max_locals);
    ASSERT_READ(code_length)
    code_length = FromBigEndian32(code_length);
    code_info->code.reserve(code_length);
    code_info->code.resize(code_length);
    if (code_length != input_stream->read(code_info->code.data(), code_length)) {
        throw sese::Exception("failed to read code");
    }
    ASSERT_READ(exceptions_count)
    exceptions_count = FromBigEndian16(exceptions_count);
    for (int k = 0; k < exceptions_count; ++k) {
        ExceptionInfo exception_info;
        ASSERT_READ(exception_info.from)
        exception_info.from = FromBigEndian16(exception_info.from);
        ASSERT_READ(exception_info.to)
        exception_info.to = FromBigEndian16(exception_info.to);
        ASSERT_READ(exception_info.target)
        exception_info.target = FromBigEndian16(exception_info.target);
        ASSERT_READ(exception_info.type)
        exception_info.type = FromBigEndian16(exception_info.type);
        code_info->exception_infos.emplace_back(exception_info);
    }
    ASSERT_READ(attributes_count)
    attributes_count = FromBigEndian16(attributes_count);
    for (int j = 0; j < attributes_count; ++j) {
        AttributeInfo attribute_info;
        uint16_t name_index;
        uint32_t length;
        ASSERT_READ(name_index)
        name_index = FromBigEndian16(name_index);
        attribute_info.name = getUtf8(constant_infos, name_index);
        ASSERT_READ(length)
        length = FromBigEndian32(length);
        if (attribute_info.name == "LineNumberTable") {
            uint16_t line_number_info_count;
            ASSERT_READ(line_number_info_count)
            line_number_info_count = FromBigEndian16(line_number_info_count);
            for (int k = 0; k < line_number_info_count; ++k) {
                LineNumberInfo line_number_info{};
                ASSERT_READ(line_number_info.start_pc)
                line_number_info.start_pc = FromBigEndian16(line_number_info.start_pc);
                ASSERT_READ(line_number_info.line_number)
                line_number_info.line_number = FromBigEndian16(line_number_info.line_number);
                code_info->line_infos.emplace_back(line_number_info);
            }
        } else {
            attribute_info.info.reserve(length);
            attribute_info.info.resize(length);
            if (length != input_stream->read(attribute_info.info.data(), length)) {
                throw sese::Exception("failed to parse attribute");
            }
            code_info->attribute_infos.emplace_back(std::move(attribute_info));
        }
    }
}
