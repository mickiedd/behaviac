#
# behaviac_behaviortree - Core behavior tree module
#
# This module provides the behavior tree framework:
#   - BehaviorNode base class and BehaviorTree container
#   - BehaviorTask execution engine
#   - Node registration system
#   - Action nodes: Action, Assignment, Compute, End, Noop, Wait, WaitFrames, WaitforSignal
#   - Composite nodes: Selector, Sequence, Parallel, IfElse, SelectorLoop, etc.
#   - Condition nodes: And, Or, True, False, Condition
#   - Decorator nodes: Loop, Not, Log, Count, Time, Frames, Repeat, Iterator, etc.
#   - Attachment nodes: Event, Precondition, Effector, AttachAction
#

file(GLOB BT_ROOT_SRC           "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB BT_ATTACHMENTS_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/attachments/*.cpp")
file(GLOB BT_ACTIONS_SRC        "${CMAKE_CURRENT_SOURCE_DIR}/nodes/actions/*.cpp")
file(GLOB BT_COMPOSITES_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/nodes/composites/*.cpp")
file(GLOB BT_CONDITIONS_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/nodes/conditions/*.cpp")
file(GLOB BT_DECORATORS_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/nodes/decorators/*.cpp")

set(BEHAVIAC_BEHAVIORTREE_SOURCES
    ${BT_ROOT_SRC}
    ${BT_ATTACHMENTS_SRC}
    ${BT_ACTIONS_SRC}
    ${BT_COMPOSITES_SRC}
    ${BT_CONDITIONS_SRC}
    ${BT_DECORATORS_SRC}
)

add_library(behaviac_behaviortree OBJECT ${BEHAVIAC_BEHAVIORTREE_SOURCES})

target_include_directories(behaviac_behaviortree
    PUBLIC "${PROJECT_SOURCE_DIR}/inc"
)

# Source grouping for IDE support
if (MSVC)
    source_group("behaviortree"                   FILES ${BT_ROOT_SRC})
    source_group("behaviortree\\attachments"      FILES ${BT_ATTACHMENTS_SRC})
    source_group("behaviortree\\nodes\\actions"   FILES ${BT_ACTIONS_SRC})
    source_group("behaviortree\\nodes\\composites" FILES ${BT_COMPOSITES_SRC})
    source_group("behaviortree\\nodes\\conditions" FILES ${BT_CONDITIONS_SRC})
    source_group("behaviortree\\nodes\\decorators" FILES ${BT_DECORATORS_SRC})
endif()
