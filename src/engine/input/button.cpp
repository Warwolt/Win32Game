#include <engine/input/button.h>

namespace engine {

	void Button::tick(bool pressed) {
		m_just_changed = m_pressed != pressed;
		m_pressed = pressed;
	}

	bool Button::is_pressed() const {
		return m_pressed;
	}

	bool Button::is_released() const {
		return !m_pressed;
	}

	bool Button::is_just_pressed() const {
		return m_pressed && m_just_changed;
	}

	bool Button::is_just_released() const {
		return !m_pressed && m_just_changed;
	}

} // namespace engine
