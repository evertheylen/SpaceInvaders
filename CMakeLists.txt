cmake_minimum_required(VERSION 2.6)
project(SpaceInvaders)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# Extend the CMake module path to find the FindSFML.cmake
# file in {project root}/cmake/Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

# -------------------------------------------
# The source files to compile
file(GLOB_RECURSE SRC  src/*.cpp src/*.hpp)
file(GLOB_RECURSE RES  src/data/*)

# The program we're compiling with the sources set above
add_executable(exec ${SRC} ${PROJECT_SOURCE_DIR}/src/exec/main.cpp)

install(TARGETS exec DESTINATION ${PROJECT_SOURCE_DIR}/bin)

# -------------------------------------------
# Deps

# Add include dirs for SFML
find_package(SFML 2 COMPONENTS system window graphics)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(exec ${SFML_LIBRARIES})

# YOMM
add_subdirectory(${PROJECT_SOURCE_DIR}/libs/yomm11)
include_directories(${PROJECT_SOURCE_DIR}/libs/yomm11/include)
target_link_libraries(exec ${PROJECT_SOURCE_DIR}/libs/yomm11/src/libyomm11.a)


# TinyXML
file(GLOB_RECURSE LIBS libs/tinyxml/*.cpp libs/tinyxml/*.h)

# PicoJSON (header only, so)
include_directories(${PROJECT_SOURCE_DIR}/libs)

# More include stuff
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/)

# Extra deps:

# X11
find_package(X11)
target_link_libraries(exec ${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

# pthread (or another thread implementation?)
find_package(Threads)
target_link_libraries(exec ${CMAKE_THREAD_LIBS_INIT})

