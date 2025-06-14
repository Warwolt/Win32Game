#pragma once

#include <engine/debug/logging.h>

#include <windows.h>

#include <stdio.h>
#include <utility>

namespace engine {
#ifdef _DEBUG
	constexpr bool IS_DEBUG_BUILD = true;
#else
	constexpr bool IS_DEBUG_BUILD = false;
#endif
} // namespace engine

#define DEBUG_ASSERT(condition, ...)                                                \
	if (!(condition)) {                                                             \
		constexpr size_t SIZE = 512;                                                \
		char msg[SIZE];                                                             \
		int offset = sprintf_s(msg, SIZE, "DEBUG_ASSERT(%s) failed: ", #condition); \
		sprintf_s(msg + offset, SIZE - offset, __VA_ARGS__);                        \
		LOG_FATAL("%s", msg);                                                       \
		if constexpr (engine::IS_DEBUG_BUILD) {                                     \
			MessageBoxA(nullptr, msg, "Fatal Error", MB_OK | MB_ICONERROR);         \
			DebugBreak();                                                           \
			engine::debug_exit();                                                   \
		}                                                                           \
	}

#define DEBUG_FAIL(...)                                                     \
	{                                                                       \
		constexpr size_t SIZE = 512;                                        \
		char msg[SIZE];                                                     \
		int offset = sprintf_s(msg, SIZE, "DEBUG_FAIL: ");                  \
		sprintf_s(msg + offset, SIZE - offset, __VA_ARGS__);                \
		LOG_FATAL("%s", msg);                                               \
		if constexpr (engine::IS_DEBUG_BUILD) {                             \
			MessageBoxA(nullptr, msg, "Fatal Error", MB_OK | MB_ICONERROR); \
			DebugBreak();                                                   \
			engine::debug_exit();                                           \
		}                                                                   \
	}

namespace engine {
	void debug_exit();
} // namespace engine
