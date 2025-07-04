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

#include <atomic>
#include <memory_resource>
#include <span>
#include <thread>
#include <vector>

#include <libhal/pointers.hpp>
#include <libhal/serial.hpp>
#include <libhal/units.hpp>

namespace hal::mac::inline v1 {
/**
 * @brief Serial communication interface using macOS console (stdin/stdout)
 *
 * Provides a way to simulate serial communication on macOS by redirecting
 * serial I/O operations to the console. This is particularly useful for
 * testing serial-based communication protocols and debugging embedded
 * applications that rely on serial interfaces.
 *
 * The implementation uses a background thread to continuously read from stdin
 * and store data in a circular buffer, while write operations are sent
 * directly to stdout.
 */
class console_serial : public hal::v5::serial
{
public:
  /**
   * @brief Create a console serial instance
   *
   * @param p_allocator Memory allocator for internal buffer management
   * @param p_buffer_size Size of the internal circular receive buffer (min: 32)
   * @return A strong_ptr to the created console_serial instance
   */
  [[nodiscard]] static hal::v5::strong_ptr<console_serial> create(
    std::pmr::polymorphic_allocator<> p_allocator,
    hal::usize p_buffer_size);

  /**
   * @brief Public constructor - but use create() instead
   */
  console_serial(hal::v5::strong_ptr_only_token,
                 std::pmr::polymorphic_allocator<> p_allocator,
                 hal::usize p_buffer_size);

  /**
   * @brief Destroy the console serial object
   *
   * Stops the background receive thread and waits for it to complete before
   * destruction.
   */
  ~console_serial() override;

  // Non-copyable and non-movable
  console_serial(console_serial const&) = delete;
  console_serial& operator=(console_serial const&) = delete;
  console_serial(console_serial&&) = delete;
  console_serial& operator=(console_serial&&) = delete;

private:
  void driver_configure(hal::v5::serial::settings const& p_settings) override;
  void driver_write(std::span<hal::byte const> p_data) override;
  std::span<hal::byte const> driver_receive_buffer() override;
  hal::usize driver_cursor() override;

  /**
   * @brief Background thread function for reading from stdin
   */
  void receive_thread_function();

  /// Memory allocator for buffer management
  std::pmr::polymorphic_allocator<> m_allocator;
  /// Circular buffer for storing received data from stdin
  std::pmr::vector<hal::byte> m_receive_buffer;
  /// Atomic cursor position for thread-safe buffer access
  std::atomic<hal::usize> m_receive_cursor{ 0 };
  /// Atomic flag to signal thread termination
  std::atomic<bool> m_stop_thread{ false };
  /// Background thread for reading from stdin
  std::thread m_receive_thread;
};
}  // namespace hal::mac::inline v1
