#pragma once

#include <unordered_map>
#include <jvm/Class.h>

namespace jvm {
    class Runtime {
    public:
        void moveArgs(std::vector<std::string> &args);

        void regClass(std::unique_ptr<Class> class_);

        void run();

    private:
        void findMain();

        std::vector<std::string> args;
        std::unordered_map<std::string, std::unique_ptr<Class> > classes;
    };
}
