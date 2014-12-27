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
#include <snappystreamcfg.hpp>
#include "tsnappystream.hpp"
#ifdef SNSTRM_WITH_BOOST_IOSTREAMS
#include "tboost_filters.hpp"
#endif
#include "tcrc32c.hpp"

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

