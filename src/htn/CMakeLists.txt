#
# behaviac_htn - Hierarchical Task Network module
#
# This module provides HTN planning support:
#   - Planner (AI planning algorithm)
#   - PlannerTask (executable plan steps)
#   - Task and Method nodes
#   - Agent properties and state for HTN
#
# Note: HTN is conditionally compiled via BEHAVIAC_USE_HTN.
#

file(GLOB BEHAVIAC_HTN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(behaviac_htn OBJECT ${BEHAVIAC_HTN_SOURCES})

target_include_directories(behaviac_htn
    PUBLIC "${PROJECT_SOURCE_DIR}/inc"
)

if (MSVC)
    source_group("htn" FILES ${BEHAVIAC_HTN_SOURCES})
endif()
