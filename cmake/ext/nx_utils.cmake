##
# Configures cmake to hoxnox's wishes =)
# 1. Sets only 2 configurations: Debug and Release
# 2. Sets install_prefixes into lib and bin
function(nx_configure)
	# only 2 configurations: Debug and Release
	if(CMAKE_CONFIGURATION_TYPES)
		set(CMAKE_CONFIGURATION_TYPES Debug Release)
		set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
		    "Reset the configurations to what we need"
		    FORCE)
	endif()
	# setup install_prefixes
	set(LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
	set(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
	set(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin")
endfunction()

##
# @param libname - Library name main part
# @param runtime_type - MT|MD (only windows meaningful)
# @param type STATIC|SHARED|BOTH - build type
# @param libsources - sources
#
# Adds library (if BUILD_SHARED_LIBS set to TRUE, two libraries, name of the shared library project
# will be libname-shared). OUTPUT_NAME conventions:
#    Release, static-runtime, static: <libname>
#    Debug, static_runtime, static: <libname>d
#    DLL, static-runtime: <libname>-<ver.major>.<ver.minor>-<compiler>.dll
#    Debug DLL: <libname>-<ver.major>.<ver.minor>-<compiler>d.dll
#    Release, static-runtime, dynamic: <libname>s
#    Debug, static-runtime, dynamic: <libname>sd
#    Release, dynamic-runtime, static: <libname>-md
#    Debug, dynamic_runtime, static: <libname>-mdd
#    DLL, dynamic-runtime: <libname>-<ver.major>.<ver.minor>-<compiler>-md.dll
#    Debug DLL: <libname>-<ver.major>.<ver.minor>-<compiler>-mdd.dll
#    Release, dynamic-runtime, dynamic: <libname>-mds
#    Debug, dynamic-runtime, dynamic: <libname>-mdsd
#
#    Version info is taken from VER_SUFFIX. Compiler info is taken from
#    DetermineVSServicePack macro on msvc system and CMAKE_CXX_COMPILER_ID on others.
#
# Если собирается разделяемая библиотека, где-то в заголовочном файле присутствуют строки
# наподобие:
# @code
# #if defined(ENTY_SHARED) && (defined(_WIN32) || defined(WIN32))
# #	if defined(${PROJECT_EXPORTS})
# #		define ENTY_API __declspec(dllexport)
# #	else
# #		define ENTY_API __declspec(dllimport)
# #	endif
# #else
# #	define ENTY_API
# #endif
# @endcode
# Для того, чтобы функция могла корректно определять для разделяемой или обычной библиотеки
# настраиваются исходники, ей необходимо знать название "главного" #define. Из примера - это
# ENTY_SHARED. Кроме того, функция устанавливает значение переменной PROJECT_EXPORTS, поэтому
# надо использовать именно её!
#
# Функция добавляет к переменной INSTALLABLE_TARGETS значения установленных целей. Её можно
# использовать для инсталляции.
#
# EXAMPLE:
# nx_add_library("enty" "MT" "BOTH" "ENTY_SHARED" ${SOURCES})
# will add 2 library targets - enty and enty-shared, witch can build (for example on Windows and
# vs-2010): enty.lib, entyd.lib, entys.lib, entysd.lib and enty-0.2-vs100.dll, enty-0.2-vs100d.dll
function(nx_add_library libname runtime_shared static shared shared_def
		VER_SUFFIX libsources)
if(NOT runtime_shared)
	if(MSVC)
		set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1" PARENT_SCOPE)
		set(CMAKE_C_FLAGS_RELEASE "/MT /O2 /Ob2 /D NDEBUG" PARENT_SCOPE)
		set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1" PARENT_SCOPE)
		set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /D NDEBUG" PARENT_SCOPE)
	else(MSVC)
		if(WIN32)
			set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -g -O0" PARENT_SCOPE)
			set(CMAKE_C_FLAGS_RELEASE "-mtune=native -mwindows -O2 -DNDEBUG" PARENT_SCOPE)
			set(CMAKE_CXX_FLAGS_RELEASE "-mtune=native -mwindows -O2 -DNDEBUG" PARENT_SCOPE)
			set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g -O0" PARENT_SCOPE)
			set(CMAKE_EXE_LINKER_FLAGS_DEBUG  "-static" PARENT_SCOPE)
			set(CMAKE_EXE_LINKER_FLAGS_RELEASE  "-s -static" PARENT_SCOPE)
		endif()
	endif()
endif()
if(static)
	add_library("${libname}" STATIC ${libsources})
	list(APPEND INSTALLABLE_TARGETS "${libname}")
	if(WIN32 AND runtime_shared)
		set_target_properties("${libname}" PROPERTIES ARCHIVE_OUTPUT_NAME_RELEASE
			"${libname}-md")
		set_target_properties("${libname}" PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG
			"${libname}-mdd")
	else()
		set_target_properties("${libname}" PROPERTIES ARCHIVE_OUTPUT_NAME_RELEASE
			"${libname}")
		set_target_properties("${libname}" PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG
			"${libname}d")
	endif()
endif()
if(shared)
	add_library("${libname}-shared" SHARED ${libsources})
	list(APPEND INSTALLABLE_TARGETS "${libname}-shared")
	if(WIN32 AND runtime_shared)
		set_target_properties("${libname}-shared" PROPERTIES ARCHIVE_OUTPUT_NAME_RELEASE
			"${libname}-mds")
		set_target_properties("${libname}-shared" PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG
			"${libname}-mdsd")
	else()
		set_target_properties("${libname}-shared" PROPERTIES LIBRARY_OUTPUT_NAME_RELEASE
			"${libname}s")
		set_target_properties("${libname}-shared" PROPERTIES LIBRARY_OUTPUT_NAME_DEBUG
			"${libname}sd")
		set_target_properties("${libname}-shared" PROPERTIES ARCHIVE_OUTPUT_NAME_RELEASE
			"${libname}s")
		set_target_properties("${libname}-shared" PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG
			"${libname}sd")
	endif()
	if(WIN32)
		if(MSVC)
			include(CMakeDetermineVSServicePack)
			DetermineVSServicePack( COMPILER )
		else()
			string(TOLOWER "${CMAKE_CXX_COMPILER_ID}" COMPILER)
		endif()
		if(runtime_shared)
			set_target_properties("${libname}-shared" PROPERTIES  RUNTIME_OUTPUT_NAME_DEBUG
				"${libname}-${VER_SUFFIX}-${COMPILER}-mdd")
			set_target_properties("${libname}-shared" PROPERTIES RUNTIME_OUTPUT_NAME_RELEASE
				"${libname}-${VER_SUFFIX}-${COMPILER}-md")
		else()
			set_target_properties("${libname}-shared" PROPERTIES  RUNTIME_OUTPUT_NAME_DEBUG
				"${libname}-${VER_SUFFIX}-${COMPILER}d")
			set_target_properties("${libname}-shared" PROPERTIES RUNTIME_OUTPUT_NAME_RELEASE
				"${libname}-${VER_SUFFIX}-${COMPILER}")
		endif()
	endif()
	set(PROJECT_EXPORTS "${libname}_shared_EXPORTS" PARENT_SCOPE)
	if(NOT static)
		set(${shared_def} TRUE PARENT_SCOPE)
	else()
		set_target_properties(${libname}-shared PROPERTIES COMPILE_DEFINITIONS "${shared_def}=TRUE")
	endif()
endif()
set(INSTALLABLE_TARGETS ${INSTALLABLE_TARGETS} PARENT_SCOPE)
endfunction()

function(nx_GTEST_ADD_TESTS executable extra_args)
    if(NOT ARGN)
        message(FATAL_ERROR "Missing ARGN: Read the documentation for GTEST_ADD_TESTS")
    endif()
    foreach(source ${ARGN})
        file(READ "${source}" contents)
        string(REGEX MATCHALL "TEST_?F?\\(([A-Za-z_0-9 ,]+)\\)" found_tests ${contents})
        foreach(hit ${found_tests})
            string(REGEX REPLACE ".*\\( *([A-Za-z_0-9]+), *([A-Za-z_0-9]+) *\\).*" "\\1.\\2" test_name ${hit})
            add_test("${executable}-${test_name}" ${executable} --gtest_filter=${test_name} ${extra_args})
        endforeach()
    endforeach()
endfunction()
