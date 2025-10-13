#include <application.h>

#include <stdio.h>

namespace application {

	State initialize(HINSTANCE instance, WNDPROC on_window_event) {
		// TODO:
		// Update CMakeLists.txt to build Source static library containing all source files
		// Call Window::initialize here
		// Update signature of this function to std::expected<State, std::string> to permit error returned

		return State {};
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
