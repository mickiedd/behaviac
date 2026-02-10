message(STATUS "tutorials: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE TUTORIAL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE TUTORIAL_INC "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(tutorial_1 ${TUTORIAL_INC} ${TUTORIAL_SRC})

target_include_directories(tutorial_1
    PRIVATE "${PROJECT_SOURCE_DIR}/inc"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(tutorial_1 libbehaviac ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(tutorial_1 PROPERTIES PREFIX "")
set_target_properties(tutorial_1 PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(tutorial_1 PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
