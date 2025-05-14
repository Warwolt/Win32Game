#include <engine/debug/logging.h>

#include <stdio.h>
#include <windows.h>

namespace engine {

	void initialize_printf() {
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

} // namespace engine
