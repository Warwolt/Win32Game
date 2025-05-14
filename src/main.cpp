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

// from https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
constexpr int fourccRIFF = 'FFIR';
constexpr int fourccDATA = 'atad';
constexpr int fourccFMT = ' tmf';
constexpr int fourccWAVE = 'EVAW';
constexpr int fourccXWMA = 'AMWX';
constexpr int fourccDPDS = 'sdpd';
static HRESULT find_chunk_in_file(HANDLE hFile, DWORD fourcc, DWORD* dwChunkSize, DWORD* dwChunkDataPosition) {
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK) {
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType) {
			case fourccRIFF:
				dwRIFFDataSize = dwChunkDataSize;
				dwChunkDataSize = 4;
				if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
					hr = HRESULT_FROM_WIN32(GetLastError());
				break;

			default:
				if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
					return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc) {
			*dwChunkSize = dwChunkDataSize;
			*dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}

	return S_OK;
}

// from https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
static HRESULT read_chunk_from_file(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset) {
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
}

namespace engine {

	class AudioPlayer {
	public:
		AudioPlayer() = default;
		friend AudioPlayer initialize_audio_player();

		// private:
		winrt::com_ptr<IXAudio2> m_audio_engine;
		IXAudio2MasteringVoice* m_mastering_voice;
		IXAudio2SourceVoice* m_source_voice;
	};

	AudioPlayer initialize_audio_player() {
		AudioPlayer audio_player;

		winrt::check_hresult(XAudio2Create(audio_player.m_audio_engine.put(), 0, XAUDIO2_DEFAULT_PROCESSOR));
		winrt::check_hresult(audio_player.m_audio_engine->CreateMasteringVoice(&audio_player.m_mastering_voice));

		// Define a format
		constexpr WORD NUMBER_OF_CHANNELS = 1;
		constexpr WORD BITS_PER_SAMPLE = 16;
		constexpr WORD SAMPLES_PER_SEC = 44100;
		constexpr WORD BITS_PER_BYTE = 8;
		constexpr WORD BLOCK_ALIGNMENT = NUMBER_OF_CHANNELS * BITS_PER_SAMPLE / BITS_PER_BYTE;
		WAVEFORMATEX wave_format_ex = {
			.wFormatTag = WAVE_FORMAT_PCM,
			.nChannels = NUMBER_OF_CHANNELS, // mono or stero
			.nSamplesPerSec = SAMPLES_PER_SEC,
			.nAvgBytesPerSec = SAMPLES_PER_SEC * BLOCK_ALIGNMENT,
			.nBlockAlign = BLOCK_ALIGNMENT,
			.wBitsPerSample = BITS_PER_SAMPLE,
			.cbSize = 0,
		};

		// Create source voice using format
		winrt::check_hresult(audio_player.m_audio_engine->CreateSourceVoice(&audio_player.m_source_voice, &wave_format_ex));

		return audio_player;
	}

} // namespace engine

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
	engine::AudioPlayer audio_player = engine::initialize_audio_player();
	std::unordered_map<uint32_t, XAUDIO2_BUFFER> audio_buffers;

	// Add cowbell audio
	uint32_t cowbell = 1;
	{
		// Load wave file
		HANDLE cowbell_file = CreateFileA(
			"assets/audio/808_cowbell.wav",
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (cowbell_file == INVALID_HANDLE_VALUE) {
			fprintf(stderr, "couldn't load wave file!\n");
			exit(1);
		}

		DWORD chunk_size;
		DWORD chunk_position;
		//check the file type, should be fourccWAVE or 'XWMA'
		find_chunk_in_file(cowbell_file, fourccRIFF, &chunk_size, &chunk_position);
		DWORD filetype;
		read_chunk_from_file(cowbell_file, &filetype, sizeof(DWORD), chunk_position);
		if (filetype != fourccWAVE) {
			fprintf(stderr, "File wasn't right format");
			exit(1);
		}

		//fill out the audio data buffer with the contents of the fourccDATA chunk
		find_chunk_in_file(cowbell_file, fourccDATA, &chunk_size, &chunk_position);
		BYTE* samples = new BYTE[chunk_size];
		read_chunk_from_file(cowbell_file, samples, chunk_size, chunk_position);

		XAUDIO2_BUFFER buffer = {
			.Flags = XAUDIO2_END_OF_STREAM,
			.AudioBytes = chunk_size,
			.pAudioData = samples,
		};
		audio_buffers.insert({ cowbell, buffer });
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

		// trigger sound with keyboard
		if (g_context.input.keyboard.key_was_pressed_now('1')) {
			audio_player.m_source_voice->Stop();
			audio_player.m_source_voice->FlushSourceBuffers();                        // stop current sound
			audio_player.m_source_voice->SubmitSourceBuffer(&audio_buffers[cowbell]); // play sound
			audio_player.m_source_voice->Start();
		}

		/* Render */
		HDC device_context = GetDC(g_context.window.handle);
		game::draw(&g_context.window.bitmap, g_context.game);
		engine::render_window(g_context.window, device_context);
		ReleaseDC(g_context.window.handle, device_context);
	}

	return 0;
}
