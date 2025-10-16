Overall goal:
- Game programming in Win32
- Simple complete games (pong, tetris etc.)
- Simple Win32 level editor (create content for the games)

# Todo
- Make alpha of image tint color adjust how much tint is added, add separate transparancy paramater
- Draw sprite from spritesheet (index into spritesheet with pixels, not UV, to avoid too much slow math)
- Add render test screen with lots of images to stress test, get a sense of budget
- Add a little Link sprite that can walk around with keyboard
- Try out LDtk Level Editor (https://ldtk.io/)

# Doing

# Done
- Optimize image drawing for case when entire image is drawn
- Integrate Tracy profiler
- DLL based hot reloading
- Text Mode UI prototype
- Diagnose what parts of render test screen is slow
- CPU Profiling
- Get rid of CommandAPI class, just work with a `std::vector<Command>` directly
- Measure frame delta
- Font support
- Debugging: print file name from LOG macros
- Draw API: Textures
- Draw API: Per vertex colors, interpolate color between vertices
- Window: Render at lower resolution and upscale it to fit screen
- Window: Borderless full screen
- Bug: ignore keyboard key/mouse buttons if window not focused
- Add engine and game init functions
- Move quit with escape into game.cpp
- Move stuff from ProgramContext down into Engine
- Draw API: Add test screen drawing various shapes
- Draw API: circles
- Draw API: alpha blending
- Draw API: rectangles, polygons
- Draw API: lines
- Mouse input
- Add logging
- Audio support
- Controller and keyboard input
- Create window and draw to it
