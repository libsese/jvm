#pragma once

#include <cstdint>

namespace jvm {
    enum AccessFlag : int32_t {
        public_ = 0x0001,
        private_ = 0x0002,
        protected_ = 0x0004,
        static_ = 0x0008,
        final_ = 0x0010,
        voilatie = 0x0040,
        transient = 0x0080,
        synthetic = 0x1000,
        enum_ = 0x40000,
        module_ = 0x8000,
    };

    struct AccessFlags {
        [[nodiscard]] bool isPublic() const {
            return public_ & access_flags;
        }

        [[nodiscard]] bool isPrivate() const {
            return private_ & access_flags;
        }

        [[nodiscard]] bool isProtected() const {
            return protected_ & access_flags;
        }

        [[nodiscard]] bool isStatic() const {
            return static_ & access_flags;
        }

        [[nodiscard]] bool isFinal() const {
            return final_ & access_flags;
        }

        [[nodiscard]] bool isVoilatie() const {
            return voilatie & access_flags;
        }

        [[nodiscard]] bool isTransient() const {
            return transient & access_flags;
        }

        [[nodiscard]] bool isSynthetic() const {
            return synthetic & access_flags;
        }

        [[nodiscard]] bool isEnum() const {
            return enum_ & access_flags;
        }

        [[nodiscard]] bool isModule() const {
            return module_ & access_flags;
        }

        uint16_t access_flags{};
    };
}
