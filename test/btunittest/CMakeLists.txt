message(STATUS "test: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE BTUNITTEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE BTUNITTEST_INC "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(btunittest ${BTUNITTEST_INC} ${BTUNITTEST_SRC})

target_include_directories(btunittest
    PRIVATE "${PROJECT_SOURCE_DIR}/inc"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(btunittest libbehaviac ${CMAKE_THREAD_LIBS_INIT})

# The behaviac designer generates code that uses ((Type*)0)->method() for static
# property access. This is technically UB but intentional in the generated code.
# Suppress the nonnull warning for this target to accommodate the generated code.
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(btunittest PRIVATE "-Wno-nonnull")
endif()

set_target_properties(btunittest PROPERTIES PREFIX "")
set_target_properties(btunittest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(btunittest PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})

if(MSVC)
    SET_TARGET_PROPERTIES(btunittest PROPERTIES LINK_FLAGS "/MAP")
elseif(XCODE)
    #
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=btunittest_gcc_${CMAKE_BUILD_TYPE}.map")
endif()
