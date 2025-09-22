#ifndef MZ_SLICE_HEADER_FILE
#define MZ_SLICE_HEADER_FILE
#pragma once

#include "globals.h"
#include <type_traits>
#include "Span.h"
#include "ElementwiseOperationsInterface.h"

namespace mz {

    /**
     * @brief A non-owning view over a strided sequence of elements.
     * Provides elementwise operations, assignment, and casting utilities.
     *
     * @tparam Ty_ The type of elements in the sequence.
     */
    template <typename Ty_>
    class Slice : public ElementwiseMutableOperationsInterface<Slice<Ty_>, Ty_> {
    public:
        // --- Type Aliases ---
        using element_type = Ty_;
        using value_type = std::remove_cvref_t<Ty_>;
        using pointer = value_type*;
        using reference = value_type&;
        using const_pointer = value_type const*;
        using const_reference = mz::return_cvref_t<value_type>::type;

    private:
        // --- Data Members ---
        pointer m_data{ nullptr };
        size_type m_size{ 0 };
        size_type m_step{ 1 };

    public:
        // --- View Struct ---
        /**
         * @brief Structure for lightweight view construction/assignment.
         */
        struct slice_view {
            pointer m_data;
            size_type m_size;
            size_type m_step;
        };

        /**
         * @brief Construct from a slice_view.
         */
        constexpr Slice(slice_view sv) noexcept : Slice{ sv.m_data, sv.m_size, sv.m_step } {}

        /**
         * @brief Assign from a slice_view.
         */
        constexpr Slice& operator = (slice_view sv) noexcept { m_data = sv.m_data; m_size = sv.m_size; return *this; }

        /**
         * @brief Get a slice_view representing this slice.
         */
        constexpr slice_view view() const noexcept { return slice_view{ m_data, m_size, m_step }; }

        // --- Constructors ---
        /**
         * @brief Default constructor. Creates an empty slice.
         */
        constexpr Slice() noexcept : m_data{ nullptr }, m_size{ 0 }, m_step{ 1 } {}

        /**
         * @brief Construct from pointer and size (step defaults to 1).
         */
        constexpr Slice(Ty_* Pointer, size_type Size) noexcept :
            m_data{ const_cast<pointer>(Pointer) },
            m_size{ Size },
            m_step{ 1 } {
        }

        /**
         * @brief Construct from pointer, size, and step.
         */
        constexpr Slice(Ty_* Pointer, size_type Size, size_type Step) noexcept :
            m_data{ const_cast<pointer>(Pointer) },
            m_size{ Size },
            m_step{ Step } {
        }

        /**
         * @brief Construct from a Span (step defaults to 1).
         */
        constexpr Slice(mz::Span<Ty_> SP) noexcept : Slice{ SP.data(), SP.size() } {}

        // --- Assignment ---
        /**
         * @brief Assign from raw pointer, size, and step.
         */
        constexpr void assign(pointer Pointer, size_type size, size_type step = 1) noexcept {
            m_data = Pointer;
            m_size = size;
            m_step = step;
        }

        /**
         * @brief Assign from another Slice.
         */
        constexpr void assign(Slice other) noexcept {
            m_data = other.m_data;
            m_size = other.m_size;
            m_step = other.m_step;
        }

        /**
         * @brief Assign from another compatible Slice (bitwise compatible types).
         */
        template <typename Q>
        constexpr void set(Slice<Q> rhs) noexcept {
            m_data = reinterpret_cast<pointer>(rhs.data());
            m_size = rhs.size();
            m_step = rhs.step();
        }

        // --- Casting ---
        /**
         * @brief Cast this slice to another compatible type.
         */
        template <typename Q>
        __inline constexpr Slice<std::remove_cvref_t<Q>> cast_as() noexcept { return Slice<std::remove_cvref_t<Q>>{static_cast_as<std::remove_cvref_t<Q>>(m_data), m_size, m_step}; }

        /**
         * @brief Cast this slice to another compatible const type.
         */
        template <typename Q>
        __inline constexpr Slice<std::remove_cvref_t<Q> const> cast_as() const noexcept {
            return Slice<std::remove_cvref_t<Q> const>{static_cast_as<std::remove_cvref_t<Q> const>(m_data), m_size, m_step};
            return const_cast<Slice*>(this)->cast_as<Q const>();
        }

        // --- Validity and Properties ---
        /**
         * @brief Returns true if the slice is valid (non-null, non-negative size, non-zero step).
         */
        constexpr bool valid() const noexcept { return m_data != nullptr && m_size >= 0 && m_step != 0; }

        /**
         * @brief Returns true if the slice is contiguous (step == 1).
         */
        constexpr bool contiguous() const noexcept { return m_step == 1; }

        /**
         * @brief Returns the number of elements in the slice.
         */
        constexpr size_type size() const noexcept { return m_size; }

        /**
         * @brief Returns the step size.
         */
        constexpr size_type step() const noexcept { return m_step; }

        /**
         * @brief Returns the signed size.
         */
        constexpr isize_type isize() const noexcept { return static_cast<isize_type>(m_size); }

        /**
         * @brief Returns the signed step.
         */
        constexpr isize_type istep() const noexcept { return static_cast<isize_type>(m_step); }

        /**
         * @brief Returns the unsigned size.
         */
        constexpr usize_type usize() const noexcept { return static_cast<usize_type>(m_size); }

        /**
         * @brief Returns the unsigned step.
         */
        constexpr usize_type ustep() const noexcept { return static_cast<usize_type>(m_step); }

        // --- Data Access ---
        /**
         * @brief Returns the raw data pointer.
         */
        constexpr pointer data() noexcept { return m_data; }
        constexpr const_pointer data() const noexcept { return m_data; }

        // --- Element Access ---
        /**
         * @brief Access element by index (applies stride).
         */
        constexpr reference operator[](size_type index) { return m_data[index * m_step]; }
        constexpr const_reference operator[](size_type index) const noexcept { return m_data[index * m_step]; }

        // --- Elementwise Assignment ---
        /**
         * @brief Elementwise assignment from another Slice.
         */
        Slice& operator = (Slice rhs)
        {
            DOMAIN_ERROR_IF(size() != rhs.size(), "Slice assignment with Slice of different size: {} != {}\n", size(), rhs.size());
            if (std::is_trivially_copyable_v<value_type> && step() == 1 && rhs.step() == 1)
            {
                memcpy(data(), rhs.data(), sizeof(value_type) * size());
            }
            else
            {
                for (size_type i = 0; i < size(); i++) { operator[](i) = rhs[i]; }
            }
            return *this;
        }

        /**
         * @brief Elementwise assignment from a compatible sequence.
         */
        template <Sequence Seq> requires requires (value_type v, typename Seq::value_type s) { v = s; }
        Slice& operator = (Seq rhs) {
            DOMAIN_ERROR_IF(size() != rhs.size(), "elementwise assignment size mismatch: {} != {}\n", size(), rhs.size());
            for (size_type i = 0; i < size(); i++) { operator[](i) = rhs[i]; }
            return *this;
        }

        /**
         * @brief Assign all elements to a single value.
         */
        template <typename U> requires requires (value_type v, U u) { v = u; }
        Slice& operator = (U const& y) noexcept { value_type t{ static_cast<value_type>(y) }; for (size_type i = 0; i < size(); i++) operator[](i) = t; return *this; }

        /**
         * @brief Elementwise assignment from an initializer_list.
         */
        template <typename U> requires requires(value_type x, U y) { x = y; }
        Slice& operator = (std::initializer_list<U> const& li) {
            DOMAIN_ERROR_IF(size() != li.size(), "elementwise assignment size mismatch: {} != {}\n", size(), li.size());
            U const* source = li.begin();
            for (size_type i = 0; i < size(); i++) { operator[](i) = source[i]; }
            return *this;
        }

        // --- Swap Operations ---
        /**
         * @brief Swap elements with another sequence (size checked).
         */
        template <Sequence Seq> requires requires (reference v, typename Seq::reference s) { std::swap(v, s); }
        void swap_elements(Seq it) {
            size_type const L{ size() };
            INVALID_ARGUMENT_IF(L != it.size(), "Slice::swap_ size mismatch: {} != {}", L, it.size());
            for (size_type i = 0; i < L; i++) { std::swap(operator[](i), it[i]); }
        }

        // --- Utility ---
        /**
         * @brief Returns a lambda for element access by index.
         */
        __inline constexpr auto&& Func() noexcept { return [&](auto i) constexpr noexcept -> reference { return operator[](i); }; }
        __inline constexpr auto&& Func() const noexcept { return [&](auto i) constexpr noexcept -> const_reference { return operator[](i); }; }

        /**
         * @brief Computes the dot product with a functor.
         */
        value_type dot(auto&& PF) const noexcept {
            value_type D{ 0 };
            for (size_type i = 0; i < size(); i++) { D += operator[](i) * PF(i); }
            return D;
        }

        /**
         * @brief Output the slice as a string using the format API.
         */
        friend std::ostream& operator << (std::ostream& os, Slice const& bf) { return os << bf.string(); }
    };

    /**
     * @brief Const version of Slice. Provides a read-only view over a strided sequence of elements.
     * Supports elementwise formatting and string conversion.
     *
     * @tparam Ty_ The type of elements in the sequence.
     */
    template <typename Ty_>
    class Slice<Ty_ const> : public ElementwiseConstOperationsInterface<Slice<Ty_ const>, Ty_ const>
    {
    public:
        using element_type = Ty_ const;
        using value_type = std::remove_cvref_t<Ty_>;
        using const_type = value_type const;
        using pointer = value_type const*;
        using reference = mz::return_cvref_t<value_type>::type;
        using const_pointer = value_type const*;
        using const_reference = mz::return_cvref_t<value_type>::type;
        using container_t = typename mz::Span<const_type>;

    private:
        pointer m_data{ nullptr };
        size_type m_size{ 0 };
        size_type m_step{ 1 };

    public:
        // --- View Struct ---
        struct slice_view {
            pointer m_data;
            size_type m_size;
            size_type m_step;
        };
        constexpr Slice(slice_view sv) noexcept : Slice{ sv.m_data, sv.m_size, sv.m_step } {}
        constexpr Slice& operator = (slice_view sv) noexcept { m_data = sv.m_data; m_size = sv.m_size; return *this; }
        constexpr slice_view view() const noexcept { return slice_view{ m_data, m_size, m_step }; }

        // --- Constructors ---
        constexpr Slice() noexcept : m_data{ nullptr }, m_size{ 0 } {}

        constexpr Slice(Ty_ const* begin, size_type Size) :
            m_data{ const_cast<pointer>(begin) },
            m_size{ Size },
            m_step{ 1 } {
        }

        constexpr Slice(Ty_ const* begin, size_type Size, size_type Step) :
            m_data{ const_cast<pointer>(begin) },
            m_size{ Size },
            m_step{ Step } {
        }

        constexpr Slice(Slice<value_type> Rhs) noexcept : Slice{ Rhs.data(), Rhs.size(), Rhs.step() } {}
        constexpr Slice(mz::Span<value_type const> Rhs) noexcept : Slice{ Rhs.data(), Rhs.size() } {}
        explicit constexpr Slice(mz::Span<value_type> Rhs) noexcept : Slice{ Rhs.data(), Rhs.size() } {}

        // --- Assignment ---
        constexpr void assign(Slice other) noexcept {
            m_data = other.m_data;
            m_size = other.m_size;
            m_step = other.m_step;
        }

        // --- Validity and Properties ---
        constexpr bool valid() const noexcept { return m_data != nullptr && m_size >= 0 && m_step != 0; }
        constexpr bool contiguous() const noexcept { return m_step == 1; }
        constexpr size_type size() const noexcept { return m_size; }
        constexpr size_type step() const noexcept { return m_step; }
        constexpr isize_type isize() const noexcept { return static_cast<isize_type>(m_size); }
        constexpr isize_type istep() const noexcept { return static_cast<isize_type>(m_step); }
        constexpr usize_type usize() const noexcept { return static_cast<usize_type>(m_size); }
        constexpr usize_type ustep() const noexcept { return static_cast<usize_type>(m_step); }

        // --- Element Access ---
        constexpr const_reference operator[](index_type index) const noexcept { return m_data[index * m_step]; }

        // --- Utility ---
        __inline constexpr auto&& Func() const noexcept { return [&](auto i) constexpr noexcept -> const_reference { return operator[](i); }; }

        /**
         * @brief Computes the dot product with a functor.
         */
        value_type dot(auto&& PF) const noexcept {
            value_type D{ 0 };
            for (size_type i = 0; i < size(); i++) { D += operator[](i) * PF(i); }
            return D;
        }

        // --- Casting ---
        template <typename Q>
        __inline constexpr Slice<Q const> cast_as() const noexcept { return Slice<Q const>{static_cast_as<Q const>(m_data), m_size, m_step}; }

        /**
         * @brief Remove constness from the slice (dangerous, use with care).
         */
        __inline constexpr Slice<value_type> remove_const() noexcept { return Slice<value_type>{const_cast<value_type*>(m_data), m_size, m_step}; }
    };

} // namespace mz

// -----------------------------------------------------------------------------
// Global algorithms for slices
// -----------------------------------------------------------------------------

/**
 * @brief Returns true if any element in the Slice is nonzero.
 */
template <typename T = double>
bool any(mz::Slice<T> bf) noexcept {
    for (size_type i = 0; i < bf.size(); i++) {
        if (bf[i]) return true;
    }
    return false;
}

/**
 * @brief Returns true if all elements in the Slice are nonzero.
 */
template <typename T = double>
bool all(mz::Slice<T> bf) noexcept {
    for (size_type i = 0; i < bf.size(); i++) {
        if (!bf[i]) return false;
    }
    return true;
}

/**
 * @brief Returns the sum of all elements in the Slice (for integral types).
 */
template <std::integral T>
long long sum(mz::Slice<T> bf) noexcept {
    long long s = 0;
    for (size_type i = 0; i < bf.size(); i++) s += bf[i];
    return s;
}

/**
 * @brief Returns the count of nonzero elements in the Slice.
 */
template <typename T>
    requires requires (T x, size_type y) { y = x ? 1 : 0; }
size_type count(mz::Slice<T> bf) {
    size_type cnt = 0;
    for (size_type i = 0; i < bf.size(); i++) bf[i] ? ++cnt : 0;
    return cnt;
}

/**
 * @brief Equality comparison for two slices of possibly different types.
 */
template<typename T, typename U>
    requires requires (T x, U y) { x != y; }
bool operator == (mz::Slice<const T> L, mz::Slice<const U> R) noexcept
{
    if (L.size() != R.size()) return false;
    for (size_type i = 0; i < L.size(); i++) {
        if (L[i] != R[i]) return false;
    }
    return true;
}

/**
 * @brief Custom formatter for mz::Slice<T> for std::format integration.
 */
template<typename T>
struct std::formatter<mz::Slice<T>> : public mz::basic_formatter16 {
    auto format(mz::Slice<T> const& p, auto& ctx) const {
        return std::format_to(ctx.out(), "{}", p.string(std::string_view{ FMT, size_t{count} }));
    }
};

#endif