#
# behaviac_common - Foundation utilities module
#
# This module provides the core infrastructure used by all other behaviac modules:
#   - Memory management and allocators
#   - String utilities and CRC hashing
#   - File I/O and filesystem abstraction
#   - Logging framework
#   - Threading primitives (mutex, wrappers)
#   - XML parsing (rapidxml-based)
#   - Serialization
#   - Socket communication
#   - Profiling
#   - Random number generation
#   - RTTI and factory infrastructure
#   - Workspace and configuration management
#   - Agent metadata system
#

file(GLOB COMMON_ROOT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB COMMON_ROOT_HDR "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

file(GLOB COMMON_FILE_SRC      "${CMAKE_CURRENT_SOURCE_DIR}/file/*.cpp")
file(GLOB COMMON_FILE_HDR      "${CMAKE_CURRENT_SOURCE_DIR}/file/*.h")
file(GLOB COMMON_LOGGER_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/logger/*.cpp")
file(GLOB COMMON_MEMORY_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/memory/*.cpp")
file(GLOB COMMON_OBJECT_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/object/*.cpp")
file(GLOB COMMON_PROFILER_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/profiler/*.cpp")
file(GLOB COMMON_RANDOM_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/randomgenerator/*.cpp")
file(GLOB COMMON_SERIAL_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/serialization/*.cpp")
file(GLOB COMMON_SOCKET_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/socket/*.cpp")
file(GLOB COMMON_STRING_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/string/*.cpp")
file(GLOB COMMON_THREAD_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/thread/*.cpp")
file(GLOB COMMON_XML_SRC       "${CMAKE_CURRENT_SOURCE_DIR}/xml/*.cpp")

set(BEHAVIAC_COMMON_SOURCES
    ${COMMON_ROOT_SRC}
    ${COMMON_ROOT_HDR}
    ${COMMON_FILE_SRC}
    ${COMMON_FILE_HDR}
    ${COMMON_LOGGER_SRC}
    ${COMMON_MEMORY_SRC}
    ${COMMON_OBJECT_SRC}
    ${COMMON_PROFILER_SRC}
    ${COMMON_RANDOM_SRC}
    ${COMMON_SERIAL_SRC}
    ${COMMON_SOCKET_SRC}
    ${COMMON_STRING_SRC}
    ${COMMON_THREAD_SRC}
    ${COMMON_XML_SRC}
)

add_library(behaviac_common OBJECT ${BEHAVIAC_COMMON_SOURCES})

target_include_directories(behaviac_common
    PUBLIC "${PROJECT_SOURCE_DIR}/inc"
)

# Source grouping for IDE support
if (MSVC)
    source_group("common"                FILES ${COMMON_ROOT_SRC} ${COMMON_ROOT_HDR})
    source_group("common\\file"          FILES ${COMMON_FILE_SRC} ${COMMON_FILE_HDR})
    source_group("common\\logger"        FILES ${COMMON_LOGGER_SRC})
    source_group("common\\memory"        FILES ${COMMON_MEMORY_SRC})
    source_group("common\\object"        FILES ${COMMON_OBJECT_SRC})
    source_group("common\\profiler"      FILES ${COMMON_PROFILER_SRC})
    source_group("common\\randomgenerator" FILES ${COMMON_RANDOM_SRC})
    source_group("common\\serialization" FILES ${COMMON_SERIAL_SRC})
    source_group("common\\socket"        FILES ${COMMON_SOCKET_SRC})
    source_group("common\\string"        FILES ${COMMON_STRING_SRC})
    source_group("common\\thread"        FILES ${COMMON_THREAD_SRC})
    source_group("common\\xml"           FILES ${COMMON_XML_SRC})
endif()
