#include <gtest/gtest.h>
#include <jvm/Runtime.h>
#include <jvm/ClassLoader.h>
#include <sese/Log.h>
#include <sese/util/Exception.h>


TEST(TestRuntime, FindMain) {
    auto class_ = jvm::ClassLoader::loadFromFile(PATH_TO_WORLD_CLASS);
    auto runtime = jvm::Runtime();
    runtime.regClass(class_);
    EXPECT_TRUE(runtime.hasMain());
}

TEST(TestRuntime, Run_World) {
    auto class_ = jvm::ClassLoader::loadFromFile(PATH_TO_WORLD_CLASS);
    auto runtime = jvm::Runtime();
    runtime.regClass(class_);
    try {
        runtime.run();
    } catch (sese::Exception &ex) {
        ex.printStacktrace();
    }
}

TEST(TestRuntime, Run_PrimeCalculator) {
    auto class_ = jvm::ClassLoader::loadFromFile(PATH_TO_PRIME_CALCULATOR_CLASS);
    auto runtime = jvm::Runtime();
    runtime.regClass(class_);
    try {
        runtime.run();
    } catch (sese::Exception &ex) {
        ex.printStacktrace();
    }
}
