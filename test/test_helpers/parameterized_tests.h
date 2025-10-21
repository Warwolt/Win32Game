#pragma once

#include <gtest/gtest.h>

#define PARAMETERIZED_TEST_EMPTY_NAME()

// Defines a parameterized GoogleTest test case in a lightweight way
//
// text_fixture_name: Name of class that is a public testing::Test
// test_case_name: Name of this test case
// parameter_type: Name of parameter type that will be grabbed with GetParam()
// parameters: std::vector holding instances of parameter_type
//
// NOTE: `parameter_type` must contain a public field `const char* name`
// The `name` field should be the name for that specific parameter case
#define PARAMETERIZED_TEST(test_fixture_name, test_case_name, parameter_type, parameters)                                                                   \
	class test_case_name : public test_fixture_name                                                                                                         \
		, public testing::WithParamInterface<parameter_type> {                                                                                              \
	};                                                                                                                                                      \
	INSTANTIATE_TEST_SUITE_P(test_fixture_name, test_case_name, testing::ValuesIn(parameters), [](testing::TestParamInfo<test_case_name::ParamType> info) { \
		return info.param.name;                                                                                                                             \
	});                                                                                                                                                     \
	TEST_P(test_case_name, PARAMETERIZED_TEST_EMPTY_NAME())
