#include <gtest/gtest.h>
#include <jvm/ClassLoader.h>
#include <sese/Log.h>
#include <sese/util/Exception.h>

TEST(TestClass, Parse) {
    try {
        auto cl = jvm::ClassLoader::loadFromFile(PATH_TO_HELLO_CLASS);
    } catch (sese::Exception &exception) {
        exception.printStacktrace();
    }
}

TEST(TestClass, Getter) {
    auto cl = jvm::ClassLoader::loadFromFile(PATH_TO_HELLO_CLASS);
    // SESE_DEBUG("%s %s", cl->getSuperName().c_str(), cl->getThisName().c_str());
    EXPECT_EQ(cl->getSuperName(), "java/lang/Object");
    EXPECT_EQ(cl->getThisName(), "Hello");
    cl->printMethods();
    cl->printFields();
    cl->printAttributes();
}
