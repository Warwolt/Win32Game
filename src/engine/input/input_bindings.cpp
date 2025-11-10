#include <engine/input/input_bindings.h>

#include <engine/input/keyboard.h>

#include <algorithm>

namespace engine {
	void InputBindings::add_keyboard_binding(std::string action_name, std::unordered_set<uint32_t> keys) {
		m_keyboard_actions_keys[action_name] = keys;
	}

	void InputBindings::update(const Keyboard& keyboard) {
		for (auto& [action_name, action_keys] : m_keyboard_actions_keys) {
			bool action_is_pressed = std::any_of(action_keys.begin(), action_keys.end(), [&](uint32_t key) { return keyboard.key_is_pressed(key); });
			m_keyboard_action_states[action_name].update(action_is_pressed);
		}
	}

	bool InputBindings::action_is_pressed(const std::string& action_name) const {
		if (auto it = m_keyboard_action_states.find(action_name); it != m_keyboard_action_states.end()) {
			return it->second.is_pressed();
		}
		return false;
	}

	bool InputBindings::action_is_released(const std::string& action_name) const {
		if (auto it = m_keyboard_action_states.find(action_name); it != m_keyboard_action_states.end()) {
			return it->second.is_released();
		}
		return true;
	}

	bool InputBindings::action_was_pressed_now(const std::string& action_name) const {
		if (auto it = m_keyboard_action_states.find(action_name); it != m_keyboard_action_states.end()) {
			return it->second.was_pressed_now();
		}
		return false;
	}

	bool InputBindings::action_was_released_now(const std::string& action_name) const {
		if (auto it = m_keyboard_action_states.find(action_name); it != m_keyboard_action_states.end()) {
			return it->second.was_released_now();
		}
		return false;
	}

} // namespace engine
