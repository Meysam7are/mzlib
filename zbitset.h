#ifndef MZ_BITSET_HEADER_FILE
#define MZ_BITSET_HEADER_FILE
#pragma once
#include <ostream>
#include <cstdint>
#include <intrin.h>
#include "globals.h"

/**
 * @brief Bit manipulation utility class for integral types.
 *
 * Provides efficient bitwise operations, bit counting, and bit scanning
 * using platform intrinsics. Designed for use with small fixed-width bitsets.
 *
 * @tparam T Integral type (e.g., uint32_t, uint64_t).
 */
template <std::integral T>
class BitsT {

public:
    T bits{ 0 }; ///< Underlying storage for bitset.

    // --- Constructors ---

    /**
     * @brief Default constructor. Initializes all bits to zero.
     */
    explicit constexpr BitsT() noexcept = default;

    /**
     * @brief Construct from any integral value.
     * @param value Initial value for bits.
     */
    explicit constexpr BitsT(std::integral auto value) noexcept : bits{ static_cast<T>(value) } {}

    /**
     * @brief Construct from type T.
     * @param value Initial value for bits.
     */
    explicit constexpr BitsT(T value) noexcept : bits{ value } {}

    // --- Element Access ---

    /**
     * @brief Access bit at given index (read-only).
     * @param index Bit index.
     * @return true if bit is set, false otherwise.
     */
    constexpr auto operator[](INDEX_T index) const noexcept { return get(index); }

    // --- Bulk Bit Operations ---

    /**
     * @brief Set all bits to 1.
     */
    constexpr void set_all_bits() noexcept { bits = -1; }

    /**
     * @brief Clear all bits (set to 0).
     */
    constexpr void clear_all_bits() noexcept { bits = 0; }

    // --- Bitwise Operators ---

    /**
     * @brief Returns true if no bits are set.
     */
    constexpr bool operator ! () const noexcept { return !bits; }

    /**
     * @brief Bitwise NOT (complement).
     * @return New BitsT with all bits inverted.
     */
    constexpr BitsT operator ~ () const noexcept { return BitsT(static_cast<T>(~bits)); }

    /**
     * @brief Bitwise AND assignment.
     * @param rhs BitsT to AND with.
     * @return Reference to this.
     */
    constexpr BitsT& operator &= (BitsT rhs) noexcept { bits &= rhs.bits; return *this; }

    /**
     * @brief Bitwise OR assignment.
     * @param rhs BitsT to OR with.
     * @return Reference to this.
     */
    constexpr BitsT& operator |= (BitsT rhs) noexcept { bits |= rhs.bits; return *this; }

    /**
     * @brief Bitwise XOR assignment.
     * @param rhs BitsT to XOR with.
     * @return Reference to this.
     */
    constexpr BitsT& operator ^= (BitsT rhs) noexcept { bits ^= rhs.bits; return *this; }

    /**
     * @brief Bitwise AND with NOT assignment (clear bits present in rhs).
     * @param rhs BitsT whose bits to clear.
     * @return Reference to this.
     */
    constexpr BitsT& operator %= (BitsT rhs) noexcept { bits &= ~rhs.bits; return *this; }

    // --- Mask Generators ---

    /**
     * @brief Returns the bitwise complement, limited to NumDimensions bits.
     * @param NumDimensions Number of bits to consider.
     * @return BitsT with complemented bits.
     */
    constexpr BitsT complement(INDEX_T NumDimensions = 32) noexcept {
        BitsT res;
        res.bits = ((1 << NumDimensions) - 1) & (~bits);
        return res;
    }

    /**
     * @brief Generate a mask with the lowest 'dimension' bits set.
     * @param dimension Number of bits to set.
     * @return BitsT mask.
     */
    constexpr static BitsT LowerMask(INDEX_T dimension) noexcept {
        BitsT res;
        res.bits = (1 << dimension) - 1;
        return res;
    }

    /**
     * @brief Generate a mask with the highest bits set, starting from 'dimension'.
     * @param dimension Number of lower bits to clear.
     * @return BitsT mask.
     */
    constexpr static BitsT UpperMask(INDEX_T dimension) noexcept {
        uint64_t lower_dim = (sizeof(T) << 3) - dimension;
        BitsT res;
        res.bits = (1 << lower_dim) - 1;
        res.bits = ~res.bits;
        return res;
    }

    // --- Bit Counting and Scanning ---

    /**
     * @brief Count leading zeros in the bitset.
     * @return Number of leading zero bits.
     */
    constexpr auto lz_count() const noexcept {
        if constexpr (sizeof(T) <= 4) {
            auto mask = static_cast<unsigned int>(bits);
            return __lzcnt(mask);
        }
        else {
            auto mask = static_cast<unsigned __int64>(bits);
            return __lzcnt64(mask);
        }
    }

    /**
     * @brief Count number of set bits (population count).
     * @return Number of bits set to 1.
     */
    constexpr auto pop_count() const noexcept {
        if constexpr (sizeof(T) <= 4) {
            auto mask = static_cast<unsigned int>(bits);
            return __popcnt(mask);
        }
        else {
            auto mask = static_cast<unsigned __int64>(bits);
            return __popcnt64(mask);
        }
    }

    /**
     * @brief Find the index of the most significant set bit.
     * @return Index of highest set bit, or -1 if none.
     */
    constexpr auto bit_scan_reverse() const noexcept {
        unsigned char uc;
        unsigned long x;
        if constexpr (sizeof(T) <= 4) {
            unsigned long mask = static_cast<unsigned long>(bits);
            uc = _BitScanReverse(&x, mask);
        }
        else {
            __int64 mask = static_cast<__int64>(bits);
            uc = _BitScanReverse64(&x, mask);
        }
        return uc ? static_cast<int>(x) : int(-1);
    }

    /**
     * @brief Find the index of the least significant set bit.
     * @return Index of lowest set bit, or -1 if none.
     */
    constexpr auto leastSignificantOne() const noexcept {
        unsigned char uc;
        unsigned long x;
        if constexpr (sizeof(T) <= 4) {
            unsigned long mask = static_cast<unsigned long>(bits);
            uc = _BitScanForward(&x, mask);
        }
        else {
            __int64 mask = static_cast<__int64>(bits);
            uc = _BitScanForward64(&x, mask);
        }
        return uc ? static_cast<int>(x) : int(-1);
    }

    // --- Individual Bit Operations ---

    /**
     * @brief Set bit at given index.
     * @param index Bit index to set.
     */
    constexpr void set(INDEX_T index) noexcept {
        if constexpr (sizeof(T) <= sizeof(long)) {
            long i = static_cast<long>(index);
            long x = static_cast<long>(bits);
            _bittestandset(&x, i);
            bits = static_cast<T>(x);
        }
        else {
            __int64 i = static_cast<__int64>(index);
            __int64 x = static_cast<__int64>(bits);
            _bittestandset64(&x, i);
            bits = static_cast<T>(x);
        }
    }

    /**
     * @brief Set bit at index and return previous value.
     * @param index Bit index.
     * @return Previous value of the bit.
     */
    constexpr auto test_and_set(INDEX_T index) noexcept {
        if constexpr (sizeof(T) <= sizeof(long)) {
            long i = static_cast<long>(index);
            long x = static_cast<long>(bits);
            auto res = _bittestandset(&x, i);
            bits = static_cast<T>(x);
            return res;
        }
        else {
            __int64 i = static_cast<__int64>(index);
            __int64 x = static_cast<__int64>(bits);
            auto res = _bittestandset64(&x, i);
            bits = static_cast<T>(x);
            return res;
        }
    }

    /**
     * @brief Clear bit at given index.
     * @param index Bit index to clear.
     */
    constexpr void clear(INDEX_T index) noexcept {
        if constexpr (sizeof(T) <= sizeof(long)) {
            long i = static_cast<long>(index);
            long x = static_cast<long>(bits);
            _bittestandreset(&x, i);
            bits = static_cast<T>(x);
        }
        else {
            __int64 i = static_cast<__int64>(index);
            __int64 x = static_cast<__int64>(bits);
            _bittestandreset64(&x, i);
            bits = static_cast<T>(x);
        }
    }

    /**
     * @brief Alias for clear(index).
     * @param index Bit index to clear.
     */
    constexpr void clr(INDEX_T index) noexcept { clear(index); }

    /**
     * @brief Clear bit at index and return previous value.
     * @param index Bit index.
     * @return Previous value of the bit.
     */
    constexpr auto test_and_clear(INDEX_T index) noexcept {
        if constexpr (sizeof(T) <= sizeof(long)) {
            long i = static_cast<long>(index);
            long x = static_cast<long>(bits);
            auto res = _bittestandreset(&x, i);
            bits = static_cast<T>(x);
            return res;
        }
        else {
            __int64 i = static_cast<__int64>(index);
            __int64 x = static_cast<__int64>(bits);
            auto res = _bittestandreset64(&x, i);
            bits = static_cast<T>(x);
            return res;
        }
    }

    /**
     * @brief Set or clear bit at index based on condition.
     * @param index Bit index.
     * @param Condition If true, set; else clear.
     */
    constexpr void update(INDEX_T index, bool Condition) noexcept { Condition ? set(index) : clear(index); }

    /**
     * @brief Set or clear bit at index based on condition, returning previous value.
     * @param index Bit index.
     * @param Condition If true, set; else clear.
     * @return Previous value of the bit.
     */
    constexpr auto test_and_update(INDEX_T index, bool Condition) noexcept { return Condition ? test_and_set(index) : test_and_clear(index); }

    /**
     * @brief Complement bit at index and return previous value.
     * @param index Bit index.
     * @return Previous value of the bit.
     */
    constexpr auto comp(INDEX_T index) noexcept {
        if constexpr (sizeof(T) <= sizeof(long)) {
            long i = static_cast<long>(index);
            long x = static_cast<long>(bits);
            auto res = _bittestandcomplement(&x, i);
            bits = static_cast<T>(x);
            return res;
        }
        else {
            __int64 i = static_cast<__int64>(index);
            __int64 x = static_cast<__int64>(bits);
            auto res = _bittestandcomplement64(&x, i);
            bits = static_cast<T>(x);
            return res;
        }
    }

    /**
     * @brief Get value of bit at index.
     * @param index Bit index.
     * @return true if bit is set, false otherwise.
     */
    constexpr auto get(INDEX_T index) const noexcept {
        if constexpr (sizeof(T) <= sizeof(long)) {
            long i = static_cast<long>(index);
            long x = static_cast<long>(bits);
            return _bittest(&x, i);
        }
        else {
            __int64 i = static_cast<__int64>(index);
            __int64 x = static_cast<__int64>(bits);
            return _bittest64(&x, i);
        }
    }

    /**
     * @brief Set or clear bit at index based on state.
     * @param index Bit index.
     * @param state If true, set; else clear.
     */
    void apply(INDEX_T index, bool state) noexcept {
        if (state) {
            set(index);
        }
        else {
            clear(index);
        }
    }

    // --- String Conversion ---

    /**
     * @brief Convert bitset to string representation.
     * @param NumBits Number of bits to display.
     * @return String of bits (LSB first).
     */
    std::string string(std::integral auto NumBits = 32) const noexcept {
        char Data[64]{};
        unsigned long long Value = bits;
        size_type Index{ 0 };
        while (Value) {
            Data[Index++] = '0' + (Value & 1);
            Value >>= 1;
        }
        NumBits = NumBits < 64 ? NumBits : 64;
        while (Index < NumBits) { Data[Index++] = '0'; }
        return std::string{ Data, size_t(Index) };
    }

    // --- Friend Operators ---

    /**
     * @brief Bitwise AND.
     */
    friend constexpr BitsT operator & (BitsT L, BitsT R) noexcept { return BitsT{ L.bits & R.bits }; }

    /**
     * @brief Bitwise OR.
     */
    friend constexpr BitsT operator | (BitsT L, BitsT R) noexcept { return BitsT{ L.bits | R.bits }; }

    /**
     * @brief Bitwise XOR.
     */
    friend constexpr BitsT operator ^ (BitsT L, BitsT R) noexcept { return BitsT{ L.bits ^ R.bits }; }

    /**
     * @brief Bitwise AND with NOT (clear bits present in R).
     */
    friend constexpr BitsT operator % (BitsT L, BitsT R) noexcept { return BitsT{ L.bits & ~R.bits }; }

    /**
     * @brief Equality comparison.
     */
    friend constexpr bool operator == (BitsT L, BitsT R) noexcept { return L.bits == R.bits; }

    /**
     * @brief Subset comparison (L is subset of R).
     */
    friend constexpr bool operator < (BitsT L, BitsT R) noexcept { return !(L % R) && !!(R % L); }

    /**
     * @brief Subset or equal comparison.
     */
    friend constexpr bool operator <= (BitsT L, BitsT R) noexcept { return (L == R) || (L < R); }

};

/**
 * @brief Dual bitset for representing two related sets of bits (e.g., positive/negative).
 *
 * Used for geometric and algebraic applications where two bitsets are needed in parallel.
 * Provides combined and individual bitwise operations.
 *
 * @tparam T Integral type for underlying bitsets.
 */
template <std::integral T>
class BitLinesT {

public:
    BitsT<T> Pos{ 0 }; ///< Positive bitset.
    BitsT<T> Neg{ 0 }; ///< Negative bitset.

    using value_type = typename BitsT<T>;

    // --- Constructors ---

    /**
     * @brief Default constructor. Both bitsets initialized to zero.
     */
    constexpr BitLinesT() noexcept = default;

    /**
     * @brief Construct from two integral values.
     * @param PosBits Initial value for positive bitset.
     * @param NegBits Initial value for negative bitset.
     */
    constexpr explicit BitLinesT(std::integral auto PosBits, std::integral auto NegBits) noexcept :
        Pos{ PosBits },
        Neg{ NegBits } {
    }

    /**
     * @brief Construct from two BitsT values.
     * @param PosBits Initial value for positive bitset.
     * @param NegBits Initial value for negative bitset.
     */
    constexpr explicit BitLinesT(value_type PosBits, value_type NegBits) noexcept :
        Pos{ PosBits },
        Neg{ NegBits } {
    }

    // --- State Queries ---

    /**
     * @brief Returns true if any positive bits are set.
     */
    constexpr bool any_pos() const noexcept { return Pos.bits; }

    /**
     * @brief Returns true if any negative bits are set.
     */
    constexpr bool any_neg() const noexcept { return Neg.bits; }

    /**
     * @brief Returns true if any bits are set in either bitset.
     */
    constexpr bool any_both() const noexcept { return Pos.bits || Neg.bits; }

    // --- Bulk Bit Operations ---

    /**
     * @brief Clear all positive bits.
     */
    constexpr void clear_all_pos() noexcept { Pos.clear_all_bits(); }

    /**
     * @brief Clear all negative bits.
     */
    constexpr void clear_all_neg() noexcept { Neg.clear_all_bits(); }

    /**
     * @brief Clear all bits in both bitsets.
     */
    constexpr void clear_all_both() noexcept { clear_all_pos(); clear_all_neg(); }

    /**
     * @brief Set all positive bits.
     */
    constexpr void set_all_pos() noexcept { Pos.set_all_bits(); }

    /**
     * @brief Set all negative bits.
     */
    constexpr void set_all_neg() noexcept { Neg.set_all_bits(); }

    /**
     * @brief Set all bits in both bitsets.
     */
    constexpr void set_all_both() noexcept { set_all_pos(); set_all_neg(); }

    // --- Bitset Views ---

    /**
     * @brief Get positive bitset.
     */
    constexpr value_type pos() const noexcept { return Pos; }

    /**
     * @brief Get bitwise NOT of positive bitset.
     */
    constexpr value_type nonpos() const noexcept { return ~Pos; }

    /**
     * @brief Get bits set in positive but not negative bitset.
     */
    constexpr value_type onlypos() const noexcept { return Pos & ~Neg; }

    /**
     * @brief Get negative bitset.
     */
    constexpr value_type neg() const noexcept { return Neg; }

    /**
     * @brief Get bitwise NOT of negative bitset.
     */
    constexpr value_type nonneg() const noexcept { return ~Neg; }

    /**
     * @brief Get bits set in negative but not positive bitset.
     */
    constexpr value_type onlyneg() const noexcept { return Neg & ~Pos; }

    /**
     * @brief Get bits set in both bitsets.
     */
    constexpr value_type both() const noexcept { return Pos & Neg; }

    /**
     * @brief Get bits set in either bitset but not both.
     */
    constexpr value_type diff() const noexcept { return Pos ^ Neg; }

    /**
     * @brief Get bits set in both or neither.
     */
    constexpr value_type same() const noexcept { return ~(Pos ^ Neg); }

    /**
     * @brief Get bits set in either bitset.
     */
    constexpr value_type either() const noexcept { return Pos | Neg; }

    /**
     * @brief Get bits set in neither bitset.
     */
    constexpr value_type neither() const noexcept { return ~(Pos | Neg); }

    // --- Individual Bit Queries ---

    /**
     * @brief Query positive bit at index.
     */
    constexpr auto pos(INDEX_T Index) const noexcept { return Pos.get(Index); }

    /**
     * @brief Query NOT positive bit at index.
     */
    constexpr auto nonpos(INDEX_T Index) const noexcept { return nonpos().get(Index); }

    /**
     * @brief Query only positive bit at index.
     */
    constexpr auto onlypos(INDEX_T Index) const noexcept { return onlypos().get(Index); }

    /**
     * @brief Query negative bit at index.
     */
    constexpr auto neg(INDEX_T Index) const noexcept { return Neg.get(Index); }

    /**
     * @brief Query NOT negative bit at index.
     */
    constexpr auto nonneg(INDEX_T Index) const noexcept { return nonneg().get(Index); }

    /**
     * @brief Query only negative bit at index.
     */
    constexpr auto onlyneg(INDEX_T Index) const noexcept { return onlyneg().get(Index); }

    /**
     * @brief Query both bits at index.
     */
    constexpr auto both(INDEX_T Index) const noexcept { return both().get(Index); }

    /**
     * @brief Query diff bits at index.
     */
    constexpr auto diff(INDEX_T Index) const noexcept { return diff().get(Index); }

    /**
     * @brief Query same bits at index.
     */
    constexpr auto same(INDEX_T Index) const noexcept { return same().get(Index); }

    /**
     * @brief Query either bits at index.
     */
    constexpr auto either(INDEX_T Index) const noexcept { return either().get(Index); }

    /**
     * @brief Query neither bits at index.
     */
    constexpr auto neither(INDEX_T Index) const noexcept { return neither().get(Index); }

    // --- Individual Bit Operations ---

    /**
     * @brief Clear positive bit at index.
     */
    constexpr void clear_pos(INDEX_T Index) noexcept { Pos.clear(Index); }

    /**
     * @brief Clear negative bit at index.
     */
    constexpr void clear_neg(INDEX_T Index) noexcept { Neg.clear(Index); }

    /**
     * @brief Clear both bits at index.
     */
    constexpr void clear_both(INDEX_T Index) noexcept { clear_pos(Index); clear_neg(Index); }

    /**
     * @brief Set positive bit at index.
     */
    constexpr void set_pos(INDEX_T Index) noexcept { Pos.set(Index); }

    /**
     * @brief Set negative bit at index.
     */
    constexpr void set_neg(INDEX_T Index) noexcept { Neg.set(Index); }

    /**
     * @brief Set both bits at index.
     */
    constexpr void set_both(INDEX_T Index) noexcept { set_pos(Index); set_neg(Index); }

    /**
     * @brief Clear positive bit at index (set non-positive).
     */
    constexpr void set_nonpos(INDEX_T Index) noexcept { clear_pos(Index); }

    /**
     * @brief Set only positive bit at index.
     */
    constexpr void set_onlypos(INDEX_T Index) noexcept { set_pos(Index); clear_neg(Index); }

    /**
     * @brief Clear negative bit at index (set non-negative).
     */
    constexpr void set_nonneg(INDEX_T Index) noexcept { clear_neg(Index); }

    /**
     * @brief Set only negative bit at index.
     */
    constexpr void set_onlyneg(INDEX_T Index) noexcept { clear_pos(Index); set_neg(Index); }

    // --- Sign and Assignment ---

    /**
     * @brief Get sign at index: +1 (pos), -1 (neg), 0 (neither).
     */
    constexpr sign_type sign(INDEX_T Index) noexcept { return (!!pos(Index)) - (!!neg(Index)); }

    /**
     * @brief Assign sign at index: +1 (pos), -1 (neg), 0 (neither).
     * @param Index Bit index.
     * @param Sign Value to assign.
     * @return Assigned sign.
     */
    sign_type assign(INDEX_T Index, sign_type Sign) noexcept
    {
        if (!Sign) { clear_both(Index); }
        else if (Sign > 0) { set_onlypos(Index); }
        else { set_onlyneg(Index); }
        return Sign;
    }

    // --- Bit Counting ---

    /**
     * @brief Count number of set bits in both bitsets.
     * @return Total number of bits set.
     */
    constexpr auto pop_count() const noexcept { return Pos.pop_count() + Neg.pop_count(); }

    // --- Bitwise Operators ---

    /**
     * @brief Swap positive and negative bitsets.
     * @return BitLinesT with swapped bitsets.
     */
    constexpr BitLinesT operator -() const noexcept { return BitLinesT{ Neg, Pos }; }

    /**
     * @brief Bitwise NOT of both bitsets.
     * @return BitLinesT with complemented bitsets.
     */
    constexpr BitLinesT operator ~() const noexcept { return BitLinesT{ ~Pos, ~Neg }; }

    /**
     * @brief Bitwise AND of two BitLinesT.
     */
    friend constexpr BitLinesT operator & (BitLinesT L, BitLinesT R) noexcept { return BitLinesT{ L.Pos & R.Pos, L.Neg & R.Neg }; }

    /**
     * @brief Bitwise OR of two BitLinesT.
     */
    friend constexpr BitLinesT operator | (BitLinesT L, BitLinesT R) noexcept { return BitLinesT{ L.Pos | R.Pos, L.Neg | R.Neg }; }

    /**
     * @brief Bitwise XOR of two BitLinesT.
     */
    friend constexpr BitLinesT operator ^ (BitLinesT L, BitLinesT R) noexcept { return BitLinesT{ L.Pos ^ R.Pos, L.Neg ^ R.Neg }; }

    /**
     * @brief Bitwise AND with NOT for both bitsets.
     */
    friend constexpr BitLinesT operator % (BitLinesT L, BitLinesT R) noexcept { return BitLinesT{ L.Pos % R.Pos, L.Neg % R.Neg }; }

    /**
     * @brief Equality comparison.
     */
    friend constexpr bool operator == (BitLinesT L, BitLinesT R) noexcept { return (L.Pos == R.Pos) && (L.Neg == R.Neg); }

    /**
     * @brief Subset or equal comparison.
     */
    friend constexpr bool operator <= (BitLinesT L, BitLinesT R) noexcept { return (L.Pos <= R.Pos) && (L.Neg <= R.Neg); }

    /**
     * @brief Subset comparison.
     */
    friend constexpr bool operator < (BitLinesT L, BitLinesT R) noexcept { return (L <= R) && (L != R); }

    /**
     * @brief Swap contents of two BitLinesT.
     */
    friend constexpr void swap(BitLinesT& L, BitLinesT& R) noexcept { std::swap(L.Pos.bits, R.Pos.bits);  std::swap(L.Neg.bits, R.Neg.bits); }

    /**
     * @brief Swap contents of two BitLinesT (rvalue overload).
     */
    friend constexpr void swap(BitLinesT& L, BitLinesT&& R) noexcept { std::swap(L.Pos.bits, R.Pos.bits);  std::swap(L.Neg.bits, R.Neg.bits); }

    /**
     * @brief Bitwise AND assignment.
     */
    constexpr BitLinesT& operator &= (BitLinesT R) noexcept { Pos &= R.Pos; Neg &= R.Neg; return *this; }

    /**
     * @brief Bitwise OR assignment.
     */
    constexpr BitLinesT& operator |= (BitLinesT R) noexcept { Pos |= R.Pos; Neg |= R.Neg; return *this; }

    /**
     * @brief Bitwise XOR assignment.
     */
    constexpr BitLinesT& operator ^= (BitLinesT R) noexcept { Pos ^= R.Pos; Neg ^= R.Neg; return *this; }

    /**
     * @brief Bitwise AND with NOT assignment.
     */
    constexpr BitLinesT& operator %= (BitLinesT R) noexcept { Pos %= R.Pos; Neg %= R.Neg; return *this; }

    // --- Geometric Ray Operations ---

    /**
     * @brief Get bits representing lines (both positive and negative).
     */
    constexpr value_type lines() const noexcept { return both(); }

    /**
     * @brief Get bits representing positive rays.
     */
    constexpr value_type posRays() const noexcept { return onlypos(); }

    /**
     * @brief Get bits representing negative rays.
     */
    constexpr value_type negRays() const noexcept { return onlyneg(); }

    /**
     * @brief Get bits representing vertices (neither positive nor negative).
     */
    constexpr value_type vertexes() const noexcept { return neither(); }

    /**
     * @brief Query line bit at index.
     */
    constexpr auto line(INDEX_T Index) const noexcept { return lines().get(Index); }

    /**
     * @brief Query positive ray bit at index.
     */
    constexpr auto posRay(INDEX_T Index) const noexcept { return posRays().get(Index); }

    /**
     * @brief Query negative ray bit at index.
     */
    constexpr auto negRay(INDEX_T Index) const noexcept { return negRays().get(Index); }

    /**
     * @brief Query vertex bit at index.
     */
    constexpr auto vertex(INDEX_T Index) const noexcept { return vertexes().get(Index); }

    /**
     * @brief Set line bit at index.
     */
    constexpr void set_line(INDEX_T Index) noexcept { set_both(Index); }

    /**
     * @brief Set positive ray bit at index.
     */
    constexpr void set_posRay(INDEX_T Index) noexcept { set_onlypos(Index); }

    /**
     * @brief Set negative ray bit at index.
     */
    constexpr void set_negRay(INDEX_T Index) noexcept { set_onlyneg(Index); }

    /**
     * @brief Set vertex bit at index.
     */
    constexpr void set_vertex(INDEX_T Index) noexcept { clear_both(Index); }

    // --- Halfspace Operations ---

    /**
     * @brief Get bits representing zero (neither positive nor negative).
     */
    constexpr value_type zero() const noexcept { return neither(); }

    /**
     * @brief Get bits representing nonzero (either positive or negative).
     */
    constexpr value_type nonzero() const noexcept { return either(); }

    /**
     * @brief Get bits representing bounded above (only positive).
     */
    constexpr value_type bnddAbove() const noexcept { return onlypos(); }

    /**
     * @brief Get bits representing bounded below (only negative).
     */
    constexpr value_type bnddBelow() const noexcept { return onlyneg(); }

    /**
     * @brief Get bits representing undefined (both positive and negative).
     */
    constexpr value_type undefined() const noexcept { return both(); }

    /**
     * @brief Query zero bit at index.
     */
    constexpr auto zero(INDEX_T Index) const noexcept { return zero().get(Index); }

    /**
     * @brief Query nonzero bit at index.
     */
    constexpr auto nonzero(INDEX_T Index) const noexcept { return nonzero().get(Index); }

    /**
     * @brief Query bounded above bit at index.
     */
    constexpr auto bnddAbove(INDEX_T Index) const noexcept { return bnddAbove().get(Index); }

    /**
     * @brief Query bounded below bit at index.
     */
    constexpr auto bnddBelow(INDEX_T Index) const noexcept { return bnddBelow().get(Index); }

    /**
     * @brief Query undefined bit at index.
     */
    constexpr auto undefined(INDEX_T Index) const noexcept { return undefined().get(Index); }

    /**
     * @brief Set zero bit at index.
     */
    constexpr void set_zero(INDEX_T Index) noexcept { return clear_both(Index); }

    /**
     * @brief Set bounded above bit at index.
     */
    constexpr void set_bnddAbove(INDEX_T Index) noexcept { return set_onlypos(Index); }

    /**
     * @brief Set bounded below bit at index.
     */
    constexpr void set_bnddBelow(INDEX_T Index) noexcept { return set_onlyneg(Index); }

    /**
     * @brief Set undefined bit at index.
     */
    constexpr void set_undefined(INDEX_T Index) noexcept { return set_both(Index); }

    // --- String Conversion ---

    /**
     * @brief Convert both bitsets to formatted string.
     * @param NumBits Number of bits to display.
     * @return String representation.
     */
    std::string string(std::integral auto NumBits = 32) const noexcept {
        return std::format("P[{:0>{}b}] N[{:0>{}b}]", Pos.bits, NumBits, Neg.bits, NumBits);
    }
};

/**
 * @brief Custom formatter for BitsT<T> for std::format.
 *
 * Allows formatting BitsT<T> with a specified number of bits.
 */
template <std::integral T>
struct std::formatter<BitsT<T>> : std::formatter<int> {
    int argid{ -1 };      ///< Argument id for dynamic bit count.
    size_type NumBits{ 0 }; ///< Number of bits to display.

    /**
     * @brief Parse format specifier for BitsT<T>.
     */
    constexpr auto parse(std::format_parse_context& ctx) {

        auto it{ ctx.begin() };
        if (it == ctx.end()) { return it; }

        char c = *it;
        if (c == '{') {
            NumBits = -1;
            argid = static_cast<int>(ctx.next_arg_id());
            ctx.advance_to(it + 2);
        }
        else if (c > '0' && c <= '9') {
            NumBits = c - '0';
            ++it;
            if (it != ctx.end()) {
                c = *it;
                if (c >= '0' && c <= '9') {
                    NumBits = NumBits * 10 + (c - '0');
                    ++it;
                }
            }
            if (it != ctx.end() && *it != '}') { throw std::format_error("Invalid format args for graph_state_t"); }
            ctx.advance_to(it);
        }
        return std::formatter<int>::parse(ctx);
        return it;
    }

    /**
     * @brief Get bit count for formatting.
     */
    int get_count(std::format_context& ctx) const {
        if (NumBits >= 0) { return NumBits; }
        return std::visit_format_arg([&](auto&& v) -> int {
            if constexpr (std::is_integral_v<std::decay_t<decltype(v)>>) { return static_cast<int>(v); }
            else throw std::format_error("Invalid format args for graph_state_t");
            }, ctx.arg(argid));
    }

    /**
     * @brief Format BitsT<T> to string.
     */
    auto format(BitsT<T> const& B, std::format_context& ctx) const {
        int Count{ get_count(ctx) };
        return std::format_to(ctx.out(), "{}", B.string(Count));

        if (NumBits >= 0) {
            return std::format_to(ctx.out(), "{}", B.string(NumBits));
        }
        int Count2 = std::visit_format_arg([&](auto&& v) -> int {
            if constexpr (std::is_integral_v<std::decay_t<decltype(v)>>) { return static_cast<int>(v); }
            else throw std::format_error("Invalid format args for graph_state_t");
            }, ctx.arg(argid));
        return std::format_to(ctx.out(), "{}", B.string(Count2));
    }
};

// --- Type Aliases for Common Bit Widths ---

using B8 = BitsT<uint8_t>;      ///< 8-bit bitset
using B16 = BitsT<uint16_t>;    ///< 16-bit bitset
using B32 = BitsT<uint32_t>;    ///< 32-bit bitset
using B64 = BitsT<uint64_t>;    ///< 64-bit bitset

using Lines8 = BitLinesT<uint8_t>;      ///< 8-bit dual bitset
using Lines16 = BitLinesT<uint16_t>;    ///< 16-bit dual bitset
using Lines32 = BitLinesT<uint32_t>;    ///< 32-bit dual bitset
using Lines64 = BitLinesT<uint64_t>;    ///< 64-bit dual bitset

#endif