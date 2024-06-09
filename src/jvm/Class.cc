#include "Class.h"

#include <sese/Log.h>
#include <sese/util/Exception.h>
#include <sese/text/StringBuilder.h>

inline void printLine() {
    SESE_INFO("================================");
}

jvm::Class::Class(sese::io::InputStream *input_stream) {
    parse(input_stream);
}

void jvm::Class::printFields() const {
    printLine();
    SESE_INFO("%s's Fields:", getThisName().c_str());
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
        SESE_INFO("%s", builder.toString().c_str());
        builder.clear();
        printAttributes(field.attribute_infos);
    }
}

void jvm::Class::printMethods() const {
    printLine();
    SESE_INFO("%s's Method:", getThisName().c_str());
    sese::text::StringBuilder builder;
    for (auto &&[_,method]: method_infos) {
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
        for (auto &&type: method.args_type) {
            if (first) {
                first = false;
            } else {
                builder.append(',');
            }
            builder.append(type.toString());
        }
        builder.append(')');
        SESE_INFO("%s", builder.toString().c_str());
        builder.clear();
        if (method.code_info) {
            SESE_INFO("Code:");
            SESE_INFO("    stack=%d, locals=%d, args_size=%zu", method.code_info->max_stack,
                      method.code_info->max_locals, method.args_type.size() + (method.isStatic() ? 0 : 1));
            if (!method.code_info->line_infos.empty()) {
                SESE_INFO("LineNumber:");
                for (auto &&line: method.code_info->line_infos) {
                    SESE_INFO("    line %d: %d", line.line_number, line.start_pc);
                }
            }
        }
        printAttributes(method.attribute_infos);
    }
}

void jvm::Class::printAttributes() const {
    printLine();
    SESE_INFO("%s's Attributes:", getThisName().c_str());
    SESE_INFO("SourceFile %s", source_file.c_str());
}

void jvm::Class::printAttributes(const std::vector<AttributeInfo> &attribute_infos) {
    for (auto &&attr: attribute_infos) {
        SESE_INFO("unparsed %s", attr.name.c_str());
    }
}
