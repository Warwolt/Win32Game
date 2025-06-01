#include <engine/input/mouse.h>

#include <engine/graphics/window.h>

#include <windows.h>

namespace engine {

	void update_mouse(Mouse* mouse, const MouseEvents& events, const Window& window) {
		auto key_is_pressed = [window](int virtual_key) -> bool {
			return window.is_focused() ? GetKeyState(virtual_key) & (1 << 16) : false;
		};

		POINT position;
		GetCursorPos(&position);
		mouse->x = events.mouse_x.value_or(mouse->x);
		mouse->y = events.mouse_y.value_or(mouse->y);
		mouse->mouse_wheel_delta = events.mouse_wheel_delta;

		mouse->left_button.update(key_is_pressed(VK_LBUTTON));
		mouse->right_button.update(key_is_pressed(VK_RBUTTON));
		mouse->middle_button.update(key_is_pressed(VK_MBUTTON));
		mouse->x1_button.update(key_is_pressed(VK_XBUTTON1));
		mouse->x2_button.update(key_is_pressed(VK_XBUTTON2));
	}

} // namespace engine
