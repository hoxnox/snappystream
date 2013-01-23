# @author  Merder Kim <hoxnox@gmail.com>
# SnappyStream cmake build script
# $date

cmake_minimum_required(VERSION 2.6)

###############################################################################
# options

# BUILD_SHARED_LIBS is a standard CMake variable, but we declare it here to
# make it prominent in the GUI.
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)
option(BUILD_STATIC_LIBS "Build static libraries" ON)
# When other libraries are using a shared version of runtime libraries,
# enty also has to use one. Note that, if BUILD_SHARED_LIBS=TRUE on windows system
# ENTY_FORCE_SHARED_CRT always be ON, because of safety of DLL building (we are using STL in API and
# do not want memory problems with two runtime libs works together (see DLL HELL problem)).
option(SNSTRM_FORCE_SHARED_CRT
	"Use shared (DLL) run-time lib even when enty is built as static lib." OFF)
option(WITHOUT_CRC32C "Don't build crc32c utility" ON)
option(BUILD_TESTS "Build enty tests." OFF)
option(BUILD_GTEST "Build gtest with the project. Usefull for windows systems, to prevent DLL-HELL" OFF)
option(GENERATE_DOC "Don't generate docs" ON)
option(PROFILE "Add gprof options -g and -pg" OFF)

###############################################################################
# general

project(snappystream)

include("${PROJECT_SOURCE_DIR}/cmake/ext/nx_utils.cmake")
nx_configure()

set(SNSTRM_VERSION_MAJOR 0)
set(SNSTRM_VERSION_MINOR 1)
# configuration header
configure_file(
	"${PROJECT_SOURCE_DIR}/snappystream.cfg"
	"${PROJECT_SOURCE_DIR}/include/snappystreamcfg.hpp"
	ESCAPE_QUOTES
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")

find_package(Snappy REQUIRED)
include_directories(${SNAPPY_INCLUDE_DIR})

set(SRC ./include/osnappystream.hpp
        ./src/osnappystream.cpp
        ./include/isnappystream.hpp
        ./src/isnappystream.cpp
        ./src/crc32c.hpp
        ./include/snappystreamcfg.hpp
        ./src/snappystreamcfg.cpp
        ./src/endians.hpp)
nx_add_library(${PROJECT_NAME} ${SNSTRM_FORCE_SHARED_CRT} ${BUILD_STATIC_LIBS} ${BUILD_SHARED_LIBS}
	"SNSTRM_SHARED" "ver" "${SRC}")
target_link_libraries(${PROJECT_NAME} ${SNAPPY_LIBRARIES})
set(LIBRARIES ${PROJECT_NAME})

###########################################################################
# gprof

if( PROFILE )
	add_definitions(-pg)
	add_definitions(-g)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

###########################################################################
# docs

if(GENERATE_DOC)
	FIND_PACKAGE(Doxygen REQUIRED)
	set(ENV{SNSTRM_ROOT} ${PROJECT_SOURCE_DIR})
	ADD_CUSTOM_TARGET(doc COMMAND ${CMAKE_COMMAND} 
		-D PDIR="${PROJECT_SOURCE_DIR}"
		-P "${PROJECT_SOURCE_DIR}/doc/doxygen.cmake")
endif()

##########################################################################
# tests

if( BUILD_TESTS )
	set(SOURCES_TEST
		./include/osnappystream.hpp
		./src/osnappystream.cpp
		./include/isnappystream.hpp
		./src/isnappystream.cpp
		./src/snappystreamcfg.cpp
		./src/crc32c.hpp
		./test/test.cpp
		./test/tsnappystream.hpp
		./test/tcrc32c.hpp
	)
	if(BUILD_GTEST)
		#TODO: Find and check GTEST_SOURCES
		set(GTEST_SOURCES "${PROJECT_SOURCE_DIR}/../gtest")
		enable_testing()
		set(TEST test_${PROJECT_NAME})
				include_directories("${GTEST_SOURCES}/src")
		add_executable(${TEST} ${SOURCES_TEST})
		target_link_libraries("${TEST}" gtest ${SNAPPY_LIBRARIES})
		nx_GTEST_ADD_TESTS("${TEST}" ${SOURCES_TEST})
	else(BUILD_GTEST)
		enable_testing()
		find_package(GTest REQUIRED)
		include_directories(${GTEST_INCLUDE_DIRS})
		set(TEST test_${PROJECT_NAME})
		add_executable(${TEST} ${SOURCES_TEST})
		if(NOT WIN32)
			SET_TARGET_PROPERTIES(${TEST} PROPERTIES LINK_FLAGS -pthread)
		endif(NOT WIN32)
		target_link_libraries("${TEST}" ${GTEST_LIBRARIES} ${SNAPPY_LIBRARIES})
		nx_GTEST_ADD_TESTS("${TEST}" ${SOURCES_TEST})
	endif(BUILD_GTEST)
else( BUILD_TESTS )
	message(STATUS "Tests disabled. Use -DBUILD_TESTS=TRUE to enable it.")
endif( BUILD_TESTS )

###########################################################################
# install

install(FILES ./include/isnappystream.hpp
              ./include/osnappystream.hpp
              ./include/snappystreamcfg.hpp
              ./include/snappystream.hpp
        DESTINATION include)
install_targets(/lib  snappystream)


###########################################################################
# snap

if(NOT WITHOUT_SNAP)
	add_executable(snap ./misc/snap.cpp)
	target_link_libraries(snap ${SNAPPY_LIBRARIES} snappystream)
endif()

if(NOT WITHOUT_CRC32C)
	add_executable(crc32c ./misc/calc-crc32c.cpp)
	target_link_libraries(crc32c ${SNAPPY_LIBRARIES} snappystream)
endif()
