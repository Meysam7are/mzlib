#ifndef MZ_CONCEPT_UTILS_HEADER_FILE
#define MZ_CONCEPT_UTILS_HEADER_FILE
#pragma once
#include <concepts>
#include <type_traits>
#include <string_view>
#include <string>

// A class consisting of 4 double variables, representing a quadruple-precision floating point number.
class qdbl_t;

namespace mz {

	// Concept: Checks if type T has a string(Fmt) method returning std::string
	// unclear: What is ctr.string(Fmt) used for?
	template <typename T>
	concept has_format_string = requires(std::remove_cvref_t<T> const& ctr, std::string_view Fmt) { { ctr.string(Fmt) } -> std::same_as<std::string>; };

	// Concept: Checks if type T is a standard arithmetic type
	template <typename T>
	concept std_arithmetic = std::is_arithmetic_v<T>;

	// Concept: Checks if type T can be converted to/from double
	// unclear: Why is static_cast used for both directions?
	template <typename T>
	concept arithmetic = requires (std::remove_cvref_t<T> a, double d) {
		a = static_cast<std::remove_cvref_t<T>>(d);
		d = static_cast<double>(a);
	};

	// Trait: is_arithmetic, true if T satisfies arithmetic concept
	template <typename T> struct is_arithmetic { static constexpr bool value = false; };
	template <arithmetic T> struct is_arithmetic<T> { static constexpr bool value = true; };
	// Inline variable for is_arithmetic
	template <typename T> inline constexpr bool is_arithmetic_v{ is_arithmetic<T>::value };

	// Concept: Checks if type T is double or qdbl_t
	// unclear: Why is qdbl_t considered floating-point?
	template <typename T>
	concept floating = std::is_same_v<std::remove_cvref_t<T>, double> || std::is_same_v<std::remove_cvref_t<T>, qdbl_t>;
//	concept floating = arithmetic<T> && !std::is_integral_v<T>;

	// Trait: is_floating, true if T satisfies floating concept
	template <typename T> struct is_floating { static constexpr bool value = false; };
	template <floating T> struct is_floating<T> { static constexpr bool value = true; };
	// Inline variable for is_floating
	template <typename T> inline constexpr bool is_floating_v{ is_floating<T>::value };

	// Concept: Checks if Func_ can be called with int and returns something convertible to double
	template <typename Func_>
	concept linalg_input = requires(Func_ F, int i) { { F(i) }-> std::convertible_to<double>; };

	// Concept: Checks if Func_ is linalg_input and returns qdbl_t
	// unclear: Why is qdbl_t required as return type?
	template <typename Func_>
	concept qdbl_t_input = linalg_input<Func_> && std::is_same_v<std::invoke_result_t<Func_, int>, qdbl_t>;

	// Trait: return_qdbl, true if T is qdbl_t_input
	template <typename T> struct return_qdbl { static constexpr bool value = false; };
	template <qdbl_t_input T> struct return_qdbl<T> { static constexpr bool value = true; };
	// Inline variable for return_qdbl
	template <typename T> inline constexpr bool is_return_qdbl_v{ return_qdbl<T>::value };

	// Trait: Checks if T is trivially copyable and has standard layout
	// Used to determine if type is contiguous in memory
	template <typename T>
	inline constexpr bool is_contiguous_v{ std::is_trivially_copyable_v<std::remove_cvref_t<T>> && std::is_standard_layout_v<std::remove_cvref_t<T>> };

	// Type trait: Returns const reference type for Ty_
	// unclear: Why is this specialization needed for small trivially copyable types?
	template <typename Ty_>
	struct return_cvref_t { using type = std::remove_cvref_t<Ty_> const&; };

	// Specialization for small trivially copyable non-array types
	template <typename Ty_>
		requires (sizeof(Ty_) <= sizeof(size_t) && std::is_trivially_copyable_v<Ty_> && !std::is_array_v<Ty_>)
	struct return_cvref_t<Ty_> { using type = std::remove_cvref_t<Ty_>;
	};
	
	// Container type aliases for generic containers
	// unclear: Why use std::_Remove_cvref_t (non-standard)?
	template<typename Ty_>
	struct container_types {
		using value_type = std::_Remove_cvref_t<Ty_>;
		using const_type = value_type const;
		using const_return = return_cvref_t<Ty_>::type;
		using const_reference = const_type&;
		using reference = value_type&;
	};

	// Concept: Checks if Func_ can be invoked with size_t and returns T
	template <typename Func_, typename T>
	concept compatible_caller = std::is_invocable_r_v<T, Func_, size_t>;

	// Concept: Checks if Itr_ can be indexed with i and assigned to T
	template <typename Itr_, typename T>
	concept compatible_indexer = requires (Itr_ && it, T t, size_t i) { t = static_cast<T>(it[i]); };

	// Concept: Checks if Itr_ supports indexing and has size()
	template <typename Itr_, typename T>
	concept compatible_iterator = compatible_indexer<Itr_, T>
		&& requires (Itr_ const& it, size_t i) { i = it.size(); };

	// Concept: Checks if Seq_ supports container-like access and types
	template <typename Seq_>
	concept sequence = requires(Seq_ const cg, Seq_ g, typename Seq_::value_type v, int i, size_t s)
	{
		{ g.operator[](i) } -> std::same_as<typename Seq_::reference>;
		{ cg.operator[](i) } -> std::same_as<typename Seq_::const_reference>;
		v = cg.operator[](i);
		s = static_cast<size_t>(cg.size());
	};

	// Concept: sequence with arithmetic value_type
	template <typename Seq_>
	concept arithmetic_sequence = sequence<Seq_> && arithmetic<typename Seq_::value_type>;

	// Concept: sequence with integral value_type
	template <typename Seq_>
	concept integral_sequence = sequence<Seq_> && std::is_integral_v<typename Seq_::value_type>;

	// Concept: integral_sequence with value_type of size 1 (likely bool)
	template <typename Seq_>
	concept boolean_sequence = integral_sequence<Seq_> && sizeof(typename Seq_::value_type) == 1;

	// Concept: Checks if T can be called with long long and returns double&
	template <typename T>
	concept itr_double_ref = requires(T F, long long i) { { F(i) } -> std::same_as<double&>; };

	// Concept: Checks if T can be called with long long and returns double, double&, or double const&
	template <typename T>
	concept itr_double =
		itr_double_ref<T> ||
		requires(T F, long long i) { { F(i) } -> std::same_as<double>; } ||
		requires(T F, long long i) { { F(i) } -> std::same_as<double const&>; };

	// Concept: Checks if T can be called with long long and returns qdbl_t&
	template <typename T>
	concept itr_qdbl_ref = requires(T && F, long long i) { { F(i) } -> std::same_as<qdbl_t&>; };

	// Concept: Checks if T can be called with long long and returns qdbl_t, qdbl_t&, or qdbl_t const&
	template <typename T>
	concept itr_qdbl =
		itr_qdbl_ref<T> ||
		requires(T F, long long i) { { F(i) } -> std::same_as<qdbl_t>; } ||
		requires(T F, long long i) { { F(i) } -> std::same_as<qdbl_t const&>; };

	// Concept: Checks if T is either itr_double or itr_qdbl
	template <typename T>
	concept itr_func = itr_double<T> || itr_qdbl<T>;

	// Concept: Checks if T is either itr_double_ref or itr_qdbl_ref
	template <typename T>
	concept itr_func_ref = itr_double_ref<T> || itr_qdbl_ref<T>;

	// Operator!= for sequences: compares size and elements
	// unclear: Should this be a free function or member?
	template <sequence Left, sequence Right> requires requires(Left::value_type x, typename Right::value_type y) { x != y; }
	bool operator != (Left const& L, Right const& R) {
		if (L.size() != R.size()) return true;
		size_t Size = L.size();
		for (size_t i = 0; i < Size; i++) { if (L[i] != R[i]) return true; }
		return false;
	}

	// Utility: Casts pointer Q* to T* if alignment and size match
	// unclear: Is this safe for all types?
	template <typename T, typename Q>
		requires (alignof(T) == alignof(Q) && sizeof(T) == sizeof(Q) && !std::is_const_v<Q>)
	constexpr T* static_cast_as(Q* ptr) noexcept {
		return static_cast<T*>(static_cast<void*>(ptr));
	}

	// Utility: Casts pointer Q* to const T* if alignment and size match
	// unclear: Is this safe for all types?
	template <typename T, typename Q>
		requires (alignof(T) == alignof(Q) && sizeof(T) == sizeof(Q) && std::is_const_v<Q>)
	constexpr T const* static_cast_as(Q* ptr) noexcept {
		return static_cast<T const*>(static_cast<void const*>(ptr));
	}

	// Utility: Casts reference Q& to T& if alignment and size match
	// unclear: Is this safe for all types?
	template <typename T, typename Q>
		requires (alignof(T) == alignof(Q) && sizeof(T) == sizeof(Q) && !std::is_const_v<Q> && !std::is_reference_v<Q>)
	constexpr std::remove_cvref_t<T>& static_cast_as(Q& Ref) noexcept {
		return *static_cast<std::remove_cvref_t<T>*>(static_cast<void*>(&Ref));
	}

	// Utility: Casts const reference Q const& to const T& if alignment and size match
	// unclear: Is this safe for all types?
	template <typename T, typename Q>
		requires (alignof(T) == alignof(Q) && sizeof(T) == sizeof(Q) && !std::is_const_v<Q> && !std::is_reference_v<Q>)
	constexpr std::remove_cvref_t<T> const& static_cast_as(Q const& CRef) noexcept {
		return *static_cast<std::remove_cvref_t<T> const*>(static_cast<void const*>(&CRef));
	}

}

#endif
