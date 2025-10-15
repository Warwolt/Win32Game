cmake_minimum_required(VERSION 3.31.6)
project(Win32Game)

option(WARNINGS_AS_ERRORS "Compile with warnings as errors" OFF)
option(BUILD_TESTS "Build tests" ON)
option(LINK_DYNAMICALLY "Link app dynamically (for hot reloading)" ON)
option(ENABLE_PROFILING "Enable profiling tool" ON)
message(STATUS "WARNINGS_AS_ERRORS: ${WARNINGS_AS_ERRORS}")
message(STATUS "BUILD_TESTS: ${BUILD_TESTS}")
message(STATUS "LINK_DYNAMICALLY: ${LINK_DYNAMICALLY}")
message(STATUS "ENABLE_PROFILING: ${ENABLE_PROFILING}")

set(SRC
    libs/stb/stb_image/stb_image.c
    libs/stb/stb_truetype/stb_truetype.c
    src/engine/audio/audio_player.cpp
    src/engine/commands.cpp
    src/engine/debug/assert.cpp
    src/engine/debug/debug.cpp
    src/engine/debug/delta_timer.cpp
    src/engine/debug/logging.cpp
    src/engine/debug/test/render_test_screen.cpp
    src/engine/engine.cpp
    src/engine/file/resource_manager.cpp
    src/engine/graphics/bitmap.cpp
    src/engine/graphics/font.cpp
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
    src/engine/math/moving_average.cpp
    src/engine/math/vec2.cpp
    src/game/game.cpp
)

set(TEST_SRC
    test/engine/button_tests.cpp
    test/engine/keyboard_tests.cpp
    test/engine/moving_average_tests.cpp
)

set(INC
    src
    libs/stb
)

# Executable
add_executable(Executable src/main.cpp)
set_target_properties(Executable PROPERTIES OUTPUT_NAME ${PROJECT_NAME}) # PROJECT_NAME.exe
set_property(TARGET Executable PROPERTY WIN32_EXECUTABLE TRUE)
set_property(TARGET Executable PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Executable)

# Application Library
if (LINK_DYNAMICALLY)
    # Build library as DLL
    add_library(Library SHARED src/application.cpp)
    target_compile_definitions(Library PRIVATE LINK_APPLICATION_DYNAMICALLY)
    # Link Exeuctable to library loader
    target_sources(Executable PRIVATE
        src/engine/windows/command_line.cpp
        src/library/application_library.cpp
        src/library/library_loader.cpp
    )
    target_compile_definitions(Executable PRIVATE LIBRARY_NAME="${PROJECT_NAME}")
else()
    # Link executable to static library
    add_library(Library STATIC src/application.cpp)
    target_link_libraries(Executable Library)
endif()
set_target_properties(Library PROPERTIES OUTPUT_NAME ${PROJECT_NAME}) # PROJECT_NAME.dll or PROJECT_NAME.lib
target_link_libraries(Library PRIVATE Source)

# Source Library
add_library(Source STATIC ${SRC})

# Tests
if(BUILD_TESTS)
    # Google Test
    add_subdirectory(libs/googletest)

    # Windows workaround for Google Test
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    endif()

    # Test Runner
    add_executable(Tests test/main.cpp ${TEST_SRC})
    target_include_directories(Tests PUBLIC src test)
    target_link_libraries(Tests PUBLIC gtest gmock Source)
endif()

# Profiling
if(ENABLE_PROFILING)
    # Tracy profiler
    option(TRACY_ENABLE "" ON)
    option(TRACY_ON_DEMAND "" ON)
    add_subdirectory(libs/tracy)
    target_link_libraries(Library PUBLIC TracyClient)
    target_link_libraries(Source PUBLIC TracyClient)
    target_compile_definitions(Library PRIVATE TRACY_ENABLE)
    target_compile_definitions(Source PRIVATE TRACY_ENABLE)
endif()

# Set shared compilation options
SET(TARGETS Executable Library Source)
if(BUILD_TESTS)
    list(APPEND TARGETS Tests)
endif()
foreach(TARGET IN ITEMS ${TARGETS})
    target_include_directories(${TARGET} PRIVATE ${INC})
    target_compile_options(${TARGET} PUBLIC /W4 /w44062 /wd4996)
    set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 23)
    if (WARNINGS_AS_ERRORS)
        target_compile_options(${TARGET} PUBLIC /WX)
    endif()
endforeach()
