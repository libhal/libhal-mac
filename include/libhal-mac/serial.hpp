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
#include <string_view>
#include <thread>
#include <vector>

#include <libhal/error.hpp>
#include <libhal/output_pin.hpp>
#include <libhal/pointers.hpp>
#include <libhal/serial.hpp>
#include <libhal/units.hpp>

namespace hal::mac::inline v1 {
/**
 * @brief Darwin (macOS) implementation of the serial interface
 *
 * This implementation provides access to USB serial ports and other serial
 * devices available through /dev/tty* and /dev/cu.* device files on macOS.
 *
 * The implementation uses a background thread to continuously read from the
 * serial port into a circular receive buffer, ensuring data is not lost due
 * to application processing delays.
 *
 * Example usage:
 * ```cpp
 * // Create serial port with 1KB receive buffer
 * auto serial_port = hal::mac::serial::create(
 *   allocator,
 *   "/dev/cu.usbserial-A50285BI",
 *   1024
 * );
 *
 * // Configure for 115200 baud, 8N1
 * serial_port->configure({
 *   .baud_rate = 115200,
 *   .stop = hal::v5::serial::settings::stop_bits::one,
 *   .parity = hal::v5::serial::settings::parity::none
 * });
 *
 * // Write data
 * std::string message = "Hello, World!\n";
 * serial_port->write(std::as_bytes(std::span(message)));
 *
 * // Read data using cursor-based API
 * auto old_cursor = serial_port->receive_cursor();
 * // ... wait for data ...
 * auto new_cursor = serial_port->receive_cursor();
 * // Process new data between old_cursor and new_cursor
 * ```
 */
class serial
  : public hal::v5::serial
  , public hal::v5::enable_strong_from_this<serial>
{
public:
  /**
   * @brief Create a serial instance
   *
   * @param p_allocator Memory allocator for the receive buffer
   * @param p_device_path Path to the serial device (e.g.,
   * "/dev/cu.usbserial-*")
   * @param p_buffer_size Size of the receive buffer in bytes (must be > 0)
   * @return A strong_ptr to the created serial instance
   * @throws hal::argument_out_of_domain if buffer_size is 0
   * @throws hal::no_such_device if the device path doesn't exist
   * @throws hal::operation_not_permitted if the device cannot be opened
   */
  [[nodiscard]] static hal::v5::strong_ptr<serial> create(
    std::pmr::polymorphic_allocator<> p_allocator,
    std::string_view p_device_path,
    usize p_buffer_size,
    hal::v5::serial::settings const& p_settings = {});

  /**
   * @brief Public constructor - but use create() instead
   */
  serial(hal::v5::strong_ptr_only_token,
         std::pmr::polymorphic_allocator<> p_allocator,
         std::string_view p_device_path,
         usize p_buffer_size,
         hal::v5::serial::settings const& p_settings);

  /**
   * @brief Destructor - stops the receive thread and closes the device
   */
  ~serial() override;

  // Non-copyable and non-movable
  serial(serial const&) = delete;
  serial& operator=(serial const&) = delete;
  serial(serial&&) = delete;
  serial& operator=(serial&&) = delete;

  /**
   * @brief Set the DTR (Data Terminal Ready) signal state
   *
<<<<<<< HEAD
   * @param p_state true to assert DTR (set high), false to deassert (set low)
=======
   * @param p_state true to assert DTR (set high), false to de-assert (set
low)
>>>>>>> 520fe79 (:tada: First commit w/ serial impl (#1))
   * @throws hal::operation_not_permitted if the operation fails
   */
  void set_dtr(bool p_state);

  /**
   * @brief Set the RTS (Request To Send) signal state
   *
<<<<<<< HEAD
   * @param p_state true to assert RTS (set high), false to deassert (set low)
=======
   * @param p_state true to assert RTS (set high), false to de-assert (set
low)
>>>>>>> 520fe79 (:tada: First commit w/ serial impl (#1))
   * @throws hal::operation_not_permitted if the operation fails
   */
  void set_rts(bool p_state);

  /**
   * @brief Get the current state of control signals
   *
   * @return A struct containing the current DTR and RTS states
   * @throws hal::operation_not_permitted if the operation fails
   */
  struct control_signals
  {
    bool dtr;
    bool rts;
    bool dsr;  // Data Set Ready (read-only)
    bool cts;  // Clear To Send (read-only)
  };

  [[nodiscard]] control_signals get_control_signals();

  /**
   * @brief Set both DTR and RTS in one operation
   *
   * @param p_dtr_state DTR state
   * @param p_rts_state RTS state
   * @throws hal::operation_not_permitted if the operation fails
   */
  void set_control_signals(bool p_dtr_state, bool p_rts_state);

private:
  /**
   * @brief Background thread function for reading data
   */
  void receive_thread_function();

  /**
   * @brief Convert libhal settings to termios configuration
   */
  void apply_termios_settings(hal::v5::serial::settings const& p_settings);

  // Implementation of serial interface
  void driver_configure(hal::v5::serial::settings const& p_settings) override;
  void driver_write(std::span<hal::byte const> p_data) override;
  std::span<hal::byte const> driver_receive_buffer() override;
  usize driver_cursor() override;

  std::pmr::vector<hal::byte> m_receive_buffer;
  int m_fd = -1;
  std::atomic<usize> m_receive_cursor{ 0 };
  std::atomic<bool> m_stop_thread{ false };
  std::thread m_receive_thread;
};

enum class modem_out
{
  dtr,
  rts,
};

enum class modem_in
{
  dsr,
  cts,
};

hal::v5::strong_ptr<hal::output_pin> acquire_output_pin(
  std::pmr::polymorphic_allocator<> p_allocator,
  hal::v5::strong_ptr<serial> p_manager,
  modem_out p_pin);
}  // namespace hal::mac::inline v1

namespace hal {
using hal::mac::v1::acquire_output_pin;
}
