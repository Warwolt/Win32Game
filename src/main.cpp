#include <application.h>

#include <chrono>
#include <format>
#include <stdio.h>

#include <dbghelp.h>
#include <windows.h>

static Application* g_application;

static LONG WINAPI write_crash_dump(EXCEPTION_POINTERS* exception_pointers) {
	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm local_tm {};
	localtime_s(&local_tm, &time);

	std::ostringstream oss;
	oss << std::put_time(&local_tm, "Win32Game_%Y-%m-%d_%H-%M") << ".dmp";
	std::string dump_filename = oss.str();

	HANDLE file = CreateFileA(
		dump_filename.c_str(),
		GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (file == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Error: failed to create dump file \"%s\". GetLastError() = %d\n", dump_filename.c_str(), GetLastError());
		return EXCEPTION_EXECUTE_HANDLER;
	}

	MINIDUMP_EXCEPTION_INFORMATION dump_info = {
		.ThreadId = GetCurrentThreadId(),
		.ExceptionPointers = exception_pointers,
		.ClientPointers = FALSE,
	};

	BOOL success = MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		file,
		MiniDumpWithFullMemory,
		&dump_info,
		nullptr,
		nullptr
	);

	CloseHandle(file);

	if (!success) {
		fprintf(stderr, "Error: failed to create dump file \"%s\". GetLastError() = %d\n", dump_filename.c_str(), GetLastError());
		return EXCEPTION_EXECUTE_HANDLER;
	}

	fprintf(stderr, "main.cpp:%d [FATAL]: Catastrophic error occured, created crash dump \"%s\"\n", __LINE__, dump_filename.c_str());

	return EXCEPTION_EXECUTE_HANDLER;
}

static LRESULT CALLBACK on_window_event(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (g_application) {
		return on_window_event(g_application, window, message, w_param, l_param);
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
	SetUnhandledExceptionFilter(write_crash_dump);
	SetProcessDPIAware();
	enable_printf();
	g_application = initialize_application(__argc, __argv, instance, on_window_event);

	/* Run */
	bool quit = false;
	while (!quit) {
		quit = update_application(g_application);
	}

	/* Quit */
	shutdown_application(g_application);
	return 0;
}
