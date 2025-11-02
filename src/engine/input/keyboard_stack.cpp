#include <engine/input/keyboard_stack.h>

#include <engine/input/input.h>

namespace engine {

	KeyboardStack::KeyboardStack(std::vector<int> keycodes)
		: m_keycodes(keycodes) {
	}

	void KeyboardStack::update(const engine::Input& input) {
		for (int keycode : m_keycodes) {
			if (input.keyboard.key_was_pressed_now(keycode)) {
				m_stack.push_back(keycode);
			}
			if (input.keyboard.key_was_released_now(keycode)) {
				std::erase(m_stack, keycode);
			}
		}
	}

	std::optional<int> KeyboardStack::top_keycode() const {
		return m_stack.empty() ? std::nullopt : std::make_optional(m_stack.back());
	}

} // namespace engine
