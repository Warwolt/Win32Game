cmake_minimum_required(VERSION 3.16.0)
project(win32game)

# This option is enabled in CI builds
option(WARNINGS_AS_ERRORS "Compile with warnings as errors" OFF)
option(UNIT_TESTS "Build unit tests" ON)

# Google Test
if(UNIT_TESTS)
    add_subdirectory(libs/googletest)
endif()

# Windows workaround for Google Test
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
endif()


set(SRC
    libs/stb/stb_image/stb_image.c
    libs/stb/stb_truetype/stb_truetype.c
    src/engine/audio/audio_player.cpp
    src/engine/commands.cpp
    src/engine/debug/assert.cpp
    src/engine/debug/debug.cpp
    src/engine/debug/logging.cpp
    src/engine/debug/test/rendering_test_screen.cpp
    src/engine/engine.cpp
    src/engine/file/resource_manager.cpp
    src/engine/graphics/bitmap.cpp
    src/engine/graphics/image.cpp
    src/engine/graphics/rect.cpp
    src/engine/graphics/renderer.cpp
    src/engine/graphics/rgba.cpp
    src/engine/graphics/window.cpp
    src/engine/input/button.cpp
    src/engine/input/gamepad.cpp
    src/engine/input/input.cpp
    src/engine/input/keyboard.cpp
    src/engine/input/mouse.cpp
    src/engine/math/ivec2.cpp
    src/engine/math/vec2.cpp
    src/game/game.cpp
)

set(INC
    libs/stb
)

set(TEST_SRC
    test/engine/button_tests.cpp
    test/engine/keyboard_tests.cpp
)

# Src Library
add_library(source STATIC ${SRC})
target_include_directories(source PUBLIC src ${INC})

# Executable
add_executable(win32game WIN32 src/main.cpp)
target_include_directories(win32game PUBLIC src ${INC})
target_link_libraries(win32game PUBLIC source)
set_property(TARGET win32game PROPERTY WIN32_EXECUTABLE TRUE)
set_property(TARGET win32game PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT win32game)

# Unit tests
if(UNIT_TESTS)
    add_executable(unit_tests test/main.cpp ${TEST_SRC})
    target_include_directories(unit_tests PUBLIC src test)
    target_link_libraries(unit_tests PUBLIC gtest gmock source)
endif()


# Set compile options for targets
SET(TARGETS source win32game)
if(UNIT_TESTS)
    list(APPEND TARGETS unit_tests)
endif()

foreach(TARGET IN ITEMS ${TARGETS})
    set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 23)
    add_compile_definitions(${TARGET} PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${TARGET} PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>: /W4 /w44062>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Wall -Wextra -Wpedantic>)
    if (WARNINGS_AS_ERRORS)
        target_compile_options(${TARGET} PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>: /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Werror>)
    endif()
endforeach()
