#include <engine/input/mouse.h>

#include <windows.h>

namespace engine {

	void update_mouse(Mouse* mouse, int16_t mouse_wheel_delta) {
		POINT position;
		GetCursorPos(&position);
		mouse->x = (int16_t)position.x;
		mouse->y = (int16_t)position.y;
		mouse->mouse_wheel_delta = mouse_wheel_delta;

		mouse->left_button.update(GetKeyState(VK_LBUTTON) & (1 << 16));
		mouse->right_button.update(GetKeyState(VK_RBUTTON) & (1 << 16));
		mouse->middle_button.update(GetKeyState(VK_MBUTTON) & (1 << 16));
		mouse->x1_button.update(GetKeyState(VK_XBUTTON1) & (1 << 16));
		mouse->x2_button.update(GetKeyState(VK_XBUTTON2) & (1 << 16));
	}

} // namespace engine
