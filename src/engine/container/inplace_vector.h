#pragma once

#include <array>

namespace engine {

	// Vector with constant size capacity stored on the stack instead of the heap
	template <typename T, size_t capacity>
	class inplace_vector {
	public:
		using iterator = std::array<T, capacity>::iterator;
		using const_iterator = std::array<T, capacity>::const_iterator;

		inplace_vector() = default;
		inplace_vector(std::initializer_list<T> elems) {
			m_size = std::min(elems.size(), capacity);
			std::copy_n(elems.begin(), m_size, m_elems.begin());
		}

		size_t size() const {
			return m_size;
		}

		bool empty() const {
			return m_size == 0;
		}

		T& operator[](size_t index) {
			return m_elems[index];
		}

		const T& operator[](size_t index) const {
			return m_elems[index];
		}

		void push_back(const T& value) {
			if (m_size < capacity) {
				m_elems[m_size] = value;
				m_size++;
			}
		}

		iterator begin() {
			return m_elems.begin();
		}

		const_iterator begin() const {
			return m_elems.begin();
		}

		iterator end() {
			return m_elems.end();
		}

		const_iterator end() const {
			return m_elems.end();
		}

	private:
		size_t m_size = 0;
		std::array<T, capacity> m_elems;
	};

} // namespace engine
