#include <gtest/gtest.h>
#include <jvm/ClassLoader.h>
#include <sese/Log.h>
#include <sese/util/Exception.h>

#define CLASS_FILE "D:/workspaces/jvm/src/test/resource/Hello.class"

TEST(TestClass, Parse) {
    try {
        auto cl = jvm::ClassLoader::loadFromFile(CLASS_FILE);
    } catch (sese::Exception &exception) {
        exception.printStacktrace();
    }
}

TEST(TestClass, Getter) {
    auto cl = jvm::ClassLoader::loadFromFile(CLASS_FILE);
    // SESE_DEBUG("%s %s", cl->getSuperName().c_str(), cl->getThisName().c_str());
    EXPECT_EQ(cl->getSuperName(), "java/lang/Object");
    EXPECT_EQ(cl->getThisName(), "Hello");
    cl->printMethods();
    cl->printFields();
    cl->printAttributes();
}
