#pragma once

namespace engine {

	struct Input;
	class CommandList;
	class Renderer;
	class ResourceManager;
	class SaveFile;

	class Scene {
	public:
		virtual ~Scene() = default;

		virtual void initialize(ResourceManager* /*resources*/, CommandList* /*commands*/) {}
		virtual void update(const Input& input, CommandList* commands) = 0;
		virtual void draw(Renderer* renderer) const = 0;

		virtual void on_pause() {}
		virtual void on_unpause() {}
	};

} // namespace engine
