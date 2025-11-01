#include <engine/input/keyboard.h>

namespace engine {

	void Keyboard::on_key_event(uint32_t key_id, bool pressed) {
		m_key_events.insert_or_assign(key_id, pressed );
		m_active_keys.insert(key_id);
	}

	void Keyboard::update() {
		for (uint32_t key_id : m_active_keys) {
			auto it = m_key_events.find(key_id);
			bool pressed = it != m_key_events.end() ? it->second : m_key_states[key_id].is_pressed();
			m_key_states[key_id].update(pressed);
		}
		m_key_events.clear();
	}

	bool Keyboard::key_is_pressed(uint32_t key_id) const {
		auto it = m_key_states.find(key_id);
		return it != m_key_states.end() ? it->second.is_pressed() : false;
	}

	bool Keyboard::key_is_released(uint32_t key_id) const {
		auto it = m_key_states.find(key_id);
		return it != m_key_states.end() ? it->second.is_released() : true;
	}

	bool Keyboard::key_was_pressed_now(uint32_t key_id) const {
		auto it = m_key_states.find(key_id);
		return it != m_key_states.end() ? it->second.was_pressed_now() : false;
	}

	bool Keyboard::key_was_released_now(uint32_t key_id) const {
		auto it = m_key_states.find(key_id);
		return it != m_key_states.end() ? it->second.was_released_now() : false;
	}

} // namespace engine
