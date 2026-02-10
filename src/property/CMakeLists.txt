#
# behaviac_property - Property system module
#
# This module provides the property and variable management system used by
# agents and behavior tree nodes to store, access, and manipulate typed data.
#   - Property definitions and instances
#   - Variable containers
#   - Operator overloads for property math
#   - Vector extensions
#

file(GLOB BEHAVIAC_PROPERTY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(behaviac_property OBJECT ${BEHAVIAC_PROPERTY_SOURCES})

target_include_directories(behaviac_property
    PUBLIC "${PROJECT_SOURCE_DIR}/inc"
)

if (MSVC)
    source_group("property" FILES ${BEHAVIAC_PROPERTY_SOURCES})
endif()
