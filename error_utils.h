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

#ifndef MZ_ERROR_UTILS_HEADER_FILE
#define MZ_ERROR_UTILS_HEADER_FILE
#pragma once
#include <format>
#include <stdexcept>
#include <exception>
#include <conio.h>
#include <stdio.h>
#include "string_utils.h"






namespace mz
{
	/**
	 * @brief Reads a single character from the console without echo.
	 * @return The character read as an int.
	 * Usage: int c = mz::getch();
	 */
	inline int getch() noexcept { return _getch(); }




    /**
     * @brief Prints a generic logic error message to stdout.
     * Usage: mz::logic_error_message();
     */
    inline void logic_error_message() noexcept { print("LogicError!"); }

    /**
     * @brief Prints a generic domain error message to stdout.
     * Usage: mz::domain_error_message();
     */
    inline void domain_error_message() noexcept { print("DomainError!"); }

    /**
     * @brief Prints a generic invalid argument error message to stdout.
     * Usage: mz::invalid_argument_message();
     */
    inline void invalid_argument_message() noexcept { print("InvalidArgumentError!"); }

    /**
     * @brief Prints a formatted logic error message to stdout.
     * @tparam TArgs Types of format arguments.
     * @param fmt Format string.
     * @param args Arguments for formatting.
     * Usage: mz::logic_error_message("Failed at index {}", idx);
     */
    template <typename... TArgs>
    inline void logic_error_message(std::format_string<TArgs...> fmt, TArgs... args) noexcept {
        //print("LogicError: ");
        //print(fmt, std::forward<TArgs>(args)...);
        print("LogicError: {}", std::vformat(fmt.get(), std::make_format_args(args...)));
    }

    /**
     * @brief Prints a formatted domain error message to stdout.
     * @tparam TArgs Types of format arguments.
     * @param fmt Format string.
     * @param args Arguments for formatting.
     * Usage: mz::domain_error_message("Domain out of range: {}", val);
     */
    template <typename... TArgs>
    inline void domain_error_message(std::format_string<TArgs...> fmt, TArgs... args) noexcept {
        //print("DomainError: ");
        //print(fmt, std::forward<TArgs>(args)...);
        print("DomainError: {}", std::vformat(fmt.get(), std::make_format_args(args...)));
    }

    /**
     * @brief Prints a formatted invalid argument error message to stdout.
     * @tparam TArgs Types of format arguments.
     * @param fmt Format string.
     * @param args Arguments for formatting.
     * Usage: mz::invalid_argument_message("Invalid value: {}", val);
     */
    template <typename... TArgs>
    inline void invalid_argument_message(std::format_string<TArgs...> fmt, TArgs... args) noexcept {
        //print("InvalidArgumentError: ");
        //print(fmt, std::forward<TArgs>(args)...);
        print("InvalidArgumentError: {}", std::vformat(fmt.get(), std::make_format_args(args...)));
    }
}

/**
 * @brief Throws std::logic_error if condition is true.
 * @param Cond Condition to check.
 * Usage: THROW_IF(x < 0);
 */
#define THROW_IF(Cond)  if (Cond) { throw std::logic_error(""); }

 /**
  * @brief Asserts condition, prints logic error message and throws if false.
  * @param Cond Condition to check.
  * @param ... Format string and arguments for error message.
  * Usage: ASSERT_IF(ptr != nullptr, "Null pointer at {}", idx);
  */
#define ASSERT_IF(Cond, ...)  if (!(Cond)) { mz::logic_error_message(__VA_ARGS__); throw std::logic_error(""); }

  /**
   * @brief Throws std::logic_error if error condition is true, with formatted message.
   * @param Error Error condition.
   * @param ... Format string and arguments for error message.
   * Usage: LOGIC_ERROR_IF(failed, "Failed at {}", idx);
   */
#define LOGIC_ERROR_IF(Error, ...)  if (Error) { mz::logic_error_message(__VA_ARGS__); throw std::logic_error(""); }

   /**
    * @brief Throws std::domain_error if error condition is true, with formatted message.
    * @param Error Error condition.
    * @param ... Format string and arguments for error message.
    * Usage: DOMAIN_ERROR_IF(out_of_range, "Value: {}", val);
    */
#define DOMAIN_ERROR_IF(Error, ...)  if (Error) { mz::domain_error_message(__VA_ARGS__); throw std::domain_error(""); }

    /**
     * @brief Throws std::invalid_argument if error condition is true, with formatted message.
     * @param Error Error condition.
     * @param ... Format string and arguments for error message.
     * Usage: INVALID_ARGUMENT_IF(bad_arg, "Bad argument: {}", arg);
     */
#define INVALID_ARGUMENT_IF(Error, ...)  if (Error) { mz::invalid_argument_message(__VA_ARGS__); throw std::invalid_argument(""); }

     /**
      * @brief Debug macro: throws std::logic_error if condition is true, with formatted message.
      * @param Cond Condition to check.
      * @param ... Format string and arguments for error message.
      * Usage: DEBUG_THROW(x < 0, "Negative value: {}", x);
      */
#define DEBUG_THROW(Cond, ...)  if (Cond) { mz::logic_error_message(__VA_ARGS__); throw std::logic_error(""); }

      /**
       * @brief Debug macro: asserts condition, throws std::logic_error if false, with formatted message.
       * @param Cond Condition to check.
       * @param ... Format string and arguments for error message.
       * Usage: DEBUG_ASSERT(ptr != nullptr, "Null pointer");
       */
#define DEBUG_ASSERT(Cond, ...)  if (!(Cond)) { mz::logic_error_message(__VA_ARGS__); throw std::logic_error(""); }










#endif
