#pragma once

#include <stdint.h>
#include <windows.h>

namespace engine {

	struct AudioID {
		uint32_t value;
	};

	struct AudioPlayerContext;

	class AudioPlayer {
	public:
		friend AudioPlayer initialize_audio_player();
		~AudioPlayer();
		AudioID add_audio_from_file(HANDLE file);
		void play(AudioID id);

	private:
		AudioPlayer() = default;
		AudioPlayerContext* m_context;
	};

    AudioPlayer initialize_audio_player();

} // namespace engine
