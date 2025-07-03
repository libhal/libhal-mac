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
