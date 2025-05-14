#pragma once

#include <memory>
#include <stdint.h>
#include <windows.h>

#include <winrt/base.h>
#include <xaudio2.h>

namespace engine {

	struct AudioID {
		uint32_t value;
	};

	struct AudioPlayerContext;

	class AudioPlayer {
	public:
		AudioPlayer() = default;
		friend AudioPlayer initialize_audio_player();

		AudioID add_audio_from_file(HANDLE file);
		void play(AudioID id);

	private:
		winrt::com_ptr<IXAudio2> m_audio_engine;
		IXAudio2MasteringVoice* m_mastering_voice;
		IXAudio2SourceVoice* m_source_voice;
		std::unordered_map<uint32_t, XAUDIO2_BUFFER> m_audio_buffers;
		uint32_t m_next_id = 1;
	};

	AudioPlayer initialize_audio_player();

} // namespace engine
