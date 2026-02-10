#
# behaviac - Behavior Tree Framework
#
# Root build file. This composes the modular OBJECT libraries from src/
# into the final libbehaviac library target, and adds tests/tutorials.
#
# Module structure (see src/CMakeLists.txt for dependency graph):
#   behaviac_common        - Foundation utilities (memory, string, file, thread, xml, etc.)
#   behaviac_property      - Property and variable management system
#   behaviac_behaviortree  - Core behavior tree nodes and execution engine
#   behaviac_agent         - Agent lifecycle and context management
#   behaviac_fsm           - Finite State Machine extension
#   behaviac_htn           - Hierarchical Task Network planning
#   behaviac_network       - Network replication support
#

# ---------------------------------------------------------------------------
# Build type configuration
# ---------------------------------------------------------------------------

if(DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Set to either \"Release\" or \"Debug\"")
else()
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Set to either \"Release\" or \"Debug\"")
endif()

mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
project(behaviac)

message(STATUS "MSVC=${MSVC}")

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(BEHAVIAC_PACKAGE_VERSION 3.6.39)

#option(BUILD_SHARED_LIBS "set to OFF to build static libraries" ON)
SET(BUILD_SHARED_LIBS ON CACHE BOOL "set to OFF to build static libraries")

# ---------------------------------------------------------------------------
# Version mode configuration
# ---------------------------------------------------------------------------

if(DEFINED BEHAVIAC_VERSION_MODE)
    set(BEHAVIAC_VERSION_MODE ${BEHAVIAC_VERSION_MODE} CACHE STRING
        "version mode, force to use dev/release version or default(dev version on Debug config and release version on Release config)")
else()
    set(BEHAVIAC_VERSION_MODE "Default" CACHE STRING
        "version mode, force to use dev/release version or default(dev version on Debug config and release version on Release config)")
endif()

SET_PROPERTY(CACHE BEHAVIAC_VERSION_MODE PROPERTY STRINGS Default ForceUseDev ForeUseRelease)
if(NOT(DEFINED BEHAVIAC_VERSION_MODE_LAST))
    set(BEHAVIAC_VERSION_MODE_LAST "NotAnOption" CACHE STRING "last version mode")
    MARK_AS_ADVANCED(FORCE BEHAVIAC_VERSION_MODE_LAST)
endif()

if(NOT (${BEHAVIAC_VERSION_MODE} MATCHES ${BEHAVIAC_VERSION_MODE_LAST}))
    set(BEHAVIAC_VERSION_MODE_LAST ${BEHAVIAC_VERSION_MODE} CACHE STRING "Updating version mode option" FORCE)
endif()

set(BEHAVIAC_RELEASE_CUSTOMIZED 0)
set(BEHAVIAC_RELEASE_ON 0)

if(${BEHAVIAC_VERSION_MODE} MATCHES "ForceUseDev")
    set(BEHAVIAC_RELEASE_CUSTOMIZED 1)
    set(BEHAVIAC_RELEASE_ON 0)
endif()
if(${BEHAVIAC_VERSION_MODE} MATCHES "ForeUseRelease")
    set(BEHAVIAC_RELEASE_CUSTOMIZED 1)
    set(BEHAVIAC_RELEASE_ON 1)
endif()

# ---------------------------------------------------------------------------
# Platform detection
# ---------------------------------------------------------------------------

if(CMAKE_GENERATOR MATCHES "Xcode")
    message(STATUS "XCODE = ${XCODE}")
endif()

if(MSVC OR XCODE)
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )
    if(XCODE)
        set(XCODE_ATTRIBUTE_GCC_PREPROCESSOR_DEFINITIONS[variant=Debug] "_DEBUG")
        set(XCODE_ATTRIBUTE_GCC_PREPROCESSOR_DEFINITIONS[variant=Release] "NDEBUG")
    endif()
else()
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions("-D_DEBUG")
    else()
        add_definitions("-DNDEBUG")
    endif()
endif()

# ---------------------------------------------------------------------------
# Release optimizations (LTO)
# ---------------------------------------------------------------------------

if(BEHAVIAC_RELEASE_ON)
    if(MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Gy /GL")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /OPT:ICF /OPT:REF /LTCG")
        set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS} /OPT:ICF /OPT:REF /LTCG")
    else()
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(-flto GXX_HAS_LTO_FLAG)
        message(STATUS "GXX_HAS_LTO_FLAG = ${GXX_HAS_LTO_FLAG}")

        if(CMAKE_BUILD_TYPE MATCHES Release AND GXX_HAS_LTO_FLAG)
            find_program(CMAKE_GCC_AR NAMES ${_CMAKE_TOOLCHAIN_PREFIX}gcc-ar${_CMAKE_TOOLCHAIN_SUFFIX} HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
            find_program(CMAKE_GCC_NM NAMES ${_CMAKE_TOOLCHAIN_PREFIX}gcc-nm HINTS ${_CMAKE_TOOLCHAIN_LOCATION})
            find_program(CMAKE_GCC_RANLIB NAMES ${_CMAKE_TOOLCHAIN_PREFIX}gcc-ranlib HINTS ${_CMAKE_TOOLCHAIN_LOCATION})

            if(CMAKE_GCC_AR AND CMAKE_GCC_NM AND CMAKE_GCC_RANLIB)
                message(STATUS "SET LTO Flags")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -O3")

                check_cxx_compiler_flag(-fno-fat-lto-objects GXX_HAS_NO_FAT_LTO_FLAG)
                message(STATUS "GXX_HAS_NO_FAT_LTO_FLAG = ${GXX_HAS_NO_FAT_LTO_FLAG}")

                if(GXX_HAS_NO_FAT_LTO_FLAG)
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-fat-lto-objects")
                endif()

                set(CMAKE_AR "${CMAKE_GCC_AR}")
                set(CMAKE_NM "${CMAKE_GCC_NM}")
                set(CMAKE_RANLIB "${CMAKE_GCC_RANLIB}")
            else()
                message(WARNING "GCC indicates LTO support, but binutils wrappers could not be found. Disabling LTO.")
            endif()
        endif()
    endif()
endif()

# ---------------------------------------------------------------------------
# 64-bit configuration
# ---------------------------------------------------------------------------

SET(BUILD_USE_64BITS OFF CACHE BOOL "set to ON to use 64 bits")

if(MSVC)
    if(CMAKE_GENERATOR MATCHES "64")
        SET(BUILD_USE_64BITS ON CACHE BOOL "CMAKE_GENERATOR is for 64 bit, set to ON accordingly" FORCE)
    endif()
endif()

if(BUILD_USE_64BITS)
    if(MSVC)
        if(NOT CMAKE_GENERATOR MATCHES "64")
            message(SEND_ERROR "You need to specify '${CMAKE_GENERATOR} Win64' to generate project file for 64 bit")
        endif()
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
    endif()
endif()

message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "BUILD_USE_64BITS = ${BUILD_USE_64BITS}")
message(STATUS "CMAKE_GENERATOR = ${CMAKE_GENERATOR}")

# ---------------------------------------------------------------------------
# Compiler flags
# ---------------------------------------------------------------------------

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

if(VC_MDD_ANDROID)
    message(STATUS "VC_MDD_ANDROID")
    cmake_minimum_required(VERSION 3.4)
endif()

set(BEHAVIAC_ANDROID_ON 0)

if(CMAKE_GENERATOR MATCHES "Visual Studio 14" AND CMAKE_SYSTEM_NAME MATCHES "VCMDDAndroid")
    set(BEHAVIAC_ANDROID_ON 1)
endif()

set(BEHAVIAC_ANDROID_STUDIO 0)

if(CMAKE_GENERATOR MATCHES "Ninja" AND CMAKE_SYSTEM_NAME MATCHES "Android")
    set(BEHAVIAC_ANDROID_STUDIO 1)
endif()

set(COMPILE_POSTFIX "")

if(MSVC OR BEHAVIAC_ANDROID_ON)
    message(STATUS "set msvc flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHs")
    set(COMPILE_POSTFIX "msvc")

    mark_as_advanced(CMAKE_BUILD_TYPE)

    if(BEHAVIAC_ANDROID_ON)
        message(STATUS "set msvc flags for android")
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "set to OFF to build static libraries")
        mark_as_advanced(BUILD_SHARED_LIBS)
        set(COMPILE_POSTFIX "msvc_android")
    endif()
else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "set to OFF to build static libraries" FORCE)
    mark_as_advanced(BUILD_SHARED_LIBS)
    mark_as_advanced(CMAKE_GNUtoMS)

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        message(STATUS "set gcc flags")
        find_package(Threads REQUIRED)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Woverloaded-virtual -Wnon-virtual-dtor -Wfloat-equal -Wno-unused-local-typedefs -finput-charset=UTF-8 ")
    endif()

    set(COMPILE_POSTFIX "gcc")
endif()

# ---------------------------------------------------------------------------
# Status output
# ---------------------------------------------------------------------------

message(STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")
message(STATUS "BUILD_ENABLE_LUA = ${BUILD_ENABLE_LUA}")
message(STATUS "BEHAVIAC_VERSION_MODE = ${BEHAVIAC_VERSION_MODE}")
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
if(MSVC OR BEHAVIAC_ANDROID_ON)
    message(STATUS "Config = Debug|Release")
else()
    message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
endif()

# ---------------------------------------------------------------------------
# Generate configuration header
# ---------------------------------------------------------------------------

configure_file(
    "${PROJECT_SOURCE_DIR}/build/_config.h.in"
    "${PROJECT_SOURCE_DIR}/inc/behaviac/common/_config.h"
    NEWLINE_STYLE WIN32
)

# ---------------------------------------------------------------------------
# Output directories
# ---------------------------------------------------------------------------

if(NOT BEHAVIAC_ANDROID_STUDIO)
    set(LIB_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
endif()
set(BINARY_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_PATH})
if(NOT BEHAVIAC_ANDROID_STUDIO)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_PATH})
endif()

set(CMAKE_DEBUG_POSTFIX "_${COMPILE_POSTFIX}_debug")
set(CMAKE_RELEASE_POSTFIX "_${COMPILE_POSTFIX}_release")

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_PATH})
    if(NOT BEHAVIAC_ANDROID_STUDIO)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_OUTPUT_PATH})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIB_OUTPUT_PATH})
    endif()
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# ---------------------------------------------------------------------------
# Include header files for the composed library (needed for IDE source groups)
# ---------------------------------------------------------------------------

file(GLOB_RECURSE BEHAVIAC_INC "${PROJECT_SOURCE_DIR}/inc/*.*")

# IDE source grouping for headers
if(MSVC)
    # usage: GroupSources("${PROJECT_SOURCE_DIR}/inc/", "inc")
    macro(GroupSources dir prefix)
        file(GLOB_RECURSE elements RELATIVE ${dir} *.*)
        foreach(element ${elements})
            get_filename_component(element_name ${element} NAME)
            get_filename_component(element_dir ${element} DIRECTORY)
            if(NOT ${element_dir} STREQUAL "")
                string(REPLACE "/" "\\" group_name ${element_dir})
                source_group("${prefix}\\${group_name}" FILES ${dir}/${element})
            else()
                source_group("${prefix}\\" FILES ${dir}/${element})
            endif()
        endforeach()
    endmacro()

    GroupSources("${PROJECT_SOURCE_DIR}/inc/" "inc")
endif()

# ---------------------------------------------------------------------------
# Build modular source libraries (OBJECT libraries)
# ---------------------------------------------------------------------------

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

# ---------------------------------------------------------------------------
# Compose libbehaviac from all module OBJECT libraries
# ---------------------------------------------------------------------------

set(SHARED_LIB "")

if(BEHAVIAC_ANDROID_STUDIO)
    set(SHARED_LIB "SHARED")
endif()

add_library(libbehaviac ${SHARED_LIB}
    ${BEHAVIAC_INC}
    $<TARGET_OBJECTS:behaviac_common>
    $<TARGET_OBJECTS:behaviac_property>
    $<TARGET_OBJECTS:behaviac_behaviortree>
    $<TARGET_OBJECTS:behaviac_agent>
    $<TARGET_OBJECTS:behaviac_fsm>
    $<TARGET_OBJECTS:behaviac_htn>
    $<TARGET_OBJECTS:behaviac_network>
)

# Public include directory so consumers get the headers
target_include_directories(libbehaviac PUBLIC "${PROJECT_SOURCE_DIR}/inc")

set_target_properties(libbehaviac PROPERTIES PREFIX "")
set_target_properties(libbehaviac PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(libbehaviac PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})

if(BEHAVIAC_ANDROID_STUDIO)
    target_link_libraries(libbehaviac log android)
endif()

# ---------------------------------------------------------------------------
# Shared library export definitions
# ---------------------------------------------------------------------------

# Helper function to add compile definitions to a target
function(add_target_definitions target)
    get_target_property(defs ${target} COMPILE_DEFINITIONS)
    if(defs MATCHES "NOTFOUND")
        set(defs "")
    endif()
    foreach(def ${defs} ${ARGN})
        list(APPEND deflist ${def})
    endforeach()
    set_target_properties(${target} PROPERTIES COMPILE_DEFINITIONS "${deflist}")
endfunction()

if(BUILD_SHARED_LIBS)
    message(STATUS "build shared libs")
    add_target_definitions(libbehaviac "BEHAVIACDLL_EXPORTS" "BEHAVIAC_DLL")
else()
    message(STATUS "build static libs")
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

if(NOT BEHAVIAC_ANDROID_STUDIO)
    add_subdirectory("${PROJECT_SOURCE_DIR}/test/btunittest")
    add_subdirectory("${PROJECT_SOURCE_DIR}/test/btremotetest")
    add_subdirectory("${PROJECT_SOURCE_DIR}/test/demo_running")
    add_subdirectory("${PROJECT_SOURCE_DIR}/test/usertest")

    if(MSVC)
        add_subdirectory("${PROJECT_SOURCE_DIR}/test/btperformance")
        if(EXISTS "${PROJECT_SOURCE_DIR}/example/spaceship")
            add_subdirectory("${PROJECT_SOURCE_DIR}/example/spaceship")
        endif()
    endif()
endif()

# ---------------------------------------------------------------------------
# Tutorials
# ---------------------------------------------------------------------------

if(NOT BEHAVIAC_ANDROID_STUDIO)
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_1/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_1_1/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_1_2/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_2/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_3/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_4/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_5/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_6/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_7/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_8/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_9/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_10/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_11/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_12/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_13/cpp")
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_14/cpp")
else()
    add_subdirectory("${PROJECT_SOURCE_DIR}/tutorials/tutorial_11/cpp")
endif()
