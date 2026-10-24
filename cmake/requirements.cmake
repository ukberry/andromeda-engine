cmake_policy(SET CMP0168 NEW)
include(FetchContent)

set(SHARED_DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/build/.fetched")

function(declare_shared_dependency Name)
    if(NOT ${Name}_FOUND)
    message(STATUS "Using FetchContent to supply: ${Name}")
    FetchContent_Declare(
        ${Name}
        ${ARGN}
        SOURCE_DIR "${SHARED_DOWNLOAD_DIR}/${Name}-src"
        BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/${Name}-build"
    )
    FetchContent_MakeAvailable(${Name})
    endif()
endfunction()

declare_shared_dependency(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
    GIT_SHALLOW TRUE
)

declare_shared_dependency(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0
    GIT_SHALLOW TRUE
)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Shaders.cmake)

function (copy_dlls target)
    if (WIN32)
        add_custom_command (
            TARGET "${target}" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:${target}>"
                    "$<TARGET_RUNTIME_DLLS:${target}>" USES_TERMINAL COMMAND_EXPAND_LISTS
        )
        if(MINGW)

        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++-6.dll
            OUTPUT_VARIABLE COMPILER_STDCXX_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        get_filename_component(STDCXX_DLL "${COMPILER_STDCXX_PATH}" ABSOLUTE)

        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgcc_s_seh-1.dll
            OUTPUT_VARIABLE COMPILER_GCC_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        get_filename_component(GCC_DLL "${COMPILER_GCC_PATH}" ABSOLUTE)

        if(EXISTS "${STDCXX_DLL}")
            add_custom_command(
                TARGET "${target}" POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${STDCXX_DLL}" "$<TARGET_FILE_DIR:${target}>"
                COMMENT "Copying libstdc++-6.dll to build directory"
            ) 
        endif()
        if(EXISTS "${GCC_DLL}")
        add_custom_command(
            TARGET "${target}" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${GCC_DLL}" "$<TARGET_FILE_DIR:${target}>"
            COMMENT "Copying libgcc_s_seh-1.dll to build directory"
        ) 
    endif()

        endif()
    endif ()
endfunction ()