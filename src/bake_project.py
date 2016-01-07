
collections = ConfigDict({
    "@execs": ["exec"],
    "@all": ["controller", "controller/sfml",
             "view", "view/sfml",
             "viewcontroller", "viewcontroller/sfml",
             "model", "model/entity",
             "util", "util/ccq", "util/stopwatch", "util/cmd",
             "exec", "game", "exceptions"]
})

extensions =  [".cpp", ".cc", ".hpp", ".hh", ".h", ".test"]

scanned_files =  ["*.cpp", "*.cc", "*.hpp", "*.hh", "*.h", "*.test"]

gcc_config = ConfigDict({
    "compiler": "g++",
    "mode": "g3",
    "std": "c++11",
    "pre_extra": "-I libs/sfml/include/ -I libs/yomm11/include",
    "post_extra": "-Wall -Wno-unused-value -pthread ",
    "input": ["*.cpp", "*.cc", "*.hpp", "*.h", "*.hh"]
})

maintenance_config = ConfigDict({
    "dirty_files": ["*.o", "*.tcpp", "*.to", "*.bin", "*.headers_baked"]
})

gtest_config = ConfigDict({
    "location": "libs/gtest",
    "static_libs": "libs/gtest/cmake_stuff",
    "template": """

// Automatically generated by Baker
#include "gtest/gtest.h"

{tests}

int main(int argc, char **argv) {{
    ::testing::InitGoogleTest(&argc, argv);
    // Get rid of the irritating threading warning when testing DEATH
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}}

"""
})
