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