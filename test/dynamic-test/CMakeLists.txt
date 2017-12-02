project(dynamic-test)

FILE(GLOB_RECURSE all_files
*.cpp
*.cc
*.c++
*.c
*.C)

add_executable(dynamic-test ${all_files})
target_link_libraries(dynamic-test)
