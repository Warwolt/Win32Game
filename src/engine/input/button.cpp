#include <engine/input/button.h>

namespace engine {

	void Button::update(bool pressed) {
		m_just_changed = m_pressed != pressed;
		m_pressed = pressed;
	}

	bool Button::is_pressed() const {
		return m_pressed;
	}

	bool Button::is_released() const {
		return !m_pressed;
	}

	bool Button::was_pressed_now() const {
		return m_pressed && m_just_changed;
	}

	bool Button::was_released_now() const {
		return !m_pressed && m_just_changed;
	}

} // namespace engine
