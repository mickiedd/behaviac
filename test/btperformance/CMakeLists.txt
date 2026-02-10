message(STATUS "test: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE BTPERFORMANCE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE BTPERFORMANCE_INC "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(btperformance ${BTPERFORMANCE_INC} ${BTPERFORMANCE_SRC})

target_include_directories(btperformance
    PRIVATE "${PROJECT_SOURCE_DIR}/inc"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(btperformance libbehaviac ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(btperformance PROPERTIES PREFIX "")
set_target_properties(btperformance PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(btperformance PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
