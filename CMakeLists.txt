cmake_minimum_required(VERSION 3.16.0)
project(rkhero)

# This option is enabled in CI builds
option(WARNINGS_AS_ERRORS "Compile with warnings as errors" OFF)

# set(SRC

# )

# Test Library
add_executable(rkhero src/main.cpp)
# target_include_directories(rkhero PUBLIC INC)
set_property(TARGET rkhero PROPERTY CPP_STANDARD 23)
set_property(TARGET rkhero PROPERTY WIN32_EXECUTABLE TRUE)
if (WARNINGS_AS_ERRORS)
    target_compile_options(rkhero PUBLIC /WX)
endif()
