#
# behaviac_agent - Agent framework module
#
# This module provides the agent system that drives behavior execution:
#   - Agent base class (creation, destruction, behavior management)
#   - Context management (agent grouping and lookup)
#   - Named events (signal/event dispatch)
#   - State save/load
#   - Task method execution
#   - Property node helpers
#

file(GLOB BEHAVIAC_AGENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(behaviac_agent OBJECT ${BEHAVIAC_AGENT_SOURCES})

target_include_directories(behaviac_agent
    PUBLIC  "${PROJECT_SOURCE_DIR}/inc"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (MSVC)
    source_group("agent" FILES ${BEHAVIAC_AGENT_SOURCES})
endif()
