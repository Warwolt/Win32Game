#include <application.h>

#include <windows.h>

static application::State g_state;

static LRESULT CALLBACK on_window_event(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	return application::on_window_event(&g_state, window, message, w_param, l_param);
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	/* Init */
	g_state = application::initialize(instance, on_window_event);

	/* Run */
	bool quit = false;
	while (!quit) {
		quit = application::update(&g_state);
	}
}
