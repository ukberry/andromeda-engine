add_executable(example example.cpp)
target_link_libraries(example PRIVATE andromeda)

add_slang_shader(shade SOURCES ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.slang)
add_dependencies(example shade)
copy_dlls(example)