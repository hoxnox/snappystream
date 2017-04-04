# @author hoxnox <hoxnox@gmail.com>
# @date 20170404 18:08:36

include(gtest)
add_executable(test_unit_${PROJECT_NAME} test.cpp)
add_dependencies(test_unit_${PROJECT_NAME} gtestlib snappystream)
target_link_libraries(test_unit_${PROJECT_NAME} snappystream ${LIBRARIES} ${GTEST_LIBRARIES})
target_include_directories(test_unit_${PROJECT_NAME} BEFORE PRIVATE "${GTEST_INCLUDE_DIRS}")
target_include_directories(test_unit_${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")

