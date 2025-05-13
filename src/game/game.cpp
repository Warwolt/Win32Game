#include <game/game.h>

#include <engine/input/input.h>
#include <engine/render/window.h>

namespace game {

	void update(GameState* game, const engine::InputDevices& input) {
		constexpr int16_t XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE = 7849; // from xinput.h

        /* Shift gradient horizontally */
        if (input.gamepad.left_stick_x > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			game->x_offset += input.gamepad.left_stick_x / 10000;
		}
		else if (input.gamepad.left_stick_x < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			game->x_offset += input.gamepad.left_stick_x / 10000;
		}

        /* Shift gradient vertically */
		if (input.gamepad.left_stick_y > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			game->y_offset -= input.gamepad.left_stick_y / 10000;
		}
		else if (input.gamepad.left_stick_y < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			game->y_offset -= input.gamepad.left_stick_y / 10000;
		}
	}

	void draw(engine::Bitmap* bitmap, const GameState& game) {
		struct Pixel {
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t padding;
		};

        // just draws a gradient right now
		int row_byte_size = bitmap->width * engine::Bitmap::BYTES_PER_PIXEL;
		uint8_t* current_row = (uint8_t*)bitmap->data;
		for (int y = 0; y < bitmap->height; y++) {
			for (int x = 0; x < bitmap->width; x++) {
				Pixel* pixel = (Pixel*)current_row + x;
				pixel->r = 0;
				pixel->g = (uint8_t)(y + game.y_offset);
				pixel->b = (uint8_t)(x + game.x_offset);
			}
			current_row += row_byte_size;
		}
	}

} // namespace game
