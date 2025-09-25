/*
 * MIT License
 * Copyright (c) 2022-2025 Meysam Zare
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MZ_ENUM_UTILS_HEADER_FILE
#define MZ_ENUM_UTILS_HEADER_FILE
#pragma once
#include <cstdint>
#include <concepts>


/**
 * @file enum_utils.h
 * @brief Utility functions and operators for working with enum types in C++20.
 *
 * This header provides comparison operators for enum types based on their underlying values,
 * and a helper function to extract the underlying integer value of an enum.
 *
 * Usage example:
 *   enum class Color : uint8_t { Red, Green, Blue };
 *   Color a = Color::Red, b = Color::Green;
 *   if (a < b) { ... }
 *   auto val = mz::to_underlying(a);
 */

 /**
  * @brief Less-than operator for enum types.
  *
  * Compares two enum values by their underlying integer representation.
  *
  * @tparam EnumT Enum type (must satisfy std::is_enum_v).
  * @param lhs Left-hand side enum value.
  * @param rhs Right-hand side enum value.
  * @return true if lhs < rhs, false otherwise.
  */
template <typename EnumT>
    requires std::is_enum_v<EnumT>
inline constexpr bool operator<(EnumT lhs, EnumT rhs) {
    return static_cast<std::underlying_type_t<EnumT>>(lhs)
        < static_cast<std::underlying_type_t<EnumT>>(rhs);
}

/**
 * @brief Greater-than operator for enum types.
 *
 * Compares two enum values by their underlying integer representation.
 *
 * @tparam EnumT Enum type (must satisfy std::is_enum_v).
 * @param lhs Left-hand side enum value.
 * @param rhs Right-hand side enum value.
 * @return true if lhs > rhs, false otherwise.
 */
template <typename EnumT>
    requires std::is_enum_v<EnumT>
inline constexpr bool operator>(EnumT lhs, EnumT rhs) {
    return static_cast<std::underlying_type_t<EnumT>>(lhs)
> static_cast<std::underlying_type_t<EnumT>>(rhs);
}

/**
 * @brief Less-than-or-equal operator for enum types.
 *
 * Compares two enum values by their underlying integer representation.
 *
 * @tparam EnumT Enum type (must satisfy std::is_enum_v).
 * @param lhs Left-hand side enum value.
 * @param rhs Right-hand side enum value.
 * @return true if lhs <= rhs, false otherwise.
 */
template <typename EnumT>
    requires std::is_enum_v<EnumT>
inline constexpr bool operator<=(EnumT lhs, EnumT rhs) {
    return static_cast<std::underlying_type_t<EnumT>>(lhs)
        <= static_cast<std::underlying_type_t<EnumT>>(rhs);
}

/**
 * @brief Greater-than-or-equal operator for enum types.
 *
 * Compares two enum values by their underlying integer representation.
 *
 * @tparam EnumT Enum type (must satisfy std::is_enum_v).
 * @param lhs Left-hand side enum value.
 * @param rhs Right-hand side enum value.
 * @return true if lhs >= rhs, false otherwise.
 */
template <typename EnumT>
    requires std::is_enum_v<EnumT>
inline constexpr bool operator>=(EnumT lhs, EnumT rhs) {
    return static_cast<std::underlying_type_t<EnumT>>(lhs)
        >= static_cast<std::underlying_type_t<EnumT>>(rhs);
}

namespace mz {

    /**
     * @brief Returns the underlying integer value of an enum.
     *
     * This utility is useful for serialization, logging, or interfacing with APIs
     * that require the raw integer value of an enum.
     *
     * @tparam EnumT Enum type (must satisfy std::is_enum_v).
     * @param value Enum value.
     * @return Underlying integer value of the enum.
     *
     * Usage:
     *   enum class Status : int { Ok = 0, Error = 1 };
     *   int raw = mz::to_underlying(Status::Error); // raw == 1
     */
    template <typename EnumT>
        requires std::is_enum_v<EnumT>
    inline constexpr std::underlying_type_t<EnumT> to_underlying(EnumT value) noexcept {
        return static_cast<std::underlying_type_t<EnumT>>(value);
    }

} // namespace mz

#endif // MZ_ENUM_UTILS_HEADER_FILE
