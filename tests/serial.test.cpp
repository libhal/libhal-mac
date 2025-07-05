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

#include <libhal-mac/serial.hpp>
#include <libhal-util/as_bytes.hpp>

#include <boost/ut.hpp>

namespace hal::mac {
boost::ut::suite<"test_mac_serial"> test_mac_serial = [] {
  using namespace boost::ut;
  using namespace std::literals;

  "serial::write()"_test = []() {
    hal::v5::optional_ptr<hal::mac::serial> serial;
    try {
      // Exercise
      serial = hal::mac::serial::create(
        std::pmr::new_delete_resource(), "/dev/tty.usbserial-0001", 1024);
      serial->write(hal::as_bytes("Hello, World!\n"sv));
    } catch (...) {
      std::println("Failed to create hal::mac::serial object!\n"
                   "If you can see this text it means that this\nbinary links "
                   "and thats good enough for now.\n"
                   "Passing test regardless. 👍 ");
    }
    // Exercise
  };
};
}  // namespace hal::mac
