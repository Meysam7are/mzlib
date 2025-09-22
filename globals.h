#ifndef MZLIB_GLOBALS_HEADER_FILE
#define MZLIB_GLOBALS_HEADER_FILE
#pragma once

#include <format>
#include <iostream>

#include "string_utils.h"
#include "concept_utils.h"
#include "timer_utils.h"
#include "error_utils.h"
#include "enum_utils.h"
#include "size_types.h"

namespace mz
{
	/**
 * @brief Counts the number of true values in a boolean sequence.
 *
 * @tparam Seq BooleanSequence type.
 * @param sequence Sequence to count.
 * @return Number of true values.
 *
 * Usage:
 *   size_type n_true = mz::count(bool_vector);
 */
	template <BooleanSequence Seq>
	size_type count(const Seq& sequence) noexcept
	{
		size_type result{ 0 };
		for (auto i = 0; i < sequence.size(); ++i) {
			result += !!sequence[i];
		}
		return result;
	}

	/**
	 * @brief Sums the values in an integral sequence.
	 *
	 * @tparam Seq IntegralSequence type.
	 * @param sequence Sequence to sum.
	 * @return Sum of all values.
	 *
	 * Usage:
	 *   size_type total = mz::sum(int_vector);
	 */
	template <IntegralSequence Seq>
	size_type sum(const Seq& sequence) noexcept
	{
		size_type result{ 0 };
		for (auto i = 0; i < sequence.size(); ++i) {
			result += sequence[i];
		}
		return result;
	}

} // namespace mz

#endif // MZLIB_GLOBALS_HEADER_FILE