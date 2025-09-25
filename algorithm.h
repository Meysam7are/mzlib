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

#pragma once
#include "globals.h"

/**
 * @file algorithm.h
 * @brief Search and partition algorithms for pointer ranges in the mz library.
 *
 * This header provides generic binary search and partitioning utilities for pointer ranges.
 * Functions are designed for use with C++20 and support custom predicates and sign functions.
 *
 * Usage example:
 *   int arr[] = { ... };
 *   auto* lower = mz::lower_bound(arr, arr + n, [](int x){ return x < threshold; });
 *   auto* upper = mz::upper_bound(arr, arr + n, [](int x){ return x < threshold; });
 *   size_type count = mz::range_sign(arr, arr + n, [](int x){ return sign(x); });
 */

namespace mz
{

    /**
     * @brief Finds the first pointer in [first, last) where predicate changes from Affirm to !Affirm.
     *
     * This is a generic binary search for pointer ranges. The predicate F must return bool.
     *
     * @tparam Affirm Value to search for (true or false).
     * @tparam T      Pointer value type.
     * @tparam Predicate Predicate type (returns bool when called with T const&).
     * @param first   Pointer to the beginning of the range.
     * @param last    Pointer to the end of the range.
     * @param predicate Predicate function.
     * @return Pointer to the first element where predicate changes.
     *
     * Usage:
     *   auto* bound = mz::extreme_bound<true>(arr, arr + n, [](const int& x){ return x < value; });
     */
    template <bool Affirm, typename T, typename Predicate>
        requires std::is_invocable_r_v<bool, Predicate, T const&>
    T* extreme_bound(T* first, T* last, Predicate&& predicate) {
        auto it = first;
        auto count = std::distance(first, last);

        while (count > 0) {
            it = first;
            auto step = count / 2;
            std::advance(it, step);
            if (predicate(*it) == Affirm) {
                first = ++it;
                count -= step + 1;
            }
            else {
                count = step;
            }
        }
        return first;
    }

    /**
     * @brief Returns the first pointer in [first, last) where predicate is false.
     *
     * Equivalent to std::lower_bound for custom predicates.
     *
     * @tparam T        Pointer value type.
     * @tparam Predicate Predicate type (returns bool).
     * @param first     Pointer to the beginning of the range.
     * @param last      Pointer to the end of the range.
     * @param predicate Predicate function.
     * @return Pointer to the first element where predicate is false.
     */
    template <typename T, typename Predicate>
        requires std::is_invocable_r_v<bool, Predicate, T const&>
    T* lower_bound(T* first, T* last, Predicate&& predicate) {
        return extreme_bound<true>(first, last, std::forward<Predicate>(predicate));
    }

    /**
     * @brief Returns the first pointer in [first, last) where predicate is true.
     *
     * Equivalent to std::upper_bound for custom predicates.
     *
     * @tparam T        Pointer value type.
     * @tparam Predicate Predicate type (returns bool).
     * @param first     Pointer to the beginning of the range.
     * @param last      Pointer to the end of the range.
     * @param predicate Predicate function.
     * @return Pointer to the first element where predicate is true.
     */
    template <typename T, typename Predicate>
        requires std::is_invocable_r_v<bool, Predicate, T const&>
    T* upper_bound(T* first, T* last, Predicate&& predicate) {
        return extreme_bound<false>(first, last, std::forward<Predicate>(predicate));
    }

    /**
     * @brief Partitions a pointer range [first, last) into three regions based on sign of predicate.
     *
     * Finds pointers first0 and first1 such that:
     *   - [first, first0):   predicate(*it) < 0
     *   - [first0, first1):  predicate(*it) == 0
     *   - [first1, last):    predicate(*it) > 0
     *
     * Updates first and last to point to the zero region.
     *
     * @tparam T        Pointer value type.
     * @tparam Predicate Predicate type (returns int).
     * @param first     Reference to pointer to the beginning of the range (updated).
     * @param last      Reference to pointer to the end of the range (updated).
     * @param predicate Predicate function (returns int: <0, 0, >0).
     * @return Number of elements in the zero region.
     *
     * Usage:
     *   int* first = arr;
     *   int* last = arr + n;
     *   size_type zero_count = mz::range_sign(first, last, [](const int& x){ return sign(x); });
     *   // [first, last) now contains all elements where sign(x) == 0
     */
    template <typename T, typename Predicate>
        requires std::is_invocable_r_v<int, Predicate, T const&>
    size_type range_sign(T*& first, T*& last, Predicate&& predicate)
    {
        auto first0 = first;
        auto count0 = last - first;

        auto first1 = first;
        auto last1 = last;

        // Find first0: start of zero region
        while (count0 > 0) {
            auto it = first0;
            auto step = count0 / 2;
            std::advance(it, step);

            int sign = predicate(*it);
            if (sign < 0) {
                first0 = ++it;
                count0 -= step + 1;
                if (first0 > first1) first1 = first0;
            }
            else if (sign == 0) {
                ++it;
                count0 = step;
                if (it > first1) first1 = it;
            }
            else {
                last1 = it;
                count0 = step;
                if (first0 > first1) first1 = first0;
            }
        }
        first = first0;

        // Find first1: end of zero region
        auto count1 = last1 - first1;
        while (count1 > 0) {
            auto it = first1;
            auto step = count1 / 2;
            std::advance(it, step);

            int sign = predicate(*it);
            if (sign <= 0) {
                first1 = ++it;
                count1 -= step + 1;
            }
            else {
                count1 = step;
            }
        }
        last = first1;
        return size_type(last - first);
    }

} // namespace mz