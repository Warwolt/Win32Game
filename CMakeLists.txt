cmake_minimum_required(VERSION 3.16.0)
project(rkhero)
enable_language(C)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# set(SRC

# )

# Test Library
add_executable(rkhero src/main.c)
# target_include_directories(rkhero PUBLIC INC)
set_property(TARGET rkhero PROPERTY C_STANDARD 11)
