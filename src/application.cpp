#include <application.h>

#include <stdio.h>

namespace application {

	State initialize(HINSTANCE instance, WNDPROC on_window_event) {
		using namespace engine;
		const IVec2 window_resolution = IVec2 { 256, 240 }; // NES resolution
		std::expected<Window, WindowError> window_result = Window::initialize(instance, on_window_event, window_resolution, "Game");
		if (!window_result) {
			// FIXME: log an error here
			// We need to figure out how to handle logging?
			std::abort();
		}
		return State {
			.window = window_result.value(),
		};
	}

	LRESULT CALLBACK on_window_event(
		State* state,
		HWND window,
		UINT message,
		WPARAM w_param,
		LPARAM l_param
	) {
		return DefWindowProc(window, message, w_param, l_param);
	}

	bool update(State* state) {
		return true;
	}

} // namespace application
