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
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition) {
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
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}

	return S_OK;
}

// from https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset) {
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;
}

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

		constexpr WORD BITS_PERS_SAMPLE = 16;
		constexpr DWORD SAMPLES_PER_SEC = 44100;
		constexpr double CYCLES_PER_SEC = 220.0;
		constexpr WORD AUDIO_BUFFER_SIZE_IN_CYCLES = 10;
		constexpr DWORD SAMPLES_PER_CYCLE = (DWORD)(SAMPLES_PER_SEC / CYCLES_PER_SEC);                  // 200 samples per cycle.
		constexpr DWORD AUDIO_BUFFER_SIZE_IN_SAMPLES = SAMPLES_PER_CYCLE * AUDIO_BUFFER_SIZE_IN_CYCLES; // 2,000 samples per buffer.

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

		// Create source voice using format and start it
		winrt::check_hresult(xaudio2_engine->CreateSourceVoice(&xaudio2_source_voice, &wave_format_ex));
		xaudio2_source_voice->Start(0);

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
		FindChunk(cowbell_file, fourccRIFF, chunk_size, chunk_position);
		DWORD filetype;
		ReadChunkData(cowbell_file, &filetype, sizeof(DWORD), chunk_position);
		if (filetype != fourccWAVE) {
			fprintf(stderr, "File wasn't right format");
			exit(1);
		}
		WAVEFORMATEX cowbell_format = {};
		FindChunk(cowbell_file, fourccFMT, chunk_size, chunk_position);
		ReadChunkData(cowbell_file, &cowbell_format, chunk_size, chunk_position);
		//fill out the audio data buffer with the contents of the fourccDATA chunk
		FindChunk(cowbell_file, fourccDATA, chunk_size, chunk_position);
		BYTE* cowbell_buffer = new BYTE[chunk_size];
		ReadChunkData(cowbell_file, cowbell_buffer, chunk_size, chunk_position);

		x_audio2_buffer.AudioBytes = chunk_size;       //size of the audio buffer in bytes
		x_audio2_buffer.pAudioData = cowbell_buffer;   //buffer containing audio data
		x_audio2_buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
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
			xaudio2_source_voice->Stop();
			xaudio2_source_voice->FlushSourceBuffers();                 // stop current sound
			xaudio2_source_voice->SubmitSourceBuffer(&x_audio2_buffer); // play sound
			xaudio2_source_voice->Start();
		}

		/* Render */
		HDC device_context = GetDC(g_context.window.handle);
		game::draw(&g_context.window.bitmap, g_context.game);
		engine::render_window(g_context.window, device_context);
		ReleaseDC(g_context.window.handle, device_context);
	}

	return 0;
}
