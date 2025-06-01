Overall goal:
- Game programming in Win32
- Simple complete games (pong, tetris etc.)
- Simple Win32 level editor (create content for the games)

Maybe ideas:
- Text mode engine debugging UI (for selecting various test screens and tools)

# Todo
- Window: Borderless full screen
- Window: Render at lower resolution and upscale it to fit screen
- Sprite support
- Font support
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

# Doing

# Done
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
