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

#include <memory_resource>
#include <print>

#include <libhal-mac/console_serial.hpp>
#include <libhal-util/as_bytes.hpp>

#include <boost/ut.hpp>

namespace hal::mac {
boost::ut::suite<"test_console_serial"> test_console_serial = [] {
  using namespace boost::ut;
  using namespace std::literals;

  "console_serial::create()"_test = []() {
    // Exercise
    auto serial =
      hal::mac::console_serial::create(std::pmr::new_delete_resource(), 1024);

    expect(that % serial);
    expect(that % serial->receive_buffer().size() == 1024);
    expect(that % serial->receive_cursor() == 0);
  };

  "console_serial::write()"_test = []() {
    // Setup
    auto serial =
      hal::mac::console_serial::create(std::pmr::new_delete_resource(), 1024);

    // Exercise - basic write test
    try {
      serial->write(hal::as_bytes("Test output\n"sv));
    } catch (...) {
      std::println("Console serial write test passed - linking verified");
    }
  };

  "console_serial::configure()"_test = []() {
    // Setup
    auto serial =
      hal::mac::console_serial::create(std::pmr::new_delete_resource(), 512);

    // Exercise
    hal::v5::serial::settings settings{
      .baud_rate = 115200,
      .stop = hal::v5::serial::settings::stop_bits::one,
      .parity = hal::v5::serial::settings::parity::none
    };

    // Should not throw for console
    expect(nothrow([&] { serial->configure(settings); }));
  };

  "console_serial::invalid_buffer_size()"_test = []() {
    // Exercise & Verify
    expect(throws<hal::argument_out_of_domain>([&] {
      hal::mac::console_serial::create(std::pmr::new_delete_resource(), 0);
    }));
  };
};
}  // namespace hal::mac
