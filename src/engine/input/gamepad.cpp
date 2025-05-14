#include <engine/input/gamepad.h>

#include <stdio.h>
#include <windows.h>
#include <xinput.h>

namespace engine {

	static struct XInputDLL {
		DWORD (*XInputGetState)(DWORD dwUserIndex, XINPUT_STATE* pState) = [](DWORD, XINPUT_STATE*) -> DWORD { return ERROR_DEVICE_NOT_CONNECTED; };
		DWORD (*XInputSetState)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) = [](DWORD, XINPUT_VIBRATION*) -> DWORD { return ERROR_DEVICE_NOT_CONNECTED; };
	} g_xinput_dll;

	void initialize_gamepad_support() {
		if (HMODULE xinput_module = LoadLibraryA("xinput1_4.dll")) {
			g_xinput_dll = {
				.XInputGetState = (decltype(XInputDLL::XInputGetState))GetProcAddress(xinput_module, "XInputGetState"),
				.XInputSetState = (decltype(XInputDLL::XInputSetState))GetProcAddress(xinput_module, "XInputSetState"),
			};
		}
		else {
			fprintf(stderr, "Error, failed to load XInput library!\n");
		}
	}

	void update_gamepad(Gamepad* gamepad, int index) {
		XINPUT_STATE controller_state = {};
		if (g_xinput_dll.XInputGetState(index, &controller_state) == ERROR_SUCCESS) {
			gamepad->dpad_up.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
			gamepad->dpad_right.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
			gamepad->dpad_down.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
			gamepad->dpad_left.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
			gamepad->start_button.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
			gamepad->back_button.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
			gamepad->left_shoulder.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
			gamepad->right_shoulder.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
			gamepad->a_button.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
			gamepad->b_button.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
			gamepad->x_button.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
			gamepad->y_button.update(controller_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
			gamepad->left_stick_x = controller_state.Gamepad.sThumbLX;
			gamepad->left_stick_y = controller_state.Gamepad.sThumbLY;
			gamepad->is_connected = true;
		}
		else {
			gamepad->is_connected = false;
		}
	}

} // namespace engine
