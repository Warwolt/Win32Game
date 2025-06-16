Overall goal:
- Game programming in Win32
- Simple complete games (pong, tetris etc.)
- Simple Win32 level editor (create content for the games)

Maybe ideas:
- Text mode engine debugging UI (for selecting various test screens and tools)

# Todo
- Diagnos what parts of render test screen is slow
- Text Mode UI prototype
- [!] Write Blackjack game
- CPU Profiling
- Measure render API, see if things can be optimized (e.g. less loops, SIMD, less memory allocations etc.)
- DLL based hot reloading
- Asset system (store audio, images, fonts, etc.)
- [!] Write Tetris game
- Collisions
- [!] Write Pong game
- Camera
- Enemy AI
- [!] Write Zelda Dungeon game
- Draw API: Polygons (triangle strips, triangle fans)

# Doing

# Done
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
- Move stuff from ProgramContext down into EngineState
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
