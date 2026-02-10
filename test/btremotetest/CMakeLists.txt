message(STATUS "test: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE BTREMOTETEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE BTREMOTETEST_INC "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(btremotetest ${BTREMOTETEST_INC} ${BTREMOTETEST_SRC})

target_include_directories(btremotetest
    PRIVATE "${PROJECT_SOURCE_DIR}/inc"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(btremotetest libbehaviac ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(btremotetest PROPERTIES PREFIX "")
set_target_properties(btremotetest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(btremotetest PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
