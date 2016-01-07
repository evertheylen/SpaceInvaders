# CMake generated Testfile for 
# Source directory: /home/uauser/Downloads/SpaceInvaders-master/libs/yomm11
# Build directory: /home/uauser/Downloads/SpaceInvaders-master/libs/yomm11/cmake_stuff
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(tests "tests/tests")
ADD_TEST(order12 "tests/order12")
ADD_TEST(order21 "tests/order21")
ADD_TEST(asteroids "examples/asteroids")
ADD_TEST(next "examples/next")
ADD_TEST(foreign "examples/foreign")
ADD_TEST(three "examples/three")
SUBDIRS(src)
SUBDIRS(tests)
SUBDIRS(examples)
