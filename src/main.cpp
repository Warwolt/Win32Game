#include <application.h>

#include <windows.h>

#include <stdio.h>

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	initialize_logging();
	printf("Hello world\n");

	application::State state = application::initialize();
	bool quit = false;
	while (!quit) {
		quit = application::update(&state);
	}

	return 0;
}
