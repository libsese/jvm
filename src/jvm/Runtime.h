#pragma once

#include <unordered_map>
#include <jvm/Class.h>

namespace jvm {
    class Runtime {
    public:
        void regClass(const std::shared_ptr<Class> &class_);

        [[nodiscard]] bool hasMain() const;

        void run();

    private:
        struct MainInfo {
            size_t methodIndex = -1;
            std::shared_ptr<Class> class_{};
        } main;

        std::unordered_map<std::string, std::shared_ptr<Class> > classes;
    };
}
