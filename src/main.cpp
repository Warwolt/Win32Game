#include <optional>
#include <stdio.h>
#include <windows.h>

namespace core {
	template<typename T, typename F>
	T unwrap(std::optional<T>&& optional, F&& on_error) {
		if (optional) {
			return *optional;
		}
		on_error();
		throw std::bad_optional_access();
	}
} // namespace core

LRESULT CALLBACK on_window_event(
	HWND window,
	UINT message,
	WPARAM w_param,
	LPARAM l_param
) {
	LRESULT result = 0;

	switch (message) {
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_PAINT: {
			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			{
				PatBlt(
					device_context, // HDC hdc
					paint.rcPaint.left, // int x
					paint.rcPaint.top, // int y
					paint.rcPaint.right - paint.rcPaint.left, // int w
					paint.rcPaint.bottom - paint.rcPaint.top, // int h
					BLACKNESS // DWORD rop
				);
			}
			EndPaint(window, &paint);
		} break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

std::optional<HWND> initialize_window(HINSTANCE instance) {
	/* Register window class */
	WNDCLASSW window_class = {
		.style =
			CS_OWNDC // give this window a unique device context
			| CS_HREDRAW | CS_VREDRAW, // redraw window when resized
		.lpfnWndProc = on_window_event,
		.hInstance = instance,
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.lpszClassName = L"HandmadeHeroWindowClass",
	};
	if (!RegisterClassW(&window_class)) {
		fprintf(stderr, "failed to register window class, aborting");
		return std::nullopt;
	}

	/* Create window */
	HWND window_handle = CreateWindowExW(
		0, // DWORD dwExStyle
		window_class.lpszClassName, // LPCWSTR lpClassName
		L"Handmade Hero", // LPCWSTR lpWindowName
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, // DWORD dwStyle
		CW_USEDEFAULT, // int X
		CW_USEDEFAULT, // int Y
		CW_USEDEFAULT, // int nWidth
		CW_USEDEFAULT, // int nHeight
		0, // HWND hWndParent
		0, // HMENU hMenu
		instance, // HINSTANCE hInstance
		0 // LPVOID lpParam
	);
	if (!window_handle) {
		// TODO handle and abort
		return std::nullopt;
	}

	return window_handle;
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	/* Create window */
	HWND window_handle = core::unwrap(initialize_window(instance), [] {
		fprintf(stderr, "Couldn't initialize window");
	});

	/* Handle messages */
	while (true) {
		MSG message;
		if (GetMessageW(&message, 0, 0, 0) <= 0) {
			break;
		}

		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return 0;
}
