#pragma once

#include <engine/scene/scene.h>

#include <engine/math/vec2.h>

#include <optional>
#include <vector>

namespace engine {

	struct Input;

	// Keeps track of most recently pressed key in a set of keyboard keys.
	//
	// Pressing a key puts it on top of the stack.
	// When releasing a key, it's removed from the stack.
	// Can be used for "NES Zelda" style 4-direction movement.
	class KeyboardStack {
	public:
		KeyboardStack(std::vector<int> keycodes);
		void update(const engine::Input& input);
		std::optional<int> top_keycode() const;

	private:
		std::vector<int> m_keycodes;
		std::vector<int> m_stack;
	};

} // namespace engine
