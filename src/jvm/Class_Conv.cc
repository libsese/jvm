#include "Class.h"

#include <sese/Log.h>
#include <sese/util/Exception.h>

std::string jvm::Class::getThisName() const {
    auto class_ptr = &constant_infos[this_class - 1];
    auto class_info = dynamic_cast<ConstantInfo_Class *>(class_ptr->get());
    auto name_ptr = &constant_infos[class_info->index - 1];
    auto name_info = dynamic_cast<ConstantInfo_Utf8 *>(name_ptr->get());
    return name_info->bytes;
}

std::string jvm::Class::getSuperName() const {
    auto class_ptr = &constant_infos[super_class - 1];
    auto class_info = dynamic_cast<ConstantInfo_Class *>(class_ptr->get());
    auto name_ptr = &constant_infos[class_info->index - 1];
    auto name_info = dynamic_cast<ConstantInfo_Utf8 *>(name_ptr->get());
    return name_info->bytes;
}
