#pragma once

#include <sese/io/FileStream.h>
#include <vector>

namespace jvm {
    class Class {
    public:
        enum Constant : int8_t {
            utf8_info = 1,
            integer_info = 3,
            float_info = 4,
            long_info = 5,
            double_info = 6,
            class_info = 7,
            string_info = 8,
            field_ref_info = 9,
            method_ref_info = 10,
            interface_method_ref_info = 11,
            name_and_type_info = 12,
            method_handle_info = 15,
            method_type_info = 16,
            invoke_dynamic_info = 18
        };

        struct AttributeInfo {
            uint16_t name_index{};
            uint32_t length{};
            std::vector<uint8_t> info;
        };

        struct FieldInfo {
            uint16_t access_flags{};
            uint16_t name_index{};
            uint16_t descriptor_index{};
            uint16_t attributes_count{};
            std::vector<AttributeInfo> attribute_infos{};
        };

        struct MethodInfo {
            uint16_t access_flags;
            uint16_t name_index;
            uint16_t descriptor_index;
            uint16_t attributes_count;
            std::vector<AttributeInfo> attribute_infos;
        };

        explicit Class(const std::string &path);

        void parse();

    private:
        void parseMagicNumber();

        void parseVersion();

        void parseConstantPool();

        void parseClass();

        void parseFields();

        void parseMethods();

        void parseAttributes();

        sese::io::File::Ptr file{};

        uint32_t magic{};
        uint16_t minor{}, major{};
        uint16_t constant_pool_count{};
        uint16_t access_flags{};
        uint16_t this_class{};
        uint16_t super_class{};
        uint16_t interface_count{};
        std::vector<uint16_t> interfaces{};
        uint16_t fields_count{};
        std::vector<FieldInfo> field_infos{};
        uint16_t methods_count{};
        std::vector<MethodInfo> method_infos{};
        uint16_t attributes_count{};
        std::vector<AttributeInfo> attribute_infos{};
    };
}
