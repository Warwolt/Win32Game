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
