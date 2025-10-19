#pragma once

namespace engine {

	struct FontID {
		int value;
	};

    constexpr FontID INVALID_FONT_ID = FontID(0);
    constexpr FontID DEFAULT_FONT_ID = FontID(1);

} // namespace engine
