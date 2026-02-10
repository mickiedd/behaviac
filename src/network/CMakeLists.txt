#
# behaviac_network - Network module
#
# This module provides network replication support for distributed agents:
#   - Network base class
#   - Network variable synchronization
#
# Note: Network is conditionally compiled via BEHAVIAC_ENABLE_NETWORKD.
#

file(GLOB BEHAVIAC_NETWORK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(behaviac_network OBJECT ${BEHAVIAC_NETWORK_SOURCES})

target_include_directories(behaviac_network
    PUBLIC "${PROJECT_SOURCE_DIR}/inc"
)

if (MSVC)
    source_group("network" FILES ${BEHAVIAC_NETWORK_SOURCES})
endif()
