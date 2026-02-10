message(STATUS "test: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE DEMO_RUNNING_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE DEMO_RUNNING_INC "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(demo_running ${DEMO_RUNNING_INC} ${DEMO_RUNNING_SRC})

target_include_directories(demo_running
    PRIVATE "${PROJECT_SOURCE_DIR}/inc"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(demo_running libbehaviac ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(demo_running PROPERTIES PREFIX "")
set_target_properties(demo_running PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(demo_running PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
