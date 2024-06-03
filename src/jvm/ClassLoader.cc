#include "ClassLoader.h"

#include <sese/io/File.h>
#include <sese/util/Exception.h>

std::unique_ptr<jvm::Class> jvm::ClassLoader::loadFromFile(const std::string &path) {
    auto file = sese::io::File::create(path, sese::io::File::B_READ);
    if (!file) throw sese::Exception("failed open class file");
    return std::make_unique<Class>(file.get());
}

std::unique_ptr<jvm::Class> jvm::ClassLoader::loadFromStream(sese::io::InputStream *input_stream) {
    return std::make_unique<Class>(input_stream);
}
