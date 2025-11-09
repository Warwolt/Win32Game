#pragma once

#include <windows.h>

#ifdef LINK_APPLICATION_DYNAMICALLY
#define APPLICATION_API __declspec(dllexport)
#else
#define APPLICATION_API
#endif

struct Application;

extern "C" APPLICATION_API LRESULT CALLBACK on_window_event(Application* application, HWND window, UINT message, WPARAM w_param, LPARAM l_param);
extern "C" APPLICATION_API Application* initialize_application(int argc, char** argv, HINSTANCE instance, WNDPROC on_window_event);
extern "C" APPLICATION_API bool update_application(Application* application);
extern "C" APPLICATION_API void shutdown_application(Application* application);
