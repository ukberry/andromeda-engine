add_executable(example example.cpp)
target_link_libraries(example PRIVATE andromeda)

copy_dlls(example)