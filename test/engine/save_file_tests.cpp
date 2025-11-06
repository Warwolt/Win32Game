#include <gtest/gtest.h>

#include <engine/file/save_file.h>

using namespace engine;

TEST(SaveFileTests, FromJsonString_BadJson_GivesError) {
	std::expected<SaveFile, SaveFileError> result = SaveFile::from_json_string("asdfg");
	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), SaveFileError::InvalidJson);
}

TEST(SaveFileTests, FromJsonString_ValidJsonObject_CanAccessProperties) {
    /* Parse json */
    const std::string json = R"({
        "my_number": 3,
        "my_boolean": true,
        "my_string": "hello"
    })";
	std::expected<SaveFile, SaveFileError> result = SaveFile::from_json_string(json);
	ASSERT_TRUE(result.has_value());

    /* Access properties */
    // SaveFile save_file = result.value();
    // ASSERT_TRUE(save_file["my_number"].is_number());
}
