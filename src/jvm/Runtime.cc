#include "Runtime.h"

void jvm::Runtime::moveArgs(std::vector<std::string> &args) {
    this->args = std::move(args);
}

void jvm::Runtime::regClass(std::unique_ptr<Class> class_) {
    classes[class_->getThisName()] = std::move(class_);
}

void jvm::Runtime::run() {
}

void jvm::Runtime::findMain() {
    for (auto &&[_, class_]: classes) {
    }
}

