#include "Class.h"

#include <sese/Log.h>
#include <sese/util/Exception.h>

std::string jvm::Class::getThisName() const {
    auto class_ptr = &constant_infos[this_class];
    auto class_info = dynamic_cast<ConstantInfo_Class *>(class_ptr->get());
    auto name_ptr = &constant_infos[class_info->index];
    auto name_info = dynamic_cast<ConstantInfo_Utf8 *>(name_ptr->get());
    return name_info->bytes;
}

std::string jvm::Class::getSuperName() const {
    auto class_ptr = &constant_infos[super_class];
    auto class_info = dynamic_cast<ConstantInfo_Class *>(class_ptr->get());
    auto name_ptr = &constant_infos[class_info->index];
    auto name_info = dynamic_cast<ConstantInfo_Utf8 *>(name_ptr->get());
    return name_info->bytes;
}
