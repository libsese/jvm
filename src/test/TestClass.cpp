#include <gtest/gtest.h>
#include <jvm/Class.h>
#include <sese/util/Exception.h>

TEST(TestClass, Parse) {
    try {
        auto cl = jvm::Class("C:/Users/kaoru/Desktop/Hello.class");
        cl.parse();
    } catch (sese::Exception &exception) {
        exception.printStacktrace();
    }
}
