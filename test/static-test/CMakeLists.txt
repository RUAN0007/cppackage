project(static-test)

FILE(GLOB_RECURSE all_files
*.cpp
*.cc
*.c++
*.c
*.C)

add_executable(static-test ${all_files})
target_link_libraries(static-test)
