#include <expected>

namespace core {

	template <typename T, typename E, typename F>
	T unwrap(std::expected<T, E>&& maybe, F&& on_error) {
		if (!maybe.has_value()) {
			on_error(maybe.error());
		}
		return maybe.value();
	}

} // namespace core
