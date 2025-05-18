#include <engine/input/mouse.h>

#include <windows.h>

namespace engine {

	void update_mouse(Mouse* mouse, const MouseEvents& events) {
		POINT position;
		GetCursorPos(&position);
		mouse->x = events.mouse_x.value_or(mouse->x);
		mouse->y = events.mouse_y.value_or(mouse->y);
		mouse->mouse_wheel_delta = events.mouse_wheel_delta;

		mouse->left_button.update(GetKeyState(VK_LBUTTON) & (1 << 16));
		mouse->right_button.update(GetKeyState(VK_RBUTTON) & (1 << 16));
		mouse->middle_button.update(GetKeyState(VK_MBUTTON) & (1 << 16));
		mouse->x1_button.update(GetKeyState(VK_XBUTTON1) & (1 << 16));
		mouse->x2_button.update(GetKeyState(VK_XBUTTON2) & (1 << 16));
	}

} // namespace engine
