#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK on_window_event(
	HWND window,
	UINT message,
	WPARAM w_param,
	LPARAM l_param
) {
	LRESULT result = 0;

	switch (message) {
		case WM_SIZE: {
			/* handle resized */
			printf("WM_SIZE\n");
		} break;

		case WM_DESTROY: {
			printf("WM_DESTROY\n");
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_CLOSE: {
			printf("WM_CLOSE\n");
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_ACTIVATEAPP: {
			/* app has been actived */
		} break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	/* Register window class */
	WNDCLASSW window_class = {
		.style =
			CS_OWNDC // give this window a unique device context
			| CS_HREDRAW | CS_VREDRAW, // redraw window when resized
		.lpfnWndProc = on_window_event,
		.hInstance = instance,
		.lpszClassName = L"HandmadeHeroWindowClass",
	};
	bool register_result = RegisterClassW(&window_class);
	if (!register_result) {
		// TODO handle and abort
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
	}

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
