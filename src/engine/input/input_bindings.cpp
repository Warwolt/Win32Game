#include <engine/input/input_bindings.h>

#include <engine/input/keyboard.h>

namespace engine {

	void InputBindings::add_keyboard_binding(std::string action_name, uint32_t key) {
	}

	void InputBindings::update(const Keyboard& keyboard) {
	}

	bool InputBindings::action_is_pressed(const std::string& action_name) const {
		return false;
	}

	bool InputBindings::action_is_released(const std::string& action_name) const {
		return false;
	}

	bool InputBindings::action_was_pressed_now(const std::string& action_name) const {
		return false;
	}

	bool InputBindings::action_was_released_now(const std::string& action_name) const {
		return false;
	}

} // namespace engine
