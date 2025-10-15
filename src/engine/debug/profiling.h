#pragma once

#include <tracy/Tracy.hpp>

// Mark end of current frame
#define CPUProfilingEndFrame() FrameMark

// Add tracy zone to the current scope
#define CPUProfilingScope() ZoneScoped

// Add tracy zone with specified color.
// Color can be chosen from common/TracyColor.hpp
// Visual reference: https://en.wikipedia.org/wiki/X11_color_names.
#define CPUProfilingScopeColor(color) ZoneScopedC(color)

#define CPUProfilingScope_Application() CPUProfilingScopeColor(tracy::Color::MediumVioletRed)
#define CPUProfilingScope_Game() CPUProfilingScopeColor(tracy::Color::DodgerBlue1)
#define CPUProfilingScope_Engine() CPUProfilingScopeColor(tracy::Color::Orange2)
#define CPUProfilingScope_Render() CPUProfilingScopeColor(tracy::Color::MediumSeaGreen)
