cmake_minimum_required(VERSION 4.0.0)
project(Win32Game)

# Executable
add_executable(Executable src/main.cpp src/application.cpp)
target_include_directories(Executable PRIVATE src)
set_target_properties(Executable PROPERTIES OUTPUT_NAME ${PROJECT_NAME}) # Win32Game.exe
set_property(TARGET Executable PROPERTY WIN32_EXECUTABLE TRUE)
set_property(TARGET Executable PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Executable)
