#include <engine/debug/logging.h>

#include <ctime>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>

namespace engine {

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
	}

	void debug_log(LogLevel log_level, const char* fmt, ...) {
		if ((int)log_level < (int)g_log_level) {
			return;
		}

		char buffer[1024] = {};
		std::time_t t = std::time(0); // get time now
		std::tm* now = std::localtime(&t);
		int offset = 0;

		/* Add timestamp and log level */
		offset += snprintf(buffer + offset, 1024 - offset, "%02d-%02d-%02d ", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday); // year
		offset += snprintf(buffer + offset, 1024 - offset, "%d:%d ", now->tm_hour, now->tm_min);                             // hour
		offset += snprintf(buffer + offset, 1024 - offset, "%s ", log_level_to_str(log_level));                              // log level

		/* Add message */
		va_list args;
		va_start(args, fmt);
		offset += vsnprintf(buffer + offset, 1024, fmt, args);
		va_end(args);

		/* Output */
		printf("%s\n", buffer);
	}

} // namespace engine
