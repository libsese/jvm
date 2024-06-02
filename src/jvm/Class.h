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

        enum AccessFlag : int32_t {
            public_ = 0x0001,
            private_ = 0x0002,
            protected_ = 0x0004,
            static_ = 0x0008,
            final_ = 0x0010,
            voilatie = 0x0040,
            transient = 0x0080,
            synthetic = 0x1000,
            enum_ = 0x40000,
            module_ = 0x8000,
        };

        static bool isPublic(int32_t flags);

        static bool isPrivate(int32_t flags);

        static bool isProtected(int32_t flags);

        static bool isStatic(int32_t flags);

        static bool isFinal(int32_t flags);

        static bool isVoilatie(int32_t flags);

        static bool isTransient(int32_t flags);

        static bool isSynthetic(int32_t flags);

        static bool isEnum(int32_t flags);

        static bool isModule(int32_t flags);

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

        [[nodiscard]] std::string getThisName() const;

        [[nodiscard]] std::string getSuperName() const;

        void printFields() const;

        void printMethods() const;

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
        std::vector<std::unique_ptr<ConstantInfo> > constant_infos;
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
