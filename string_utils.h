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

#ifndef MZLIB_STRING_UTILS_HEADER_FILE
#define MZLIB_STRING_UTILS_HEADER_FILE
#pragma once

#include <format>
#include <iostream>
#include "concept_utils.h"


/**
 * @file string_utils.h
 * @brief Common formatting and sequence utilities for the mz library.
 *
 * This header provides formatting helpers for sequences and basic statistics
 * functions such as counting and summing elements in containers.
 * All functions and types use modern C++20 style and naming conventions.
 */



 /**
  * @brief Macro for formatting into a buffer using std::format_to.
  * @param bf Buffer to insert formatted output.
  * @param ... Format string and arguments.
  * Usage: FORMAT_TO(buffer, "Value: {}", value);
  */
#define FORMAT_TO(bf, ...) std::format_to(std::back_inserter(bf), __VA_ARGS__)

  /**
   * @brief Macro for formatting into a buffer using std::vformat_to.
   * @param bf Buffer to insert formatted output.
   * @param Fmt Format string.
   * @param ... Arguments for formatting.
   * Usage: VFORMAT_TO(buffer, fmt, arg1, arg2);
   */
#define VFORMAT_TO(bf, Fmt, ...) std::vformat_to(std::back_inserter(bf), Fmt, std::make_format_args(__VA_ARGS__))














namespace mz {


    /**
     * @brief Prints a string_view to stdout.
     * @param sv The string_view to print.
     * Usage: mz::print("Hello\n");
     */
    inline void print(std::string_view sv) noexcept { fwrite(sv.data(), 1, sv.size(), stdout); }

    /**
     * @brief Prints a formatted string to stdout.
     * @tparam TArgs Types of format arguments.
     * @param fmt Format string.
     * @param args Arguments for formatting.
     * Usage: mz::print("Value: {}", 42);
     */
    template <typename... TArgs>
    inline void print(std::format_string<TArgs...> fmt, TArgs... args) noexcept {
        print(std::vformat(fmt.get(), std::make_format_args(args...)));
    }








    /**
     * @brief Helper struct for parsing and storing a basic format string (up to 15 chars).
     *
     * Used for custom formatting scenarios. The format string is parsed and stored in FMT.
     * Throws std::format_error if the format string is invalid or too long.
     */
    struct basic_formatter16 {
        uint8_t count{ 2 };         ///< Number of format characters stored.
        char FMT[15]{ '{',':' }; ///< Format string buffer.

        /**
         * @brief Parses a format string from the context and stores it in format[].
         * @param ctx Format context (must support begin()/end()).
         * @return Iterator to the end of the parsed format string.
         * @throws std::format_error if the format string is invalid or too long.
         */
        constexpr auto parse(auto& ctx) {
            auto it = ctx.begin();
            while (it != ctx.end() && *it != '}' && count < 15) {
                FMT[count++] = *it++;
            }
            if (count >= 15)
                throw std::format_error("Invalid format args: too long.");

            if (it != ctx.end() && *it != '}')
                throw std::format_error("Invalid format args for QuotableString.");
            FMT[count++] = '}';
            return it;
        }
    };




    template <typename BufferT, Sequence Seq>
    void format_sequence_to(
        BufferT& buffer,
        Seq const& generator,
        std::string_view fmt = "{}",
        std::string_view separator = ",",
        std::string_view enclosure = "[]")
    {
        auto Size{ generator.size() };

        if (enclosure.size() > 1) {
            FORMAT_TO(buffer, "{}", enclosure[0]);
        }
        if (Size > 0) {
            std::vformat_to(std::back_inserter(buffer), fmt, std::make_format_args(generator[0]));
        }
        for (auto i = 1; i < Size; ++i) {
            FORMAT_TO(buffer, "{}", separator);
            std::vformat_to(std::back_inserter(buffer), fmt, std::make_format_args(generator[i]));
        }
        if (enclosure.size() > 1) {
            FORMAT_TO(buffer, "{}", enclosure[1]);
        }
    }






    /**
     * @brief Formats a sequence using a generator function and writes to a buffer.
     *
     * @tparam BufferT Buffer type (e.g., std::string).
     * @tparam FuncT   Generator function type (returns value to format).
     * @param buffer   Output buffer.
     * @param generator Function that takes an index and returns a value to format.
     * @param size     Number of elements in the sequence.
     * @param fmt      Format string for each element.
     * @param separator Separator string between elements.
     * @param enclosure Enclosure string (e.g., "[]").
     *
     * Usage:
     *   std::string buf;
     *   mz::format_sequence_to(buf, [](int i){ return arr[i]; }, arr.size());
     */
    template <typename BufferT, typename FuncT>
    void format_generator_to(
        BufferT& buffer,
        FuncT&& generator,
        std::integral auto size,
        std::string_view fmt = "{}",
        std::string_view separator = ",",
        std::string_view enclosure = "[]")
    {
        if (enclosure.size() > 1) {
            FORMAT_TO(buffer, "{}", enclosure[0]);
        }
        if (size > 0) {
            std::vformat_to(std::back_inserter(buffer), fmt, std::make_format_args(generator(0)));
        }
        for (auto i = 1; i < size; ++i) {
            FORMAT_TO(buffer, "{}", separator);
            std::vformat_to(std::back_inserter(buffer), fmt, std::make_format_args(generator(i)));
        }
        if (enclosure.size() > 1) {
            FORMAT_TO(buffer, "{}", enclosure[1]);
        }
    }

    /**
     * @brief Formats a sequence of strings using a generator function and writes to a buffer.
     *
     * @tparam BufferT Buffer type (e.g., std::string).
     * @tparam FuncT   Generator function type (returns std::string).
     * @param buffer   Output buffer.
     * @param generator Function that takes an index and returns a std::string.
     * @param size     Number of elements in the sequence.
     * @param separator Separator string between elements.
     * @param enclosure Enclosure string (e.g., "[]").
     *
     * Usage:
     *   std::string buf;
     *   mz::format_string_generator_to(buf, [](int i){ return arr[i].to_string(); }, arr.size());
     */
    template <typename BufferT, typename FuncT>
        requires requires (int i, FuncT&& generator) { { generator(i) } -> std::same_as<std::string>; }
    void format_string_generator_to(
        BufferT& buffer,
        FuncT&& generator,
        std::integral auto size,
        std::string_view separator = ",",
        std::string_view enclosure = "[]")
    {
        if (enclosure.size() > 1) {
            FORMAT_TO(buffer, "{}", enclosure[0]);
        }
        if (size > 0) {
            FORMAT_TO(buffer, "{}", generator(0));
        }
        for (auto i = 1; i < size; ++i) {
            FORMAT_TO(buffer, "{}{}", separator, generator(i));
        }
        if (enclosure.size() > 1) {
            FORMAT_TO(buffer, "{}", enclosure[1]);
        }
    }

    /**
     * @brief Formats a sequence and returns the result as a string.
     *
     * @tparam FuncT Generator function type.
     * @param generator Function that takes an index and returns a value to format.
     * @param size     Number of elements in the sequence.
     * @param fmt      Format string for each element.
     * @param separator Separator string between elements.
     * @param enclosure Enclosure string (e.g., "[]").
     * @return Formatted string.
     *
     * Usage:
     *   std::string result = mz::format([](int i){ return arr[i]; }, arr.size());
    template <typename FuncT>
    std::string format(
        FuncT&& generator,
        size_t size,
        std::string_view fmt = "{}",
        std::string_view separator = ",",
        std::string_view enclosure = "[]")
    {
        std::string buffer;
        format_generator_to(buffer, std::forward<FuncT>(generator), size, fmt, separator, enclosure);
        return buffer;
    }
     */



} // namespace mz

#endif // MZLIB_GLOBALS_HEADER_FILE