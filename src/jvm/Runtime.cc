#include "Opcode.h"
#include "Runtime.h"

#include <sese/Log.h>
#include <sese/util/Endian.h>
#include <sese/util/Exception.h>

void jvm::Runtime::regClass(const std::shared_ptr<Class> &class_) {
    classes[class_->getThisName()] = class_;
    if (main.class_ == nullptr) {
        auto iter = class_->method_infos.find(main_signature);
        if (iter != class_->method_infos.end()) {
            main.class_ = class_;
            main.method = iter;
        }
    }
}

bool jvm::Runtime::hasMain() const {
    return main.class_ != nullptr;
}

jvm::Runtime::MethodRefResult jvm::Runtime::getMethodRefResult(const std::shared_ptr<Class> &class_, uint16_t index) {
    auto method_ref = dynamic_cast<Class::ConstantInfo_MethodRef *>(class_->constant_infos[
        index].get());
    auto name_and_type = dynamic_cast<Class::ConstantInfo_NameAndType *>(class_->constant_infos[
        method_ref->name_and_type_index].get());
    auto method_name = dynamic_cast<Class::ConstantInfo_Utf8 *>(class_->constant_infos[name_and_type
        ->name_index].get())->bytes;
    auto method_type = dynamic_cast<Class::ConstantInfo_Utf8 *>(class_->constant_infos[name_and_type
        ->descriptor_index].get())->bytes;
    auto class_info = dynamic_cast<Class::ConstantInfo_Class *>(class_->constant_infos[method_ref->
        class_info_index].get());
    auto class_name = dynamic_cast<Class::ConstantInfo_Utf8 *>(class_->constant_infos[class_info->
        index].get())->bytes;
    return {class_name, method_name + method_type};
}

void jvm::Runtime::run() {
    auto &&main_method = main.method->second;
    auto &&code = main_method.code_info;
    main.data.locals.resize(code->max_locals);
    Info empty;
    run(empty, main);
}

void jvm::Runtime::run(Info &prev, Info &current) {
    auto &&code = current.method->second.code_info;
    for (size_t pc = 0; pc < code->code.size();) {
        auto op = static_cast<Opcode>(code->code[pc]);
        switch (op) {
            case iconst_1: {
                current.data.stacks.emplace(1);
                pc += 1;
                break;
            }
            case bipush: {
                uint8_t byte = code->code[pc + 1];
                current.data.stacks.emplace(byte);
                pc += 2;
                break;
            }
            case iload_0: {
                current.data.stacks.emplace(current.data.locals[0].getInt());
                pc += 1;
                break;
            }
            case iload_1: {
                current.data.stacks.emplace(current.data.locals[1].getInt());
                pc += 1;
                break;
            }
            case iload_2: {
                current.data.stacks.emplace(current.data.locals[2].getInt());
                pc += 1;
                break;
            }
            case istore_1: {
                current.data.locals[1] = sese::Value(current.data.stacks.top().getInt());
                pc += 1;
                break;
            }
            case istore_2: {
                current.data.locals[2] = sese::Value(current.data.stacks.top().getInt());
                pc += 1;
                break;
            }
            case istore_3: {
                current.data.locals[3] = sese::Value(current.data.stacks.top().getInt());
                pc += 1;
                break;
            }
            case iadd: {
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                i += current.data.stacks.top().getInt();
                current.data.stacks.pop();
                current.data.stacks.emplace(i);
                pc += 1;
                break;
            }
            case ireturn: {
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                prev.data.stacks.emplace(i);
                pc += 1;
                goto end;
            }
            case return_: {
                pc += 1;
                goto end;
            }
            case invokestatic: {
                uint16_t constant_index;
                memcpy(&constant_index, &code->code[pc + 1], 2);
                constant_index = FromBigEndian16(constant_index);
                auto result = getMethodRefResult(current.class_, constant_index);
                auto class_ = classes[result.class_name];
                auto method = class_->method_infos.find(result.method_id);
                auto &&c = method->second.code_info;
                Info info;
                info.class_ = class_;
                info.method = method;
                info.data.locals.resize(c->max_locals);
                for (int j = 0; j != c->max_locals; ++j) {
                    info.data.locals[j] = std::move(current.data.stacks.top());
                    current.data.stacks.pop();
                }
                run(current, info);
                pc += 3;
                break;
            }
            default:
                SESE_ERROR("opcode %d", op);
                throw sese::Exception("Unsupported opcode");
        }
    }
end:
    return;
}
