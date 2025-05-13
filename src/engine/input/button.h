#pragma once

#include <optional>

namespace engine {

	class Button {
	public:
		void update(bool pressed);
		bool is_pressed() const;
		bool is_released() const;
		bool was_pressed_now() const;
		bool was_released_now() const;

	private:
		bool m_pressed = false;
		bool m_just_changed = false;
	};

} // namespace engine
