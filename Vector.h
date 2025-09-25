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

#ifndef MZ_VECTOR_HEADER_FILE
#define MZ_VECTOR_HEADER_FILE
#pragma once
#include <span>
#include <iostream>
#include <algorithm>
#include "globals.h"
#include "zstream.h"
#include "Span.h"
#include "Slice.h"





namespace mz {

	/**
	 * @brief Dynamic, contiguous, resizable array with elementwise operations.
	 * Provides arithmetic, bitwise, and logical operations, as well as serialization and interoperability with Span/Slice.
	 * @tparam T Element type.
	 */
	template <typename T>
	class Vector : public ElementwiseMutableOperationsInterface<Vector<T>,T> {



		friend constexpr void swap(Vector& L, Vector& R) noexcept { L.swap_data(R); }
		friend constexpr void swap(Vector& L, Vector&& R) noexcept { L.swap_data(R); }




		// --- Type Aliases ---
	public:
		using element_type = T;
		using value_type = std::remove_cvref_t<element_type>;
		using const_type = value_type const;
		using lvalue_type = value_type&;
		using rvalue_type = value_type&&;

		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const_type*;
		using const_reference = const_type&;


		// --- Data Members ---
	protected:

		pointer m_data{ nullptr };      // Pointer to data buffer
		size_type m_size{ 0 };          // Number of elements
		size_type m_cap{ 0 };           // Allocated capacity

		// --- Utility ---
		/**
		 * @brief Clamp length to [0, m_size].
		 */
		constexpr size_type trim_length(size_type Length) const noexcept { return Length < 0 ? 0 : (Length < m_size ? Length : m_size); }

		/**
		 * @brief Swap contents with another vector.
		 */
		constexpr void swap_data(Vector& other) noexcept {
			std::swap(m_data, other.m_data);
			std::swap(m_size, other.m_size);
			std::swap(m_cap, other.m_cap);
		}

		constexpr void swap_data(Vector&& other) noexcept { swap_data(other); }


	public:

// --- Constructors, Destructor, Assignment ---

		/**
		 * @brief Destructor. Releases memory.
		 */
		constexpr ~Vector() noexcept { delete[] m_data; m_data = nullptr; m_size = 0; m_cap = 0; }

		/**
		 * @brief Default constructor. Empty vector.
		 */
		explicit constexpr Vector() noexcept : m_data{ nullptr }, m_size{ 0 }, m_cap{ 0 } {}

		/**
		 * @brief Construct with given capacity and size.
		 * @param Capacity Number of elements to allocate.
		 * @param Size Number of elements to initialize.
		 */
		explicit constexpr Vector(INDEX_T Capacity, INDEX_T Size) noexcept :
			m_data{ new value_type[Capacity] },
			m_size{ static_cast<size_type>(Size) },
			m_cap{ static_cast<size_type>(Capacity) } {
		}

		/**
		 * @brief Move constructor. Transfers ownership.
		 */
		constexpr Vector(Vector&& rhs) noexcept :
			m_data{ rhs.m_data },
			m_size{ rhs.m_size },
			m_cap{ rhs.m_cap } {
			rhs.m_data = nullptr;
			rhs.m_size = 0;
			rhs.m_cap = 0;
		}

		/**
		 * @brief Copy constructor. Deep copy.
		 */
		Vector(const Vector& rhs) noexcept :
			m_data{ new value_type[rhs.m_size] },
			m_size{ rhs.m_size },
			m_cap{ rhs.m_size }
		{
			for (size_type i = 0; i < m_size; i++) m_data[i] = rhs.m_data[i];
		}

		/**
		 * @brief Move assignment. Swaps data.
		 */
		Vector& operator = (Vector&& rhs) noexcept { if (this != &rhs) swap_data(rhs); return *this; }

		/**
		 * @brief Copy assignment. Uses copy-and-swap idiom.
		 */
		Vector& operator = (Vector const& rhs) noexcept { if (this != &rhs) swap_data(Vector(rhs)); return *this; }


	
// --- Capacity Management ---

		/**
		 * @brief Reserve capacity. Optionally keep existing data.
		 */
		void reserve(INDEX_T Capacity, bool KeepExistingData) noexcept {
			long long OldCapacity = m_cap;
			long long NewCapacity = Capacity;
			if (OldCapacity < NewCapacity) {
				pointer Ptr = new value_type[NewCapacity];
				if (KeepExistingData && m_size > 0) {
					if constexpr (std::is_trivially_copyable_v<value_type>) {
						memcpy(Ptr, m_data, sizeof(value_type) * m_size);
					}
					else {
						for (size_type i = 0; i < m_size; i++) { Ptr[i] = m_data[i]; }
					}
				}
				else {
					m_size = 0;
				}
				std::swap(Ptr, m_data);
				m_cap = static_cast<size_type>(NewCapacity);
				delete[] Ptr;
			}
		}

		/**
		 * @brief Reserve extra capacity.
		 */
		void reserve_extra(INDEX_T ExtraCapacity) {
			DOMAIN_ERROR_IF(m_size < 0, "Vector::reserve_extra: size = {} < 0", m_size);
			DOMAIN_ERROR_IF(m_size > m_cap, "Vector::reserve_extra: size = {} > capacity = {}", m_size, m_cap);
			reserve(m_size + ExtraCapacity, true);
		}

		/**
		 * @brief Reserve and clear vector.
		 */
		void reserve_and_clear(INDEX_T Capacity) noexcept {
			long long OldCapacity = m_cap;
			long long NewCapacity = Capacity;
			if (OldCapacity < NewCapacity) {
				pointer Ptr = new value_type[NewCapacity];
				std::swap(Ptr, m_data);
				m_cap = static_cast<size_type>(NewCapacity);
				delete[] Ptr;
			}
			m_size = 0;
		}

		/**
		 * @brief Reserve and resize vector.
		 */
		void reserve_and_resize(INDEX_T Capacity) noexcept {
			long long OldCapacity = m_cap;
			long long NewCapacity = Capacity;
			if (OldCapacity < NewCapacity) {
				pointer Ptr = new value_type[NewCapacity];
				std::swap(Ptr, m_data);
				m_cap = static_cast<size_type>(NewCapacity);
				delete[] Ptr;
			}
			m_size = static_cast<size_type>(Capacity);
		}

		/**
		 * @brief Resize vector, optionally keeping existing data.
		 */
		void resize(INDEX_T Size, bool KeepExistingData) noexcept {
			reserve(Size, KeepExistingData);
			m_size = static_cast<size_type>(Size);
		}

		/**
		 * @brief Resize and initialize all elements to given value.
		 */
		void resize_and_initialize(INDEX_T Size, const_reference Value) noexcept {
			reserve(Size, false);
			m_size = static_cast<size_type>(Size);
			for (size_type i = 0; i < m_size; i++) { m_data[i] = Value; }
		}

		/**
		 * @brief Resize and clear (zero) all elements.
		 */
		void resize_and_clear(INDEX_T Size) noexcept {
			reserve(Size, false);
			m_size = static_cast<size_type>(Size);
			memset(m_data, 0, sizeof(value_type) * m_size);
		}

		/**
		 * @brief Double capacity if full.
		 */
		void enlarge() noexcept {
			if (m_size == m_cap) {
				auto new_cap = m_cap ? m_cap * 2 : 2;
				reserve(new_cap, true);
			}
		}

		/**
		 * @brief Shrink capacity to fit size.
		 */
		void shrink_to_fit() noexcept {
			if (m_size < m_cap) {
				pointer Ptr = new value_type[m_size];
				if constexpr (std::is_trivially_copyable_v<value_type>) {
					memcpy(Ptr, m_data, sizeof(value_type) * m_size);
				}
				else {
					for (size_type i = 0; i < m_size; i++) { Ptr[i] = m_data[i]; }
				}
				std::swap(m_data, Ptr);
				m_cap = m_size;
				delete[] Ptr;
			}
		}



// --- Construction/Assignment from compatible Sequences (mz::Span, mz::Slice, etc.) ---
		/**
		 * @brief Construct from a compatible sequence. 
		 */
		template <Sequence Seq>
			requires requires (value_type x, typename Seq::value_type y) { x = y; }
		Vector(Seq const& rhs) noexcept :
			m_data{ new value_type[rhs.size()] },
			m_size{ static_cast<size_type>(rhs.size()) },
			m_cap{ static_cast<size_type>(rhs.size()) }
		{
			for (size_type i = 0; i < m_size; i++) { m_data[i] = rhs[i]; }
		};

		/**
		 * @brief Assign from a compatible sequence, does not free memory if capacity is sufficient.
		 */
		template <Sequence Seq>
			requires requires(value_type x, typename Seq::value_type y) { x = y; }
		Vector& operator = (Seq const& rhs) noexcept
		{
			if (m_cap < rhs.size()) { swap(*this, Vector(rhs)); }
			else {
				m_size = rhs.size();
				for (size_type i = 0; i < m_size; i++) { m_data[i] = rhs[i]; }
			}
			return *this;
		}

		/**
		 * @brief Assign all elements to a single value.
		 */
		template <typename U>
			requires requires(value_type x, U y) { x = y; }
		Vector& operator = (U const& x) noexcept
		{
			value_type const t = static_cast<value_type>(x);
			for (size_type i = 0; i < m_size; i++) { m_data[i] = t; }
			return *this;
		}

		/**
		 * @brief Assign from a std::span.
		 */
		template <typename U>
			requires requires(value_type x, U y) { x = y; }
		Vector& operator = (std::span<U> rhs) noexcept
		{
			resize(size_type(rhs.size()), false);
			for (size_type i = 0; i < m_size; ++i) { m_data[i] = rhs[i]; }
			return *this;
		}



// --- Capacity and Size ---
		/**
		 * @brief Remove all elements (does not free memory).
		 */
		constexpr void clear() noexcept { m_size = 0; }

		/**
		 * @brief Free memory and reset to empty state.
		 */
		void clean() noexcept { swap(*this, Vector()); }

		/**
		 * @brief Set size to capacity (fill with default-initialized elements).
		 */
		constexpr void expand_to_capacity() noexcept { m_size = m_cap; }

		/**
		 * @brief Returns true if vector is empty.
		 */
		constexpr bool empty() const noexcept { return m_size == 0; }

		/**
		 * @brief Returns allocated capacity.
		 */
		constexpr size_type capacity() const noexcept { return m_cap; }

		/**
		 * @brief Returns number of elements.
		 */
		constexpr size_type size() const noexcept { return m_size; }

		/**
		 * @brief Returns step size (always 1).
		 */
		consteval size_type step() const noexcept { return 1; }

		constexpr isize_type isize() const noexcept { return static_cast<isize_type>(m_size); }
		constexpr isize_type istep() const noexcept { return static_cast<isize_type>(1); }
		constexpr usize_type usize() const noexcept { return static_cast<usize_type>(m_size); }
		constexpr usize_type ustep() const noexcept { return static_cast<usize_type>(1); }


// --- Data Access ---

		constexpr pointer end() noexcept { return m_data + m_size; }
		constexpr pointer data() noexcept { return m_data; }
		constexpr pointer begin() noexcept { return m_data; }
		constexpr const_pointer end() const noexcept { return m_data + m_size; }
		constexpr const_pointer data() const noexcept { return m_data; }
		constexpr const_pointer begin() const noexcept { return m_data; }


// --- Element Access (Unsafe) ---

		/**
		 * @brief Access last element (undefined if empty).
		 */
		constexpr reference unsafe_back() noexcept { return m_data[m_size - 1]; }
		constexpr const_reference unsafe_back() const noexcept { return m_data[m_size - 1]; }

		/**
		 * @brief Access first element (undefined if empty).
		 */
		constexpr reference unsafe_front() noexcept { return m_data[0]; }
		constexpr const_reference unsafe_front() const noexcept { return m_data[0]; }

		/**
		 * @brief Access element by index (no bounds checking).
		 */
		constexpr reference operator[](INDEX_T index) noexcept { return m_data[index]; }
		constexpr const_reference operator[](INDEX_T index) const noexcept { return m_data[index]; }


// --- Element Access (Safe) ---

		/**
		 * @brief Access last element (throws if empty).
		 */
		reference back() { DOMAIN_ERROR_IF(m_size < 1, "empty Vector::back()"); return unsafe_back(); }
		const_reference back() const { DOMAIN_ERROR_IF(m_size < 1, "empty Vector::back()"); return unsafe_back(); }

		/**
		 * @brief Access first element (throws if empty).
		 */
		reference front() { DOMAIN_ERROR_IF(m_size < 1, "empty Vector::front()"); return unsafe_front(); }
		const_reference front() const { DOMAIN_ERROR_IF(m_size < 1, "empty Vector::front()"); return unsafe_front(); }

		/**
		 * @brief Access element by index (throws if out of range).
		 */
		reference at(INDEX_T index) { INVALID_ARGUMENT_IF(m_size < size_type(index), "Vector.at({}) index > {} out of range", index, m_size); return operator[](index); }
		const_reference at(INDEX_T index) const { INVALID_ARGUMENT_IF(m_size < size_type(index), "Vector.at({}) index > {} out of range", index, m_size); return operator[](index); }


// --- Modifiers ---

		/**
		 * @brief Remove and return last element, undefined if empty (use pop_back() to check).
		 */
		constexpr value_type unsafe_pop_back() noexcept { return std::move(m_data[--m_size]); }

		/**
		 * @brief Remove and return last element, throws if empty.
		 */
		value_type pop_back() { DOMAIN_ERROR_IF(m_size < 1, "empty Vector::pop_back()"); return std::move(m_data[--m_size]); }

		/**
		 * @brief Add element to end, undefined if capacity exceeded (use enlarge() or push_back() to ensure capacity).
		 */
		constexpr void unsafe_push_back(value_type&& e) noexcept { m_data[m_size++] = std::move(e); }
		constexpr void unsafe_push_back(value_type const& e) noexcept { m_data[m_size++] = e; }

		/**
		 * @brief Add element to end, enlarging if necessary.
		 */
		void push_back(value_type&& e) noexcept { enlarge(); m_data[m_size] = std::move(e); ++m_size; }
		void push_back(value_type const& e) noexcept { enlarge(); m_data[m_size++] = e; }

		/**
		 * @brief Construct element in-place at end.
		 */
		template <class... Args>
		reference emplace_back(Args&&... args) noexcept { enlarge(); return *new (m_data + m_size++) value_type(std::forward<Args>(args)...); }


		template <Sequence Seq>
			requires requires(value_type x, typename Seq::value_type y) { x = y; }
		void append(Seq rhs) noexcept 
		{
			size_type NewSize = m_size + rhs.size();
			reserve(NewSize, true);
			for (size_type i = 0; i < rhs.size(); i++) {
				m_data[m_size + i] = rhs[i];
			}
			m_size += rhs.size();
		}

		/**
		 * @brief Append elements from a compatible sequence, enlarging if necessary.
		 */
		template <typename Ry_>
		void append(mz::Span<Ry_> rhs) noexcept requires is_contiguous_v<value_type>&& std::is_same_v<value_type, std::remove_cvref_t<Ry_>>
		{
			reserve(m_size + rhs.size(), true);
			memcpy(m_data + m_size, rhs.data(), rhs.size() * sizeof(value_type));
			m_size += rhs.size();
		}



// --- Subscript/Selector ---

		/**
		 * @brief Select elements by index sequence. Does not check for out of bounds.
		 */
		template <IntegralSequence Seq>
		Vector operator[](Seq it) const noexcept
		{
			Vector Res(size_type(it.size()));
			for (size_type i = 0; i < it.size(); i++) { Res[i] = m_data[it[i]]; }
			return Res;
		}

		/**
		 * @brief Select elements by boolean mask. Checks for length match.
		 */
		template <BooleanSequence Seq>
		Vector operator[](Seq it) const
		{
			INVALID_ARGUMENT_IF(it.size() != size(), "Vector::[] boolean selector length difference {} != {}\n", it.size(), size());
			size_type Count = count(it);
			Vector Res(Count);
			Res.expand_to_capacity();
			pointer Ptr{ Res.data() };
			for (size_type i = 0; i < it.size(); i++) { if (it[i]) { *Ptr++ = m_data[i]; } }
			return Res;
		}


// --- Serialization ---
		/**
		 * @brief Save vector to stream.
		 */
		void save(mz::Stream& ss) const noexcept
			requires requires(mz::Stream& ss, const_reference x) { ss << x; } {
			ss << size_type(m_size);
			for (size_type i = 0; i < m_size; i++) ss << m_data[i];
		}

		void load(mz::Stream& ss) noexcept
			requires requires(mz::Stream& ss, reference x) { ss >> x; } {
			size_type L;
			ss >> L;
			resize(L, false);
			for (size_type i = 0; i < L; i++) ss >> m_data[i];
		}


		void save2(mz::Stream& ss, auto&& SAVER) const noexcept {
			ss << size_type(m_size);
			for (size_type i = 0; i < m_size; i++) { SAVER(ss, m_data[i]); }
		}

		void load2(mz::Stream& ss, auto&& LOADER) noexcept {
			size_type L;
			ss >> L;
			resize(L, false);
			for (size_type i = 0; i < L; i++) { LOADER(ss, m_data[i]); }
		}


		template<typename Func_> 
			requires requires(mz::Stream& s, value_type& t, Func_&& F) { { F(s, t) } -> std::same_as<bool>; }
		bool load3(mz::Stream& s, Func_&& F, uint64_t Enc = 0) noexcept {

			if (s.read_label(Enc)) return true;

			size_type L;
			s >> L;
			resize(L, false);
			for (size_type i = 0; i < L; i++) { if (F(s, m_data[i])) return true; }
			
			if (s.read_label(Enc)) return true;

			return false;
		}


		bool load3(mz::Stream& s, uint64_t Enc = 0) noexcept {

			if (s.read_label(Enc)) return true;

			size_type L;
			s >> L;
			resize(L, false);
			for (size_type i = 0; i < L; i++) { s >> m_data[i]; }

			if (s.read_label(Enc)) return true;

			return false;
		}


		template<typename Func_>
			requires requires(mz::Stream& s, value_type const& ct, Func_&& F) { { F(s, ct) } -> std::same_as<void>; }
		void save3(mz::Stream& s, Func_&& F, uint64_t Enc = 0) const noexcept {
			s.write_label(Enc);
			size_type L = size();
			s << L;
			for (size_type i = 0; i < L; i++) { F(s, m_data[i]); }
			s.write_label(Enc);
		}


		void save3(mz::Stream& s, uint64_t Enc = 0) const noexcept {
			s.write_label(Enc);
			size_type L = size();
			s << L;
			for (size_type i = 0; i < L; i++) { s << m_data[i]; }
			s.write_label(Enc);
		}















		friend mz::Stream& operator >> (mz::Stream& ss, Vector& facet)
			requires requires(mz::Stream& ss, const_reference x) { ss << x; } { facet.load(ss); return ss; }
		friend mz::Stream& operator << (mz::Stream& ss, const Vector& facet)
			requires requires(mz::Stream& ss, const_reference x) { ss << x; } { facet.save(ss); return ss; }


		Vector& operator << (value_type&& value) noexcept { push_back(std::move(value)); return *this; }
		Vector& operator << (const_reference value) noexcept { push_back(value); return *this; }


		friend std::ostream& operator << (std::ostream& os, Vector const& v)
			requires requires(std::ostream& os, const_reference x) { os << x; }
		{
			for (size_type i = 0; i < v.size(); i++) os << v[i] << ",";
			return os << "\n";
		}


		friend std::iostream& operator >> (std::iostream& ss, Vector& vec)
			requires requires(std::iostream& ss, reference x) { ss >> x; }
		{
			size_type L;
			ss >> L;
			vec.resize(L, false);
			pointer V = vec.data();
			for (size_type i = 0; i < L; i++) ss >> V[i];
			return ss;
		}


		friend std::iostream& operator << (std::iostream& ss, Vector const& vec)
			requires requires(std::iostream& ss, const_reference x) { ss << x; }
		{
			size_type L = vec.size();
			ss << L;
			for (size_type i = 0; i < L; i++) ss << vec[i];
			return ss;
		}


		friend bool operator == (Vector const& lhs, Vector const& rhs) noexcept
			requires requires (const_reference x, const_reference y) { x == y; }
		{
			if (&lhs == &rhs) return false;
			size_type nL = lhs.size();
			if (nL != rhs.size()) return false;
			for (size_type i = 0; i < nL; i++) {
				if (!(lhs[i] == rhs[i])) return false;
			}
			return true;
		}


// --- Interoperability with mz::Span and mz::Slice ---

		/**
		 * @brief Get a Slice view of the vector.
		 */
		inline constexpr mz::Slice<value_type> slice() noexcept { return mz::Slice(data(), size()); }
		inline constexpr mz::Slice<value_type const> const slice() const noexcept { return mz::Slice(data(), size()); }

		/**
		 * @brief Get a Span view of the vector.
		 */
		inline constexpr mz::Span<value_type> span() noexcept { return mz::Span<value_type>(m_data, m_size); }
		inline constexpr mz::Span<value_type const> const span() const noexcept { return mz::Span<value_type const>(m_data, m_size); }

		/**
		 * @brief Get a subspan from First for Length elements.
		 */
		inline constexpr mz::Span<value_type> span(size_type First, size_type Length) noexcept {
			Length = First + Length <= m_size ? Length : m_size - First;
			pointer Ptr = (Length >= 0 && First >= 0) ? m_data + First : nullptr;
			return mz::Span<value_type>(Ptr, Length);
		}
		inline constexpr mz::Span<value_type const> const span(size_type First, size_type Length) const noexcept {
			Length = First + Length <= m_size ? Length : m_size - First;
			const_pointer Ptr = (Length >= 0 && First >= 0) ? m_data + First : nullptr;
			return mz::Span<value_type const>(Ptr, Length);
		}

		/**
		 * @brief Get head/tail subspans.
		 */
		constexpr mz::Span<value_type> head(size_type Length) noexcept { return mz::Span<value_type>(m_data, trim_length(Length)); }
		constexpr mz::Span<const_type> const head(size_type Length) const noexcept { return mz::Span<const_type>(m_data, trim_length(Length)); }
		constexpr mz::Span<value_type> tail(size_type Length) noexcept { Length = trim_length(Length); return mz::Span<value_type>(m_data + m_size - Length, Length); }
		constexpr mz::Span<const_type> const tail(size_type Length) const noexcept { Length = trim_length(Length); return mz::Span<const_type>(m_data + m_size - Length, Length); }

// --- Sorting and Uniqueness ---

		/**
		 * @brief Sort elements in ascending order.
		 */
		Vector& sort() noexcept { std::sort(m_data, m_data + m_size); return *this; }

		/**
		 * @brief Sort elements with custom comparator.
		 */
		Vector& sort(auto&& CompFunction) noexcept { std::sort(m_data, m_data + m_size, CompFunction); return *this; }

		/**
		 * @brief Remove consecutive duplicates. Returns number of repeats removed.
		 */
		long long unique() noexcept {
			auto end = std::unique(m_data, m_data + m_size);
			long long NewSize = end - m_data;
			long long NumRepeat = m_size - NewSize;
			m_size = size_type(NewSize);
			return NumRepeat;
		}
		/**
		 * @brief Remove consecutive duplicates with custom comparator.
		 */
		long long unique(auto&& CompFunction) noexcept {
			auto end = std::unique(m_data, m_data + m_size, CompFunction);
			long long NewSize = end - m_data;
			long long NumRepeat = m_size - NewSize;
			m_size = size_type(NewSize);
			return NumRepeat;
		}









		// --- Search ---
		/**
		 * @brief Find lower bound of value.
		 */
		pointer lower_bound(const_reference CR) noexcept { return std::lower_bound(m_data, m_data + m_size, CR); }
		const_pointer lower_bound(const_reference CR) const noexcept { return std::lower_bound(m_data, m_data + m_size, CR); }
		pointer lower_bound(const_reference CR, auto&& Func) noexcept { return std::lower_bound(m_data, m_data + m_size, CR, Func); }
		const_pointer lower_bound(const_reference CR, auto&& Func) const noexcept { return std::lower_bound(m_data, m_data + m_size, CR, Func); }

		/**
		 * @brief Find index of value (returns -1 if not found).
		 */
		size_type find(const_reference CR) const noexcept {
			auto ptr = lower_bound(CR);
			if (ptr != m_data + m_size && CR == ptr[0]) {
				return static_cast<size_type>(ptr - m_data);
			}
			return -1;
		}

		/* 
		 * Elementwise operations based on definitions in ElementwiseOperationsInterface.h 
		 * Applies to mz::Span, mz::Slice, mz::Vector, etc
		 */
		template <mz::Sequence Seq> friend Vector operator | (Vector&& Lhs, Seq const& Rhs) noexcept { return Lhs |= Rhs; }
		template <mz::Sequence Seq> friend Vector operator & (Vector&& Lhs, Seq const& Rhs) noexcept { return Lhs &= Rhs; }
		template <mz::Sequence Seq> friend Vector operator ^ (Vector&& Lhs, Seq const& Rhs) noexcept { return Lhs ^= Rhs; }
		template <mz::Sequence Seq> friend Vector operator + (Vector&& Lhs, Seq const& Rhs) noexcept { return Lhs += Rhs; }
		template <mz::Sequence Seq> friend Vector operator - (Vector&& Lhs, Seq const& Rhs) noexcept { return Lhs -= Rhs; }
		template <mz::Sequence Seq> friend Vector operator * (Vector&& Lhs, Seq const& Rhs) noexcept { return Lhs *= Rhs; }
		template <mz::Sequence Seq> friend Vector operator / (Vector&& Lhs, Seq const& Rhs) noexcept { return Lhs /= Rhs; }

		template <mz::Sequence SeqL, mz::Sequence SeqR> friend Vector operator | (SeqL const& Lhs, SeqR const& Rhs) noexcept { return Vector{ Lhs } |= Rhs; }
		template <mz::Sequence SeqL, mz::Sequence SeqR> friend Vector operator & (SeqL const& Lhs, SeqR const& Rhs) noexcept { return Vector{ Lhs } &= Rhs; }
		template <mz::Sequence SeqL, mz::Sequence SeqR> friend Vector operator ^ (SeqL const& Lhs, SeqR const& Rhs) noexcept { return Vector{ Lhs } ^= Rhs; }
		template <mz::Sequence SeqL, mz::Sequence SeqR> friend Vector operator + (SeqL const& Lhs, SeqR const& Rhs) noexcept { return Vector{ Lhs } += Rhs; }
		template <mz::Sequence SeqL, mz::Sequence SeqR> friend Vector operator - (SeqL const& Lhs, SeqR const& Rhs) noexcept { return Vector{ Lhs } -= Rhs; }
		template <mz::Sequence SeqL, mz::Sequence SeqR> friend Vector operator * (SeqL const& Lhs, SeqR const& Rhs) noexcept { return Vector{ Lhs } *= Rhs; }
		template <mz::Sequence SeqL, mz::Sequence SeqR> friend Vector operator / (SeqL const& Lhs, SeqR const& Rhs) noexcept { return Vector{ Lhs } /= Rhs; }



	};
}



template <typename S>
struct SAVER {
	template<typename T>
		requires requires(mz::Stream& ss, T const& t, S const& s) { s(ss, t); }
	void operator()(mz::Stream& ss, mz::Vector<T> const& vec) const noexcept {
		S saver{};
		ss << size_type(vec.size());
		for (size_type i = 0; i < vec.size(); i++) saver(ss, vec[i]);
	}
};

template <typename L>
struct LOADER {
	template<typename T>
		requires requires(mz::Stream& ss, T& t, L const& l) { l(ss, t); }
	void operator()(mz::Stream& ss, mz::Vector<T>& vec) const noexcept {
		L loader{};
		size_type length{ 0 };
		ss >> length;
		vec.resize(length, false);
		for (size_type i = 0; i < vec.size(); i++) loader(ss, vec[i]);
	}
};


template<typename T>
struct std::formatter<mz::Vector<T>> : public mz::basic_formatter16 {
	auto format(mz::Vector<T> const& p, auto& ctx) const {
		return std::format_to(ctx.out(), "{}", p.string(std::string_view{ FMT, size_t{count} }));
	}
};





#endif