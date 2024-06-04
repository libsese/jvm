#include "Class.h"

#include <sese/Log.h>
#include <sese/util/Exception.h>
#include <sese/text/StringBuilder.h>

jvm::Class::Class(sese::io::InputStream *input_stream) {
    parse(input_stream);
}

void jvm::Class::printFields() const {
    sese::text::StringBuilder builder;
    for (auto &&field: field_infos) {
        if (field.isPublic()) {
            builder.append("public ");
        } else if (field.isPrivate()) {
            builder.append("private ");
        } else if (field.isProtected()) {
            builder.append("protected ");
        }
        if (field.isStatic()) {
            builder.append("static ");
        }
        builder.append(field.type.toString());
        builder.append(' ');
        builder.append(field.name);
        SESE_DEBUG("%s", builder.toString().c_str());
        builder.clear();
        printAttributes(field.attribute_infos);
    }
}

void jvm::Class::printMethods() const {
    sese::text::StringBuilder builder;
    for (auto &&method: method_infos) {
        auto name = method.name == "<init>" ? getThisName() : method.name;
        if (method.isPublic()) {
            builder.append("public ");
        } else if (method.isPrivate()) {
            builder.append("private ");
        } else if (method.isProtected()) {
            builder.append("protected ");
        }
        if (method.isStatic()) {
            builder.append("static ");
        }
        builder.append(method.return_type.toString());
        builder.append(' ');
        builder.append(name);
        builder.append('(');
        bool first = true;
        for (auto &&type : method.args_type) {
            if (first) {
                first = false;
            } else {
                builder.append(',');
            }
            builder.append(type.toString());
        }
        builder.append(')');
        SESE_DEBUG("%s", builder.toString().c_str());
        builder.clear();
        printAttributes(method.attribute_infos);
    }
}

void jvm::Class::printAttributes() const {
    printAttributes(attribute_infos);
}

void jvm::Class::printAttributes(const std::vector<AttributeInfo> &attribute_infos) {
    for (auto &&attr: attribute_infos) {
        SESE_DEBUG("%s", attr.name.c_str());
    }
}
