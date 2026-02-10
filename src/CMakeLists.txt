#
# behaviac source module aggregator
#
# This file includes each module subdirectory and defines the module
# dependency graph. Each module is built as a CMake OBJECT library,
# and all are composed into the final libbehaviac target by the root
# CMakeLists.txt.
#
# Module Dependency Graph:
#
#   behaviac_common          (foundation - no internal deps)
#       ^
#       |
#   behaviac_property        (depends on: common)
#       ^
#       |
#   behaviac_behaviortree    (depends on: common, property)
#       ^       ^
#       |       |
#       |   behaviac_fsm     (depends on: common, behaviortree)
#       |       ^
#       |       |
#   behaviac_agent           (depends on: common, property, behaviortree)
#       ^       ^
#       |       |
#   behaviac_htn             (depends on: common, behaviortree, agent)
#       |
#   behaviac_network         (depends on: common, agent)
#

# --- Foundation layer ---
add_subdirectory(common)

# --- Property system ---
add_subdirectory(property)

# --- Behavior tree core ---
add_subdirectory(behaviortree)

# --- Agent framework ---
add_subdirectory(agent)

# --- Finite State Machine extension ---
add_subdirectory(fsm)

# --- Hierarchical Task Network extension ---
add_subdirectory(htn)

# --- Network replication ---
add_subdirectory(network)
