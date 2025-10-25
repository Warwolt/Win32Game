#pragma once

namespace engine {

	struct Input;
	class Renderer;

	class Scene {
	public:
		virtual ~Scene() {};
		virtual void update(const Input& input) = 0;
		virtual void draw(Renderer* renderer) = 0;
	};

} // namespace engine
