#include <engine/debug/assert.h>

#include <windows.h>

#include <utility>

namespace engine {

	void debug_exit(const char* msg) {
		MessageBoxA(nullptr, msg, "Fatal Error", MB_OK | MB_ICONERROR);
		DebugBreak();
		std::exit(1);
	}

} // namespace engine