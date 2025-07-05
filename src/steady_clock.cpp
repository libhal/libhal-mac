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

#include <libhal-mac/steady_clock.hpp>

namespace hal::mac::inline v1 {

hal::v5::strong_ptr<steady_clock> steady_clock::create(
  std::pmr::polymorphic_allocator<> p_allocator)
{
  return hal::v5::make_strong_ptr<steady_clock>(p_allocator);
}

steady_clock::steady_clock(hal::v5::strong_ptr_only_token)
  : m_start_time(std::chrono::steady_clock::now())
{
}

hal::v5::hertz steady_clock::driver_frequency()
{
  // std::chrono::steady_clock frequency is represented by its period
  using period = std::chrono::steady_clock::period;

  // Convert period (seconds per tick) to frequency (ticks per second)
  // frequency = 1 / period = period::den / period::num
  constexpr auto frequency_hz = period::den / period::num;

  return frequency_hz;
}

hal::u64 steady_clock::driver_uptime()
{
  auto now = std::chrono::steady_clock::now();
  auto duration = now - m_start_time;
  return duration.count();
}

hal::v5::strong_ptr<legacy_steady_clock> legacy_steady_clock::create(
  std::pmr::polymorphic_allocator<> p_allocator)
{
  return hal::v5::make_strong_ptr<legacy_steady_clock>(p_allocator);
}

legacy_steady_clock::legacy_steady_clock(hal::v5::strong_ptr_only_token)
  : m_start_time(std::chrono::steady_clock::now())
{
}

hal::hertz legacy_steady_clock::driver_frequency()
{
  // std::chrono::steady_clock frequency is represented by its period
  using period = std::chrono::steady_clock::period;

  // Convert period (seconds per tick) to frequency (ticks per second)
  // frequency = 1 / period = period::den / period::num
  constexpr auto frequency_hz = period::den / period::num;

  return static_cast<hal::hertz>(frequency_hz);
}

hal::u64 legacy_steady_clock::driver_uptime()
{
  auto const now = std::chrono::steady_clock::now();
  auto const duration = now - m_start_time;
  return duration.count();
}
}  // namespace hal::mac::inline v1
