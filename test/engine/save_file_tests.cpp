#include <gtest/gtest.h>

#include <engine/file/save_file.h>

using namespace engine;

TEST(SaveFileTests, FromJsonString_BadJson_GivesError) {
	std::expected<SaveFile, SaveFileError> result = SaveFile::from_json_string("asdfg");
	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), SaveFileError::InvalidJson);
}

TEST(SaveFileTests, FromJsonString_ValidJsonObject_CanAccessProperties) {
    const std::string json = R"({
        "my_number": 3,
        "my_boolean": true,
        "my_string": "hello"
    })";
	std::expected<SaveFile, SaveFileError> result = SaveFile::from_json_string(json);
	ASSERT_TRUE(result.has_value());
    SaveFile save_file = result.value();

    ASSERT_TRUE(save_file["my_number"].is_number());
    EXPECT_EQ(save_file["my_number"].as_number().value(), 3);

    ASSERT_TRUE(save_file["my_boolean"].is_boolean());
    EXPECT_EQ(save_file["my_boolean"].as_boolean().value(), true);

    ASSERT_TRUE(save_file["my_string"].is_string());
    EXPECT_EQ(save_file["my_string"].as_string().value(), "hello");

    ASSERT_TRUE(save_file["non-existing"].is_null());
}
