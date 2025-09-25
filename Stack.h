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

#ifndef MZ_STACK_HEADER_FILE
#define MZ_STACK_HEADER_FILE

#pragma once
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include "globals.h"

/**
 * @file Stack.h
 * @brief Provides a simple stack container for storing pointers to objects of type T.
 *
 * mz::Stack<T> is a non-owning, dynamically resizable stack of pointers to T.
 * It supports push, pop, sorting, uniqueness, and basic access methods.
 * The user is responsible for managing the lifetime of the pointed-to objects.
 */

namespace mz {

    /**
     * @brief A non-owning, dynamically resizable stack of pointers to T.
     *
     * This class stores pointers to objects of type T. It does not manage the memory
     * of the objects themselves, only the pointer array. The user must ensure that
     * the objects pointed to remain valid for the lifetime of the stack.
     *
     * @tparam T The type of object pointers stored.
     */
    template <class T>
    class Stack {
    public:
        // Type aliases for convenience and generic programming
        using value_type = std::remove_cv_t<T>;
        using reference = value_type&;
        using pointer = value_type*;
        using pointer_reference = pointer&;
        using const_pointer = value_type const*;
        using data_type = pointer*;
        using const_data_type = const_pointer*;

    private:
        data_type m_data = nullptr; ///< Pointer to array of T*
        size_type m_size = 0;       ///< Number of elements in stack
        size_type m_cap = 0;       ///< Capacity of stack

    public:
        /**
         * @brief Swaps the contents of this stack with another.
         * @param rhs The other stack to swap with.
         */
        constexpr void swap_with(Stack& rhs) noexcept {
            std::swap(m_data, rhs.m_data);
            std::swap(m_size, rhs.m_size);
            std::swap(m_cap, rhs.m_cap);
        }

        /**
         * @brief Friend swap function for std::swap compatibility.
         */
        friend constexpr void swap(Stack& lhs, Stack& rhs) noexcept { lhs.swap_with(rhs); }
        friend constexpr void swap(Stack& lhs, Stack&& rhs) noexcept { swap(lhs, rhs); }

        /**
         * @brief Resizes the internal pointer array in place, doubling its capacity.
         *        Used when pushing beyond current capacity.
         */
        void resize_in_place() noexcept {
            Stack temp(m_cap * 2 + 2);
            temp.m_size = m_size;
            std::memcpy(temp.m_data, m_data, sizeof(pointer) * m_size);
            swap(*this, temp);
        }

        // --- Constructors and Destructor ---

        /**
         * @brief Default constructor. Creates an empty stack.
         */
        Stack() : m_data{ nullptr }, m_size{ 0 }, m_cap{ 0 } {}

        /**
         * @brief Constructs a stack with a given initial capacity.
         * @param capacity Initial capacity of the stack.
         */
        explicit Stack(size_type capacity)
            : m_data{ new pointer[capacity] }, m_size{ 0 }, m_cap{ capacity } {
        }

        /**
         * @brief Destructor. Deletes the internal pointer array.
         */
        ~Stack() { delete[] m_data; }

        // --- Capacity and Size ---

        /**
         * @brief Returns the number of elements in the stack.
         */
        size_type size() const noexcept { return m_size; }

        /**
         * @brief Returns the number of elements as a signed integer.
         */
        isize_type isize() const noexcept { return static_cast<isize_type>(m_size); }

        /**
         * @brief Returns the number of elements as an unsigned integer.
         */
        usize_type usize() const noexcept { return static_cast<usize_type>(m_size); }

        /**
         * @brief Returns the current capacity of the stack.
         */
        size_type capacity() const noexcept { return m_cap; }

        // --- Iterators ---

        /**
         * @brief Returns a pointer to one-past-the-last element (mutable).
         */
        constexpr data_type end() noexcept { return m_data + m_size; }

        /**
         * @brief Returns a pointer to the first element (mutable).
         */
        constexpr data_type begin() noexcept { return m_data; }

        /**
         * @brief Returns a pointer to one-past-the-last element (const).
         */
        constexpr const_data_type end() const noexcept { return m_data + m_size; }

        /**
         * @brief Returns a pointer to the first element (const).
         */
        constexpr const_data_type begin() const noexcept { return m_data; }

        // --- Element Access ---

        /**
         * @brief Returns the pointer at the given index, or nullptr if out of bounds.
         * @param index Index of the element.
         */
        constexpr pointer operator[](index_type index) const noexcept {
            return index < m_size ? m_data[index] : nullptr;
        }

        /**
         * @brief Swaps two elements at the given indices.
         * @param lidx Index of the first element.
         * @param ridx Index of the second element.
         */
        constexpr void swap_elements(index_type lidx, index_type ridx) noexcept {
            pointer temp = m_data[lidx];
            m_data[lidx] = m_data[ridx];
            m_data[ridx] = temp;
        }

        // --- Modifiers ---

        /**
         * @brief Clears the stack, setting all pointers to nullptr and size to zero.
         */
        Stack& clear() noexcept {
            if (m_size) {
                std::memset(m_data, 0, sizeof(pointer) * m_size);
                m_size = 0;
            }
            return *this;
        }

        /**
         * @brief Pushes a reference to an element onto the stack.
         * @param elem Reference to the element to push.
         * @return Position where the element was inserted.
         */
        size_type push(reference elem) noexcept {
            if (m_size == m_cap) resize_in_place();
            size_type pos = m_size++;
            m_data[pos] = &elem;
            return pos;
        }

        /**
         * @brief Pushes a pointer to an element onto the stack.
         * @param elem Pointer to the element to push.
         * @return Position where the element was inserted, or INT_MIN if elem is nullptr.
         */
        size_type push(pointer elem) noexcept {
            if (elem) return push(*elem);
            return INT_MIN;
        }

        /**
         * @brief Returns the last element in the stack (const).
         */
        inline const_pointer unsafe_back() const noexcept { return m_data[m_size - 1]; }

        /**
         * @brief Returns the last element in the stack (mutable).
         */
        inline pointer_reference unsafe_back() noexcept { return m_data[m_size - 1]; }

        /**
         * @brief Pops and returns the last non-null pointer in the stack.
         *        Sets the pointer to nullptr after popping.
         * @return Pointer to the popped element, or nullptr if stack is empty.
         */
        inline pointer pop() {
            pointer res = nullptr;
            while (m_size > 0) {
                --m_size;
                res = m_data[m_size];
                m_data[m_size] = nullptr;
                if (res) return res;
            }
            return res;
        }

        /**
         * @brief Pops and returns the pointer at the given index, setting it to nullptr.
         * @param index Index of the element to pop.
         * @return Pointer to the popped element, or nullptr if index is out of bounds.
         */
        inline pointer pop(index_type index) {
            pointer elem = nullptr;
            if (index >= 0 && size_type(index) < m_size) {
                elem = m_data[index];
                m_data[index] = nullptr;
            }
            return elem;
        }

        // --- Copy and Move Semantics (deleted for safety) ---

        Stack(Stack&&) = delete;
        Stack(Stack const&) = delete;
        Stack& operator=(Stack&&) = delete;
        Stack& operator=(Stack const&) = delete;

        // --- State Queries ---

        /**
         * @brief Returns true if the stack is empty.
         */
        constexpr bool empty() const noexcept { return m_size == 0; }

        /**
         * @brief Returns true if the stack is non-empty.
         */
        constexpr operator bool() const noexcept { return m_size != 0; }

        /**
         * @brief Returns true if the stack is empty.
         */
        constexpr bool operator!() const noexcept { return m_size == 0; }

        // --- Algorithms ---

        /**
         * @brief Sorts the stack in-place by pointer value.
         * @return Reference to this stack.
         */
        Stack& sort() noexcept {
            std::sort(m_data, m_data + m_size);
            return *this;
        }

        /**
         * @brief Removes duplicate pointers from the stack, setting unused slots to nullptr.
         */
        void unique() noexcept {
            auto end_ptr = std::unique(m_data, m_data + m_size);
            for (auto ptr = end_ptr; ptr < m_data + m_size; ++ptr) { *ptr = nullptr; }
            m_size = static_cast<size_type>(end_ptr - m_data);
        }
    };

} // namespace mz

#endif // MZ_STACK_HEADER_FILE