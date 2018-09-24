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
option(WITH_CRC32C "Build crc32c utility" OFF)
option(WITH_TESTS "Build tests." OFF)
option(WITH_DOCS "Don't generate docs" OFF)
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

set(SNSTRM_VERSION_MAJOR 1)
set(SNSTRM_VERSION_MINOR 0)
set(SNSTRM_VERSION_PATCH 0)

# configuration header
configure_file(
	"${PROJECT_SOURCE_DIR}/snappystream.cfg"
	"${PROJECT_SOURCE_DIR}/include/snappystreamcfg.hpp"
	ESCAPE_QUOTES
)

include_directories("${PROJECT_SOURCE_DIR}/include")

########################################################################

if (WITH_CONAN)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
	conan_set_find_paths()
	list(APPEND LIBRARIES ${CONAN_LIBS})
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

###########################################################################
# boost::iostreams

if (WITH_BOOST_IOSTREAMS)
	if(NOT BOOST_ROOT)
		set(BOOST_ROOT $ENV{BOOST_ROOT})
	endif()
	find_package(Boost 1.36.0 REQUIRED COMPONENTS iostreams system)
	include_directories(${Boost_INCLUDE_DIRS})
	list(APPEND LIBRARIES ${Boost_LIBRARIES})
endif()

###########################################################################
# snappy

find_package(Snappy REQUIRED)
include_directories(BEFORE ${SNAPPY_INCLUDE_DIR})
message(STATUS "Libs: ${SNAPPY_LIBRARIES}, include: ${SNAPPY_INCLUDE_DIR}")
list(APPEND LIBRARIES ${SNAPPY_LIBRARIES})

########################################################################

set(SRC ./include/osnappystream.hpp
        ./src/osnappystream.cpp
        ./include/isnappystream.hpp
        ./src/isnappystream.cpp
        ./src/crc32c.hpp
        ./include/snappystreamcfg.hpp
        ./src/snappystreamcfg.cpp
        ./src/endians.hpp)
nx_add_library(${PROJECT_NAME} ${SNSTRM_FORCE_SHARED_CRT} ${BUILD_STATIC_LIBS}
	${BUILD_SHARED_LIBS} "SNSTRM_SHARED" "ver" "${SRC}")

###########################################################################
# gprof

if( PROFILE )
	add_definitions(-pg)
	add_definitions(-g)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

###########################################################################
# docs and tests

if(WITH_DOCS)
	add_subdirectory(doc)
endif()

if(WITH_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

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
	target_link_libraries(snap snappystream ${LIBRARIES})
endif()

if(WITH_CRC32C)
	add_executable(crc32c ./misc/calc-crc32c.cpp)
	target_link_libraries(crc32c snappystream ${LIBRARIES})
endif()

