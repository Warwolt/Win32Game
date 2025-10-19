#pragma once

namespace engine {

	struct InputDevices;
	class Renderer;

	class FontTestScreen {
	public:
		void update(const InputDevices& input);
		void draw(Renderer* renderer) const;
	};

} // namespace engine
