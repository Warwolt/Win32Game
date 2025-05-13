cmake_minimum_required(VERSION 3.16.0)
project(rkhero)

add_subdirectory(libs/googletest)

# This option is enabled in CI builds
option(WARNINGS_AS_ERRORS "Compile with warnings as errors" OFF)

set(SRC
    src/engine/input/button.cpp
    src/engine/input/gamepad.cpp
    src/engine/input/keyboard.cpp
    src/engine/render/window.cpp
    src/engine/logging.cpp
    src/game/game.cpp
)

set(TEST_SRC
    test/engine/button_tests.cpp
    test/engine/keyboard_tests.cpp
)

# Src Library
add_library(source STATIC ${SRC})
target_include_directories(source PUBLIC src)
set_property(TARGET source PROPERTY CXX_STANDARD 23)
if (WARNINGS_AS_ERRORS)
    target_compile_options(source PUBLIC /WX)
endif()

# Executable
add_executable(rkhero WIN32 src/main.cpp)
target_include_directories(rkhero PUBLIC src)
target_link_libraries(rkhero PUBLIC source)
set_property(TARGET rkhero PROPERTY CXX_STANDARD 23)
set_property(TARGET rkhero PROPERTY WIN32_EXECUTABLE TRUE)
if (WARNINGS_AS_ERRORS)
    target_compile_options(rkhero PUBLIC /WX)
endif()


# Unit tests
add_executable(unit_tests test/main.cpp ${TEST_SRC})
target_include_directories(unit_tests PUBLIC src test)
target_link_libraries(unit_tests PUBLIC gtest gmock source)
set_property(TARGET unit_tests PROPERTY CXX_STANDARD 23)
if (WARNINGS_AS_ERRORS)
    target_compile_options(unit_tests PUBLIC /WX)
endif()

# Windows workaround for gtest
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
endif()
