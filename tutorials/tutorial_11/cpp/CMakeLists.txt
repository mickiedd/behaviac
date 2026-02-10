message(STATUS "tutorials: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE TUTORIAL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE TUTORIAL_INC "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

if(BEHAVIAC_ANDROID_STUDIO)
    add_library(tutorial_11 SHARED ${TUTORIAL_INC} ${TUTORIAL_SRC})
else()
    add_executable(tutorial_11 ${TUTORIAL_INC} ${TUTORIAL_SRC})
endif()

target_include_directories(tutorial_11
    PRIVATE "${PROJECT_SOURCE_DIR}/inc"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(tutorial_11 libbehaviac ${CMAKE_THREAD_LIBS_INIT})

if(NOT BEHAVIAC_ANDROID_STUDIO)
    set_target_properties(tutorial_11 PROPERTIES PREFIX "")
    set_target_properties(tutorial_11 PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    set_target_properties(tutorial_11 PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
endif()
