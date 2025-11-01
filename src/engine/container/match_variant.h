#pragma once

// MATCH_VARIANT and MATCH_CASE is a pair of macros that allow a switch-case
// like std::variant destructuring.
//
// Because of the `_match_variant_var` variable, MATCH_VARIANT cannot be nested.
// In that case, consider using IF_MATCH_VARIANT.
//
// MATCH_VARIANT(my_vec) {
//     MATCH_CASE0(IVec0) {
//       // do something
//     }
//     MATCH_CASE(IVec2, x, y) {
//         // do something with x and y
//     }
//     MATCH_CASE(IVec3, x, y, z) {
//         // do something with x and y and z
//     }
// }

#define MATCH_VARIANT(variant) \
	if (auto& _match_variant_var = variant; true)

#define MATCH_VARIANT_IF(variant, type)                   \
	if (auto* result##type = std::get_if<type>(&variant)) \
		if (auto& _match_variant_var = *result##type; true)

#define MATCH_CASE0(type) \
	if (auto* result##type = std::get_if<type>(&_match_variant_var))

#define MATCH_CASE(type, ...)                                        \
	if (auto* result##type = std::get_if<type>(&_match_variant_var)) \
		if (auto& [__VA_ARGS__] = *result##type; true)

#define IF_MATCH_VARIANT(variant, type, ...)              \
	if (auto* result##type = std::get_if<type>(&variant)) \
		if (auto& [__VA_ARGS__] = *result##type; true)
