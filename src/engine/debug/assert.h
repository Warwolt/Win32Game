#pragma once

#include <engine/debug/logging.h>

#include <stdexcept>
#include <stdio.h>

#ifdef _DEBUG
#define DEBUG_ASSERT(condition, ...)              \
	if (!(condition)) {                           \
		char msg[256];                            \
		sprintf_s(msg, 256, __VA_ARGS__);        \
		LOG_FATAL("%s", msg);                     \
		throw std::logic_error::logic_error(msg); \
	}
#else
#define DEBUG_ASSERT(condition, ...) (void)(condition)
#endif