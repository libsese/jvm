#include "Type.h"

#include <sese/util/Exception.h>
#include <sese/text/StringBuilder.h>

#include <cassert>

void jvm::TypeInfo::parse(std::string raw_name) {
    assert(!raw_name.empty());
    // 计算 raw_name 中 [ 的个数
    isArray = std::count(raw_name.begin(), raw_name.end(), '[');
    if (isArray) {
        raw_name = raw_name.substr(isArray, raw_name.length() - isArray);
    }
    assert(!raw_name.empty());
    if (raw_name.length() == 1 && raw_name[0] != 'L') {
        type = static_cast<Type>(raw_name[0]);
        return;
    }
    if (raw_name[0] == 'L') {
        type = object;
        externalName = raw_name.substr(1, raw_name.length() - 1);
        assert(!externalName.empty());
    }
}

void jvm::TypeInfo::set(Type type, bool array) {
    assert(type != object);
    this->type = type;
    this->isArray = array;
}

void jvm::TypeInfo::set(const std::string &object_name, bool array) {
    type = object;
    externalName = object_name;
    isArray = array;
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
            builder.append(externalName);
            break;
    }
    for (uint8_t i = 0; i < isArray; i++) {
        builder.append("[]");
    }
    return builder.toString();
}
