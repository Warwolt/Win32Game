#include <gtest/gtest.h>

#include <engine/container/flat_map.h>

TEST(FlatMapTests, DefaultConstructed_IsEmpty) {
	engine::flat_map<std::string, int> flat_map;

	EXPECT_EQ(flat_map.size(), 0);
	EXPECT_TRUE(flat_map.empty());
}

TEST(FlatMapTests, ConstructedWithInitializerList_IsNonEmpty) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 3 },
	};

	EXPECT_EQ(flat_map.size(), 3);
	EXPECT_FALSE(flat_map.empty());
}

TEST(FlatMapTests, ConstructedWithInitializerList_DuplicateEntriesIgnored) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 1 },
		{ "second", 2 },
		{ "second", 22 },
	};
	std::unordered_map<std::string, int> unordered_map = {
		{ "first", 1 },
		{ "second", 2 },
		{ "second", 22 },
	};

	EXPECT_EQ(flat_map["first"], 1);
	EXPECT_EQ(flat_map["second"], 2);
	EXPECT_EQ(unordered_map["first"], 1);
	EXPECT_EQ(unordered_map["second"], 2);
	EXPECT_EQ(flat_map.size(), 2);
}

TEST(FlatMapTests, Equality_DifferentSizes_NotEqual) {
	engine::flat_map<std::string, int> lhs = {
		{ "first", 1 },
		{ "second", 2 },
	};
	engine::flat_map<std::string, int> rhs = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 3 },
	};

	EXPECT_NE(lhs, rhs);
}

TEST(FlatMapTests, Equality_EqualElements_DifferentOrder_AreEqual) {
	engine::flat_map<std::string, int> lhs = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 3 },
	};
	engine::flat_map<std::string, int> rhs = {
		{ "second", 2 },
		{ "third", 3 },
		{ "first", 1 },
	};

	EXPECT_EQ(lhs, rhs);
	EXPECT_EQ(rhs, lhs);
}

TEST(FlatMapTests, CopyConstructor) {
	engine::flat_map<std::string, int> lhs = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 3 },
	};
	engine::flat_map<std::string, int> rhs = lhs;

	EXPECT_EQ(lhs, rhs);
	EXPECT_EQ(rhs, lhs);
}

TEST(FlatMapTests, CopyAssignment) {
	engine::flat_map<std::string, int> lhs = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 3 },
	};
	engine::flat_map<std::string, int> rhs;
	rhs = lhs;

	EXPECT_EQ(lhs, rhs);
	EXPECT_EQ(rhs, lhs);
}

TEST(FlatMapTests, BracketOperator_NonExistingKey_DefaultConstructsElement) {
	engine::flat_map<std::string, int> flat_map;

	int value = flat_map["key"];

	engine::flat_map<std::string, int> expected = { { "key", 0 } };
	EXPECT_EQ(value, 0);
	EXPECT_EQ(flat_map, expected);
}

TEST(FlatMapTests, BracketOperator_ExistingKey_ReturnsThatValue) {
	engine::flat_map<std::string, int> flat_map = { { "first", 1 } };

	int value = flat_map["first"];

	EXPECT_EQ(value, 1);
}

TEST(FlatMapTests, AtOperator_NonExistingKey_ThrowsException) {
	engine::flat_map<std::string, int> flat_map;
	EXPECT_THROW(flat_map.at("key"), std::out_of_range);

	const engine::flat_map<std::string, int> const_vec_map;
	EXPECT_THROW(const_vec_map.at("key"), std::out_of_range);
}

TEST(FlatMapTests, AtOperator_ExistingKey_ReturnsThatValue) {
	engine::flat_map<std::string, int> flat_map = { { "key", 1234 } };
	EXPECT_EQ(flat_map.at("key"), 1234);

	const engine::flat_map<std::string, int> const_vec_map = { { "key", 1234 } };
	EXPECT_EQ(const_vec_map.at("key"), 1234);
}

TEST(FlatMapTests, AccessUnderlyingData) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 3 },
	};

	std::vector<std::pair<std::string, int>> values = flat_map.data();

	const std::vector<std::pair<std::string, int>> expected = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 3 },
	};
	EXPECT_EQ(values, expected);
}

TEST(FlatMapTests, Iterator_IteratesInOrderOfInsertion) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 33 },
	};
	const engine::flat_map<std::string, int> const_vec_map = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 33 },
	};

	std::vector<int> values;
	for (const auto& [key, value] : flat_map) {
		values.push_back(value);
	}
	std::vector<int> const_values;
	for (const auto& [key, value] : const_vec_map) {
		const_values.push_back(value);
	}

	ASSERT_EQ(values.size(), 3);
	EXPECT_EQ(values[0], 1);
	EXPECT_EQ(values[1], 2);
	EXPECT_EQ(values[2], 33);

	ASSERT_EQ(const_values.size(), 3);
	EXPECT_EQ(const_values[0], 1);
	EXPECT_EQ(const_values[1], 2);
	EXPECT_EQ(const_values[2], 33);
}

TEST(FlatMapTests, Insert_NewKey_AddsElementToEnd) {
	engine::flat_map<std::string, int> flat_map {
		{ "first", 1 },
	};

	const auto& [iterator, was_inserted] = flat_map.insert({ "second", 22 });
	std::vector<std::pair<std::string, int>> values;
	for (auto& [key, value] : flat_map) {
		values.push_back({ key, value });
	}

	const std::vector<std::pair<std::string, int>> expected = {
		{ "first", 1 },
		{ "second", 22 },
	};
	EXPECT_TRUE(was_inserted);
	ASSERT_NE(iterator, flat_map.end());
	EXPECT_EQ(iterator->second, 22);
	EXPECT_EQ(flat_map["first"], 1);
	EXPECT_EQ(flat_map["second"], 22);
	EXPECT_EQ(values, expected);
}

TEST(FlatMapTests, Insert_ExistingKey_OverwritesValue) {
	engine::flat_map<std::string, int> flat_map {
		{ "first", 1 },
	};

	const auto& [iterator, was_inserted] = flat_map.insert({ "first", 12 });
	std::vector<int> values;
	for (auto& [key, value] : flat_map) {
		values.push_back(value);
	}

	const std::vector<int> expected = { 12 };
	EXPECT_FALSE(was_inserted);
	ASSERT_NE(iterator, flat_map.end());
	EXPECT_EQ(iterator->second, 12);
	EXPECT_EQ(flat_map["first"], 12);
	EXPECT_EQ(values, expected);
}

TEST(FlatMapTests, Clear_RemovesAllElements) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 1 },
		{ "second", 2 },
		{ "third", 33 },
	};

	flat_map.clear();
	std::vector<int> values;
	for (auto& [key, value] : flat_map) {
		values.push_back(value);
	}

	EXPECT_TRUE(flat_map.empty());
	EXPECT_EQ(flat_map.size(), 0);
	EXPECT_TRUE(values.empty());
}

TEST(FlatMapTests, Erase_WithMatchingKey_RemovesElement) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 10 },
		{ "second", 20 },
		{ "third", 30 },
	};

	flat_map.erase("second");

	const engine::flat_map<std::string, int> expected = {
		{ "first", 10 },
		{ "third", 30 },
	};
	EXPECT_EQ(flat_map, expected);
}

TEST(FlatMapTests, Erase_ReturnsIteratorToNextElement) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 10 },
		{ "second", 20 },
		{ "third", 30 },
	};

	auto it = flat_map.erase("second");

	ASSERT_NE(it, flat_map.end());
	EXPECT_EQ(it->second, 30);
}

TEST(FlatMapTests, Erase_LastElemenet_RemovesElement) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 10 },
		{ "second", 20 },
		{ "third", 30 },
	};

	flat_map.erase("third");

	const engine::flat_map<std::string, int> expected = {
		{ "first", 10 },
		{ "second", 20 },
	};
	EXPECT_EQ(flat_map, expected);
}

TEST(FlatMapTests, Erase_NonExistingKey_DoesNothing) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 10 },
		{ "second", 20 },
		{ "third", 30 },
	};

	flat_map.erase("non-existing");

	const engine::flat_map<std::string, int> expected = {
		{ "first", 10 },
		{ "second", 20 },
		{ "third", 30 },
	};
	EXPECT_EQ(flat_map, expected);
	EXPECT_EQ(flat_map.size(), 3);
	EXPECT_FALSE(flat_map.empty());
}

TEST(FlatMapTests, Find_ExistingKey_GetsThatItem) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 10 },
		{ "second", 20 },
		{ "third", 30 },
	};

	auto it = flat_map.find("first");

	ASSERT_NE(it, flat_map.end());
	EXPECT_EQ(*it, std::make_pair("first", 10));
}

TEST(FlatMapTests, Find_ExistingKey_GetsThatItem_Const) {
	const engine::flat_map<std::string, int> flat_map = {
		{ "first", 10 },
		{ "second", 20 },
		{ "third", 30 },
	};

	auto it = flat_map.find("first");

	ASSERT_NE(it, flat_map.end());
	EXPECT_EQ(*it, std::make_pair("first", 10));
}

TEST(FlatMapTests, Find_NonExistingKey_GetsEndIterator) {
	engine::flat_map<std::string, int> flat_map = {
		{ "first", 10 },
		{ "second", 20 },
		{ "third", 30 },
	};

	auto it = flat_map.find("non-existing");

	EXPECT_EQ(it, flat_map.end());
}

TEST(FlatMapTests, InsertOrAssign_Insert) {
	engine::flat_map<std::string, int> flat_map;

	auto [it, was_inserted] = flat_map.insert_or_assign("first", 10);

	EXPECT_TRUE(was_inserted);
	EXPECT_EQ(*it, std::make_pair("first", 10));
}

TEST(FlatMapTests, InsertOrAssign_Assign) {
	engine::flat_map<std::string, int> flat_map = { { "first", 10 } };

	auto [it, was_inserted] = flat_map.insert_or_assign("first", 11);

	EXPECT_FALSE(was_inserted);
	EXPECT_EQ(*it, std::make_pair("first", 11));
}
