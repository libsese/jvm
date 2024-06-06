#pragma once

#include <stack>
#include <unordered_map>
#include <jvm/Class.h>
#include <sese/util/Value.h>

namespace jvm {
    class Runtime {
    public:
        struct StackFrame {
            std::stack<sese::Value> stacks;
            std::vector<sese::Value> locals;
        };

        void regClass(const std::shared_ptr<Class> &class_);

        [[nodiscard]] bool hasMain() const;

        void run();

    private:
        struct Info {
            std::shared_ptr<Class> class_{};
            size_t methodIndex = -1;
            StackFrame data;
        } main;

        void run(Info &prev, Info &current);

        uint16_t pc = 0;
        std::stack<StackFrame> stacks;

        std::unordered_map<std::string, std::shared_ptr<Class> > classes;
    };
}
