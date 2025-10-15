#pragma once

#include <tracy/Tracy.hpp>

// Mark end of current frame
#define CPUProfilingEndFrame() FrameMark

// Add tracy zone to the current scope
#define CPUProfilingScope() ZoneScoped
