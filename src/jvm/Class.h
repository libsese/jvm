#pragma once

#include <jvm/AccessFlags.h>
#include <sese/io/InputStream.h>
#include <vector>

namespace jvm {
    class Class : public AccessFlags {
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
            invoke_dynamic_info = 18,
            module_info = 19,
            package_info = 20
        };

        struct ConstantInfo {
            virtual ~ConstantInfo() = default;

            uint8_t tag;
        };

        struct ConstantInfo_Utf8 final : ConstantInfo {
            std::string bytes;
        };

        struct ConstantInfo_Integer final : ConstantInfo {
            int32_t bytes;
        };

        struct ConstantInfo_Float final : ConstantInfo {
            float_t bytes;
        };

        struct ConstantInfo_Long final : ConstantInfo {
            int64_t bytes;
        };

        struct ConstantInfo_Double final : ConstantInfo {
            double_t bytes;
        };

        struct ConstantInfo_Class final : ConstantInfo {
            uint16_t index;
        };

        struct ConstantInfo_String final : ConstantInfo {
            uint16_t index;
        };

        struct ConstantInfo_FieldRef final : ConstantInfo {
            uint16_t class_info_index;
            uint16_t name_and_type_index;
        };

        struct ConstantInfo_MethodRef final : ConstantInfo {
            uint16_t class_info_index;
            uint16_t name_and_type_index;
        };

        struct ConstantInfo_InterfaceMethodRef final : ConstantInfo {
            uint16_t class_info_index;
            uint16_t name_and_type_index;
        };

        struct ConstantInfo_NameAndType final : ConstantInfo {
            uint16_t name_index;
            uint16_t descriptor_index;
        };

        struct ConstantInfo_MethodHandle final : ConstantInfo {
            uint8_t refrence_kind;
            uint16_t refrence_index;
        };

        struct ConstantInfo_MethodType final : ConstantInfo {
            uint16_t descriptor_index;
        };

        struct ConstantInfo_InvokeDynamic final : ConstantInfo {
            uint16_t bootstrap_method_attr_index;
            uint16_t name_and_type_index;
        };

        struct ConstantInfo_Module final : ConstantInfo {
            uint16_t name_index;
        };

        struct ConstantInfo_Package final : ConstantInfo {
            uint16_t name_index;
        };

        struct AttributeInfo {
            // uint16_t name_index{};
            std::string name{};
            // uint32_t length{};
            std::vector<uint8_t> info;
        };

        struct FieldInfo : AccessFlags {
            // uint16_t name_index{};
            std::string name{};
            // uint16_t descriptor_index{};
            std::string descriptor{};
            // uint16_t attributes_count{};
            std::vector<AttributeInfo> attribute_infos{};
        };

        struct MethodInfo : AccessFlags {
            // uint16_t name_index;
            std::string name{};
            // uint16_t descriptor_index;
            std::string descriptor{};
            // uint16_t attributes_count;
            std::vector<AttributeInfo> attribute_infos{};
        };

        explicit Class(sese::io::InputStream *input_stream);

        [[nodiscard]] std::string getThisName() const;

        [[nodiscard]] std::string getSuperName() const;

        void printFields() const;

        void printMethods() const;

        void printAttributes() const;

        static void printAttributes(const std::vector<AttributeInfo> &attribute_infos);

    private:
        void parse(sese::io::InputStream *input_stream);

        void parseMagicNumber(sese::io::InputStream *input_stream);

        void parseVersion(sese::io::InputStream *input_stream);

        void parseConstantPool(sese::io::InputStream *input_stream);

        void parseClass(sese::io::InputStream *input_stream);

        void parseFields(sese::io::InputStream *input_stream);

        void parseMethods(sese::io::InputStream *input_stream);

        void parseAttributes(sese::io::InputStream *input_stream);

        uint32_t magic{};
        uint16_t minor{}, major{};
        uint16_t constant_pool_count{};
        std::vector<std::unique_ptr<ConstantInfo> > constant_infos;
        uint16_t access_flags{};
        uint16_t this_class{};
        uint16_t super_class{};
        // uint16_t interface_count{};
        std::vector<uint16_t> interfaces{};
        // uint16_t fields_count{};
        std::vector<FieldInfo> field_infos{};
        // uint16_t methods_count{};
        std::vector<MethodInfo> method_infos{};
        // uint16_t attributes_count{};
        std::vector<AttributeInfo> attribute_infos{};
    };
}
