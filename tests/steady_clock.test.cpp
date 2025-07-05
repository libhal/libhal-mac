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

#include <chrono>
#include <memory_resource>
#include <thread>

#include <libhal-mac/steady_clock.hpp>

#include <boost/ut.hpp>

namespace hal::mac {
boost::ut::suite<"test_steady_clock"> test_steady_clock = [] {
  using namespace boost::ut;

  "steady_clock::create()"_test = []() {
    // Exercise
    auto clock =
      hal::mac::steady_clock::create(std::pmr::new_delete_resource());

    expect(that % clock);
  };

  "steady_clock::frequency()"_test = []() {
    // Setup
    auto clock =
      hal::mac::steady_clock::create(std::pmr::new_delete_resource());

    // Exercise
    auto freq = clock->frequency();

    // Verify - frequency should be positive
    expect(that % freq > 0);
  };

  "steady_clock::uptime_increases()"_test = []() {
    // Setup
    auto clock =
      hal::mac::steady_clock::create(std::pmr::new_delete_resource());

    // Exercise
    auto uptime1 = clock->uptime();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto uptime2 = clock->uptime();

    // Verify - uptime should increase
    expect(that % uptime2 > uptime1);
  };

  "legacy_steady_clock::create()"_test = []() {
    // Exercise
    auto clock =
      hal::mac::legacy_steady_clock::create(std::pmr::new_delete_resource());

    expect(that % clock);
  };

  "legacy_steady_clock::frequency()"_test = []() {
    // Setup
    auto clock =
      hal::mac::legacy_steady_clock::create(std::pmr::new_delete_resource());

    // Exercise
    auto freq = clock->frequency();

    // Verify - frequency should be positive
    expect(that % freq > 0);
  };

  "legacy_steady_clock::uptime_increases()"_test = []() {
    // Setup
    auto clock =
      hal::mac::legacy_steady_clock::create(std::pmr::new_delete_resource());

    // Exercise
    auto uptime1 = clock->uptime();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto uptime2 = clock->uptime();

    // Verify - uptime should increase
    expect(that % uptime2 > uptime1);
  };
};
}  // namespace hal::mac
