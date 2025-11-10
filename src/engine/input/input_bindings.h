#pragma once

#include <engine/input/button.h>

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

namespace engine {

	class Keyboard;

	// Hm, we want to map both Enter and Z key to the same action.
	// So, each keyboard action maps to one or more keyboard keys.
	// "ui_confirm" -> [VK_Z, VK_ENTER]

	class InputBindings {
	public:
		void add_keyboard_binding(std::string action_name, std::unordered_set<uint32_t> keys);
		void update(const Keyboard& keyboard);

		bool action_is_pressed(const std::string& action_name) const;
		bool action_is_released(const std::string& action_name) const;
		bool action_was_pressed_now(const std::string& action_name) const;
		bool action_was_released_now(const std::string& action_name) const;

	private:
		std::unordered_map<std::string, std::unordered_set<uint32_t>> m_keyboard_actions_keys;
		std::unordered_map<std::string, engine::Button> m_keyboard_action_states;
	};

} // namespace engine
