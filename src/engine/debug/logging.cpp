#include <engine/debug/logging.h>

#include <ctime>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>

#pragma warning(disable: 4005) // consoleapi2.h from windows.h has conflicting macros

namespace engine {

// foreground color
#define COLOR_BLACK "\033[30m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_PURPLE "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"
#define COLOR_RESET "\033[0m"

// background color
#define BACKGROUND_BLACK "\033[40m"
#define BACKGROUND_RED "\033[41m"
#define BACKGROUND_GREEN "\033[42m"
#define BACKGROUND_YELLOW "\033[43m"
#define BACKGROUND_BLUE "\033[44m"
#define BACKGROUND_PURPLE "\033[45m"
#define BACKGROUND_CYAN "\033[46m"
#define BACKGROUND_WHITE "\033[47m"

	static LogLevel g_log_level;

	static const char* log_level_to_str(LogLevel level) {
		switch (level) {
			case LogLevel::Debug:
				return "[ DEBUG ]";
			case LogLevel::Info:
				return "[ INFO  ]";
			case LogLevel::Warning:
				return "[WARNING]";
			case LogLevel::Error:
				return "[ ERROR ]";
			case LogLevel::Fatal:
				return "[ FATAL ]";
		}
		return "";
	}

	static const char* log_level_color(LogLevel level) {
		switch (level) {
			case LogLevel::Debug:
				return COLOR_GREEN;
			case LogLevel::Info:
				return "";
			case LogLevel::Warning:
				return COLOR_YELLOW;
			case LogLevel::Error:
				return COLOR_RED;
			case LogLevel::Fatal:
				return BACKGROUND_RED COLOR_BLACK;
		}
		return "";
	}

	void initialize_logging(LogLevel level) {
		g_log_level = level;

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

	void debug_log(LogLevel log_level, const char* fmt, ...) {
		char buffer[1024] = {};
		int offset = 0;

		/* Check log level */
		if ((int)log_level < (int)g_log_level) {
			return;
		}

		/* Add color */
		offset += snprintf(buffer + offset, 1024 - offset, log_level_color(log_level));

		/* Add timestamp */
		std::time_t t = std::time(0); // get time now
		std::tm* now = std::localtime(&t);
		offset += snprintf(buffer + offset, 1024 - offset, "%02d-%02d-%02d ", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday); // year
		offset += snprintf(buffer + offset, 1024 - offset, "%d:%d ", now->tm_hour, now->tm_min);                                   // hour

		/* Log level */
		offset += snprintf(buffer + offset, 1024 - offset, "%s ", log_level_to_str(log_level)); // log level

		/* Add message */
		va_list args;
		va_start(args, fmt);
		offset += vsnprintf(buffer + offset, 1024, fmt, args);
		va_end(args);

		/* Output */
		printf("%s%s\n", buffer, COLOR_RESET);
	}

} // namespace engine
