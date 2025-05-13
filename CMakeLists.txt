cmake_minimum_required(VERSION 3.16.0)
project(rkhero)

# This option is enabled in CI builds
option(WARNINGS_AS_ERRORS "Compile with warnings as errors" OFF)

set(SRC
    src/input/button.cpp
)

add_executable(rkhero WIN32 src/main.cpp ${SRC})
target_include_directories(rkhero PUBLIC src)
set_property(TARGET rkhero PROPERTY CXX_STANDARD 23)
set_property(TARGET rkhero PROPERTY WIN32_EXECUTABLE TRUE)
if (WARNINGS_AS_ERRORS)
    target_compile_options(rkhero PUBLIC /WX)
endif()
