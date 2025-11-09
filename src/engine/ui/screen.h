#pragma once

namespace game {
	struct GameData;
} // namespace game

namespace engine {

	struct Input;
	class CommandList;
	class Renderer;
	class ResourceManager;

	class Screen {
	public:
		virtual ~Screen() = default;
		virtual void initialize(game::GameData* /*game*/, ResourceManager* /*resources*/, CommandList* /*commands*/) {}
		virtual void update(game::GameData* game, const Input& input, CommandList* commands) = 0;
		virtual void draw(const game::GameData& game, Renderer* renderer) const = 0;
	};

} // namespace engine
