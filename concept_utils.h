#ifndef MZ_CONCEPT_UTILS_HEADER_FILE
#define MZ_CONCEPT_UTILS_HEADER_FILE
#pragma once
#include <concepts>
#include <type_traits>
#include <string_view>
#include <string>

// Forward declaration for quadruple-precision floating-point type.
class qdbl_t; // Represents a quadruple-precision floating-point number.

namespace mz {

	/**
	 * @brief Concept to check if a type has a string method that takes a format string and returns std::string.
	 *
	 * This concept is satisfied if the type T provides a member function:
	 *   std::string T::string(std::string_view fmt) const;
	 *
	 * Usage:
	 *   template <HasFormatString T>
	 *   void print_formatted(const T& obj, std::string_view fmt);
	 */
	template <typename T>
	concept HasFormatString = requires(std::remove_cvref_t<T> const& obj, std::string_view fmt) {
		{ obj.string(fmt) } -> std::same_as<std::string>;
	};

	/**
	 * @brief Concept to check if a type is a standard arithmetic type.
	 *
	 * Satisfied for built-in types such as int, float, double, etc.
	 * Usage:
	 *   template <StdArithmetic T>
	 *   T add(T a, T b) { return a + b; }
	 */
	template <typename T>
	concept StdArithmetic = std::is_arithmetic_v<T>;

	/**
	 * @brief Concept to check if a type can be converted to and from double.
	 *
	 * Satisfied for types that support static_cast to and from double.
	 * Useful for custom numeric types interoperable with double.
	 * Usage:
	 *   template <Arithmetic T>
	 *   T from_double(double d) { return static_cast<T>(d); }
	 */
	template <typename T>
	concept Arithmetic = requires(std::remove_cvref_t<T> value, double d) {
		value = static_cast<std::remove_cvref_t<T>>(d);
		d = static_cast<double>(value);
	};

	/**
	 * @brief Trait to check if a type satisfies the Arithmetic concept.
	 *
	 * Usage:
	 *   static_assert(IsArithmetic<MyType>::value, "Type must be Arithmetic");
	 */
	template <typename T>
	struct IsArithmetic { static constexpr bool value = false; };

	template <Arithmetic T>
	struct IsArithmetic<T> { static constexpr bool value = true; };

	template <typename T>
	inline constexpr bool is_arithmetic_v = IsArithmetic<T>::value;

	/**
	 * @brief Concept to check if a type is a floating-point type (double or qdbl_t).
	 *
	 * Satisfied for double and qdbl_t types.
	 * Usage:
	 *   template <Floating T>
	 *   void process_float(T value);
	 */
	template <typename T>
	concept Floating = std::is_same_v<std::remove_cvref_t<T>, double> ||
		std::is_same_v<std::remove_cvref_t<T>, qdbl_t>;

	/**
	 * @brief Trait to check if a type satisfies the Floating concept.
	 *
	 * Usage:
	 *   static_assert(IsFloating<double>::value, "Type must be Floating");
	 */
	template <typename T>
	struct IsFloating { static constexpr bool value = false; };

	template <Floating T>
	struct IsFloating<T> { static constexpr bool value = true; };

	template <typename T>
	inline constexpr bool is_floating_v = IsFloating<T>::value;

	/**
	 * @brief Trait to check if a type is contiguous in memory.
	 *
	 * Satisfied for types that are trivially copyable and have standard layout.
	 * Useful for low-level memory operations.
	 * Usage:
	 *   static_assert(is_contiguous_v<MyType>, "Type must be contiguous");
	 */
	template <typename T>
	inline constexpr bool is_contiguous_v =
		std::is_trivially_copyable_v<std::remove_cvref_t<T>> &&
		std::is_standard_layout_v<std::remove_cvref_t<T>>;

	/**
	 * @brief Helper trait to determine the preferred return type for small, trivially copyable types.
	 *
	 * For types smaller than or equal to size_t, returns by value; otherwise, returns by const reference.
	 * Usage:
	 *   typename return_cvref_t<MyType>::type get_value();
	 */
	template <typename Ty_>
	struct return_cvref_t { using type = std::remove_cvref_t<Ty_> const&; };

/*
	template <typename Ty_>
		requires (sizeof(Ty_) <= sizeof(size_t) &&
	std::is_trivially_copyable_v<Ty_> &&
		!std::is_array_v<Ty_>)
		struct return_cvref_t<Ty_> { using type = std::remove_cvref_t<Ty_>; };
*/
	
	/**
	 * @brief Concept to check if a type is a sequence with specific requirements.
	 *
	 * Satisfied for types that provide:
	 *   - operator[] for mutable and const access
	 *   - value_type, reference, const_reference types
	 *   - size() method returning size_t
	 * Usage:
	 *   template <Sequence Seq>
	 *   void process_sequence(const Seq& seq);
	 */
	template <typename Seq>
	concept Sequence = requires(Seq const cg, Seq g, typename Seq::value_type v, int i, size_t s) {
		{ g.operator[](i) } -> std::same_as<typename Seq::reference>;
		{ cg.operator[](i) } -> std::same_as<typename Seq::const_reference>;
		v = cg.operator[](i);
		s = static_cast<size_t>(cg.size());
	};

	/**
	 * @brief Concept to check if a sequence contains Arithmetic types.
	 *
	 * Usage:
	 *   template <ArithmeticSequence Seq>
	 *   void process_arithmetic_sequence(const Seq& seq);
	 */
	template <typename Seq>
	concept ArithmeticSequence = Sequence<Seq> && Arithmetic<typename Seq::value_type>;

	/**
	 * @brief Concept to check if a sequence contains integral types.
	 *
	 * Usage:
	 *   template <IntegralSequence Seq>
	 *   void process_integral_sequence(const Seq& seq);
	 */
	template <typename Seq>
	concept IntegralSequence = Sequence<Seq> && std::is_integral_v<typename Seq::value_type>;

	/**
	 * @brief Concept to check if a sequence contains boolean types.
	 *
	 * Satisfied for sequences of integral types of size 1 (e.g., bool).
	 * Usage:
	 *   template <BooleanSequence Seq>
	 *   void process_boolean_sequence(const Seq& seq);
	 */
	template <typename Seq>
	concept BooleanSequence = IntegralSequence<Seq> && sizeof(typename Seq::value_type) == 1;

















	
	/**
	 * @brief Utility to perform a static cast between types with the same alignment and size.
	 *
	 * Allows reinterpretation of pointers and references between types T and Q,
	 * provided they have the same alignment and size.
	 *
	 * @tparam T Target type
	 * @tparam Q Source type
	 * @param ptr Pointer to Q
	 * @return Pointer to T
	 *
	 * Safety: Use only when you are certain the types are layout-compatible.
	 */
	template <typename T, typename Q>
		requires (alignof(T) == alignof(Q) && sizeof(T) == sizeof(Q) && !std::is_const_v<Q>)
	constexpr T* static_cast_as(Q* ptr) noexcept {
		return static_cast<T*>(static_cast<void*>(ptr));
	}

	/**
	 * @brief Const overload for static_cast_as.
	 */
	template <typename T, typename Q>
		requires (alignof(T) == alignof(Q) && sizeof(T) == sizeof(Q) && std::is_const_v<Q>)
	constexpr T const* static_cast_as(Q* ptr) noexcept {
		return static_cast<T const*>(static_cast<void const*>(ptr));
	}


	/**
	 * @brief Utility to perform a static cast between references of types with the same alignment and size.
	 *
	 * Returns a reference to T from a reference to Q.
	 */
	template <typename T, typename Q>
		requires (alignof(T) == alignof(Q) && sizeof(T) == sizeof(Q) &&
	!std::is_const_v<Q> && !std::is_reference_v<Q>)
		constexpr std::remove_cvref_t<T>& static_cast_as(Q& ref) noexcept {
		return *static_cast<std::remove_cvref_t<T>*>(static_cast<void*>(&ref));
	}

	/**
	 * @brief Const overload for static_cast_as with references.
	 */
	template <typename T, typename Q>
		requires (alignof(T) == alignof(Q) && sizeof(T) == sizeof(Q) &&
	!std::is_const_v<Q> && !std::is_reference_v<Q>)
		constexpr std::remove_cvref_t<T> const& static_cast_as(Q const& cref) noexcept {
		return *static_cast<std::remove_cvref_t<T> const*>(static_cast<void const*>(&cref));
	}

}





#endif