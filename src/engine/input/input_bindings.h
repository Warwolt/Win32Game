#pragma once

#include <engine/input/button.h>

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace engine {

	class Keyboard;

	// Hm, we want to map both Enter and Z key to the same action.
	// So, each keyboard action maps to one or more keyboard keys.
	// "ui_confirm" -> [VK_Z, VK_ENTER]

	class InputBindings {
	public:
		void add_keyboard_binding(std::string action_name, uint32_t key);
		void update(const Keyboard& keyboard);

		bool action_is_pressed(const std::string& action_name) const;
		bool action_is_released(const std::string& action_name) const;
		bool action_was_pressed_now(const std::string& action_name) const;
		bool action_was_released_now(const std::string& action_name) const;

	private:
		std::unordered_map<std::string, std::vector<uint32_t>> m_keyboard_actions;
		std::unordered_map<uint32_t, engine::Button> m_key_states;
	};

} // namespace engine
