message(STATUS "test: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE USERTEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE USERTEST_INC "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(usertest ${USERTEST_INC} ${USERTEST_SRC})

target_include_directories(usertest
    PRIVATE "${PROJECT_SOURCE_DIR}/inc"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(usertest libbehaviac ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(usertest PROPERTIES PREFIX "")
set_target_properties(usertest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(usertest PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
