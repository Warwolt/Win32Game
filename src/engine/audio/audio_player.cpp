#include <engine/audio/audio_player.h>

#include <winrt/base.h>
#include <xaudio2.h>

namespace engine {

	struct AudioPlayerContext {
		winrt::com_ptr<IXAudio2> audio_engine;
		IXAudio2MasteringVoice* mastering_voice;
		IXAudio2SourceVoice* source_voice;
		std::unordered_map<uint32_t, XAUDIO2_BUFFER> audio_buffers;
		uint32_t next_id = 1;
	};

	// from https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
	constexpr int fourccRIFF = 'FFIR';
	constexpr int fourccDATA = 'atad';
	constexpr int fourccFMT = ' tmf';
	constexpr int fourccWAVE = 'EVAW';
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

	AudioPlayer::~AudioPlayer() {
		delete m_context;
	}

	AudioPlayer initialize_audio_player() {
		AudioPlayerContext* context = new AudioPlayerContext;

		winrt::check_hresult(XAudio2Create(context->audio_engine.put(), 0, XAUDIO2_DEFAULT_PROCESSOR));
		winrt::check_hresult(context->audio_engine->CreateMasteringVoice(&context->mastering_voice));

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
		winrt::check_hresult(context->audio_engine->CreateSourceVoice(&context->source_voice, &wave_format_ex));

		AudioPlayer audio_player;
		audio_player.m_context = context;
		return audio_player;
	}

	AudioID AudioPlayer::add_audio_from_file(HANDLE file) {
		DWORD chunk_size;
		DWORD chunk_position;
		//check the file type, should be fourccWAVE or 'XWMA'
		find_chunk_in_file(file, fourccRIFF, &chunk_size, &chunk_position);
		DWORD filetype;
		read_chunk_from_file(file, &filetype, sizeof(DWORD), chunk_position);
		// FIXME: return an error code here instead
		if (filetype != fourccWAVE) {
			fprintf(stderr, "AudioPlayer::add_audio_from_file called with non .wav file");
			exit(1);
		}

		//fill out the audio data buffer with the contents of the fourccDATA chunk
		find_chunk_in_file(file, fourccDATA, &chunk_size, &chunk_position);
		BYTE* samples = new BYTE[chunk_size];
		read_chunk_from_file(file, samples, chunk_size, chunk_position);

		uint32_t id = m_context->next_id++;
		XAUDIO2_BUFFER buffer = {
			.Flags = XAUDIO2_END_OF_STREAM,
			.AudioBytes = chunk_size,
			.pAudioData = samples,
		};
		m_context->audio_buffers.insert({ id, buffer });

		return AudioID { id };
	}

	void AudioPlayer::play(AudioID id) {
		m_context->source_voice->Stop();
		m_context->source_voice->FlushSourceBuffers();                                    // stop previous sound
		m_context->source_voice->SubmitSourceBuffer(&m_context->audio_buffers[id.value]); // play next sound
		m_context->source_voice->Start();
	}

} // namespace engine
