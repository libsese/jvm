#include "Type.h"

#include <sese/util/Exception.h>
#include <sese/text/StringBuilder.h>

#include <cassert>
#include <algorithm>

void jvm::TypeInfo::parse(std::string raw_name) {
    assert(!raw_name.empty());
    // 计算 raw_name 中 [ 的个数
    is_array = static_cast<uint8_t>(std::count(raw_name.begin(), raw_name.end(), '['));
    if (is_array) {
        raw_name = raw_name.substr(is_array, raw_name.length() - is_array);
    }
    assert(!raw_name.empty());
    if (raw_name.length() == 1 && raw_name[0] != 'L') {
        type = static_cast<Type>(raw_name[0]);
        return;
    }
    if (raw_name[0] == 'L') {
        type = object;
        external_name = raw_name.substr(1, raw_name.length() - 2);
        assert(!external_name.empty());
    }
}

void jvm::TypeInfo::set(Type type, bool array) {
    assert(type != object);
    this->type = type;
    this->is_array = array;
}

void jvm::TypeInfo::set(const std::string &object_name, bool array) {
    type = object;
    external_name = object_name;
    is_array = array;
}

std::string jvm::TypeInfo::toString() const {
    sese::text::StringBuilder builder;
    switch (type) {
        case void_:
            builder.append("void");
            break;
        case byte:
            builder.append("byte");
            break;
        case char_:
            builder.append("char");
            break;
        case double_:
            builder.append("double");
            break;
        case float_:
            builder.append("float");
            break;
        case int_:
            builder.append("int");
            break;
        case long_:
            builder.append("long");
            break;
        case short_:
            builder.append("short");
            break;
        case boolean:
            builder.append("boolean");
            break;
        case object:
            builder.append(external_name);
            break;
    }
    for (uint8_t i = 0; i < is_array; i++) {
        builder.append("[]");
    }
    return builder.toString();
}
