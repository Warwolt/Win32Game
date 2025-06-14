#pragma once

#include <engine/debug/logging.h>

#include <windows.h>

#include <stdio.h>
#include <utility>

#ifdef _DEBUG
#define DEBUG_ASSERT(condition, ...)                                                \
	if (!(condition)) {                                                             \
		constexpr size_t SIZE = 512;                                                \
		char msg[SIZE];                                                             \
		int offset = sprintf_s(msg, SIZE, "DEBUG_ASSERT(%s) failed: ", #condition); \
		sprintf_s(msg + offset, SIZE - offset, __VA_ARGS__);                        \
		LOG_FATAL("%s", msg);                                                       \
		::engine::debug_exit(msg);                                                  \
	}
#else
#define DEBUG_ASSERT(condition, ...) (void)(condition)
#endif

#ifdef _DEBUG
#define DEBUG_FAIL(...)                                      \
	{                                                        \
		constexpr size_t SIZE = 512;                         \
		char msg[SIZE];                                      \
		int offset = sprintf_s(msg, SIZE, "DEBUG_FAIL: ");   \
		sprintf_s(msg + offset, SIZE - offset, __VA_ARGS__); \
		LOG_FATAL("%s", msg);                                \
		::engine::debug_exit(msg);                           \
	}
#else
#define DEBUG_ASSERT(condition, ...) (void)(condition)
#endif

namespace engine {
	void debug_exit(const char* msg);
}