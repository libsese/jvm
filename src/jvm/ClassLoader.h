#pragma once

#include <jvm/Class.h>

namespace jvm {
    class ClassLoader {
    public:
        /// 尝试从文件种加载 class
        /// @param path class 文件路径
        /// @exception sese::Exception
        /// @return class 对象
        static std::shared_ptr<Class> loadFromFile(const std::string &path);

        /// 尝试从流中加载 class
        /// @param input_stream class 输入流
        /// @exception sese::Exception
        /// @return class 对象
        static std::shared_ptr<Class> loadFromStream(sese::io::InputStream *input_stream);
    };
}