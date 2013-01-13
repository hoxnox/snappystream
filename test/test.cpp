/**@file test/test.cpp
 * @author Merder Kim <hoxnox@gmail.com>
 * @date $date$
 * @copyright (c) 2013 Merder Kim <hoxnox@gmail.com>
 * @license BSD
 *
 * @brief Test launcher.*/

// Google Testing Framework
#include <gtest/gtest.h>

// test cases
#include "tsnappy-stream.hpp"

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

