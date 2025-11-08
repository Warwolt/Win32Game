#pragma once

namespace game {
	struct GameData;
} // namespace game

namespace engine {

	struct Input;
	class CommandList;
	class Renderer;
	class ResourceManager;
	class SaveFile;

	class Scene {
	public:
		virtual ~Scene() = default;

		virtual void initialize(game::GameData* /*game*/, ResourceManager* /*resources*/, CommandList* /*commands*/) {}
		virtual void update(game::GameData* /*game*/, const Input& input, CommandList* commands) = 0;
		virtual void draw(Renderer* renderer) const = 0;

		virtual void on_pause() {}
		virtual void on_unpause() {}
		virtual void on_save_file_loaded(const SaveFile& /*save_file*/) {}
	};

} // namespace engine
