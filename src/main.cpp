#include <engine/audio/audio_player.h>
#include <engine/debug/logging.h>
#include <engine/graphics/color.h>
#include <engine/graphics/window.h>
#include <engine/input/gamepad.h>
#include <engine/input/input.h>
#include <engine/input/keyboard.h>
#include <engine/input/mouse.h>
#include <engine/math/ivec2.h>
#include <game/game.h>

#include <format>
#include <variant>
#include <vector>
#include <windows.h>
#include <windowsx.h>

namespace engine {

	struct Assets {
		struct Audio {
			engine::AudioID cowbell;
		} audio;
	};

	class Renderer {
	public:
		void clear_screen();
		void draw_pixel(int32_t x, int32_t y, Color color);
		void draw_line(IVec2 start, IVec2 end, Color color);
		void render(engine::Bitmap* bitmap);

	private:
		struct ClearScreen {
		};
		struct DrawPixel {
			int32_t x;
			int32_t y;
			Color color;
		};
		struct DrawLine {
			IVec2 start;
			IVec2 end;
			Color color;
		};
		using DrawCommand = std::variant<
			ClearScreen,
			DrawPixel,
			DrawLine>;
		std::vector<DrawCommand> m_draw_commands;

		void _clear_screen(engine::Bitmap* bitmap);
		void _put_pixel(engine::Bitmap* bitmap, int32_t x, int32_t y, Color color);
		void _put_line(engine::Bitmap* bitmap, IVec2 start, IVec2 end, Color color);
	};

	void Renderer::clear_screen() {
		m_draw_commands.push_back(ClearScreen {});
	}

	void Renderer::draw_pixel(int32_t x, int32_t y, Color color) {
		m_draw_commands.push_back(DrawPixel { x, y, color });
	}

	void Renderer::draw_line(IVec2 start, IVec2 end, Color color) {
		m_draw_commands.push_back(DrawLine { start, end, color });
	}

	void Renderer::render(engine::Bitmap* bitmap) {
		for (DrawCommand& command : m_draw_commands) {
			if (auto* clear_screen = std::get_if<ClearScreen>(&command)) {
				_clear_screen(bitmap);
			}
			if (auto* draw_pixel = std::get_if<DrawPixel>(&command)) {
				_put_pixel(bitmap, draw_pixel->x, draw_pixel->y, draw_pixel->color);
			}
			if (auto* draw_line = std::get_if<DrawLine>(&command)) {
				_put_line(bitmap, draw_line->start, draw_line->end, draw_line->color);
			}
		}
		m_draw_commands.clear();
	}

	void Renderer::_clear_screen(engine::Bitmap* bitmap) {
		ZeroMemory(bitmap->data, bitmap->width * bitmap->height * sizeof(int32_t));
	}

	void Renderer::_put_pixel(engine::Bitmap* bitmap, int32_t x, int32_t y, Color color) {
		bitmap->put(x, y, BGRPixel { color.b, color.g, color.r });
	}

	void Renderer::_put_line(engine::Bitmap* bitmap, IVec2 start, IVec2 end, Color color) {
		// vertical line
		if (start.x == end.x) {
			int32_t y0 = min(start.y, end.y);
			int32_t y1 = max(start.y, end.y);
			for (int32_t y = y0; y <= y1; y++) {
				_put_pixel(bitmap, start.x, y, color);
			}
		}
		// sloped line
		else {
			// delta is the longer side of the triangle formed by the line
			// if dx is greater, x_step will be +1 or -1 and y_step will be the slope
			// if dy is greater, we flip it along the diagonal
			int32_t dx = end.x - start.x;
			int32_t dy = end.y - start.y;
			int32_t delta = max(std::abs(dx), std::abs(dy));
			float x_step = (float)dx / (float)delta;
			float y_step = (float)dy / (float)delta;
			for (int32_t i = 0; i <= delta; i++) {
				_put_pixel(bitmap, (int32_t)(start.x + i * x_step), (int32_t)(start.y + i * y_step), color);
			}
		}
	}

} // namespace engine

struct ProgramContext {
	bool should_quit;
	engine::MouseEvents mouse_events;
	engine::Window window;
	engine::InputDevices input;
	engine::AudioPlayer audio;
	engine::Assets assets;
	game::GameState game;
};

static ProgramContext g_context;

static engine::Window initialize_window_or_abort(HINSTANCE instance, WNDPROC wnd_proc, const char* window_title) {
	std::expected<engine::Window, engine::WindowError> window_result = engine::initialize_window(instance, wnd_proc, window_title);
	if (!window_result.has_value()) {
		std::string message = std::format("Couldn't create window: {}", engine::window_error_to_str(window_result.error()));
		MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	return window_result.value();
}

static void pump_window_messages() {
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
		if (message.message == WM_QUIT) {
			g_context.should_quit = true;
		}
	}
}

static LRESULT CALLBACK on_window_event(
	HWND window,
	UINT message,
	WPARAM w_param,
	LPARAM l_param
) {
	switch (message) {
		case WM_SIZE: {
			if (window == g_context.window.handle) {
				engine::on_window_resized(&g_context.window);
			}
		} break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.input.keyboard.on_key_event(key_id, true);
		} break;

		case WM_SYSKEYUP:
		case WM_KEYUP: {
			uint32_t key_id = (uint32_t)w_param;
			g_context.input.keyboard.on_key_event(key_id, false);
		} break;

		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_MOUSEWHEEL: {
			g_context.mouse_events.mouse_wheel_delta += GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;
		} break;

		case WM_MOUSEMOVE: {
			g_context.mouse_events.mouse_x = (int16_t)GET_X_LPARAM(l_param);
			g_context.mouse_events.mouse_y = (int16_t)GET_Y_LPARAM(l_param);
		} break;

		case WM_PAINT: {
			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			game::draw(&g_context.window.bitmap, g_context.game);
			engine::render_window(g_context.window, device_context);
			EndPaint(window, &paint);
		} break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

static engine::AudioID load_audio_from_file(const char* path) {
	HANDLE cowbell_file = CreateFileA(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	std::expected<engine::AudioID, std::string> result = g_context.audio.add_audio_from_file(cowbell_file);
	CloseHandle(cowbell_file);

	if (!result.has_value()) {
		fprintf(stderr, "Failed to load audio file \"%s\": %s\n", path, result.error().c_str());
		exit(1);
	}

	return result.value();
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	engine::initialize_logging(engine::LogLevel::Debug);
	engine::initialize_gamepad_support();
	g_context.window = initialize_window_or_abort(instance, on_window_event, "Game");
	g_context.audio = engine::initialize_audio_player();
	g_context.assets.audio.cowbell = load_audio_from_file("assets/audio/808_cowbell.wav");

	engine::Renderer renderer;

	/* Main loop */
	while (!g_context.should_quit) {
		/* Input */
		pump_window_messages();
		engine::update_input_devices(&g_context.input, g_context.mouse_events);
		g_context.mouse_events = {};

		/* Update */
		game::update(&g_context.game, g_context.input);

		// quick quit while prototyping
		if (g_context.input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			g_context.should_quit = true;
		}

		// trigger sound with keyboard
		if (g_context.input.keyboard.key_was_pressed_now('1')) {
			g_context.audio.play(g_context.assets.audio.cowbell);
		}

		/* Render */
		renderer.clear_screen();
		engine::IVec2 window_center = {
			g_context.window.bitmap.width / 2,
			g_context.window.bitmap.height / 2
		};
		engine::IVec2 start = window_center + engine::IVec2 { 0, 0 };
		engine::IVec2 end = engine::IVec2 { g_context.input.mouse.x, g_context.input.mouse.y };
		engine::Color color = { 0, 255, 0, 255 };
		renderer.draw_line(start, end, color);

		HDC device_context = GetDC(g_context.window.handle);
		game::draw(&g_context.window.bitmap, g_context.game);
		renderer.render(&g_context.window.bitmap);
		engine::render_window(g_context.window, device_context);
		ReleaseDC(g_context.window.handle, device_context);
	}

	return 0;
}
