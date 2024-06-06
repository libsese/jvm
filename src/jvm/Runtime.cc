#include "Runtime.h"

void jvm::Runtime::regClass(const std::shared_ptr<Class> &class_) {
    classes[class_->getThisName()] = class_;
    if (main.class_ == nullptr) {
        for (int i = 0; i < class_->method_infos.size(); ++i) {
            auto &&method = class_->method_infos[i];
            if (method.name == "main" &&
                method.args_type.size() == 1 &&
                method.isStatic() &&
                method.isPublic()) {
                main.class_ = class_;
                main.methodIndex = i;
            }
        }
    }
}

bool jvm::Runtime::hasMain() const {
    return main.class_ != nullptr;
}

void jvm::Runtime::run() {
}
