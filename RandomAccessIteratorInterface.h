#pragma once
#include <concepts>
#include <type_traits>
#include <stdexcept>

/**
 * @file random_access_iterator.h
 * @brief Provides generic random access iterator classes for containers in the mz library.
 *
 * This header defines two iterator classes:
 *   - mz::ConstRandomAccessIteratorInterface: a const random access iterator.
 *   - mz::MutableRandomAccessIteratorInterface: a mutable random access iterator.
 *
 * These iterators are designed to work with containers that provide
 * get_lvalue(index) and get_pointer(index) member functions.
 *
 * Usage example:
 *   mz::Vector<int> vec;
 *   mz::MutableRandomAccessIteratorInterface<int> it(&vec, 0);
 *   ++it; *it = 42;
 */

namespace mz {

    /**
     * @brief Const random access iterator for a container type ContainerT.
     *
     * Requirements for ContainerT:
     *   - ContainerT::difference_type, value_type, const_reference, const_pointer typedefs.
     *   - ContainerT* must be a pointer to the container.
     *   - ContainerT must provide get_lvalue(index) and get_pointer(index) methods.
     */
    template <typename ContainerT>
    class ConstRandomAccessIteratorInterface {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = typename ContainerT::difference_type;
        using value_type = typename ContainerT::value_type;
        using reference = typename ContainerT::const_reference;
        using pointer = typename ContainerT::const_pointer;

    private:
        ContainerT* container_;
        long long index_;

    public:
        // Constructors
        constexpr ConstRandomAccessIteratorInterface(ContainerT* container, long long index) noexcept
            : container_(container), index_(index) {
        }

        constexpr ConstRandomAccessIteratorInterface() noexcept
            : container_(nullptr), index_(0) {
        }

        // Copy constructor
        constexpr ConstRandomAccessIteratorInterface(const ConstRandomAccessIteratorInterface& other) noexcept
            : container_(other.container_), index_(other.index_) {
        }

        // Move constructor
        constexpr ConstRandomAccessIteratorInterface(ConstRandomAccessIteratorInterface&& other) noexcept
            : container_(other.container_), index_(other.index_) {
        }

        // Copy assignment
        constexpr ConstRandomAccessIteratorInterface& operator=(const ConstRandomAccessIteratorInterface& other) noexcept {
            if (this != &other) {
                container_ = other.container_;
                index_ = other.index_;
            }
            return *this;
        }

        // Move assignment
        constexpr ConstRandomAccessIteratorInterface& operator=(ConstRandomAccessIteratorInterface&& other) noexcept {
            if (this != &other) {
                container_ = other.container_;
                index_ = other.index_;
            }
            return *this;
        }

        // Accessors
        constexpr long long index() const noexcept { return index_; }
        constexpr ContainerT* container() const noexcept { return container_; }

        // Element access
        constexpr reference operator[](difference_type offset) const noexcept {
            return *(*this + offset);
        }

        constexpr reference operator*() const noexcept {
            return container_->get_lvalue(index_);
        }

        constexpr pointer operator->() const noexcept {
            return container_->get_pointer(index_);
        }

        // Increment/decrement
        constexpr ConstRandomAccessIteratorInterface& operator++() noexcept {
            ++index_; return *this;
        }
        constexpr ConstRandomAccessIteratorInterface& operator--() noexcept {
            --index_; return *this;
        }
        constexpr ConstRandomAccessIteratorInterface operator++(int) noexcept {
            ConstRandomAccessIteratorInterface temp{ *this }; ++index_; return temp;
        }
        constexpr ConstRandomAccessIteratorInterface operator--(int) noexcept {
            ConstRandomAccessIteratorInterface temp{ *this }; --index_; return temp;
        }

        // Arithmetic
        constexpr ConstRandomAccessIteratorInterface& operator+=(difference_type n) noexcept {
            index_ += n; return *this;
        }
        constexpr ConstRandomAccessIteratorInterface& operator-=(difference_type n) noexcept {
            index_ -= n; return *this;
        }
        constexpr ConstRandomAccessIteratorInterface operator+(difference_type n) const noexcept {
            ConstRandomAccessIteratorInterface temp{ *this }; temp.index_ += n; return temp;
        }
        constexpr ConstRandomAccessIteratorInterface operator-(difference_type n) const noexcept {
            ConstRandomAccessIteratorInterface temp{ *this }; temp.index_ -= n; return temp;
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
     * @brief Mutable random access iterator for a container type ContainerT.
     *
     * Inherits from ConstRandomAccessIteratorInterface and provides mutable access.
     * Requirements for ContainerT:
     *   - ContainerT::reference, pointer typedefs.
     *   - get_lvalue(index) and get_pointer(index) must return mutable reference/pointer.
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

        // Constructors
        constexpr MutableRandomAccessIteratorInterface(ContainerT* container, long long index) noexcept
            : Base{ container, index } {
        }

        constexpr MutableRandomAccessIteratorInterface() noexcept
            : Base{ nullptr, 0 } {
        }

        // Copy constructor
        constexpr MutableRandomAccessIteratorInterface(const MutableRandomAccessIteratorInterface& other) noexcept
            : Base(other) {
        }

        // Move constructor
        constexpr MutableRandomAccessIteratorInterface(MutableRandomAccessIteratorInterface&& other) noexcept
            : Base(std::move(other)) {
        }

        // Copy assignment
        constexpr MutableRandomAccessIteratorInterface& operator=(const MutableRandomAccessIteratorInterface& other) noexcept {
            Base::operator=(other);
            return *this;
        }

        // Move assignment
        constexpr MutableRandomAccessIteratorInterface& operator=(MutableRandomAccessIteratorInterface&& other) noexcept {
            Base::operator=(std::move(other));
            return *this;
        }

        // Element access (mutable)
        constexpr reference operator[](difference_type offset) const noexcept {
            // Note: const_cast is used to convert const_reference to reference.
            return const_cast<reference>(Base::operator[](offset));
        }
        constexpr reference operator*() const noexcept {
            return const_cast<reference>(Base::operator*());
        }
        constexpr pointer operator->() const noexcept {
            return const_cast<pointer>(Base::operator->());
        }

        // Increment/decrement
        constexpr MutableRandomAccessIteratorInterface& operator++() noexcept { Base::operator++(); return *this; }
        constexpr MutableRandomAccessIteratorInterface& operator--() noexcept { Base::operator--(); return *this; }
        constexpr MutableRandomAccessIteratorInterface operator++(int) noexcept { MutableRandomAccessIteratorInterface temp{ *this }; Base::operator++(); return temp; }
        constexpr MutableRandomAccessIteratorInterface operator--(int) noexcept { MutableRandomAccessIteratorInterface temp{ *this }; Base::operator--(); return temp; }

        // Arithmetic
        constexpr MutableRandomAccessIteratorInterface& operator+=(difference_type n) noexcept { Base::operator+=(n); return *this; }
        constexpr MutableRandomAccessIteratorInterface& operator-=(difference_type n) noexcept { Base::operator-=(n); return *this; }
        constexpr MutableRandomAccessIteratorInterface operator+(difference_type n) const noexcept { MutableRandomAccessIteratorInterface temp{ *this }; temp += n; return temp; }
        constexpr MutableRandomAccessIteratorInterface operator-(difference_type n) const noexcept { MutableRandomAccessIteratorInterface temp{ *this }; temp -= n; return temp; }
    };

} // namespace mz




