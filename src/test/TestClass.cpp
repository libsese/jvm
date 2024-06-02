#include <gtest/gtest.h>
#include <jvm/Class.h>
#include <sese/Log.h>
#include <sese/util/Exception.h>

TEST(TestClass, Parse) {
    try {
        auto cl = jvm::Class("C:/Users/kaoru/Desktop/Hello.class");
        cl.parse();
    } catch (sese::Exception &exception) {
        exception.printStacktrace();
    }
}

TEST(TestClass, Getter) {
    auto cl = jvm::Class("C:/Users/kaoru/Desktop/Hello.class");
    cl.parse();
    SESE_DEBUG("%s %s", cl.getSuperName().c_str(), cl.getThisName().c_str());
    cl.printMethods();
    cl.printFields();
}
