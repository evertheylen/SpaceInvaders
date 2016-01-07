# CMake generated Testfile for 
# Source directory: /Bestanden/Copy/root/Bestanden/School/ADV_PROG/space_invaders/libs/yomm11
# Build directory: /Bestanden/Copy/root/Bestanden/School/ADV_PROG/space_invaders/libs/yomm11
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(tests "tests/tests")
add_test(order12 "tests/order12")
add_test(order21 "tests/order21")
add_test(asteroids "examples/asteroids")
add_test(next "examples/next")
add_test(foreign "examples/foreign")
add_test(three "examples/three")
subdirs(src)
subdirs(tests)
subdirs(examples)
