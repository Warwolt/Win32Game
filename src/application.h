#pragma once

#include <windows.h>

#ifdef LINK_APPLICATION_DYNAMICALLY
#define APPLICATION_API __declspec(dllexport)
#else
#define APPLICATION_API
#endif

struct State;

extern "C" APPLICATION_API LRESULT CALLBACK on_window_event(State* state, HWND window, UINT message, WPARAM w_param, LPARAM l_param);
extern "C" APPLICATION_API State* initialize_application(int argc, char** argv, HINSTANCE instance, WNDPROC on_window_event);
extern "C" APPLICATION_API bool update_application(State* state);
extern "C" APPLICATION_API void shutdown_application(State* state);
