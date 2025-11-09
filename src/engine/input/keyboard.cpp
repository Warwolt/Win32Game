#include <engine/input/keyboard.h>

namespace engine {

	void Keyboard::on_key_event(uint32_t key, bool pressed) {
		m_key_events.insert_or_assign(key, pressed);
		m_active_keys.insert(key);
	}

	void Keyboard::update() {
		for (uint32_t key : m_active_keys) {
			auto it = m_key_events.find(key);
			bool pressed = it != m_key_events.end() ? it->second : m_key_states[key].is_pressed();
			m_key_states[key].update(pressed);
		}
		m_key_events.clear();
	}

	bool Keyboard::key_is_pressed(uint32_t key) const {
		auto it = m_key_states.find(key);
		return it != m_key_states.end() ? it->second.is_pressed() : false;
	}

	bool Keyboard::key_is_released(uint32_t key) const {
		auto it = m_key_states.find(key);
		return it != m_key_states.end() ? it->second.is_released() : true;
	}

	bool Keyboard::key_was_pressed_now(uint32_t key) const {
		auto it = m_key_states.find(key);
		return it != m_key_states.end() ? it->second.was_pressed_now() : false;
	}

	bool Keyboard::key_was_released_now(uint32_t key) const {
		auto it = m_key_states.find(key);
		return it != m_key_states.end() ? it->second.was_released_now() : false;
	}

} // namespace engine
