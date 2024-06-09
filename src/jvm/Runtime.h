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
        constexpr static auto main_signature = "main([Ljava/lang/String;)V";

        struct Info {
            std::shared_ptr<Class> class_{};
            std::map<std::string, Class::MethodInfo>::iterator method;
            StackFrame data;
        } main;

        void run(Info &prev, Info &current);

        static struct MethodRefResult {
            std::string class_name;
            std::string method_id;
        } getMethodRefResult(const std::shared_ptr<Class> &class_, uint16_t index);


        std::unordered_map<std::string, std::shared_ptr<Class> > classes;
    };
}
