Goal:
- Get a minimalistic NES Zelda game done.
  - [x] Main menu
  - [ ] Level with enemy and goal
  - [x] Pause screen
  - [ ] Game over screen
  - [ ] Victory screen

# Todo
- Add "load game" menu that lists save files in some save file directory
- Add "file menu" in debug builds, move hot reloading there, free up F5 key for e.g. quick save
- Enemies walking around
- Support importing Aseprite sprite sheets
- Bug investigation: Probably the crash on hot reload is caused by the re-loaded DLL using new heap compared to the previous DLL. Can we investigate if this really the case? Reference: (https://devblogs.microsoft.com/oldnewthing/20161209-00/?p=94905)
- Add keyboard & mouse input test screen, display stream of events and data like e.g. mouse position
- Try out LDtk Level Editor (https://ldtk.io/) or Tiled (https://www.mapeditor.org/)
- Try out FMOD as audio player
- Add audio test screen, display volume of current played sample as a little bar
  - maybe relevant: https://stackoverflow.com/questions/35438815/how-do-i-retrieve-the-current-sample-play-position-from-an-xaudio2-source-voice
- Add render test screen with lots of images to stress test, get a sense of budget

# Doing
- Add input bindings so that both Z and Enter can be used as confirmation

# Done
- Draw API: Support for drawing text centered in a box
- Add quick save / quick load shortcuts with F5 F9
- Make CommandList::run_commands take Engine* instead of separte dependencies
- Create nicer interface for triggering animations that hides dependencies like AnimationLibrary and current time etc. (Add `play_animation` to CommandList? Add AnimationCommands as sub-object to CommandList?)
- Save file system, save & quit in menu
- Add pause menu in-game
- Add a little Link sprite that can walk around with keyboard
- Add support to generate crash dump (https://learn.microsoft.com/en-us/windows/win32/api/minidumpapiset/nf-minidumpapiset-minidumpwritedump)
- Add PR check for formatting
- Basic scene system, to support both the debug screens and basic gameplay scene
- Store Bitmap inside renderer so we can easily access its dimensions when drawing
- CommandList class, replace std::vector<Command>
- Remove XAudio2 audio player, raw pointer stuff too annoying, need modern C++ audio lib
- Add basic animation system, apply it in the test screens
- Add way to specify which test screen page to open from command line
- Add test screens for rendering geomtry, sprites, and text
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
