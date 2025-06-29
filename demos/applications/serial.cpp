#include <libhal/error.hpp>
#include <thread>
#include <unistd.h>

#include <memory_resource>
#include <print>
#include <span>
#include <string_view>

#include <libhal-mac/serial.hpp>
#include <libhal-util/as_bytes.hpp>
#include <libhal-util/serial.hpp>

void application()
{
  using namespace std::chrono_literals;

  std::println("Running hal::mac::serial application...");
  constexpr auto usb_serial_path = "/dev/tty.usbserial-59760081941";
  hal::v5::optional_ptr<hal::mac::serial> serial;
  try {
    serial = hal::mac::serial::create(std::pmr::new_delete_resource(),
                                      usb_serial_path,
                                      1024,
                                      { .baud_rate = 115200 });

  } catch (hal::no_such_device const&) {
    std::println("The usb serial path {} was not found!", usb_serial_path);
  }

  // USB serial devices reset sequence:

  // Assert DTR and RTS
  serial->set_control_signals(true, true);
  std::this_thread::sleep_for(10ms);
  // De-activate RTS (boot) line
  serial->set_rts(false);
  std::this_thread::sleep_for(10ms);
  // De-activate DTR (reset) line to reset device
  serial->set_dtr(false);
  std::this_thread::sleep_for(10ms);

  auto const received_buffer = serial->receive_buffer();
  auto previous_cursor = serial->receive_cursor();

  while (true) {
    constexpr std::string_view test_str = "Hello from libhal-mac!\n";
    serial->write(hal::as_bytes(test_str));

    std::this_thread::sleep_for(1s);

    auto const cursor = serial->receive_cursor();

    if (cursor == previous_cursor) {
      std::println("Nothing to read...");
      continue;
    }

    std::println("Received: ");

    if (cursor < previous_cursor) {
      auto const delta = received_buffer.size() - previous_cursor;
      for (auto i = 0; i < delta; i++) {
        std::print("{}",
                   static_cast<char>(received_buffer[previous_cursor + i]));
      }
      previous_cursor = 0;
    }

    auto const delta = cursor - previous_cursor;
    for (auto i = 0; i < delta; i++) {
      std::print("{}", static_cast<char>(received_buffer[previous_cursor + i]));
    }

    previous_cursor = cursor;

    std::this_thread::sleep_for(1s);
  }
}
