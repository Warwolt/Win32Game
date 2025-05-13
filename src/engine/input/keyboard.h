#pragma once

#include <engine/input/button.h>

#include <stdint.h>
#include <unordered_map>
#include <unordered_set>

namespace engine {

	class Keyboard {
	public:
		void on_key_event(uint32_t key_id, bool pressed);
		void update();

		bool key_is_pressed(uint32_t key_id) const;
		bool key_is_released(uint32_t key_id) const;
		bool key_was_pressed_now(uint32_t key_id) const;
		bool key_was_released_now(uint32_t key_id) const;

	private:
		std::unordered_map<uint32_t, bool> m_key_events;
		std::unordered_set<uint32_t> m_active_keys;
		std::unordered_map<uint32_t, engine::Button> m_key_states;
	};

} // namespace engine
