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
option(BUILD_TESTS "Build tests." OFF)
option(BUILD_GTEST "Build gtest with the project. Usefull for windows systems, to prevent DLL-HELL" ON)
option(GENERATE_DOC "Don't generate docs" ON)
option(PROFILE "Add gprof options -g and -pg" OFF)
option(WITH_BOOST_IOSTREAMS "Include boost::iostreams filters" OFF)
option(WITH_CONAN "Use conan to resolve dependencies" OFF)

###############################################################################
# general

project(snappystream)

include("${PROJECT_SOURCE_DIR}/cmake/ext/nx_utils.cmake")
nx_configure()

if(WITH_BOOST_IOSTREAMS)
	set(SNSTRM_WITH_BOOST_IOSTREAMS true)
endif()

set(SNSTRM_VERSION_MAJOR 0)
set(SNSTRM_VERSION_MINOR 2)
set(SNSTRM_VERSION_PATCH 2)

# configuration header
configure_file(
	"${PROJECT_SOURCE_DIR}/snappystream.cfg"
	"${PROJECT_SOURCE_DIR}/include/snappystreamcfg.hpp"
	ESCAPE_QUOTES
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")

########################################################################

if (WITH_CONAN)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
	list(APPEND LIBRARIES ${CONAN_LIBS})
endif()

###########################################################################
# boost::iostreams

if (WITH_BOOST_IOSTREAMS)
	if (NOT WITH_CONAN)
		if(NOT BOOST_ROOT)
			set(BOOST_ROOT $ENV{BOOST_ROOT})
		endif()
		find_package(Boost 1.36.0 REQUIRED COMPONENTS iostreams system)
		include_directories(${Boost_INCLUDE_DIRS})
		list(APPEND LIBRARIES ${Boost_LIBRARIES})
	endif()
endif()

###########################################################################
# snappy

if (NOT WITH_CONAN)
	find_package(Snappy REQUIRED)
	include_directories(${SNAPPY_INCLUDE_DIR})
	list(APPEND LIBRARIES ${SNAPPY_LIBRARIES})
endif()

########################################################################

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
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

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
	FIND_PACKAGE(Doxygen)
	if(DOXYGEN_FOUND)
		set(ENV{SNSTRM_ROOT} ${PROJECT_SOURCE_DIR})
		ADD_CUSTOM_TARGET(doc COMMAND ${CMAKE_COMMAND}
			-D PDIR="${PROJECT_SOURCE_DIR}"
			-P "${PROJECT_SOURCE_DIR}/doc/doxygen.cmake")
	else()
		message(WARNING "Doxygen not found. Documentation wouldn't be generated")
	endif()
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
		include(ExternalProject)
		ExternalProject_Add(
			gtestlib
			URL http://googletest.googlecode.com/files/gtest-1.7.0.zip
			#SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
			PREFIX "gtest"
			CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> -DCMAKE_PREFIX_PATH=<INSTALL_DIR>
			                                                -DBUILD_SHARED_LIBS=False
			INSTALL_COMMAND ""
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		ExternalProject_Get_Property(gtestlib BINARY_DIR)
		ExternalProject_Get_Property(gtestlib SOURCE_DIR)
		set(GTEST_INCLUDE_DIRS ${SOURCE_DIR}/include)
		set(GTEST_LIBRARIES ${BINARY_DIR}/libgtest.a)
	else()
		find_package(GTest REQUIRED)
	endif()
	enable_testing()
	include_directories(${GTEST_INCLUDE_DIRS})
	set(TEST test_${PROJECT_NAME})
	add_executable(${TEST} ${SOURCES_TEST})
	add_dependencies(${TEST} gtestlib)
	if(NOT WIN32)
		SET_TARGET_PROPERTIES(${TEST} PROPERTIES LINK_FLAGS -pthread)
	endif(NOT WIN32)
	target_link_libraries("${TEST}" ${GTEST_LIBRARIES} ${LIBRARIES})
	nx_GTEST_ADD_TESTS("${TEST}" ${SOURCES_TEST})
else( BUILD_TESTS )
	message(STATUS "Tests disabled. Use -DBUILD_TESTS=TRUE to enable it.")
endif( BUILD_TESTS )

###########################################################################
# install

set(SN_FILES
	./include/isnappystream.hpp
	./include/osnappystream.hpp
	./include/snappystreamcfg.hpp
	./include/snappystream.hpp
)
if (WITH_BOOST_IOSTREAMS)
	list(APPEND SN_FILES
		./include/OutputSnappyStreamBoostFilter.hpp
		./include/InputSnappyStreamBoostFilter.hpp)
endif()

install(FILES ${SN_FILES} DESTINATION include)
install_targets(/lib  snappystream)


###########################################################################
# snap

if(NOT WITHOUT_SNAP)
	add_executable(snap ./misc/snap.cpp)
	target_link_libraries(snap ${LIBRARIES} snappystream)
endif()

if(NOT WITHOUT_CRC32C)
	add_executable(crc32c ./misc/calc-crc32c.cpp)
	target_link_libraries(crc32c ${LIBRARIES} snappystream)
endif()

