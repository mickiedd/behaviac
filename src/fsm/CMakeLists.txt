#
# behaviac_fsm - Finite State Machine module
#
# This module extends the behavior tree framework with FSM support:
#   - FSM container node
#   - State nodes (State, WaitState, WaitFramesState)
#   - Transition conditions (Transition, AlwaysTransition, WaitTransition)
#   - Start conditions
#

file(GLOB BEHAVIAC_FSM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(behaviac_fsm OBJECT ${BEHAVIAC_FSM_SOURCES})

target_include_directories(behaviac_fsm
    PUBLIC "${PROJECT_SOURCE_DIR}/inc"
)

if (MSVC)
    source_group("fsm" FILES ${BEHAVIAC_FSM_SOURCES})
endif()
