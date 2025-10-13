cmake_minimum_required(VERSION 4.0.0)
project(Win32Game)

set(SRC
    libs/stb/stb_image/stb_image.c
    libs/stb/stb_truetype/stb_truetype.c
    src/engine/audio/audio_player.cpp
    src/engine/commands.cpp
    src/engine/debug/assert.cpp
    src/engine/debug/debug.cpp
    src/engine/debug/delta_timer.cpp
    src/engine/debug/logging.cpp
    src/engine/debug/test/rendering_test_screen.cpp
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

set(INC
    src
    libs/stb
)

# Executable
add_executable(Executable src/main.cpp)
set_target_properties(Executable PROPERTIES OUTPUT_NAME ${PROJECT_NAME}) # Win32Game.exe
set_property(TARGET Executable PROPERTY WIN32_EXECUTABLE TRUE)
set_property(TARGET Executable PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Executable)
target_link_libraries(Executable Library)

# Application Library
add_library(Library STATIC src/application.cpp)
target_link_libraries(Library Source)

# Source Library
add_library(Source STATIC ${SRC})

# Set shared compilation options
SET(TARGETS Executable Library Source)
foreach(TARGET IN ITEMS ${TARGETS})
    target_include_directories(${TARGET} PRIVATE ${INC})
    target_compile_options(${TARGET} PUBLIC /W4 /w44062 /wd4996)
    set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 23)
endforeach()
