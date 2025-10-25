#pragma once

#include <gtest/gtest.h>

#define TEST_PARAMETERIZED_EMPTY_NAME()

// Defines a parameterized GoogleTest test case in a lightweight way
//
// text_fixture_name: Name of test fixture
// test_case_name: Name of this test case
// parameter_type: Name of parameter type that will be grabbed with GetParam()
// parameters: any argument to TEST_P, e.g. testing::Values(x1, x2, x3) or testing::ValuesIn(vector)
//
// NOTE: if `parameter_type` contains a `name` member then that will be used as test case name
#define TEST_PARAMETERIZED(test_fixture_name, test_case_name, parameter_type, parameters)                                                \
	class test_case_name : public testing::Test                                                                                          \
		, public testing::WithParamInterface<parameter_type> {                                                                           \
	};                                                                                                                                   \
	INSTANTIATE_TEST_SUITE_P(test_fixture_name, test_case_name, parameters, [](testing::TestParamInfo<test_case_name::ParamType> info) { \
		if constexpr (requires { info.param.name; }) {                                                                                   \
			return std::string(info.param.name);                                                                                         \
		}                                                                                                                                \
		else {                                                                                                                           \
			return testing::PrintToString(info.param);                                                                                   \
		}                                                                                                                                \
	});                                                                                                                                  \
	TEST_P(test_case_name, TEST_PARAMETERIZED_EMPTY_NAME())
