#include "Class.h"

#include <sese/Log.h>
#include <sese/util/Exception.h>
#include <sese/text/StringBuilder.h>

bool jvm::Class::isPublic(int32_t flags) {
    return public_ & flags;
}

bool jvm::Class::isPrivate(int32_t flags) {
    return private_ & flags;
}

bool jvm::Class::isProtected(int32_t flags) {
    return protected_ & flags;
}

bool jvm::Class::isStatic(int32_t flags) {
    return static_ & flags;
}

bool jvm::Class::isFinal(int32_t flags) {
    return final_ & flags;
}

bool jvm::Class::isVoilatie(int32_t flags) {
    return voilatie & flags;
}

bool jvm::Class::isTransient(int32_t flags) {
    return transient & flags;
}

bool jvm::Class::isSynthetic(int32_t flags) {
    return synthetic & flags;
}

bool jvm::Class::isEnum(int32_t flags) {
    return enum_ & flags;
}

bool jvm::Class::isModule(int32_t flags) {
    return module_ & flags;
}

jvm::Class::Class(const std::string &path) {
    SESE_DEBUG("open %s", path.c_str());
    file = sese::io::File::create(path, sese::io::File::B_READ);
    if (!file) throw sese::Exception("cannot open file");
    SESE_DEBUG("open success");
}

void jvm::Class::printMethods() const {
    sese::text::StringBuilder builder;
    for (auto &&method: method_infos) {
        auto name_ptr = &constant_infos[method.name_index - 1];
        auto name_info = dynamic_cast<ConstantInfo_Utf8 *>(name_ptr->get());
        auto name = name_info->bytes == "<init>" ? getThisName() : name_info->bytes;
        if (isPublic(method.access_flags)) {
            builder.append("public ");
        } else if (isPrivate(method.access_flags)) {
            builder.append("private ");
        } else if (isProtected(method.access_flags)) {
            builder.append("protected ");
        }
        if (isStatic(method.access_flags)) {
            builder.append("static ");
        }
        builder.append(name);
        SESE_DEBUG("method %s", builder.toString().c_str());
        builder.clear();
    }
}

void jvm::Class::printFields() const {
    sese::text::StringBuilder builder;
    for (auto &&field: field_infos) {
        if (isPublic(field.access_flags)) {
            builder.append("public ");
        } else if (isPrivate(field.access_flags)) {
            builder.append("private ");
        } else if (isProtected(field.access_flags)) {
            builder.append("protected ");
        }
        if (isStatic(field.access_flags)) {
            builder.append("static ");
        }
        auto name_ptr = &constant_infos[field.name_index - 1];
        auto name_info = dynamic_cast<ConstantInfo_Utf8 *>(name_ptr->get());
        auto name = name_info->bytes;
        builder.append(name);
        SESE_DEBUG("field %s", builder.toString().c_str());
        builder.clear();
    }
}
