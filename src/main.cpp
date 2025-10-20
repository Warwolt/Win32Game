#include <application.h>

#include <stdio.h>
#include <windows.h>

static State* g_state;

static LRESULT CALLBACK on_window_event(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (g_state) {
		return on_window_event(g_state, window, message, w_param, l_param);
	}
	return DefWindowProc(window, message, w_param, l_param);
}

static void enable_printf() {
	// If we're debugging, we're using the Visual Studio debug output window
	// printf won't work, but the LOG_ macros will print there
	if (IsDebuggerPresent()) {
		return;
	}

	/* Get console */
	bool has_console = AttachConsole(ATTACH_PARENT_PROCESS); // attach to parent terminal
	if (!has_console) {
		has_console = AllocConsole(); // create a new console window
	}

	/* Attach std streams */
	if (has_console) {
		FILE* fi = 0;
		freopen_s(&fi, "CONOUT$", "wt", stdout);
		freopen_s(&fi, "CONOUT$", "wt", stderr);
	}
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	/* Init */
	SetProcessDPIAware();
	enable_printf();
	g_state = initialize_application(instance, on_window_event);

	/* Run */
	bool quit = false;
	while (!quit) {
		quit = update_application(g_state);
	}

	/* Quit */
	shutdown_application(g_state);
	return 0;
}
