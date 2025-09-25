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

#ifndef MZ_SPAN_HEADER_FILE
#define MZ_SPAN_HEADER_FILE
#pragma once


#include "globals.h"
#include <algorithm>
#include <type_traits>
#include <functional>
#include "ElementwiseOperationsInterface.h"

namespace mz {

	/**
	 * @brief A lightweight, non-owning view over a contiguous sequence of elements.
	 * Provides elementwise operations, queue-like access, and assignment/casting utilities.
	 *
	 * @tparam ElementT The type of elements in the sequence.
	 */
	template <typename ElementT>
	class Span : public ElementwiseMutableOperationsInterface<Span<ElementT>, ElementT> {

	public:
		// Type aliases for element access and manipulation
		using element_type = ElementT;
		using value_type = std::remove_cvref_t<ElementT>;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = value_type const*;
		using const_reference = value_type const&;

		// Indicates if the span is over non-const data
		static inline constexpr bool nonconst{ !std::is_const_v<ElementT> };


		/**
		 * @brief Returns the trimmed length, clamped to [0, size_].
		 */
		constexpr size_type trim_length(size_type Length) const noexcept { return Length < 0 ? 0 : (Length < size_ ? Length : size_); }


	private:
		pointer data_{ nullptr };
		size_type size_{ 0 };


		friend void swap(Span& L, Span& R) { L.swap_elements(R); }

	public:

		/**
		 * @brief Default constructor creates an empty Span.
		 */
		constexpr Span() noexcept = default;

		/**
		 * @brief Construct from pointer and size.
		 */
		constexpr Span(ElementT* Pointer, std::integral auto Size) noexcept :
			data_{ const_cast<pointer>(Pointer) },
			size_{ static_cast<size_type>(Size) } {
		}

		/**
		 * @brief Construct from begin and end pointers.
		 */
		constexpr explicit Span(ElementT* begin, ElementT* end) noexcept :
			data_{ const_cast<pointer>(begin) },
			size_{ static_cast<size_type>(end - begin) } {
		}
		

		/**
		 * @brief Construct from a span_view struct.
		 */
		struct span_view {
			pointer data;
			size_type size;
		};
		constexpr Span(span_view view) noexcept
			: Span(view.data, view.size) {
		}

		/**
		 * @brief Assign from a span_view.
		 */
		constexpr Span& operator=(span_view view) noexcept {
			data_ = view.data;
			size_ = view.size;
			return *this;
		}

		/**
		 * @brief Get a span_view representing this span.
		 */
		constexpr span_view view() const noexcept {
			return span_view{ data_, size_ };
		}

		/**
		 * @brief Assign from another compatible span (bitwise compatible types).
		 */
		template <typename OtherT>
			requires (alignof(value_type) == alignof(OtherT) && sizeof(value_type) == sizeof(OtherT))
		constexpr void assign(Span<OtherT> other) noexcept {
			data_ = reinterpret_cast<pointer>(other.begin());
			size_ = other.size();
		}

		/**
		 * @brief Cast this span to another compatible type.
		 */
		template <typename OtherT>
			requires (alignof(value_type) == alignof(OtherT) && sizeof(value_type) == sizeof(OtherT))
		constexpr Span<OtherT> cast_as() noexcept {
			return Span<OtherT>{reinterpret_cast<OtherT*>(data_), size_};
		}

		/**
		  * @brief Cast this span to another compatible const type.
		  */
		template <typename OtherT>
			requires (alignof(value_type) == alignof(OtherT) && sizeof(value_type) == sizeof(OtherT))
		constexpr Span<const OtherT> cast_as() const noexcept {
			return Span<const OtherT>{reinterpret_cast<const OtherT*>(data_), size_};
		}


		// --- Container Methods ---

		/**
		 * @brief Get the number of elements in the span.
		 */
		constexpr size_type size() const noexcept { return size_; }

		/**
		 * @brief Get the pointer to the first element.
		 */
		constexpr pointer begin() noexcept { return data_; }
		constexpr const_pointer begin() const noexcept { return data_; }

		/**
		 * @brief Get the pointer past the last element.
		 */
		constexpr pointer end() noexcept { return data_ + size_; }
		constexpr const_pointer end() const noexcept { return data_ + size_; }

		/**
		 * @brief Get the raw data pointer.
		 */
		constexpr pointer data() noexcept { return data_; }
		constexpr const_pointer data() const noexcept { return data_; }

		/**
		 * @brief Check if the span is empty.
		 */
		constexpr bool empty() const noexcept { return size_ == 0; }

		/**
		 * @brief Clear the span (set to empty).
		 */
		constexpr void clear() noexcept { data_ = nullptr; size_ = 0; }

		/**
		 * @brief Get a subspan of the first n elements.
		 */
		constexpr Span head(size_type length) noexcept {
			return Span(data_, trim_length(length));
		}

		/**
		 * @brief Get a subspan of the last n elements.
		 */
		constexpr Span tail(size_type length) noexcept {
			length = trim_length(length);
			return Span(data_ + size_ - length, length);
		}

		/**
		 * @brief Returns the step size (always 1 for Span).
		 */
		static constexpr size_type step() noexcept { return 1; }

		/**
		 * @brief Returns true if the span is contiguous (always true).
		 */
		static constexpr bool contiguous() noexcept { return true; }


		// --- Element Access ---

		/**
		 * @brief Access element by index (unchecked).
		 */
		constexpr reference operator[](std::integral auto Index) noexcept { return data_[Index]; }
		constexpr const_reference operator[](std::integral auto Index) const noexcept { return data_[Index]; }

		// --- Swap Operations ---

		/**
		 * @brief Swap elements with another sequence (unchecked).
		 */
		template <typename Sequence>
			requires requires (reference a, typename Sequence::reference b) { std::swap(a, b); }
		void swap_elements_unchecked(Sequence other) noexcept {
			for (size_type i = 0; i < size_; ++i) {
				std::swap(operator[](i), other[i]);
			}
		}

		/**
		 * @brief Swap elements with another sequence (size checked).
		 */
		template <typename Sequence>
			requires requires (reference a, typename Sequence::reference b) { std::swap(a, b); }
		void swap_elements(Sequence other) {
			DOMAIN_ERROR_IF(size_ != other.size(), "Span::swap_elements size mismatch: {} != {}", size_, other.size());
			swap_elements_unchecked(other);
		}

		/**
		 * @brief Swap elements with another span (size checked).
		 */
		void swap(Span& other) {
			swap_elements(other);
		}

		// --- Queue Operations ---

			// Unsafe queue operations (no bounds checking)
		constexpr reference unsafe_pop_back() noexcept { return data_[--size_]; }
		constexpr reference unsafe_pop_front() noexcept { --size_; return *(data_++); }
		constexpr Span unsafe_pop_back(std::integral auto Count) noexcept { size_ -= static_cast<size_type>(Count);  return Span{ data_ + size_, Count }; }
		constexpr Span unsafe_pop_front(std::integral auto Count) noexcept { size_ -= static_cast<size_type>(Count); data_ += Count; return Span{ data_ - Count, Count }; }

		constexpr reference unsafe_back() noexcept { return data_[size_ - 1]; }
		constexpr reference unsafe_front() noexcept { return data_[0]; }
		constexpr Span unsafe_back(std::integral auto Count) noexcept { return Span{ data_ + size_ - Count, Count }; }
		constexpr Span unsafe_front(std::integral auto Count) noexcept { return Span{ data_, Count }; }

		constexpr const_reference unsafe_back() const noexcept { return const_cast<Span*>(this)->unsafe_back(); }
		constexpr const_reference unsafe_front() const noexcept { return const_cast<Span*>(this)->unsafe_front(); }
		constexpr Span unsafe_back(std::integral auto Count) const noexcept { return const_cast<Span*>(this)->unsafe_back(Count); }
		constexpr Span unsafe_front(std::integral auto Count) const noexcept { return const_cast<Span*>(this)->unsafe_front(Count); }




		// Safe queue operations (with bounds checking)
		reference pop_back() { DOMAIN_ERROR_IF(size_ <= 0, "Span::back() empty Span"); return unsafe_pop_back(); }
		reference pop_front() { DOMAIN_ERROR_IF(size_ <= 0, "Span::back() empty Span"); return unsafe_pop_front(); }
		Span pop_back(std::integral auto Count) { DOMAIN_ERROR_IF(size_ < Count, "Span::back() empty Span"); return unsafe_pop_back(Count); }
		Span pop_front(std::integral auto Count) { DOMAIN_ERROR_IF(size_ < Count, "Span::back() empty Span"); return unsafe_pop_front(Count); }

		reference back() { DOMAIN_ERROR_IF(size_ <= 0, "Span::back() empty Span"); return unsafe_back(); }
		reference front() { DOMAIN_ERROR_IF(size_ <= 0, "Span::back() empty Span"); return unsafe_front(); }
		Span back(std::integral auto Count) { DOMAIN_ERROR_IF(size_ < Count, "Span::back({}) overflow", Count); return const_cast<Span*>(this)->back(Count); }
		Span front(std::integral auto Count) { DOMAIN_ERROR_IF(size_ < Count, "Span::front({}) overflow", Count); return const_cast<Span*>(this)->front(Count); }

		const_reference back() const { return const_cast<Span*>(this)->back(); }
		const_reference front() const { return const_cast<Span*>(this)->front(); }
		Span back(std::integral auto Count) const { return const_cast<Span*>(this)->back(Count); }
		Span front(std::integral auto Count) const { return const_cast<Span*>(this)->front(Count); }



/*
		inline constexpr long long isize() const noexcept { return static_cast<isize_type>(size_); }
		inline constexpr long long istep() const noexcept { return static_cast<isize_type>(1); }
		inline constexpr size_t usize() const noexcept { return static_cast<usize_type>(size_); }
		inline constexpr size_t ustep() const noexcept { return static_cast<usize_type>(1); }
*/


		// --- Filter Operations ---

		/**
		 * @brief Count elements from the front matching a predicate, with optional step size.
		 */
		template <typename Predicate>
		size_type count_filter_front(auto&& F, std::integral auto StepSize = 1) const noexcept
		{
			size_type Step{ static_cast<size_type>(StepSize) };
			Step = Step > 1 ? Step : 1;
			Step = Step < size() ? Step : size();

			auto Ptr{ begin() };
			auto End{ end() };
			while (Ptr < End && F(*Ptr)) { Ptr += Step; }
			if (Ptr < End) { End = Ptr++; }
			Ptr -= Step;
			if (Ptr < begin()) { Ptr = begin(); }
			while (Ptr < End && F(*Ptr)) { ++Ptr; }
			return static_cast<size_type>(Ptr - begin());
		}

		/**
		 * @brief Pop elements from the front matching a predicate.
		 */
		template <typename Predicate>
		Span pop_front_filter(Predicate pred, size_type step_size = 1) noexcept {
			return unsafe_pop_front(count_filter_front(pred, step_size));
		}		

// --- Search ---
		/**
		 * @brief Find lower bound of value.
		 */
		pointer lower_bound(const_reference CR) noexcept { return std::lower_bound(data_, data_ + size_, CR); }
		const_pointer lower_bound(const_reference CR) const noexcept { return std::lower_bound(data_, data_ + size_, CR); }
		pointer lower_bound(const_reference CR, auto&& Func) noexcept { return std::lower_bound(data_, data_ + size_, CR, Func); }
		const_pointer lower_bound(const_reference CR, auto&& Func) const noexcept { return std::lower_bound(data_, data_ + size_, CR, Func); }

		/**
		 * @brief Find index of value (returns -1 if not found).
		 */
		size_type find(const_reference CR) const noexcept {
			auto ptr = lower_bound(CR);
			if (ptr != data_ + size_ && CR == ptr[0]) {
				return static_cast<size_type>(ptr - data_);
			}
			return -1;
		}



		// --- Assignment Operations ---

		/**
		 * @brief Elementwise assignment from another sequence.
		 */
		template <Sequence Seq> 
			requires requires (value_type v, typename Seq::value_type s) { v = s; }
		Span& operator = (Seq rhs) {
			DOMAIN_ERROR_IF(size() != rhs.size(), "elementwise assignment size mismatch: {} != {}\n", size(), rhs.size());
			for (size_type i = 0; i < size(); i++) { data_[i] = rhs[i]; }
			return *this;
		}


		/**
		 * @brief Assignment from another span.
		 */
		Span& operator = (Span rhs)
		{
			DOMAIN_ERROR_IF(size() != rhs.size(), "Span assignment with Span of different size: {} != {}\n", size(), rhs.size());
			if (std::is_trivially_copyable_v<value_type>)
			{
				memcpy(begin(), rhs.begin(), sizeof(value_type) * size());
			}
			else
			{
				for (size_type i = 0; i < size(); i++) { operator[](i) = rhs[i]; }
			}
			return *this;
		}


		/**
		 * @brief Assign all elements to a single value.
		 */
		template <typename U> requires requires (value_type t, U u) { t = u; } 
		Span& operator = (U const& y) noexcept { 
			value_type t{ static_cast<value_type>(y) }; 
			for (size_type i = 0; i < size(); i++) operator[](i) = t; 
			return *this; 
		}

		/**
		 * @brief Elementwise assignment from an initializer_list.
		 */
		template <typename U> requires requires(value_type x, U y) { x = y; }
		Span& operator = (std::initializer_list<U> const& li) {
			DOMAIN_ERROR_IF(size() != li.size(), "elementwise assignment size mismatch: {} != {}\n", size(), li.size());
			U const* source = li.begin();
			for (size_type i = 0; i < size(); i++) { operator[](i) = source[i]; }
			return *this;
		}


		// --- Sorting ---

		/**
		 * @brief Sort the elements in ascending order.
		 */
		void sort() noexcept { std::sort(data_, data_ + size_); }

		/**
		 * @brief Sort the elements using a custom comparator.
		 */
		void sort(auto&& comp) noexcept { std::sort(data_, data_ + size_, comp); }




		/**
		 * @brief Output the span as a string using the format API.
		 */
		friend std::ostream& operator << (std::ostream& os, Span const& bf) { return os << bf.string(); }



	};

}





namespace mz 
{


	/**
	 * @brief Specialization of Span for const element types.
	 * Provides read-only access and operations on a contiguous sequence of const elements.
	 *
	 * @tparam ElementT The type of const elements in the sequence.
	 */
	template <typename ElementT>
	class Span<ElementT const> : public ElementwiseConstOperationsInterface<Span<ElementT const>, ElementT const> {


	public:

		using element_type = ElementT const;
		using value_type = std::remove_cvref_t<ElementT>;
		using const_type = value_type const;
		using pointer = value_type const*;
		using reference = const_type&;
		using const_pointer = value_type const*;
		using const_reference = const_type&;


	private:

		pointer data_{ nullptr };
		size_type size_{ 0 };

		constexpr size_type trim_length(size_type Length) const noexcept { return Length < 0 ? 0 : (Length < size_ ? Length : size_); }

	public:


		constexpr Span() noexcept : data_{ nullptr }, size_{ 0 } {}

		/**
		 * @brief Construct from pointer and size.
		 */
		constexpr Span(ElementT const* Pointer, std::integral auto Size) noexcept :
			data_{ const_cast<pointer>(Pointer) },
			size_{ static_cast<size_type>(Size) } {
		}

		/**
		 * @brief Construct from begin and end pointers.
		 */
		constexpr explicit Span(ElementT const* begin, ElementT const* end) noexcept :
			data_{ const_cast<pointer>(begin) },
			size_{ static_cast<size_type>(end - begin) } {
		}



		struct span_view {
			pointer data_;
			size_type size_;
		};
		constexpr Span(span_view sv) noexcept : Span{ sv.data_, sv.size_ } {}
		constexpr Span& operator = (span_view sv) noexcept { data_ = sv.data_; size_ = sv.size_; return *this; }
		constexpr span_view view() const noexcept { return span_view{ data_, size_ }; }




		template <typename Q>
		__inline constexpr void assign(Span<Q> rhs) noexcept {
			data_ = reinterpret_cast<pointer>(rhs.begin());
			size_ = rhs.size();
		}


		constexpr Span(Span<value_type> Rhs) noexcept : Span{ Rhs.data(), Rhs.size() } {}



		constexpr void clear() noexcept { data_ = nullptr; size_ = 0; }
		constexpr bool empty() const noexcept { return size_ <= 0; }

		constexpr bool valid() const noexcept { return data_ != nullptr && size_ >= 0; }
		consteval bool contiguous() const noexcept { return true; }
		constexpr size_type size() const noexcept { return size_; }
		consteval size_type step() const noexcept { return 1; }


		constexpr const_pointer end() const noexcept { return data_ + size_; }
		constexpr const_pointer data() const noexcept { return data_; }
		constexpr const_pointer begin() const noexcept { return data_; }
		constexpr const_reference operator[](index_type Index) const noexcept { return data_[Index]; }





		constexpr reference unsafe_back() const noexcept { return data_[size_ - 1]; }
		constexpr reference unsafe_pop_back() noexcept { return data_[--size_]; }

		constexpr mz::Span<const_type> const head(size_type Length) const noexcept { return mz::Span<const_type>(data_, trim_length(Length)); }
		constexpr mz::Span<const_type> const tail(size_type Length) const noexcept { Length = trim_length(Length); return mz::Span<const_type>(data_ + size_ - Length, Length); }

		friend std::ostream& operator << (std::ostream& os, Span const& bf) { return os << bf.string(); }




		template <typename Q>
			requires (alignof(value_type) == alignof(Q) && sizeof(value_type) == sizeof(Q))
		__inline constexpr Span<Q const> cast_as() const noexcept { return Span<Q const>{static_cast_as<Q const>(data_), size_}; }

		__inline constexpr Span<value_type> remove_const() noexcept { return Span<value_type>{const_cast<value_type*>(data_), size_}; }


	};

}



// -----------------------------------------------------------------------------
// Global algorithms for spans
// -----------------------------------------------------------------------------

/**
 * @brief Returns true if any element in the Span is nonzero.
 */
template <typename T = double>
bool any(mz::Span<T> bf) noexcept {
	for (size_type i = 0; i < bf.size(); i++) {
		if (bf[i]) return true;
	}
	return false;
}

/**
 * @brief Returns true if all elements in the Span are nonzero.
 */
template <typename T = double>
bool all(mz::Span<T> bf) noexcept {
	for (size_type i = 0; i < bf.size(); i++) {
		if (!bf[i]) return false;
	}
	return true;
}

/**
 * @brief Returns the sum of all elements in the Span (for integral types).
 */
template <std::integral T>
long long sum(mz::Span<T> bf) noexcept {
	long long s = 0;
	for (size_type i = 0; i < bf.size(); i++) s += bf[i];
	return s;
}


/**
 * @brief Returns the count of nonzero elements in the Span.
 */
template <typename T>
size_type count(mz::Span<T> bf) {
	size_type cnt = 0;
	for (size_type i = 0; i < bf.size(); i++) bf[i] ? ++cnt : 0;
	return cnt;
}

/**
 * @brief Custom formatter for mz::Span<T> for std::format integration.
 */
template<typename T>
struct std::formatter<mz::Span<T>> : public mz::basic_formatter16 {
	auto format(mz::Span<T> const& p, auto& ctx) const {
		return std::format_to(ctx.out(), "{}", p.string(std::string_view{ FMT, size_t{count} }));
	}
};


#endif // MZ_SPAN_HEADER_FILE


