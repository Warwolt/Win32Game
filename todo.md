Overall goal:
- Game programming in Win32
- Simple complete games (pong, tetris etc.)
- Simple Win32 level editor (create content for the games)

# Todo
- Move quit with escape into game.cpp
- Move stuff from ProgramContext down into EngineState
- Bug: ignore keyboard key/mouse buttons if window not focused
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
- Draw API: Add test screen drawing various shapes

# Done
- Draw API: circles
- Draw API: alpha blending
- Draw API: rectangles, polygons
- Draw API: lines
- Mouse input
- Add logging
- Audio support
- Controller and keyboard input
- Create window and draw to it
