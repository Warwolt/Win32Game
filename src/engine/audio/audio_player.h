#pragma once

#include <expected>
#include <memory>
#include <stdint.h>

#include <string>
#include <unordered_map>
#include <windows.h>
#include <xaudio2.h>

namespace engine {

	struct AudioID {
		uint32_t value;
	};

	class AudioPlayer {
	public:
		AudioPlayer() = default;
		~AudioPlayer();
		friend AudioPlayer initialize_audio_player();

		std::expected<AudioID, std::string> add_audio_from_file(HANDLE file);
		void play(AudioID id);

	private:
		IXAudio2* m_audio_engine = nullptr;
		IXAudio2MasteringVoice* m_mastering_voice = nullptr;
		IXAudio2SourceVoice* m_source_voice = nullptr;
		std::unordered_map<uint32_t, XAUDIO2_BUFFER> m_audio_buffers;
		uint32_t m_next_id = 1;
	};

	AudioPlayer initialize_audio_player();

} // namespace engine
