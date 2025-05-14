#include <engine/input/gamepad.h>
#include <engine/input/input.h>
#include <engine/input/keyboard.h>
#include <engine/logging.h>
#include <engine/render/window.h>
#include <game/game.h>

#include <format>
#include <windows.h>

#include <winrt/base.h>
#include <xaudio2.h>

struct ProgramContext {
	bool should_quit;
	engine::Window window;
	engine::InputDevices input;
	game::GameState game;
};

static ProgramContext g_context;

engine::Window initialize_window_or_abort(HINSTANCE instance, WNDPROC wnd_proc) {
	std::expected<engine::Window, engine::WindowError> window_result = engine::initialize_window(instance, wnd_proc);
	if (!window_result.has_value()) {
		std::string message = std::format("Couldn't create window: {}", engine::window_error_to_str(window_result.error()));
		MessageBoxA(0, message.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	return window_result.value();
}

void pump_window_messages() {
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
		if (message.message == WM_QUIT) {
			g_context.should_quit = true;
		}
	}
}

LRESULT CALLBACK on_window_event(
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

		case WM_PAINT: {
			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);
			{
				game::draw(&g_context.window.bitmap, g_context.game);
				engine::render_window(g_context.window, device_context);
			}
			EndPaint(window, &paint);
		} break;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE /*prev_instance*/,
	LPSTR /*command_line*/,
	int /*command_show*/
) {
	engine::initialize_printf();
	engine::initialize_gamepad_support();
	g_context.window = initialize_window_or_abort(instance, on_window_event);

	// Initialize audio
	winrt::com_ptr<IXAudio2> xaudio2_engine = {};
	IXAudio2MasteringVoice* xaudio2_mastering_voice = {};
	IXAudio2SourceVoice* xaudio2_source_voice = {};
	XAUDIO2_BUFFER x_audio2_buffer = {};
	{
		// code based on https://learn.microsoft.com/en-us/windows/win32/xaudio2/full-project

		// Constant literals.
		constexpr WORD BITS_PERS_SAMPLE = 16;            // 16 bits per sample.
		constexpr DWORD SAMPLES_PER_SEC = 44100;         // 44,100 samples per second.
		constexpr double CYCLES_PER_SEC = 110.0;         // 440 cycles per second (frequency of the audible tone).
		constexpr double VOLUME = 0.5;                   // 50% volume.
		constexpr WORD AUDIO_BUFFER_SIZE_IN_CYCLES = 10; // 10 cycles per audio buffer.
		constexpr double PI = 3.14159265358979323846;

		// Calculated constants.
		constexpr DWORD SAMPLES_PER_CYCLE = (DWORD)(SAMPLES_PER_SEC / CYCLES_PER_SEC);                     // 200 samples per cycle.
		constexpr DWORD AUDIO_BUFFER_SIZE_IN_SAMPLES = SAMPLES_PER_CYCLE * AUDIO_BUFFER_SIZE_IN_CYCLES;    // 2,000 samples per buffer.
		constexpr UINT32 AUDIO_BUFFER_SIZE_IN_BYTES = AUDIO_BUFFER_SIZE_IN_SAMPLES * BITS_PERS_SAMPLE / 8; // 4,000 bytes per

		// FIXME: double check that we actually need to do this?
		// Initialize COM
		HRESULT com_result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(com_result)) {
			fprintf(stderr, "failed to initialize COM for audio, aborting");
			exit(1);
		}

		// Initialize engine and master voice
		winrt::check_hresult(XAudio2Create(xaudio2_engine.put(), 0, XAUDIO2_DEFAULT_PROCESSOR));
		winrt::check_hresult(xaudio2_engine->CreateMasteringVoice(&xaudio2_mastering_voice));

		// Define a format
		WAVEFORMATEX wave_format_ex = {};
		wave_format_ex.wFormatTag = WAVE_FORMAT_PCM;
		wave_format_ex.nChannels = 1; // 1 channel
		wave_format_ex.nSamplesPerSec = SAMPLES_PER_SEC;
		wave_format_ex.nBlockAlign = wave_format_ex.nChannels * BITS_PERS_SAMPLE / 8;
		wave_format_ex.nAvgBytesPerSec = wave_format_ex.nSamplesPerSec * wave_format_ex.nBlockAlign;
		wave_format_ex.wBitsPerSample = BITS_PERS_SAMPLE;
		wave_format_ex.cbSize = 0;

		// Create source voice using format
		winrt::check_hresult(xaudio2_engine->CreateSourceVoice(&xaudio2_source_voice, &wave_format_ex));

		// Fill a buffer
		std::array<byte, AUDIO_BUFFER_SIZE_IN_BYTES> sinewave = {};
		double phase = 0;
		uint32_t buffer_index = 0;
		while (buffer_index < AUDIO_BUFFER_SIZE_IN_BYTES) {
			phase += (2 * PI) / SAMPLES_PER_CYCLE;
			int16_t sample = (int16_t)(sin(phase) * INT16_MAX * VOLUME);
			sinewave[buffer_index++] = (byte)sample; // values are little-endian
			sinewave[buffer_index++] = (byte)(sample >> 8);
		}

		// Submit the buffer to the source voice, and start the voice.
		x_audio2_buffer = {
			.Flags = XAUDIO2_END_OF_STREAM,
			.AudioBytes = AUDIO_BUFFER_SIZE_IN_BYTES,
			.pAudioData = sinewave.data(),
		};
		winrt::check_hresult(xaudio2_source_voice->SubmitSourceBuffer(&x_audio2_buffer));
	}

	/* Main loop */
	while (!g_context.should_quit) {
		/* Input */
		pump_window_messages();
		engine::update_input_devices(&g_context.input);

		/* Update */
		game::update(&g_context.game, g_context.input);
		if (g_context.input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			g_context.should_quit = true;
		}

		if (g_context.input.keyboard.key_was_pressed_now('A')) {
			xaudio2_source_voice->SubmitSourceBuffer(&x_audio2_buffer);
			xaudio2_source_voice->Start(0);
		}

		/* Render */
		HDC device_context = GetDC(g_context.window.handle);
		game::draw(&g_context.window.bitmap, g_context.game);
		engine::render_window(g_context.window, device_context);
		ReleaseDC(g_context.window.handle, device_context);
	}

	return 0;
}
