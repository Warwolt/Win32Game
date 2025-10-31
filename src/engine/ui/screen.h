#pragma once

namespace engine {

    struct Input;
    class CommandList;
    class Renderer;

	class Screen {
	public:
		virtual ~Screen() {};
		virtual void update(const Input& input, CommandList* commands) = 0;
		virtual void draw(Renderer* renderer) const = 0;
	};

} // namespace engine
