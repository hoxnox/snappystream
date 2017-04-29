PROJECT(SnappyStreamTest)
cmake_minimum_required(VERSION 2.8.12)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
conan_set_find_paths()

ADD_EXECUTABLE(pkg_test test.cpp)
TARGET_LINK_LIBRARIES(pkg_test ${CONAN_LIBS})
