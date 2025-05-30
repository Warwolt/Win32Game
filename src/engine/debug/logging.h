#pragma once

namespace engine {

	enum class LogLevel {
		Debug = 0,
		Info = 1,
		Warning = 2,
		Error = 3,
		Fatal = 4,
	};

	void initialize_logging(LogLevel level);
	void debug_log(LogLevel level, const char* fmt, ...);

} // namespace engine

#define LOG_DEBUG(...) engine::debug_log(engine::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...) engine::debug_log(engine::LogLevel::Info, __VA_ARGS__)
#define LOG_WARNING(...) engine::debug_log(engine::LogLevel::Warning, __VA_ARGS__)
#define LOG_ERROR(...) engine::debug_log(engine::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL(...) engine::debug_log(engine::LogLevel::Fatal, __VA_ARGS__)

// clang-format off
#define LOG_DEBUG_IF(condition, ...) do { if (condition) LOG_DEBUG(__VA_ARGS__); } while (0)
#define LOG_INFO_IF(condition, ...) do { if (condition) LOG_INFO(__VA_ARGS__); } while (0)
#define LOG_WARNING_IF(condition, ...) do { if (condition) LOG_WARNING(__VA_ARGS__); } while (0)
#define LOG_ERROR_IF(condition, ...) do { if (condition) LOG_ERROR(__VA_ARGS__); } while (0)
#define LOG_FATAL_IF(condition, ...) do { if (condition) LOG_FATAL(__VA_ARGS__); } while (0)
// clang-format on

// clang-format off
#define LOG_DEBUG_ONCE(...) do { static bool once = false; if (!once) LOG_DEBUG(__VA_ARGS__); once = true; } while (0)
#define LOG_INFO_ONCE(...) do { static bool once = false; if (!once) LOG_INFO(__VA_ARGS__); once = true; } while (0)
#define LOG_WARNING_ONCE(...) do { static bool once = false; if (!once) LOG_WARNING(__VA_ARGS__); once = true; } while (0)
#define LOG_ERROR_ONCE(...) do { static bool once = false; if (!once) LOG_ERROR(__VA_ARGS__); once = true; } while (0)
#define LOG_FATAL_ONCE(...) do { static bool once = false; if (!once) LOG_FATAL(__VA_ARGS__); once = true; } while (0)
// clang-format on
