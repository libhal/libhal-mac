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

#include <libhal-mac/console.hpp>

#include <chrono>
#include <cstdio>

#include <libhal/error.hpp>

namespace hal::mac::inline v1 {

hal::v5::strong_ptr<console_serial> console_serial::create(
  std::pmr::polymorphic_allocator<> p_allocator,
  hal::usize p_buffer_size)
{
  if (p_buffer_size < 32) {
    p_buffer_size = 32;
  }

  return hal::v5::make_strong_ptr<console_serial>(
    p_allocator, p_allocator, p_buffer_size);
}

console_serial::console_serial(hal::v5::strong_ptr_only_token,
                               std::pmr::polymorphic_allocator<> p_allocator,
                               hal::usize p_buffer_size)
  : m_allocator(p_allocator)
  , m_receive_buffer(p_buffer_size, hal::byte{ 0 }, p_allocator)
  , m_receive_thread(&console_serial::receive_thread_function, this)
{
}

console_serial::~console_serial()
{
  m_stop_thread.store(true, std::memory_order_release);
  if (m_receive_thread.joinable()) {
    m_receive_thread.join();
  }
}

void console_serial::driver_configure(
  hal::v5::serial::settings const& p_settings)
{
  // Console doesn't need configuration - settings are ignored
  static_cast<void>(p_settings);
}

void console_serial::driver_write(std::span<hal::byte const> p_data)
{
  // Use fwrite to stdout for binary safety
  std::fwrite(p_data.data(), 1, p_data.size(), stdout);
  std::fflush(stdout);
}

std::span<hal::byte const> console_serial::driver_receive_buffer()
{
  return m_receive_buffer;
}

hal::usize console_serial::driver_cursor()
{
  return m_receive_cursor.load(std::memory_order_acquire);
}

void console_serial::receive_thread_function()
{
  while (!m_stop_thread.load(std::memory_order_acquire)) {
    int ch = std::getchar();

    if (ch != EOF) {
      auto current_cursor = m_receive_cursor.load(std::memory_order_acquire);
      m_receive_buffer[current_cursor] = static_cast<hal::byte>(ch);

      auto new_cursor = (current_cursor + 1) % m_receive_buffer.size();
      m_receive_cursor.store(new_cursor, std::memory_order_release);
    } else {
      // No data available, sleep briefly to avoid busy waiting
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

}  // namespace hal::mac::inline v1
