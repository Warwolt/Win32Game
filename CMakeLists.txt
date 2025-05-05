cmake_minimum_required(VERSION 3.16.0)
project(rkhero)
enable_language(C)

set(IGNORED_WARNINGS
    /wd4100
)
add_compile_options(/W4 /WX ${IGNORED_WARNINGS})

# set(SRC

# )

# Test Library
add_executable(rkhero src/main.c)
# target_include_directories(rkhero PUBLIC INC)
set_property(TARGET rkhero PROPERTY C_STANDARD 11)
set_property(TARGET rkhero PROPERTY WIN32_EXECUTABLE TRUE)
