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

#ifndef MZ_IX_HEADER_FILE
#define MZ_IX_HEADER_FILE
#pragma once

#include <type_traits>
#include <limits>
#include <span>
#include <algorithm>
#include "globals.h"
#include "zstream.h"
#include "Slice.h"
#include "Vector.h"

/**
 * @brief XA is a specialized integer vector with custom move assignment logic.
 *
 * Inherits from mz::Vector<int> and provides constructors and assignment operators
 * tailored for efficient memory management and element access. The move assignment
 * operator is unique: it swaps data only if the right-hand side has a larger capacity,
 * otherwise it copies the data.
 */
class XA : public mz::Vector<int> {

public:

    /**
     * @brief Construct an XA with a given capacity, uninitialized.
     * @param Capacity Number of elements to allocate.
     */
    explicit constexpr XA(size_type Capacity) noexcept : mz::Vector<int>{ Capacity, 0 } {}

    /**
     * @brief Default constructor. Creates an empty XA.
     */
    explicit constexpr XA() noexcept = default;

    /**
     * @brief Construct an XA from a pointer to data.
     *
     * Allocates storage for Capacity elements and copies from ptr.
     * @param Capacity Number of elements to allocate and copy.
     * @param ptr Pointer to source data.
     */
    explicit XA(size_type Capacity, const_pointer ptr) noexcept : XA(Capacity) {
        m_size = m_cap;
        memcpy(m_data, ptr, sizeof(value_type) * m_size);
    }

    /**
     * @brief Clear all elements (does not free memory).
     * @return Reference to this XA.
     */
    constexpr XA& clear() noexcept { mz::Vector<int>::clear(); return *this; }

    /**
     * @brief Check if a key exists in the sorted vector.
     *
     * Uses binary search (lower_bound) for efficient lookup.
     * @param key Value to search for.
     * @return true if key is present, false otherwise.
     */
    bool exists(value_type key) const noexcept {
        auto lb = std::lower_bound(m_data, m_data + m_size, key);
        return lb != m_data + m_size && *lb == key;
    }

    /**
     * @brief Move constructor. Transfers ownership from rhs.
     */
    XA(XA&& rhs) noexcept : mz::Vector<int>{ std::move(rhs) } {}

    /**
     * @brief Copy constructor. Deep copies from rhs.
     */
    XA(const XA& rhs) noexcept : mz::Vector<int>{ rhs } {}

    /**
     * @brief Move assignment operator.
     *
     * Differs from mz::Vector<int>:
     * - If rhs has larger or equal capacity, swaps data for efficiency.
     * - Otherwise, resizes and copies data from rhs.
     * This avoids unnecessary heap allocations when possible.
     * @param rhs Source XA to move from.
     * @return Reference to this XA.
     */
    XA& operator = (XA&& rhs) noexcept {
        if (this != &rhs)
        {
            if (capacity() <= rhs.capacity()) {
                swap(*this, rhs); // Efficient swap if rhs has enough capacity
            }
            else {
                resize(rhs.size(), false); // Reuse existing buffer, copy data
                memcpy(data(), rhs.data(), sizeof(value_type) * size());
            }
        }
        return *this;
    }

    /**
     * @brief Copy assignment operator.
     *
     * Uses base class copy assignment.
     * @param rhs Source XA to copy from.
     * @return Reference to this XA.
     */
    XA& operator = (const XA& rhs) noexcept {
        mz::Vector<int>::operator=(rhs);
        return *this;
    }

};

#endif