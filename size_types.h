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

#ifndef MZ_SIZE_TYPES_HEADER_FILE
#define MZ_SIZE_TYPES_HEADER_FILE
#pragma once

#include <cstdint>
#include <concepts>
#include <climits>

/**
 * @file size_types.h
 * @brief Defines common size, index, and ID types for the mz library.
 *
 * This header provides type aliases for signed/unsigned sizes, indices, and IDs,
 * as well as utility functions for arithmetic and comparison on integral types.
 * It also provides null value utilities for ID types.
 *
 * Usage:
 *   size_type sz = 10;
 *   index_type idx = 5;
 *   if (mz::less(idx, sz)) { ... }
 */

 //-----------------------------------------------------------------------------
 // Type Aliases for Sizes, Indices, and IDs
 //-----------------------------------------------------------------------------


  /**
   * @brief Macro for constraining index types to integral types.
   * Usage: template<INDEX_T I> void foo(I i);
   */
#define INDEX_T std::integral auto



 // Uncomment to use signed size types. Comment out to use unsigned types.
#define MZ_SIGNED_SIZE

#ifdef MZ_SIGNED_SIZE
using arg_type = int;           ///< Function argument type (signed).
using size_type = int;           ///< Main size type (signed).
using index_type = long long;     ///< Index type (signed, large range).
#else
using arg_type = unsigned int;  ///< Function argument type (unsigned).
using size_type = unsigned int;  ///< Main size type (unsigned).
using index_type = unsigned long long; ///< Index type (unsigned, large range).
#endif

using sign_type = int;           ///< Type for sign representation.
using isize_type = std::make_signed_t<size_type>;   ///< Signed version of size_type.
using usize_type = std::make_unsigned_t<size_type>; ///< Unsigned version of size_type.

// Pointer and reference aliases for size types.
using size_pointer = size_type*;
using size_reference = size_type&;
using size_const_pointer = const size_type*;
using size_const_reference = const size_type;

using isize_pointer = isize_type*;
using isize_reference = isize_type&;
using isize_const_pointer = const isize_type*;
using isize_const_reference = const isize_type;

using usize_pointer = usize_type*;
using usize_reference = usize_type&;
using usize_const_pointer = const usize_type*;
using usize_const_reference = const usize_type;

using id_type = int; ///< General ID type.

//-----------------------------------------------------------------------------
// Null Value Definitions
//-----------------------------------------------------------------------------

/**
 * @brief Integer null value, using minimum int value.
 */
#define INT_NULL INT_MIN

 //-----------------------------------------------------------------------------
 // Arithmetic and Comparison Utilities
 //-----------------------------------------------------------------------------

namespace mz {

	/**
	 * @brief Multiplies two integral values and returns a long long result.
	 * @param lhs Left operand.
	 * @param rhs Right operand.
	 * @return Product as long long.
	 */
	inline constexpr long long multiply_as_ll(std::integral auto lhs, std::integral auto rhs) noexcept {
		return static_cast<long long>(lhs) * static_cast<long long>(rhs);
	}

	/**
	 * @brief Adds two integral values and returns a long long result.
	 * @param lhs Left operand.
	 * @param rhs Right operand.
	 * @return Sum as long long.
	 */
	inline constexpr long long add_as_ll(std::integral auto lhs, std::integral auto rhs) noexcept {
		return static_cast<long long>(lhs) + static_cast<long long>(rhs);
	}

	/**
	 * @brief Compares two signed integral values.
	 * @return -1 if lhs < rhs, 1 if lhs > rhs, 0 if equal.
	 */
	inline constexpr int compare(std::signed_integral auto lhs, std::signed_integral auto rhs) noexcept {
		return lhs < rhs ? -1 : (lhs > rhs ? 1 : 0);
	}

	/**
	 * @brief Compares two unsigned integral values.
	 * @return -1 if lhs < rhs, 1 if lhs > rhs, 0 if equal.
	 */
	inline constexpr int compare(std::unsigned_integral auto lhs, std::unsigned_integral auto rhs) noexcept {
		return lhs < rhs ? -1 : (lhs > rhs ? 1 : 0);
	}

	/**
	 * @brief Compares signed and unsigned integral values.
	 * @return -1 if lhs < rhs, 1 if lhs > rhs, 0 if equal.
	 */
	inline constexpr int compare(std::signed_integral auto lhs, std::unsigned_integral auto rhs) noexcept {
		long long rhs_ll = static_cast<long long>(rhs);
		return lhs < rhs_ll ? -1 : (lhs > rhs_ll ? 1 : 0);
	}

	/**
	 * @brief Compares unsigned and signed integral values.
	 * @return -1 if lhs < rhs, 1 if lhs > rhs, 0 if equal.
	 */
	inline constexpr int compare(std::unsigned_integral auto lhs, std::signed_integral auto rhs) noexcept {
		long long lhs_ll = static_cast<long long>(lhs);
		return lhs_ll < rhs ? -1 : (lhs_ll > rhs ? 1 : 0);
	}

	/**
	 * @brief Returns true if lhs < rhs for signed integral types.
	 */
	inline constexpr bool less(std::signed_integral auto lhs, std::signed_integral auto rhs) noexcept {
		return lhs < rhs;
	}

	/**
	 * @brief Returns true if lhs < rhs for unsigned integral types.
	 */
	inline constexpr bool less(std::unsigned_integral auto lhs, std::unsigned_integral auto rhs) noexcept {
		return lhs < rhs;
	}

	/**
	 * @brief Returns true if lhs < rhs for signed/unsigned mixed types.
	 */
	inline constexpr bool less(std::signed_integral auto lhs, std::unsigned_integral auto rhs) noexcept {
		return lhs < static_cast<long long>(rhs);
	}

	/**
	 * @brief Returns true if lhs < rhs for unsigned/signed mixed types.
	 */
	inline constexpr bool less(std::unsigned_integral auto lhs, std::signed_integral auto rhs) noexcept {
		return static_cast<long long>(lhs) < rhs;
	}

	//-----------------------------------------------------------------------------
	// ID Utilities
	//-----------------------------------------------------------------------------

	namespace id {

		/**
		 * @brief Traits for working with ID types.
		 * @tparam T Integral type for ID.
		 *
		 * Provides bit count, positive/negative null values, and null checks.
		 */
		template <std::integral T>
		struct id_traits {
			static constexpr size_t bit_count = sizeof(T) * 8 - 1; ///< Number of bits minus sign.
			static constexpr T negative_null = static_cast<T>(1ull << bit_count); ///< Negative null value.
			static constexpr T positive_null = ~negative_null; ///< Positive null value.

			/**
			 * @brief Checks if the value is a null ID.
			 */
			static constexpr bool is_null(T value) noexcept { return value == negative_null || value == positive_null; }

			/**
			 * @brief Checks if the value is not a null ID.
			 */
			static constexpr bool not_null(T value) noexcept { return value != negative_null && value != positive_null; }
		};

		/**
		 * @brief Returns the bitwise complement of x.
		 */
		template <std::integral T>
		inline constexpr T bitwise_complement(T x) noexcept { return ~x; }

		/**
		 * @brief Returns the positive null value for the ID type.
		 */
		template <std::integral T>
		inline constexpr T positive_null() noexcept { return id_traits<T>::positive_null; }

		/**
		 * @brief Returns the negative null value for the ID type.
		 */
		template <std::integral T>
		inline constexpr T negative_null() noexcept { return id_traits<T>::negative_null; }

		/**
		 * @brief Checks if the value is a null ID.
		 */
		template <std::integral T>
		inline constexpr bool is_null(T value) noexcept { return id_traits<T>::is_null(value); }

		/**
		 * @brief Checks if the value is not a null ID.
		 */
		template <std::integral T>
		inline constexpr bool not_null(T value) noexcept { return id_traits<T>::not_null(value); }

		/**
		 * @brief Returns the positive representation of a signed integral value.
		 * If x is negative, returns bitwise complement; otherwise returns x.
		 */
		template <std::signed_integral T>
		inline constexpr T to_positive(T x) noexcept { return x < 0 ? ~x : x; }

		/**
		 * @brief Returns the negative representation of a signed integral value.
		 * If x is non-negative, returns bitwise complement; otherwise returns x.
		 */
		template <std::signed_integral T>
		inline constexpr T to_negative(T x) noexcept { return x < 0 ? x : ~x; }

		/**
		 * @brief Checks if a signed integral value is positive.
		 */
		template <std::signed_integral T>
		inline constexpr bool is_positive(T x) noexcept { return x >= 0; }

		/**
		 * @brief Checks if a signed integral value is negative.
		 */
		template <std::signed_integral T>
		inline constexpr bool is_negative(T x) noexcept { return x < 0; }

		/**
		 * @brief Returns the positive representation of an unsigned integral value.
		 */
		template <std::unsigned_integral T>
		inline constexpr std::make_signed_t<T> to_positive(T x) noexcept { return to_positive(static_cast<std::make_signed_t<T>>(x)); }

		/**
		 * @brief Returns the negative representation of an unsigned integral value.
		 */
		template <std::unsigned_integral T>
		inline constexpr std::make_signed_t<T> to_negative(T x) noexcept { return to_negative(static_cast<std::make_signed_t<T>>(x)); }

		/**
		 * @brief Checks if an unsigned integral value is negative.
		 */
		template <std::unsigned_integral T>
		inline constexpr bool is_negative(T x) noexcept { return is_negative(static_cast<std::make_signed_t<T>>(x)); }

		/**
		 * @brief Checks if an unsigned integral value is positive.
		 */
		template <std::unsigned_integral T>
		inline constexpr bool is_positive(T x) noexcept { return is_positive(static_cast<std::make_signed_t<T>>(x)); }

		/**
		 * @brief Checks if an index is within the bounds of a container of given size.
		 * @param index Index to check.
		 * @param size Size of the container.
		 * @return true if index is valid, false otherwise.
		 */
		inline constexpr bool in_bounds(std::integral auto index, size_t size) noexcept {
			return static_cast<size_t>(index) < size;
		}

	} // namespace id

} // namespace mz

#endif // MZ_SIZE_TYPES_HEADER_FILE




