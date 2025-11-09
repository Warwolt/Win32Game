#include <application.h>

#include <engine/windows/command_line.h>
#include <library/library_loader.h>

#include <windows.h>

#include <cstdlib>
#include <expected>
#include <string>

#define LOAD_FUNCTION(library, function, variable)                                       \
	if (void* fn = GetProcAddress(library, function)) {                                  \
		variable = (decltype(variable))(fn);                                             \
	}                                                                                    \
	else {                                                                               \
		return std::unexpected(std::format("Failed to load function \"{}\"", function)); \
	}

struct ApplicationLibrary : public library::Library {
	LRESULT (*on_window_event)(Application* application, HWND window, UINT message, WPARAM w_param, LPARAM l_param);
	Application* (*initialize_application)(int argc, char** argv, HINSTANCE instance, WNDPROC on_window_event);
	bool (*update_application)(Application*);
	void (*shutdown_application)(Application*);

	std::expected<void, std::string> load_functions(HINSTANCE library_handle) override {
		LOAD_FUNCTION(library_handle, "on_window_event", this->on_window_event);
		LOAD_FUNCTION(library_handle, "initialize_application", this->initialize_application);
		LOAD_FUNCTION(library_handle, "update_application", this->update_application);
		LOAD_FUNCTION(library_handle, "shutdown_application", this->shutdown_application);
		return {};
	}
};

static library::LibraryLoader g_loader = library::LibraryLoader(LIBRARY_NAME, "cmake --build build --target Library");
static ApplicationLibrary g_library;

Application* initialize_application(int argc, char** argv, HINSTANCE instance, WNDPROC on_window_event) {
	/* Load library */
	std::expected<void, std::string> load_result = g_loader.load_library(&g_library);
	if (!load_result) {
		fprintf(stderr, "Error loading library: %s\n", load_result.error().c_str());
		std::abort();
	}

	/* Initialize app */
	printf("Hot reloading enabled, hot reload with F7 key\n");
	return g_library.initialize_application(argc, argv, instance, on_window_event);
}

LRESULT CALLBACK on_window_event(Application* application, HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	/* Trigger hot reload */
	if (message == WM_KEYDOWN && w_param == VK_F7) {
		std::expected<void, std::string> result = g_loader.trigger_hot_reload();
		if (!result) {
			fprintf(stderr, "Error when triggering library hot reload: %s\n", result.error().c_str());
		}
	}

	/* Handle window event */
	return g_library.on_window_event(application, window, message, w_param, l_param);
}

bool update_application(Application* application) {
	/* Update hot reloading */
	std::expected<void, std::string> result = g_loader.update_hot_reloading(&g_library);
	if (!result) {
		fprintf(stderr, "Error when updating hot reloading: %s\n", result.error().c_str());
	}

	/* Update application */
	return g_library.update_application(application);
}

void shutdown_application(Application* application) {
	g_library.shutdown_application(application);
}
