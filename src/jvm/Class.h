#pragma once

#include <map>
#include <jvm/AccessFlags.h>
#include <jvm/Type.h>

#include <sese/io/InputStream.h>

#include <vector>

namespace jvm {
    class Runtime;

    class Class : public AccessFlags {
    public:
        friend class Runtime;

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

            uint8_t tag{};
        };

        struct ConstantInfo_Utf8 final : ConstantInfo {
            ConstantInfo_Utf8() {
                tag = utf8_info;
            }

            std::string bytes;
        };

        struct ConstantInfo_Integer final : ConstantInfo {
            ConstantInfo_Integer() {
                tag = integer_info;
            }

            int32_t bytes{};
        };

        struct ConstantInfo_Float final : ConstantInfo {
            ConstantInfo_Float() {
                tag = float_info;
            }

            float_t bytes{};
        };

        struct ConstantInfo_Long final : ConstantInfo {
            ConstantInfo_Long() {
                tag = long_info;
            }

            int64_t bytes{};
        };

        struct ConstantInfo_Double final : ConstantInfo {
            ConstantInfo_Double() {
                tag = double_info;
            }

            double_t bytes{};
        };

        struct ConstantInfo_Class final : ConstantInfo {
            ConstantInfo_Class() {
                tag = class_info;
            }

            uint16_t index{};
        };

        struct ConstantInfo_String final : ConstantInfo {
            ConstantInfo_String() {
                tag = string_info;
            }

            uint16_t index{};
        };

        struct ConstantInfo_FieldRef final : ConstantInfo {
            ConstantInfo_FieldRef() {
                tag = field_ref_info;
            }

            uint16_t class_info_index{};
            uint16_t name_and_type_index{};
        };

        struct ConstantInfo_MethodRef final : ConstantInfo {
            ConstantInfo_MethodRef() {
                tag = method_ref_info;
            }

            uint16_t class_info_index{};
            uint16_t name_and_type_index{};
        };

        struct ConstantInfo_InterfaceMethodRef final : ConstantInfo {
            ConstantInfo_InterfaceMethodRef() {
                tag = interface_method_ref_info;
            }

            uint16_t class_info_index{};
            uint16_t name_and_type_index{};
        };

        struct ConstantInfo_NameAndType final : ConstantInfo {
            ConstantInfo_NameAndType() {
                tag = name_and_type_info;
            }

            uint16_t name_index{};
            uint16_t descriptor_index{};
        };

        struct ConstantInfo_MethodHandle final : ConstantInfo {
            ConstantInfo_MethodHandle() {
                tag = method_handle_info;
            }

            uint8_t reference_kind{};
            uint16_t reference_index{};
        };

        struct ConstantInfo_MethodType final : ConstantInfo {
            ConstantInfo_MethodType() {
                tag = method_type_info;
            }

            uint16_t descriptor_index{};
        };

        struct ConstantInfo_InvokeDynamic final : ConstantInfo {
            ConstantInfo_InvokeDynamic() {
                tag = invoke_dynamic_info;
            }

            uint16_t bootstrap_method_attr_index{};
            uint16_t name_and_type_index{};
        };

        struct ConstantInfo_Module final : ConstantInfo {
            ConstantInfo_Module() {
                tag = module_info;
            }

            uint16_t name_index{};
        };

        struct ConstantInfo_Package final : ConstantInfo {
            ConstantInfo_Package() {
                tag = package_info;
            }

            uint16_t name_index{};
        };

        struct AttributeInfo {
            // uint16_t name_index{};
            std::string name{};
            // uint32_t length{};
            std::vector<uint8_t> info{};
        };

        struct ExceptionInfo {
            uint16_t from{};
            uint16_t to{};
            uint16_t target{};
            uint16_t type{};
        };

        struct LineNumberInfo {
            uint16_t start_pc;
            uint16_t line_number;
        };

        struct CodeInfo {
            uint16_t max_stack;
            uint16_t max_locals;
            std::vector<uint8_t> code;
            std::vector<ExceptionInfo> exception_infos;
            std::vector<LineNumberInfo> line_infos;
            std::vector<AttributeInfo> attribute_infos;
        };

        struct FieldInfo : AccessFlags {
            // uint16_t name_index{};
            std::string name{};
            // uint16_t descriptor_index{};
            // std::string descriptor{};
            TypeInfo type;
            // uint16_t attributes_count{};
            std::vector<AttributeInfo> attribute_infos{};
        };

        struct MethodInfo : AccessFlags {
            std::string name{};
            std::string descriptor{};
            TypeInfo return_type;
            std::vector<TypeInfo> args_type;
            std::vector<AttributeInfo> attribute_infos{};
            std::unique_ptr<CodeInfo> code_info;
            std::vector<ExceptionInfo> exception_infos;
        };

        explicit Class(sese::io::InputStream *input_stream);

        [[nodiscard]] std::string getThisName() const;

        [[nodiscard]] std::string getSuperName() const;

        [[nodiscard]] const std::string &getSourceName() const { return source_file; }

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

        void parseAttributeCode(sese::io::InputStream *input_stream, CodeInfo *code_info) const;

        uint32_t magic{};
        uint16_t minor{}, major{};
        std::vector<std::unique_ptr<ConstantInfo> > constant_infos;
        uint16_t access_flags{};
        uint16_t this_class{};
        uint16_t super_class{};
        std::vector<uint16_t> interfaces{};
        std::vector<FieldInfo> field_infos{};
        std::map<std::string, MethodInfo> method_infos{};
        std::vector<AttributeInfo> attribute_infos{};
        std::vector<ExceptionInfo> exception_infos{};
        std::string source_file{};
    };
}
