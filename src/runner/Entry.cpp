#include <sese/Init.h>
#include <sese/Log.h>
#include <sese/util/ArgParser.h>

#include <jvm/ClassLoader.h>
#include <jvm/Runtime.h>

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    auto args = sese::ArgParser();
    args.parse(argc, argv);

    auto class_path = args.getValueByKey("--class-path", "");
    if (class_path.empty()) {
        SESE_ERROR("require --class-path");
        return -1;
    }

    auto mode = args.getValueByKey("--mode", "run");
    if (mode != "run" && mode != "print") {
        SESE_ERROR("require --mode=(run|print)");
        return -1;
    }
    SESE_INFO("mode: %s", mode.c_str());

    auto cl = jvm::ClassLoader::loadFromFile(class_path);
    if (mode == "run") {
        jvm::Runtime runtime;
        runtime.regClass(cl);
        if (!runtime.hasMain()) {
            SESE_ERROR("cannot found main method in class %s", cl->getThisName().c_str());
            return -1;
        }
        runtime.run();
    } else if (mode == "print") {
        cl->printMethods();
        cl->printFields();
        cl->printAttributes();
    }
    return 0;
}
