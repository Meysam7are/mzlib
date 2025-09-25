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

#ifndef MZ_TIMER_UTILS_HEADER_FILE
#define MZ_TIMER_UTILS_HEADER_FILE
#pragma once

#include <time.h>
#include <iostream>
#include <format>

namespace mz {

    /**
     * @brief Generates a seed value based on the current nanoseconds.
     *
     * This function retrieves the current time in nanoseconds and returns it as a seed value.
     * It is useful for initializing random number generators or other time-dependent operations.
     *
     * @return long The current time in nanoseconds.
     */
    inline long nanoseed() {
        timespec ts;
        if (timespec_get(&ts, TIME_UTC) == 0) {
            throw std::runtime_error("Failed to retrieve the current time using timespec_get.");
        }
        return ts.tv_nsec;
    }

    /**
 * @class Timer
 * @brief A utility class for measuring time durations.
 *
 * This class provides functionality to measure elapsed time between events
 * and accumulate total durations. It supports high-resolution timing and
 * provides formatted string representations of the measured durations.
 */
    class Timer {
        // Constants for time conversions
        static constexpr int64_t kNanosecondsPerMillisecond = 1000000ll; // 1 million (for milliseconds)
        static constexpr int64_t kNanosecondsPerSecond = 1000000000ll; // 1 billion (for seconds)

        // Member variables to store timing information
        int64_t last_interval_ns = 0;    // Duration of the last measured interval (in nanoseconds)
        int64_t total_elapsed_ns = 0;   // Accumulated total duration (in nanoseconds)
        int64_t last_timestamp_ns = 0;   // Timestamp of the last measurement (in nanoseconds)

    public:
        /**
         * @brief Default constructor.
         * Initializes the timer and sets the initial timestamp.
         * @throws std::runtime_error If `timespec_get` fails.
         */
        Timer() : last_interval_ns(0), total_elapsed_ns(0), last_timestamp_ns(now()) {}

        /**
         * @brief Gets the current time in nanoseconds.
         *
         * This function retrieves the current time using `timespec_get` and converts
         * it to nanoseconds for high-resolution timing.
         *
         * @return int64_t The current time in nanoseconds.
         * @throws std::runtime_error If `timespec_get` fails.
         */
        int64_t now() const {
            struct timespec ts;
            if (timespec_get(&ts, TIME_UTC) == 0) {
                throw std::runtime_error("Failed to retrieve the current time using timespec_get.");
            }
            return ts.tv_sec * kNanosecondsPerSecond + ts.tv_nsec;
        }

        /**
         * @brief Gets the duration of the last measured interval in seconds.
         *
         * @return double The last duration in seconds.
         */
        constexpr double last_seconds() const noexcept {
            return static_cast<double>(last_interval_ns) / static_cast<double>(kNanosecondsPerSecond);
        }

        /**
         * @brief Gets the accumulated total duration in seconds.
         *
         * @return double The total duration in seconds.
         */
        constexpr double total_seconds() const noexcept {
            return static_cast<double>(total_elapsed_ns) / static_cast<double>(kNanosecondsPerSecond);
        }

        /**
         * @brief Starts a new timing interval.
         *
         * Resets the last duration and updates the timestamp to the current time.
         * @throws std::runtime_error If `now` fails.
         */
        void reset() {
            last_interval_ns = 0;
            last_timestamp_ns = now();
        }

        /**
         * @brief Stops the current timing interval and updates the durations.
         *
         * This function calculates the duration of the current interval, adds it
         * to the total duration, and returns the last duration in seconds.
         *
         * @return double The last duration in seconds.
         * @throws std::runtime_error If `now` fails.
         */
        double stamp() {
            int64_t NextTimeStamp = now(); // Get the current time
            last_interval_ns = NextTimeStamp - last_timestamp_ns; // Calculate the interval
            total_elapsed_ns += last_interval_ns; // Accumulate the total duration
            return last_seconds(); // Return the last duration in seconds
        }

        /**
         * @brief Gets a formatted string representation of the durations.
         *
         * The string includes the last duration and the total duration, formatted
         * to show seconds and milliseconds.
         *
         * @return std::string The formatted string representation.
         */
        std::string string() const noexcept {
            return std::format(
                "Last: {}.{:<3d} sec, Total: {}.{:<3d} sec",
                (last_interval_ns / kNanosecondsPerSecond), (last_interval_ns % kNanosecondsPerSecond) / kNanosecondsPerMillisecond,
                (total_elapsed_ns / kNanosecondsPerSecond), (total_elapsed_ns % kNanosecondsPerSecond) / kNanosecondsPerMillisecond
            );
        }

        /**
         * @brief Gets a formatted string representation and optionally stamps the duration.
         *
         * If `Stamp` is true, the current interval is stamped before generating the string.
         *
         * @param Stamp Whether to stamp the current interval.
         * @return std::string The formatted string representation.
         */
        std::string string(bool Stamp) noexcept {
            if (Stamp) {
                stamp();
            }
            return string();
        }
    };




} // namespace mz


/**
 * @brief Overloads the stream insertion operator for the Duration class.
 *
 * This operator stamps the current interval and outputs the formatted string
 * representation of the durations to the provided output stream.
 *
 * @param os The output stream.
 * @param d The Duration object.
 * @return std::ostream& The output stream.
 */
inline std::ostream& operator<<(std::ostream& os, mz::Timer& d) {
    d.stamp(); // Stamp the current interval
    return os << d.string(); // Output the formatted string
}

#endif // MZ_TIMER_UTILS_HEADER_FILE