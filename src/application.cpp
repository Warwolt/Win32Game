#include <application.h>

namespace application {

	// Temporary hack to bootstrap stuff, we need to be able to printf
	// Should later be handled entirely by the application
	static void initialize_logging() {
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

		/* Enable colors */
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	}

	State initialize() {
        initialize_logging();
		return State {
		};
	}

	bool update(State* state) {
		return true;
	}

} // namespace application
