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

#pragma region 字节码逻辑实现

#define lconst_N(i) \
    case lconst_##i: { \
        current.data.stacks.emplace(i); \
        pc += 1; \
        break; \
    }

#define iconst_N(i) \
    case iconst_##i: { \
        current.data.stacks.emplace(i); \
        pc += 1; \
        break; \
    }

#define fconst_N(i) \
    case fconst_##i: { \
        current.data.stacks.emplace(i##.0);; \
        pc += 1; \
        break; \
    }

#define dconst_N(i) \
    case dconst_##i: { \
        current.data.stacks.emplace(i##.0);; \
        pc += 1; \
        break; \
    }

#define iload_N(i) \
    case iload_##i: { \
        current.data.stacks.emplace(current.data.locals[i].getInt()); \
        pc += 1; \
        break; \
    }

#define lload_N(i) \
    case lload_##i: { \
        current.data.stacks.emplace(current.data.locals[i].getInt()); \
        pc += 1; \
        break; \
    }

#define fload_N(i) \
    case fload_##i: { \
        current.data.stacks.emplace(current.data.locals[i].getDouble()); \
        pc += 1; \
        break; \
    }

#define dload_N(i) \
    case dload_##i: { \
        current.data.stacks.emplace(current.data.locals[i].getDouble()); \
        pc += 1; \
        break; \
    }

#define istore_N(i) \
    case istore_##i: { \
        current.data.locals[i] = sese::Value(current.data.stacks.top().getInt()); \
        current.data.stacks.pop(); \
        pc += 1; \
        break; \
    }

#define lstore_N(i) \
    case lstore_##i: { \
        current.data.locals[i] = sese::Value(current.data.stacks.top().getInt()); \
        current.data.stacks.pop(); \
        pc += 1; \
        break; \
    }

#define fstore_N(i) \
    case fstore_##i: { \
        current.data.locals[i] = sese::Value(current.data.stacks.top().getDouble()); \
        current.data.stacks.pop(); \
        pc += 1; \
        break; \
    }

#define dstore_N(i) \
    case dstore_##i: { \
        current.data.locals[i] = sese::Value(current.data.stacks.top().getDouble()); \
        current.data.stacks.pop(); \
        pc += 1; \
        break; \
    }

void jvm::Runtime::run(Info &prev, Info &current) {
    // SESE_INFO("call %s.%s", current.class_->getThisName().c_str(), current.method->first.c_str());
    auto &&code = current.method->second.code_info;
    for (size_t pc = 0; pc < code->code.size();) {
        auto op = static_cast<Opcode>(code->code[pc]);
        switch (op) {
            case nop: {
                pc += 1;
                break;
            }
            case aconst_null: {
                current.data.stacks.emplace();
                pc += 1;
                break;
            }
            case iconst_m1: {
                current.data.stacks.emplace(-1);
                pc += 1;
                break;
            }
            iconst_N(0)
            iconst_N(1)
            iconst_N(2)
            iconst_N(3)
            iconst_N(4)
            iconst_N(5)
            lconst_N(0)
            lconst_N(1)
            fconst_N(0)
            fconst_N(1)
            fconst_N(2)
            dconst_N(0)
            dconst_N(1)
            case bipush: {
                uint8_t byte = code->code[pc + 1];
                current.data.stacks.emplace(byte);
                pc += 2;
                break;
            }
            case sipush: {
                uint16_t bytes;
                memcpy(&bytes, &code->code[pc + 1], 2);
                bytes = FromBigEndian16(bytes);
                current.data.stacks.emplace(bytes);
                pc += 3;
                break;
            }
            case ldc: {
                uint8_t index = code->code[pc + 1];
                auto i = current.class_->constant_infos[index]->tag;
                if (i == Class::integer_info) {
                    auto wrapper =
                            dynamic_cast<Class::ConstantInfo_Integer *>(current.class_->constant_infos[index].get());
                    current.data.stacks.emplace(wrapper->bytes);
                } else if (i == Class::float_info) {
                    auto wrapper =
                            dynamic_cast<Class::ConstantInfo_Float *>(current.class_->constant_infos[index].get());
                    current.data.stacks.emplace(wrapper->bytes);
                } else if (i == Class::string_info) {
                    // todo 字符串暂时未处理
                    throw sese::Exception("ldc received arguments of an illegal type");
                } else {
                    throw sese::Exception("ldc received arguments of an illegal type");
                }
                pc += 2;
                break;
            }
            case ldc_w: {
                uint16_t index;
                memcpy(&index, &code->code[pc + 1], 2);
                index = FromBigEndian16(index);
                auto i = current.class_->constant_infos[index]->tag;
                if (i == Class::integer_info) {
                    auto wrapper =
                            dynamic_cast<Class::ConstantInfo_Integer *>(current.class_->constant_infos[index].get());
                    current.data.stacks.emplace(wrapper->bytes);
                } else if (i == Class::float_info) {
                    auto wrapper =
                            dynamic_cast<Class::ConstantInfo_Float *>(current.class_->constant_infos[index].get());
                    current.data.stacks.emplace(wrapper->bytes);
                } else if (i == Class::string_info) {
                    // todo 字符串暂时未处理
                    throw sese::Exception("ldc received arguments of an illegal type");
                } else {
                    throw sese::Exception("ldc received arguments of an illegal type");
                }
                pc += 3;
                break;
            }
            case ldc2_w: {
                uint16_t index;
                memcpy(&index, &code->code[pc + 1], 2);
                index = FromBigEndian16(index);
                auto i = current.class_->constant_infos[index]->tag;
                if (i == Class::long_info) {
                    auto wrapper =
                            dynamic_cast<Class::ConstantInfo_Long *>(current.class_->constant_infos[index].get());
                    current.data.stacks.emplace(static_cast<int>(wrapper->bytes));
                } else if (i == Class::double_info) {
                    auto wrapper =
                            dynamic_cast<Class::ConstantInfo_Double *>(current.class_->constant_infos[index].get());
                    current.data.stacks.emplace(wrapper->bytes);
                } else {
                    throw sese::Exception("ldc_w received arguments of an illegal type");
                }
                pc += 3;
                break;
                break;
            }
            case iload: {
                uint8_t index = code->code[pc + 1];
                current.data.stacks.emplace(current.data.locals[index].getInt());
                pc += 2;
                break;
            }
            iload_N(0)
            iload_N(1)
            iload_N(2)
            iload_N(3)
            case lload: {
                uint8_t index = code->code[pc + 1];
                current.data.stacks.emplace(current.data.locals[index].getInt());
                pc += 2;
                break;
            }
            lload_N(0)
            lload_N(1)
            lload_N(2)
            lload_N(3)
            case fload: {
                uint8_t index = code->code[pc + 1];
                current.data.stacks.emplace(current.data.locals[index].getDouble());
                pc += 2;
                break;
            }
            fload_N(0)
            fload_N(1)
            fload_N(2)
            fload_N(3)
            case dload: {
                uint8_t index = code->code[pc + 1];
                current.data.stacks.emplace(current.data.locals[index].getDouble());
                pc += 2;
                break;
            }
            dload_N(0)
            dload_N(1)
            dload_N(2)
            dload_N(3)
            // todo aload
            case istore: {
                uint8_t index = code->code[pc + 1];
                current.data.locals[index] = sese::Value(current.data.stacks.top().getInt());
                current.data.stacks.pop();
                pc += 2;
                break;
            }
            istore_N(0)
            istore_N(1)
            istore_N(2)
            istore_N(3)
            case lstore: {
                uint8_t index = code->code[pc + 1];
                current.data.locals[index] = sese::Value(current.data.stacks.top().getInt());
                current.data.stacks.pop();
                pc += 2;
                break;
            }
            lstore_N(0)
            lstore_N(1)
            lstore_N(2)
            lstore_N(3)
            case fstore: {
                uint8_t index = code->code[pc + 1];
                current.data.locals[index] = sese::Value(current.data.stacks.top().getDouble());
                current.data.stacks.pop();
                pc += 2;
                break;
            }
            fstore_N(0)
            fstore_N(1)
            fstore_N(2)
            fstore_N(3)
            case dstore: {
                uint8_t index = code->code[pc + 1];
                current.data.locals[index] = sese::Value(current.data.stacks.top().getDouble());
                current.data.stacks.pop();
                pc += 2;
                break;
            }
            dstore_N(0)
            dstore_N(1)
            dstore_N(2)
            dstore_N(3)
            // todo astore
            // todo stack 相关指令暂时未实现，因为没有区分 int/long 和 float/double 类型
            case ladd:
            case iadd: {
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                i += current.data.stacks.top().getInt();
                current.data.stacks.pop();
                current.data.stacks.emplace(i);
                pc += 1;
                break;
            }
            case dadd:
            case fadd: {
                auto i = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                i += current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                current.data.stacks.emplace(i);
                pc += 1;
                break;
            }
            case lsub:
            case isub: {
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                current.data.stacks.emplace(value1 - value2);
                pc += 1;
                break;
            }
            case dsub:
            case fsub: {
                auto value2 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                current.data.stacks.emplace(value1 - value2);
                pc += 1;
                break;
            }
            case lmul:
            case imul: {
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                i *= current.data.stacks.top().getInt();
                current.data.stacks.pop();
                current.data.stacks.emplace(i);
                pc += 1;
                break;
            }
            case dmul:
            case fmul: {
                auto i = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                i *= current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                current.data.stacks.emplace(i);
                pc += 1;
                break;
            }
            case ldiv:
            case idiv: {
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                current.data.stacks.emplace(value1 / value2);
                pc += 1;
                break;
            }
            case ddiv:
            case fdiv: {
                auto value2 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                current.data.stacks.emplace(value1 / value2);
                pc += 1;
                break;
            }
            case lrem:
            case irem: {
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto result = value1 % value2;
                current.data.stacks.emplace(result);
                pc += 1;
                break;
            }
            case drem:
            case frem: {
                auto value2 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                auto result = std::fmod(value1, value2);
                current.data.stacks.emplace(result);
                pc += 1;
                break;
            }
            case lneg:
            case ineg: {
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                i = 0 - i;
                current.data.stacks.emplace(i);
                pc += 1;
                break;
            }
            case dneg:
            case fneg: {
                auto i = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                i = 0.0 - i;
                current.data.stacks.emplace(i);
                pc += 1;
                break;
            }
            // todo shl 相关指令暂时未实现，因为没有区分 int/long 和 float/double 类型
            case i2d: {
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto d = static_cast<double>(i);
                current.data.stacks.emplace(d);
                pc += 1;
                break;
            }
            case iinc: {
                uint8_t index, ii;
                memcpy(&index, &code->code[pc + 1], 1);
                memcpy(&ii, &code->code[pc + 2], 1);
                auto i = current.data.locals[index].getInt() + ii;
                current.data.locals[index] = sese::Value(i);
                pc += 3;
                break;
            }
            // todo i2l 相关指令暂时未实现，因为没有区分 int/long 和 float/double 类型
            case lcmp: {
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto i = (value1 < value2) ? -1 : ((value1 == value2) ? 0 : 1);
                current.data.stacks.emplace(i);
                pc += 1;
                break;
            }
            case dcmpl:
            case fcmpl: {
                auto value1 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                auto value2 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                if (std::isnan(value1) || std::isnan(value2)) {
                    current.data.stacks.emplace(-1);
                } else {
                    auto i = (value1 < value2) ? -1 : ((value1 == value2) ? 0 : 1);
                    current.data.stacks.emplace(i);
                }
                pc += 1;
                break;
            }
            case dcmpg:
            case fcmpg: {
                auto value1 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                auto value2 = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                if (std::isnan(value1) || std::isnan(value2)) {
                    current.data.stacks.emplace(1);
                } else {
                    auto i = (value1 < value2) ? -1 : ((value1 == value2) ? 0 : 1);
                    current.data.stacks.emplace(i);
                }
                pc += 1;
                break;
            }
            case ifeq: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (i == 0) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case ifne: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (i != 0) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case iflt: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (i < 0) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case ifge: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (i >= 0) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case ifgt: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (i > 0) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case ifle: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (i <= 0) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case if_icmpeq: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (value1 == value2) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case if_icmpne: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (value1 != value2) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case if_icmplt: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (value1 < value2) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case if_icmpge: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (value1 >= value2) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case if_icmpgt: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (value1 > value2) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            case if_icmple: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                auto value2 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                auto value1 = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                if (value1 <= value2) {
                    pc += pos;
                } else {
                    pc += 3;
                }
                break;
            }
            // todo if_acmpeq
            case goto_: {
                int16_t pos;
                memcpy(&pos, &code->code[pc + 1], 2);
                pos = FromBigEndian16(pos);
                pc += pos;
                break;
            }
            // todo jsr
            case lreturn:
            case ireturn: {
                auto i = current.data.stacks.top().getInt();
                current.data.stacks.pop();
                prev.data.stacks.emplace(i);
                pc += 1;
                SESE_INFO("exit %s.%s with return value %d",
                          current.class_->getThisName().c_str(),
                          current.method->first.c_str(),
                          i);
                goto end;
            }
            case dreturn:
            case freturn: {
                auto i = current.data.stacks.top().getDouble();
                current.data.stacks.pop();
                prev.data.stacks.emplace(i);
                pc += 1;
                SESE_INFO("exit %s.%s with return value %le",
                          current.class_->getThisName().c_str(),
                          current.method->first.c_str(),
                          i);
                goto end;
            }
            // todo areturn
            case return_: {
                pc += 1;
                SESE_INFO("exit %s.%s", current.class_->getThisName().c_str(), current.method->first.c_str());
                goto end;
            }
            // todo 178 ... 195 大概率不会去实现的指令，多态等相关
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
                for (int j = 0; j < method->second.args_type.size(); ++j) {
                    info.data.locals[j] = std::move(current.data.stacks.top());
                    current.data.stacks.pop();
                }
                run(current, info);
                pc += 3;
                break;
            }
            // todo 部分跳转和宽索引指令
            default:
                SESE_ERROR("opcode %d", op);
                throw sese::Exception("Unsupported opcode");
        }
    }
end:
    return;
}

#pragma endregion
