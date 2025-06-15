#pragma once

#include <initializer_list>
#include <stdint.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace engine {

	template <typename Key, typename Value>
	class flat_map {
	public:
		using value_type = std::pair<Key, Value>;
		using iterator = std::vector<value_type>::iterator;
		using const_iterator = std::vector<value_type>::const_iterator;

		flat_map() = default;
		flat_map(std::initializer_list<value_type> init) {
			size_t i = 0;
			for (const auto& [key, value] : init) {
				if (!m_indexes.contains(key)) {
					m_values.push_back({ key, value });
					m_indexes.emplace(key, i++);
				}
			}
		}

		size_t size() const {
			return m_values.size();
		}

		bool empty() const {
			return m_values.empty();
		}

		bool contains(const Key& key) const {
			return m_indexes.contains(key);
		}

		bool operator==(const flat_map<Key, Value>& rhs) const {
			if (m_values.size() != rhs.size()) {
				return false;
			}

			for (auto& [key, index] : m_indexes) {
				if (!rhs.m_indexes.contains(key)) {
					return false;
				}
				size_t rhs_index = rhs.m_indexes.at(key);
				if (m_values.at(index) != rhs.m_values.at(rhs_index)) {
					return false;
				}
			}

			return true;
		}

		bool operator!=(const flat_map<Key, Value>& rhs) const {
			return !(*this == rhs);
		}

		Value& operator[](const Key& key) {
			if (m_indexes.contains(key)) {
				return m_values[m_indexes[key]].second;
			}
			m_values.push_back({ key, Value() });
			m_indexes[key] = m_values.size() - 1;
			return m_values.back().second;
		}

		Value& at(const Key& key) {
			return m_values[m_indexes.at(key)].second;
		}

		const Value& at(const Key& key) const {
			return m_values[m_indexes.at(key)].second;
		}

		const std::vector<value_type>& data() const {
			return m_values;
		};

		iterator begin() {
			return m_values.begin();
		}

		iterator end() {
			return m_values.end();
		}

		const_iterator begin() const {
			return m_values.begin();
		}

		const_iterator end() const {
			return m_values.end();
		}

		std::pair<const_iterator, bool> insert(value_type&& key_val) {
			const auto& [key, value] = key_val;

			if (m_indexes.contains(key)) {
				//  existing key
				size_t index = m_indexes[key];
				iterator it = m_values.begin() + index;
				it->second = value;

				return { it, false };
			}
			else {
				// new key
				m_values.push_back(key_val);
				size_t index = m_values.size() - 1;
				m_indexes[key] = index;
				iterator it = m_values.begin() + index;

				return { it, true };
			}
		}

		std::pair<const_iterator, bool> insert_or_assign(const Key& key, Value value) {
			if (m_indexes.contains(key)) {
				// assign
				size_t index = m_indexes[key];
				iterator it = m_values.begin() + index;
				it->second = value;
				return { it, false };
			}
			else {
				// insert
				m_values.push_back(std::make_pair(key, value));
				size_t index = m_values.size() - 1;
				m_indexes[key] = index;
				iterator it = m_values.begin() + index;
				it->second = value;

				return { it, true };
			}
		}

		void clear() noexcept {
			m_values.clear();
			m_indexes.clear();
		}

		const_iterator erase(const Key& key) {
			auto it = m_indexes.find(key);
			if (it == m_indexes.end()) {
				return m_values.end();
			}

			size_t index = it->second;

			// replace element-to-remove with last element
			std::swap(m_values[index], m_values.back());
			m_indexes[_last_key()] = index;

			// remove last element
			m_values.pop_back();
			m_indexes.erase(key);

			return m_values.begin() + index;
		}

		iterator find(const Key& key) {
			auto it = m_indexes.find(key);
			if (it == m_indexes.end()) {
				return m_values.end();
			}
			size_t index = it->second;
			return std::next(m_values.begin(), index);
		}

		const_iterator find(const Key& key) const {
			auto it = m_indexes.find(key);
			if (it == m_indexes.end()) {
				return m_values.end();
			}
			size_t index = it->second;
			return std::next(m_values.begin(), index);
		}

	private:
		const Key& _last_key() {
			auto is_last_index = [&](const std::pair<Key, size_t>& key_val) {
				return key_val.second == m_values.size() - 1;
			};
			return std::find_if(m_indexes.begin(), m_indexes.end(), is_last_index)->first;
		}

		std::vector<value_type> m_values;
		std::unordered_map<Key, size_t> m_indexes;
	};

} // namespace core
