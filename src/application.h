#pragma once

#include <engine/engine.h>
#include <game/game.h>

#include <windows.h>

namespace application {

	struct State {
		bool is_initialized = false;
		engine::Engine engine;
		game::Game game;
	};

	State initialize(HINSTANCE instance, WNDPROC on_window_event);
	LRESULT CALLBACK on_window_event(State* state, HWND window, UINT message, WPARAM w_param, LPARAM l_param);
	bool update(State* state);

} // namespace application
