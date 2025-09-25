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

 /**
  * @file RandomAccessIteratorInterface.h
  * @brief Generic random access iterator for mz containers and views.
  *
  * Provides flexible iterator implementation that can be used with:
  * 1. Containers that return references (traditional storage containers)
  * 2. Views that return values (e.g., Span, Slice, or computed views)
  *
  * The implementation detects the container's capabilities through concepts:
  * - HasGetLvalue: Container provides reference access via get_lvalue
  * - HasGetValue: Container provides value access via get_value
  * - HasGetPointer: Container provides pointer access via get_pointer
  *
  * Example usage:
  *   auto it = container.begin();
  *   ++it;
  *   auto value = *it;
  */

#pragma once
#include <concepts>
#include <type_traits>
#include <stdexcept>
#include <iterator>

namespace mz {

    /**
     * @brief Concept for containers that provide reference access
     *
     * Matches containers with a get_lvalue method that returns a reference
     */
    template <typename T>
    concept HasGetLvalue = requires(T t, std::ptrdiff_t i) {
        { t.get_lvalue(i) };
    };

    /**
     * @brief Concept for containers that provide value access
     *
     * Matches containers with a get_value method that returns a value
     */
    template <typename T>
    concept HasGetValue = requires(T t, std::ptrdiff_t i) {
        { t.get_value(i) };
    };

    /**
     * @brief Concept for containers that provide pointer access
     *
     * Matches containers with a get_pointer method that returns a pointer
     */
    template <typename T>
    concept HasGetPointer = requires(T t, std::ptrdiff_t i) {
        { t.get_pointer(i) };
    };

    /**
     * @brief Const random access iterator interface for containers
     *
     * Provides a generic iterator that adapts to the container's access methods:
     * - For containers with get_lvalue: returns references
     * - For containers with get_value: returns values
     * - operator->() is only available for containers with get_pointer
     *
     * @tparam ContainerT Container type to iterate over
     */
    template <typename ContainerT>
    class ConstRandomAccessIteratorInterface {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = typename ContainerT::difference_type;
        using value_type = typename ContainerT::value_type;
        using reference = typename ContainerT::const_reference;
        using pointer = typename ContainerT::const_pointer;

    protected:
        ContainerT* container_ = nullptr;
        std::ptrdiff_t index_ = 0;

    public:
        // Constructors
        constexpr ConstRandomAccessIteratorInterface() noexcept = default;

        constexpr ConstRandomAccessIteratorInterface(ContainerT* container, std::ptrdiff_t index) noexcept
            : container_(container), index_(index) {
        }

        // Copy/move operations
        constexpr ConstRandomAccessIteratorInterface(const ConstRandomAccessIteratorInterface&) noexcept = default;
        constexpr ConstRandomAccessIteratorInterface(ConstRandomAccessIteratorInterface&&) noexcept = default;
        constexpr ConstRandomAccessIteratorInterface& operator=(const ConstRandomAccessIteratorInterface&) noexcept = default;
        constexpr ConstRandomAccessIteratorInterface& operator=(ConstRandomAccessIteratorInterface&&) noexcept = default;

        // Accessors
        constexpr std::ptrdiff_t index() const noexcept { return index_; }
        constexpr ContainerT* container() const noexcept { return container_; }

        // Element access - adapts to container capabilities
        constexpr auto operator*() const noexcept {
            if constexpr (HasGetLvalue<ContainerT>) {
                return container_->get_lvalue(index_);
            }
            else if constexpr (HasGetValue<ContainerT>) {
                return container_->get_value(index_);
            }
        }

        constexpr auto operator[](difference_type offset) const noexcept {
            return *(*this + offset);
        }

        // operator-> only if get_pointer is available
        //template <typename T = ContainerT>
        constexpr auto operator->() const noexcept
            requires HasGetPointer<ContainerT>
        {
            return container_->get_pointer(index_);
        }

        // Increment/decrement
        constexpr ConstRandomAccessIteratorInterface& operator++() noexcept {
            ++index_;
            return *this;
        }

        constexpr ConstRandomAccessIteratorInterface& operator--() noexcept {
            --index_;
            return *this;
        }

        constexpr ConstRandomAccessIteratorInterface operator++(int) noexcept {
            auto temp = *this;
            ++index_;
            return temp;
        }

        constexpr ConstRandomAccessIteratorInterface operator--(int) noexcept {
            auto temp = *this;
            --index_;
            return temp;
        }

        // Arithmetic
        constexpr ConstRandomAccessIteratorInterface& operator+=(difference_type n) noexcept {
            index_ += n;
            return *this;
        }

        constexpr ConstRandomAccessIteratorInterface& operator-=(difference_type n) noexcept {
            index_ -= n;
            return *this;
        }

        constexpr ConstRandomAccessIteratorInterface operator+(difference_type n) const noexcept {
            auto temp = *this;
            temp += n;
            return temp;
        }

        constexpr ConstRandomAccessIteratorInterface operator-(difference_type n) const noexcept {
            auto temp = *this;
            temp -= n;
            return temp;
        }

        // Iterator difference
        difference_type operator-(const ConstRandomAccessIteratorInterface& other) const {
            if (container_ == nullptr || other.container_ == nullptr) {
                throw std::runtime_error("Subtracting null iterators");
            }
            if (container_ != other.container_) {
                throw std::runtime_error("Subtracting iterators from different containers");
            }
            return index_ - other.index_;
        }

        // Comparisons
        constexpr bool operator==(const ConstRandomAccessIteratorInterface& other) const noexcept {
            return (container_ == other.container_) && (index_ == other.index_);
        }

        constexpr bool operator!=(const ConstRandomAccessIteratorInterface& other) const noexcept {
            return !(*this == other);
        }

        bool operator<(const ConstRandomAccessIteratorInterface& other) const {
            if (container_ == nullptr || other.container_ == nullptr) {
                throw std::runtime_error("Comparing null iterators");
            }
            if (container_ != other.container_) {
                throw std::runtime_error("Comparing iterators from different containers");
            }
            return index_ < other.index_;
        }

        bool operator>(const ConstRandomAccessIteratorInterface& other) const { return other < *this; }
        bool operator<=(const ConstRandomAccessIteratorInterface& other) const { return !(*this > other); }
        bool operator>=(const ConstRandomAccessIteratorInterface& other) const { return !(*this < other); }
    };

    /**
     * @brief Mutable random access iterator interface for containers
     *
     * Provides a generic iterator that adapts to the container's access methods
     * and allows modification of elements when using reference-returning containers.
     *
     * @tparam ContainerT Container type to iterate over
     */
    template <typename ContainerT>
    class MutableRandomAccessIteratorInterface : public ConstRandomAccessIteratorInterface<ContainerT> {
        using Base = ConstRandomAccessIteratorInterface<ContainerT>;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = typename ContainerT::difference_type;
        using value_type = typename ContainerT::value_type;
        using reference = typename ContainerT::reference;
        using pointer = typename ContainerT::pointer;

        // Constructors and copy/move operations
        using Base::Base;
        constexpr MutableRandomAccessIteratorInterface() noexcept = default;
        constexpr MutableRandomAccessIteratorInterface(const MutableRandomAccessIteratorInterface&) noexcept = default;
        constexpr MutableRandomAccessIteratorInterface(MutableRandomAccessIteratorInterface&&) noexcept = default;
        constexpr MutableRandomAccessIteratorInterface& operator=(const MutableRandomAccessIteratorInterface&) noexcept = default;
        constexpr MutableRandomAccessIteratorInterface& operator=(MutableRandomAccessIteratorInterface&&) noexcept = default;

        // Element access - adapts to container capabilities
        constexpr auto operator*() const noexcept {
            if constexpr (HasGetLvalue<ContainerT>) {
                return this->container_->get_lvalue(this->index_);
            }
            else if constexpr (HasGetValue<ContainerT>) {
                return this->container_->get_value(this->index_);
            }
        }

        constexpr auto operator[](difference_type offset) const noexcept {
            return *(*this + offset);
        }

        // operator-> only if get_pointer is available
        //template <typename T = ContainerT>
        constexpr auto operator->() const noexcept
            requires HasGetPointer<ContainerT>
        {
            return this->container_->get_pointer(this->index_);
        }

        // Arithmetic operations (return MutableRandomAccessIteratorInterface)
        constexpr MutableRandomAccessIteratorInterface& operator++() noexcept { Base::operator++(); return *this; }
        constexpr MutableRandomAccessIteratorInterface& operator--() noexcept { Base::operator--(); return *this; }
        constexpr MutableRandomAccessIteratorInterface operator++(int) noexcept { MutableRandomAccessIteratorInterface temp{ *this }; Base::operator++(); return temp; }
        constexpr MutableRandomAccessIteratorInterface operator--(int) noexcept { MutableRandomAccessIteratorInterface temp{ *this }; Base::operator--(); return temp; }
        constexpr MutableRandomAccessIteratorInterface& operator+=(difference_type n) noexcept { Base::operator+=(n); return *this; }
        constexpr MutableRandomAccessIteratorInterface& operator-=(difference_type n) noexcept { Base::operator-=(n); return *this; }
        constexpr MutableRandomAccessIteratorInterface operator+(difference_type n) const noexcept { MutableRandomAccessIteratorInterface temp{ *this }; temp += n; return temp; }
        constexpr MutableRandomAccessIteratorInterface operator-(difference_type n) const noexcept { MutableRandomAccessIteratorInterface temp{ *this }; temp -= n; return temp; }

        // Iterator difference
        difference_type operator-(const MutableRandomAccessIteratorInterface& other) const {
            if (this->container_ == nullptr || other.container_ == nullptr) {
                throw std::runtime_error("Subtracting null iterators");
            }
            if (this->container_ != other.container_) {
                throw std::runtime_error("Subtracting iterators from different containers");
            }
            return this->index_ - other.index_;
        }


        // Comparisons
        constexpr bool operator==(const MutableRandomAccessIteratorInterface& other) const noexcept {
            return (this->container_ == other.container_) && (this->index_ == other.index_);
        }

        constexpr bool operator!=(const MutableRandomAccessIteratorInterface& other) const noexcept {
            return !(*this == other);
        }

        bool operator<(const MutableRandomAccessIteratorInterface& other) const {
            if (this->container_ == nullptr || other.container_ == nullptr) {
                throw std::runtime_error("Comparing null iterators");
            }
            if (this->container_ != other.container_) {
                throw std::runtime_error("Comparing iterators from different containers");
            }
            return this->index_ < other.index_;
        }

        bool operator>(const MutableRandomAccessIteratorInterface& other) const { return other < *this; }
        bool operator<=(const MutableRandomAccessIteratorInterface& other) const { return !(*this > other); }
        bool operator>=(const MutableRandomAccessIteratorInterface& other) const { return !(*this < other); }



    };

} // namespace mz
