#ifndef MZLIB_ELEMENTWISE_VIEW_H
#define MZLIB_ELEMENTWISE_VIEW_H
#pragma once

#include "globals.h"
#include <algorithm>
#include <type_traits>
#include <functional>

namespace mz {


	/**
	 * @class ElementwiseConstOperationsInterface
	 * @brief CRTP base class providing element-wise operations for constant containers.
	 *
	 * This class enables derived containers to perform element-wise formatting and string conversion.
	 * The derived class must implement `operator[]` and `size()` methods.
	 *
	 * @tparam DerivedT The derived container type (CRTP pattern).
	 * @tparam ElementT The type of elements stored in the container.
	 */
    template <typename DerivedT, typename ElementT>
    class ElementwiseConstOperationsInterface {


    public:
        // Type aliases for element access and manipulation
        using element_type = ElementT;
        using value_type = std::remove_cvref_t<ElementT>;
        using const_type = value_type const;
        using const_reference = const_type&;



    private:

        /**
         * @brief Access element at specified index (const).
         * Used internally for formatting.
         */
        inline constexpr const_reference c_at(size_type index) const noexcept {
            return static_cast<DerivedT const*>(this)->operator[](index);
        }

        /**
         * @brief Get const reference to the derived container.
         * Used internally for formatting.
         */
        inline constexpr DerivedT const& c_ref() const noexcept {
            return *static_cast<DerivedT const*>(this);
        }

        /**
         * @brief Get the number of elements in the container.
         * Used internally for formatting.
         */
        inline constexpr size_type size() const noexcept {
            return static_cast<DerivedT const*>(this)->size();
        }


    public:


        /**
         * @brief Format the container to a buffer using generic formatting.
         * If the element type does not have a custom format string, this is used.
         */
        void format_to(auto& bf, std::string_view Fmt = "{}", std::string_view Sep = ",", std::string_view Enc = "[]") const noexcept requires (!HasFormatString<value_type>)
        {
            format_sequence_to(bf, c_ref(), Fmt, Sep, Enc);
        }

        /**
         * @brief Format the container to a buffer using custom element formatting.
         * If the element type has a custom format string, this is used.
         */
        void format_to(auto& bf, std::string_view Fmt = "{}", std::string_view Sep = ",", std::string_view Enc = "[]") const noexcept requires HasFormatString<value_type>
        {
            format_string_generator_to(bf, [&](auto i) constexpr noexcept -> std::string { return c_at(i).string(Fmt); }, size(), Sep, Enc);
        }

        /**
         * @brief Returns a string representation of the container.
         * Uses the appropriate format_to overload.
         */
        std::string string(std::string_view Fmt = "{}", std::string_view Sep = ",", std::string_view Enc = "[]") const noexcept
        {
            std::string bf;
            format_to(bf, Fmt, Sep, Enc);
            return bf;
        }


    };


    /**
     * @class ElementwiseMutableOperationsInterface
     * @brief CRTP base class providing element-wise operations for containers.
     *
     * This class enables derived containers to perform element-wise arithmetic and logical operations.
     * The derived class must implement `operator[]` and `size()` methods.
     *
     * @tparam DerivedT The derived container type (CRTP pattern).
     * @tparam ElementT The type of elements stored in the container.
     */
    template <typename DerivedT, typename ElementT>
    class ElementwiseMutableOperationsInterface : public ElementwiseConstOperationsInterface<DerivedT, ElementT> {
    public:
        // Type aliases for element access and manipulation
        using element_type = ElementT;
        using value_type = std::remove_cvref_t<ElementT>;
        using const_type = const value_type;
        using lvalue_type = value_type&;
        using rvalue_type = value_type&&;
        using pointer = value_type*;
        using reference = value_type&;
        using const_pointer = const value_type*;
        using const_reference = const_type&;
        using container_type = ElementwiseMutableOperationsInterface<DerivedT, value_type>;
        //using size_type = std::size_t;

	//private:

        /**
         * @brief Access element at specified index (mutable).
         * @param index Index of the element.
         * @return Reference to the element.
         */
        inline constexpr reference at(size_type index) noexcept {
            return static_cast<DerivedT*>(this)->operator[](index);
        }

        /**
         * @brief Access element at specified index (const).
         * @param index Index of the element.
         * @return Const reference to the element.
         */
        inline constexpr const_reference at(size_type index) const noexcept {
            return static_cast<const DerivedT*>(this)->operator[](index);
        }

        /**
         * @brief Get mutable reference to the derived container.
         */
        inline constexpr DerivedT& ref() noexcept {
            return *static_cast<DerivedT*>(this);
        }

        /**
         * @brief Get const reference to the derived container.
         */
        inline constexpr const DerivedT& cref() const noexcept {
            return *static_cast<const DerivedT*>(this);
        }

        /**
         * @brief Get the number of elements in the container.
         */
        inline constexpr size_type size() const noexcept {
            return static_cast<const DerivedT*>(this)->size();
        }


    //public

        // ------------------------------------------------------------------------
        // Element-wise arithmetic and logical operations with scalars
        // ------------------------------------------------------------------------

        /**
         * @brief Element-wise bitwise OR with scalar.
         */
        template <typename U>
            requires requires (value_type t, U u) { t |= u; }
        DerivedT& operator|=(U u) noexcept {
            for (size_type i = 0; i < size(); ++i) at(i) |= u;
            return ref();
        }

        /**
         * @brief Element-wise bitwise AND with scalar.
         */
        template <typename U>
            requires requires (value_type t, U u) { t &= u; }
        DerivedT& operator&=(U u) noexcept {
            for (size_type i = 0; i < size(); ++i) at(i) &= u;
            return ref();
        }

        /**
         * @brief Element-wise bitwise XOR with scalar.
         */
        template <typename U>
            requires requires (value_type t, U u) { t ^= u; }
        DerivedT& operator^=(U u) noexcept {
            for (size_type i = 0; i < size(); ++i) at(i) ^= u;
            return ref();
        }

        /**
         * @brief Element-wise addition with scalar.
         */
        template <typename U>
            requires requires (value_type t, U u) { t += u; }
        DerivedT& operator+=(U u) noexcept {
            for (size_type i = 0; i < size(); ++i) at(i) += u;
            return ref();
        }

        /**
         * @brief Element-wise subtraction with scalar.
         */
        template <typename U>
            requires requires (value_type t, U u) { t -= u; }
        DerivedT& operator-=(U u) noexcept {
            for (size_type i = 0; i < size(); ++i) at(i) -= u;
            return ref();
        }

        /**
         * @brief Element-wise multiplication with scalar.
         */
        template <typename U>
            requires requires (value_type t, U u) { t *= u; }
        DerivedT& operator*=(U u) noexcept {
            for (size_type i = 0; i < size(); ++i) at(i) *= u;
            return ref();
        }

        /**
         * @brief Element-wise division with scalar.
         */
        template <typename U>
            requires requires (value_type t, U u) { t /= u; }
        DerivedT& operator/=(U u) noexcept {
            for (size_type i = 0; i < size(); ++i) at(i) /= u;
            return ref();
        }

        // ------------------------------------------------------------------------
        // Element-wise operations with another sequence
        // ------------------------------------------------------------------------

        /**
         * @brief Element-wise bitwise AND with another sequence.
         * @throws domain_error if sizes do not match.
         */
        template <Sequence Seq>
            requires requires(value_type x, typename Seq::value_type y) { x &= y; }
        DerivedT& operator&=(Seq seq) {
            DOMAIN_ERROR_IF(size() != seq.size(), "elementwise AND size mismatch: {} != {}\n", size(), seq.size());
            for (size_type i = 0; i < size(); ++i) at(i) &= seq[i];
            return ref();
        }

        /**
         * @brief Element-wise bitwise OR with another sequence.
         * @throws domain_error if sizes do not match.
         */
        template <Sequence Seq>
            requires requires(value_type x, typename Seq::value_type y) { x |= y; }
        DerivedT& operator|=(Seq seq) {
            DOMAIN_ERROR_IF(size() != seq.size(), "elementwise OR size mismatch: {} != {}\n", size(), seq.size());
            for (size_type i = 0; i < size(); ++i) at(i) |= seq[i];
            return ref();
        }

        /**
         * @brief Element-wise bitwise XOR with another sequence.
         * @throws domain_error if sizes do not match.
         */
        template <Sequence Seq>
            requires requires(value_type x, typename Seq::value_type y) { x ^= y; }
        DerivedT& operator^=(Seq seq) {
            DOMAIN_ERROR_IF(size() != seq.size(), "elementwise XOR size mismatch: {} != {}\n", size(), seq.size());
            for (size_type i = 0; i < size(); ++i) at(i) ^= seq[i];
            return ref();
        }

        /**
         * @brief Element-wise addition with another sequence.
         * @throws domain_error if sizes do not match.
         */
        template <Sequence Seq>
            requires requires(value_type x, typename Seq::value_type y) { x += y; }
        DerivedT& operator+=(Seq seq) {
            DOMAIN_ERROR_IF(size() != seq.size(), "elementwise addition size mismatch: {} != {}\n", size(), seq.size());
            for (size_type i = 0; i < size(); ++i) at(i) += seq[i];
            return ref();
        }

        /**
         * @brief Element-wise subtraction with another sequence.
         * @throws domain_error if sizes do not match.
         */
        template <Sequence Seq>
            requires requires(value_type x, typename Seq::value_type y) { x -= y; }
        DerivedT& operator-=(Seq seq) {
            DOMAIN_ERROR_IF(size() != seq.size(), "elementwise subtraction size mismatch: {} != {}\n", size(), seq.size());
            for (size_type i = 0; i < size(); ++i) at(i) -= seq[i];
            return ref();
        }

        /**
         * @brief Element-wise multiplication with another sequence.
         * @throws domain_error if sizes do not match.
         */
        template <Sequence Seq>
            requires requires(value_type x, typename Seq::value_type y) { x *= y; }
        DerivedT& operator*=(Seq seq) {
            DOMAIN_ERROR_IF(size() != seq.size(), "elementwise multiplication size mismatch: {} != {}\n", size(), seq.size());
            for (size_type i = 0; i < size(); ++i) at(i) *= seq[i];
            return ref();
        }

        /**
         * @brief Element-wise division with another sequence.
         * @throws domain_error if sizes do not match.
         */
        template <Sequence Seq>
            requires requires(value_type x, typename Seq::value_type y) { x /= y; }
        DerivedT& operator/=(Seq seq) {
            DOMAIN_ERROR_IF(size() != seq.size(), "elementwise division size mismatch: {} != {}\n", size(), seq.size());
            for (size_type i = 0; i < size(); ++i) at(i) /= seq[i];
            return ref();
        }

        // ------------------------------------------------------------------------
        // Element-wise operations with a callable (functor/lambda)
        // ------------------------------------------------------------------------

        /**
         * @brief Element-wise bitwise AND with a callable.
         * The callable must accept a size_t index and return a value convertible to value_type.
         */
        template <typename Func>
            requires std::is_invocable_r_v<value_type, Func, size_type>&&
            requires (value_type t, std::invoke_result_t<Func, size_type> u) { t &= u; }
        DerivedT& operator&=(Func&& func) {
            for (size_type i = 0; i < size(); ++i) at(i) &= func(i);
            return ref();
        }

        /**
         * @brief Element-wise bitwise OR with a callable.
         */
        template <typename Func>
            requires std::is_invocable_r_v<value_type, Func, size_type>&&
            requires (value_type t, std::invoke_result_t<Func, size_type> u) { t |= u; }
        DerivedT& operator|=(Func&& func) {
            for (size_type i = 0; i < size(); ++i) at(i) |= func(i);
            return ref();
        }

        /**
         * @brief Element-wise bitwise XOR with a callable.
         */
        template <typename Func>
            requires std::is_invocable_r_v<value_type, Func, size_type>&&
            requires (value_type t, std::invoke_result_t<Func, size_type> u) { t ^= u; }
        DerivedT& operator^=(Func&& func) {
            for (size_type i = 0; i < size(); ++i) at(i) ^= func(i);
            return ref();
        }

        /**
         * @brief Element-wise addition with a callable.
         */
        template <typename Func>
            requires std::is_invocable_r_v<value_type, Func, size_type>&&
            requires (value_type t, std::invoke_result_t<Func, size_type> u) { t += u; }
        DerivedT& operator+=(Func&& func) {
            for (size_type i = 0; i < size(); ++i) at(i) += func(i);
            return ref();
        }

        /**
         * @brief Element-wise subtraction with a callable.
         */
        template <typename Func>
            requires std::is_invocable_r_v<value_type, Func, size_type>&&
            requires (value_type t, std::invoke_result_t<Func, size_type> u) { t -= u; }
        DerivedT& operator-=(Func&& func) {
            for (size_type i = 0; i < size(); ++i) at(i) -= func(i);
            return ref();
        }

        /**
         * @brief Element-wise multiplication with a callable.
         */
        template <typename Func>
            requires std::is_invocable_r_v<value_type, Func, size_type>&&
            requires (value_type t, std::invoke_result_t<Func, size_type> u) { t *= u; }
        DerivedT& operator*=(Func&& func) {
            for (size_type i = 0; i < size(); ++i) at(i) *= func(i);
            return ref();
        }

        /**
         * @brief Element-wise division with a callable.
         */
        template <typename Func>
            requires std::is_invocable_r_v<value_type, Func, size_type>&&
            requires (value_type t, std::invoke_result_t<Func, size_type> u) { t /= u; }
        DerivedT& operator/=(Func&& func) {
            for (size_type i = 0; i < size(); ++i) at(i) /= func(i);
            return ref();
        }

        // ------------------------------------------------------------------------
        // Element-wise operations with initializer_list
        // ------------------------------------------------------------------------

        /**
         * @brief Apply a binary function to each element and corresponding value from initializer_list.
         * @param values Initializer list of values.
         * @param func Binary function to apply (element, value).
         * @throws domain_error if sizes do not match.
         */
        template <typename U, typename Func>
        void apply(std::initializer_list<U> const& values, Func&& func) {
            DOMAIN_ERROR_IF(size() != values.size(), "ElementwiseMutableOperationsInterface::apply size mismatch");
            U const* src = values.begin();
            for (size_type i = 0; i < size(); ++i) func(at(i), src[i]);
        }

        /**
         * @brief Element-wise bitwise AND with initializer_list.
         */
        template <typename U>
            requires requires(value_type x, U y) { x &= y; }
        DerivedT& operator&=(std::initializer_list<U> const& values) {
            DOMAIN_ERROR_IF(size() != values.size(), "elementwise AND size mismatch: {} != {}\n", size(), values.size());
            U const* src = values.begin();
            for (size_type i = 0; i < size(); ++i) at(i) &= src[i];
            return ref();
        }

        /**
         * @brief Element-wise bitwise OR with initializer_list.
         */
        template <typename U>
            requires requires(value_type x, U y) { x |= y; }
        DerivedT& operator|=(std::initializer_list<U> const& values) {
            DOMAIN_ERROR_IF(size() != values.size(), "elementwise OR size mismatch: {} != {}\n", size(), values.size());
            U const* src = values.begin();
            for (size_type i = 0; i < size(); ++i) at(i) |= src[i];
            return ref();
        }

        /**
         * @brief Element-wise bitwise XOR with initializer_list.
         */
        template <typename U>
            requires requires(value_type x, U y) { x ^= y; }
        DerivedT& operator^=(std::initializer_list<U> const& values) {
            DOMAIN_ERROR_IF(size() != values.size(), "elementwise XOR size mismatch: {} != {}\n", size(), values.size());
            U const* src = values.begin();
            for (size_type i = 0; i < size(); ++i) at(i) ^= src[i];
            return ref();
        }

        /**
         * @brief Element-wise addition with initializer_list.
         */
        template <typename U>
            requires requires(value_type x, U y) { x += y; }
        DerivedT& operator+=(std::initializer_list<U> const& values) {
            DOMAIN_ERROR_IF(size() != values.size(), "elementwise addition size mismatch: {} != {}\n", size(), values.size());
            U const* src = values.begin();
            for (size_type i = 0; i < size(); ++i) at(i) += src[i];
            return ref();
        }

        /**
         * @brief Element-wise subtraction with initializer_list.
         */
        template <typename U>
            requires requires(value_type x, U y) { x -= y; }
        DerivedT& operator-=(std::initializer_list<U> const& values) {
            DOMAIN_ERROR_IF(size() != values.size(), "elementwise subtraction size mismatch: {} != {}\n", size(), values.size());
            U const* src = values.begin();
            for (size_type i = 0; i < size(); ++i) at(i) -= src[i];
            return ref();
        }

        /**
         * @brief Element-wise multiplication with initializer_list.
         */
        template <typename U>
            requires requires(value_type x, U y) { x *= y; }
        DerivedT& operator*=(std::initializer_list<U> const& values) {
            DOMAIN_ERROR_IF(size() != values.size(), "elementwise multiplication size mismatch: {} != {}\n", size(), values.size());
            U const* src = values.begin();
            for (size_type i = 0; i < size(); ++i) at(i) *= src[i];
            return ref();
        }

        /**
         * @brief Element-wise division with initializer_list.
         */
        template <typename U>
            requires requires(value_type x, U y) { x /= y; }
        DerivedT& operator/=(std::initializer_list<U> const& values) {
            DOMAIN_ERROR_IF(size() != values.size(), "elementwise division size mismatch: {} != {}\n", size(), values.size());
            U const* src = values.begin();
            for (size_type i = 0; i < size(); ++i) at(i) /= src[i];
            return ref();
        }
    };

} // namespace mz

#endif // MZLIB_ELEMENTWISE_VIEW_H




#ifdef _FALSE_EMPTY

#ifndef MZ_ELEMENTWISE_OPERAIONS_HEADER_FILE
#define MZ_ELEMENTWISE_OPERAIONS_HEADER_FILE
#pragma once


#include "globals.h"
#include <algorithm>
#include <type_traits>
#include <functional>

namespace mz {

	/**
	 * @class BaseView
	 * @brief A CRTP (Curiously Recurring Template Pattern) base class for element-wise operations on containers.
	 *
	 * This class provides a common interface for containers to perform element-wise operations.
	 * Derived classes must implement the `operator[]` and `size()` methods.
	 *
	 * @tparam Derived The derived class (CRTP pattern).
	 * @tparam ElementType The type of elements in the container.
	 */
	template <typename Itr, typename Ty_>
	class BaseView {

	public:
		using element_type = Ty_;
		using value_type = std::remove_cvref_t<Ty_>;
		using const_type = value_type const;
		using lvalue_type = value_type&;
		using rvalue_type = value_type&&;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = value_type const*;
		using const_reference = const_type&;
		using container_t = typename BaseView<Itr, value_type>;




		inline constexpr reference at_(std::integral auto Index) noexcept { return static_cast<Itr*>(this)->operator[](Index); }
		inline constexpr const_reference at_(std::integral auto Index) const noexcept { static_cast<Itr const*>(this)->operator[](Index); }

		inline constexpr Itr& ref() noexcept { return *static_cast<Itr*>(this); }
		inline constexpr Itr const& cref() const noexcept { return *static_cast<Itr const*>(this); }

	public:



		inline constexpr size_type size() const noexcept { return static_cast<Itr const*>(this)->size(); }






		/*
		*		Elementwise operations
		*/



		// Logical Operations: elementwise
		template <typename U> requires requires (value_type t, U u) { t |= u; } Itr& operator |= (U u) noexcept { for (size_type i = 0; i < size(); i++) at_(i) |= u; return ref(); }
		template <typename U> requires requires (value_type t, U u) { t &= u; } Itr& operator &= (U u) noexcept { for (size_type i = 0; i < size(); i++) at_(i) &= u; return ref(); }
		template <typename U> requires requires (value_type t, U u) { t ^= u; } Itr& operator ^= (U u) noexcept { for (size_type i = 0; i < size(); i++) at_(i) ^= u; return ref(); }
		template <typename U> requires requires (value_type t, U u) { t += u; } Itr& operator += (U u) noexcept { for (size_type i = 0; i < size(); i++) at_(i) += u; return ref(); }
		template <typename U> requires requires (value_type t, U u) { t -= u; } Itr& operator -= (U u) noexcept { for (size_type i = 0; i < size(); i++) at_(i) -= u; return ref(); }
		template <typename U> requires requires (value_type t, U u) { t *= u; } Itr& operator *= (U u) noexcept { for (size_type i = 0; i < size(); i++) at_(i) *= u; return ref(); }
		template <typename U> requires requires (value_type t, U u) { t /= u; } Itr& operator /= (U u) noexcept { for (size_type i = 0; i < size(); i++) at_(i) /= u; return ref(); }


		template <Sequence Seq> requires requires(value_type x, typename Seq::value_type y) { x &= y; }
		Itr& operator &= (Seq it) {
			DOMAIN_ERROR_IF(size() != it.size(), "elementwise AND size mismatch: {} != {}\n", size(), it.size());
			for (size_type i = 0; i < size(); i++) { at_(i) &= it[i]; }
			return ref();
		}

		template <Sequence Seq> requires requires(value_type x, typename Seq::value_type y) { x |= y; }
		Itr& operator |= (Seq it) {
			DOMAIN_ERROR_IF(size() != it.size(), "elementwise OR size mismatch: {} != {}\n", size(), it.size());
			for (size_type i = 0; i < size(); i++) { at_(i) |= it[i]; }
			return ref();
		}

		template <Sequence Seq> requires requires(value_type x, typename Seq::value_type y) { x ^= y; }
		Itr& operator ^= (Seq it) {
			DOMAIN_ERROR_IF(size() != it.size(), "elementwise XOR size mismatch: {} != {}\n", size(), it.size());
			for (size_type i = 0; i < size(); i++) { at_(i) ^= it[i]; }
			return ref();
		}

		template <Sequence Seq> requires requires(value_type x, typename Seq::value_type y) { x += y; }
		Itr& operator += (Seq it) {
			DOMAIN_ERROR_IF(size() != it.size(), "elementwise addition size mismatch: {} != {}\n", size(), it.size());
			for (size_type i = 0; i < size(); i++) { at_(i) += it[i]; }
			return ref();
		}

		template <Sequence Seq> requires requires(value_type x, typename Seq::value_type y) { x -= y; }
		Itr& operator -= (Seq it) {
			DOMAIN_ERROR_IF(size() != it.size(), "elementwise subtraction size mismatch: {} != {}\n", size(), it.size());
			for (size_type i = 0; i < size(); i++) { at_(i) -= it[i]; }
			return ref();
		}

		template <Sequence Seq> requires requires(value_type x, typename Seq::value_type y) { x *= y; }
		Itr& operator *= (Seq it) {
			DOMAIN_ERROR_IF(size() != it.size(), "elementwise multiplication size mismatch: {} != {}\n", size(), it.size());
			for (size_type i = 0; i < size(); i++) { at_(i) *= it[i]; }
			return ref();
		}

		template <Sequence Seq> requires requires(value_type x, typename Seq::value_type y) { x /= y; }
		Itr& operator /= (Seq it) {
			DOMAIN_ERROR_IF(size() != it.size(), "elementwise division size mismatch: {} != {}\n", size(), it.size());
			for (size_type i = 0; i < size(); i++) { at_(i) /= it[i]; }
			return ref();
		}




		template <typename Func_>
			requires std::is_invocable_r_v<value_type, Func_, size_t>&&
			requires (value_type t, std::invoke_result_t<Func_, size_t> u) { t &= u; }
		Itr& operator &= (Func_&& it) {
			for (size_type i = 0; i < size(); i++) { at_(i) &= it(i); }
			return *static_cast<Itr*>(this);
		};

		template <typename Func_>
			requires std::is_invocable_r_v<value_type, Func_, size_t>&&
			requires (value_type t, std::invoke_result_t<Func_, size_t> u) { t |= u; }
		Itr& operator |= (Func_&& it) {
			for (size_type i = 0; i < size(); i++) { at_(i) |= it(i); }
			return ref();
		}

		template <typename Func_>
			requires std::is_invocable_r_v<value_type, Func_, size_t>&&
			requires (value_type t, std::invoke_result_t<Func_, size_t> u) { t ^= u; }
		Itr& operator ^= (Func_&& it) {
			for (size_type i = 0; i < size(); i++) { at_(i) ^= it(i); }
			return ref();
		}

		template <typename Func_>
			requires std::is_invocable_r_v<value_type, Func_, size_t>&&
			requires (value_type t, std::invoke_result_t<Func_, size_t> u) { t += u; }
		Itr& operator += (Func_&& it) {
			for (size_type i = 0; i < size(); i++) { at_(i) += it(i); }
			return ref();
		}

		template <typename Func_>
			requires std::is_invocable_r_v<value_type, Func_, size_t>&&
			requires (value_type t, std::invoke_result_t<Func_, size_t> u) { t -= u; }
		Itr& operator -= (Func_&& it) {
			for (size_type i = 0; i < size(); i++) { at_(i) -= it(i); }
			return ref();
		}

		template <typename Func_>
			requires std::is_invocable_r_v<value_type, Func_, size_t>&&
		//requires compatible_caller<Func_, value_type>&&
			requires (value_type t, std::invoke_result_t<Func_, size_t> u) { t *= u; }
		Itr& operator *= (Func_&& it) {
			for (size_type i = 0; i < size(); i++) { at_(i) *= it(i); }
			return ref();
		}

		template <typename Func_>
			requires std::is_invocable_r_v<value_type, Func_, size_t>&&
		//requires compatible_caller<Func_, value_type>&&
			requires (value_type t, std::invoke_result_t<Func_, size_t> u) { t /= u; }
		Itr& operator /= (Func_&& it) {
			for (size_type i = 0; i < size(); i++) { at_(i) /= it(i); }
			return ref();
		}



















		template <typename U>
		void apply(std::initializer_list<U> const& Li, auto&& Func)
		{
			DOMAIN_ERROR_IF(size() != Li.size(), "mz::Span::apply(initializer_list,Fun) size mismatch");
			U const* src{ Li.begin() };
			for (size_type i = 0; i < size(); i++) { Func(at_(i), src[i]); }
		}






		template <typename U> requires requires(value_type x, U y) { x &= y; }
		Itr& operator &= (std::initializer_list<U> const& li) {
			DOMAIN_ERROR_IF(size() != li.size(), "elementwise AND size mismatch: {} != {}\n", size(), li.size());
			U const* source = li.begin();
			for (size_type i = 0; i < size(); i++) { at_(i) &= source[i]; }
			return ref();
		}

		template <typename U> requires requires(value_type x, U y) { x |= y; }
		Itr& operator |= (std::initializer_list<U> const& li) {
			DOMAIN_ERROR_IF(size() != li.size(), "elementwise OR size mismatch: {} != {}\n", size(), li.size());
			U const* source = li.begin();
			for (size_type i = 0; i < size(); i++) { at_(i) |= source[i]; }
			return ref();
		}

		template <typename U> requires requires(value_type x, U y) { x ^= y; }
		Itr& operator ^= (std::initializer_list<U> const& li) {
			DOMAIN_ERROR_IF(size() != li.size(), "elementwise XOR size mismatch: {} != {}\n", size(), li.size());
			U const* source = li.begin();
			for (size_type i = 0; i < size(); i++) { at_(i) ^= source[i]; }
			return ref();
		}

		template <typename U> requires requires(value_type x, U y) { x += y; }
		Itr& operator += (std::initializer_list<U> const& li) {
			DOMAIN_ERROR_IF(size() != li.size(), "elementwise addition size mismatch: {} != {}\n", size(), li.size());
			U const* source = li.begin();
			for (size_type i = 0; i < size(); i++) { at_(i) += source[i]; }
			return ref();
		}

		template <typename U> requires requires(value_type x, U y) { x -= y; }
		Itr& operator -= (std::initializer_list<U> const& li) {
			DOMAIN_ERROR_IF(size() != li.size(), "elementwise subtraction size mismatch: {} != {}\n", size(), li.size());
			U const* source = li.begin();
			for (size_type i = 0; i < size(); i++) { at_(i) -= source[i]; }
			return ref();
		}

		template <typename U> requires requires(value_type x, U y) { x *= y; }
		Itr& operator *= (std::initializer_list<U> const& li) {
			DOMAIN_ERROR_IF(size() != li.size(), "elementwise multiplication size mismatch: {} != {}\n", size(), li.size());
			U const* source = li.begin();
			for (size_type i = 0; i < size(); i++) { at_(i) *= source[i]; }
			return ref();
		}

		template <typename U> requires requires(value_type x, U y) { x /= y; }
		Itr& operator /= (std::initializer_list<U> const& li) {
			DOMAIN_ERROR_IF(size() != li.size(), "elementwise division size mismatch: {} != {}\n", size(), li.size());
			U const* source = li.begin();
			for (size_type i = 0; i < size(); i++) { at_(i) /= source[i]; }
			return ref();
		}

		










	};

}








#endif // MZ_SPAN_HEADER_FILE


#endif