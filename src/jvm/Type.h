#pragma once

#include <string>

namespace jvm {
    enum Type : char {
        void_ = 'V',
        byte = 'B',
        char_ = 'C',
        double_ = 'D',
        float_ = 'F',
        int_ = 'I',
        long_ = 'J',
        short_ = 'S',
        boolean = 'Z',
        object = 'L'
    };

    struct TypeInfo {
        uint8_t is_array{0};
        Type type{void_};
        std::string external_name;

        void parse(std::string raw_name);

        void set(Type type, bool array);

        void set(const std::string &object_name, bool array);

        [[nodiscard]] std::string toString() const;
    };
}
