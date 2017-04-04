# @author hoxnox <hoxnox@gmail.com>
# @date 20170322 16:08:27

function(add_gtests gtname sources)
	foreach(source ${sources})
		file(READ "${source}" contents)
		string(REGEX MATCHALL "TEST_?F?\\(([A-Za-z_0-9 ,]+)\\)"
			found_tests ${contents})
		foreach(hit ${found_tests})
			string(REGEX REPLACE
			       ".*\\( *([A-Za-z_0-9]+), *([A-Za-z_0-9]+) *\\).*" "\\1.\\2"
			       test_name ${hit})
			add_test("${gtname}-${test_name}"
			         test_${gtname}_${PROJECT_NAME}
			         --gtest_filter=${test_name})
		endforeach()
	endforeach()
endfunction()

if (NOT TARGET gtestlib)
	include(Vendoring)
	if (WITH_SYSTEM_GTEST)
		if(NOT GTEST_ROOT)
			set(GTEST_ROOT $ENV{GTEST_ROOT})
		endif()
		find_package(GTest REQUIRED)
		add_custom_target(gtestlib)
	else()
		if (NOT STAGING_DIR)
			set(STAGING_DIR ${CMAKE_BINARY_DIR}/staging)
		endif()
		sources_url(GTEST
			"google/googletest/googletest-release-1.8.0.tar.gz"
			"https://github.com/google/googletest/archive/release-1.8.0.tar.gz")
		include(ExternalProject)
		ExternalProject_Add(gtestlib
			URL ${GTEST_URL}
			DOWNLOAD_NAME googletest-release-1.8.0.tar.gz
			URL_HASH SHA256=58a6f4277ca2bc8565222b3bbd58a177609e9c488e8a72649359ba51450db7d8 
			#PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest"
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
				-DBUILD_SHARED_LIBS=False
				-DCMAKE_CXX_COMPILER:STRING='${CMAKE_CXX_COMPILER}'
				-DCMAKE_CXX_FLAGS:STRING='${CMAKE_CXX_FLAGS}'
			INSTALL_DIR "${STAGING_DIR}"
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		ExternalProject_Get_Property(gtestlib BINARY_DIR)
		ExternalProject_Get_Property(gtestlib SOURCE_DIR)
		set(GTEST_INCLUDE_DIRS ${STAGING_DIR}/include)
		set(GTEST_LIBRARIES ${STAGING_DIR}/lib/libgtest_main.a ${STAGING_DIR}/lib/libgtest.a)
	endif()
else()
	if ((DEFINED STAGING_DIR) AND (NOT WITH_SYSTEM_GTEST))
		set(GTEST_ROOT ${STAGING_DIR})
		find_package(GTest REQUIRED)
	endif()
endif()

find_package(Threads)
set(GTEST_LIBRARIES ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

