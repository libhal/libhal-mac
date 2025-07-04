// Copyright 2024 - 2025 Khalil Estell and the libhal contributors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <chrono>
#include <memory_resource>

#include <libhal/pointers.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/units.hpp>

namespace hal::mac::inline v1 {
/**
 * @brief Steady clock implementation using std::chrono::steady_clock for v5
 * interface
 *
 * Provides a high-resolution, monotonic time source for embedded applications
 * running on macOS. The clock is guaranteed to never go backwards, making it
 * suitable for measuring time intervals, implementing timeouts, and scheduling
 * periodic tasks.
 *
 * The uptime is measured from the time of object construction, and the
 * frequency is derived from std::chrono::steady_clock's period.
 */
class steady_clock
  : public hal::v5::steady_clock
  , public hal::v5::enable_strong_from_this<steady_clock>
{
public:
  /**
   * @brief Create a steady clock instance
   *
   * @param p_allocator Memory allocator (unused but follows libhal patterns)
   * @return A strong_ptr to the created steady_clock instance
   */
  [[nodiscard]] static hal::v5::strong_ptr<steady_clock> create(
    std::pmr::polymorphic_allocator<> p_allocator);

  /**
   * @brief Public constructor - but use create() instead
   */
  steady_clock(hal::v5::strong_ptr_only_token);

private:
  hal::v5::hertz driver_frequency() override;
  hal::u64 driver_uptime() override;

  /// Reference time point from construction for uptime calculations
  std::chrono::steady_clock::time_point m_start_time;
};

/**
 * @brief Steady clock implementation using std::chrono::steady_clock for legacy
 * interface
 *
 * Provides the same functionality as steady_clock but implements the
 * legacy hal::steady_clock interface for backward compatibility with older
 * libhal code that hasn't been updated to the v5 API.
 */
class legacy_steady_clock
  : public hal::steady_clock
  , public hal::v5::enable_strong_from_this<legacy_steady_clock>
{
public:
  /**
   * @brief Create a legacy steady clock instance
   *
   * @param p_allocator Memory allocator (unused but follows libhal patterns)
   * @return A strong_ptr to the created legacy_steady_clock instance
   */
  [[nodiscard]] static hal::v5::strong_ptr<legacy_steady_clock> create(
    std::pmr::polymorphic_allocator<> p_allocator);

  /**
   * @brief Public constructor - but use create() instead
   */
  legacy_steady_clock(hal::v5::strong_ptr_only_token);

private:
  hal::hertz driver_frequency() override;
  hal::u64 driver_uptime() override;

  /// Reference time point from construction for uptime calculations
  std::chrono::steady_clock::time_point m_start_time;
};
}  // namespace hal::mac::inline v1
