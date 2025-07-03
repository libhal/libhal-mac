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

#include <libhal-mac/serial.hpp>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <libhal/output_pin.hpp>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <libhal/error.hpp>
#include <libhal/pointers.hpp>

namespace hal::mac::inline v1 {

namespace {
/**
 * @brief Convert libhal baud rate to termios speed constant
 */
speed_t baud_rate_to_speed(hal::hertz p_baud_rate)
{
  switch (static_cast<int>(p_baud_rate)) {
    case 50:
      return B50;
    case 75:
      return B75;
    case 110:
      return B110;
    case 134:
      return B134;
    case 150:
      return B150;
    case 200:
      return B200;
    case 300:
      return B300;
    case 600:
      return B600;
    case 1200:
      return B1200;
    case 1800:
      return B1800;
    case 2400:
      return B2400;
    case 4800:
      return B4800;
    case 9600:
      return B9600;
    case 19200:
      return B19200;
    case 38400:
      return B38400;
    case 57600:
      return B57600;
    case 115200:
      return B115200;
    case 230400:
      return B230400;
    default:
      return B0;
  }
}
}  // anonymous namespace

hal::v5::strong_ptr<serial> serial::create(
  std::pmr::polymorphic_allocator<> p_allocator,
  std::string_view p_device_path,
  usize p_buffer_size,
  hal::v5::serial::settings const& p_settings)
{
  if (p_buffer_size == 0) {
    throw hal::argument_out_of_domain(nullptr);
  }

  return hal::v5::make_strong_ptr<serial>(
    p_allocator, p_allocator, p_device_path, p_buffer_size, p_settings);
}

serial::serial(hal::v5::strong_ptr_only_token,
               std::pmr::polymorphic_allocator<> p_allocator,
               std::string_view p_device_path,
               usize p_buffer_size,
               hal::v5::serial::settings const& p_settings)
  : m_receive_buffer(p_buffer_size, hal::byte{ 0 }, p_allocator)
{

  // Open the serial device
  m_fd = ::open(p_device_path.data(), O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (m_fd == -1) {
    if (errno == ENOENT) {
      throw hal::no_such_device(m_fd, this);
    } else {
      throw hal::operation_not_permitted(this);
    }
  }

  driver_configure(p_settings);

  // Start the receive thread
  m_receive_thread = std::thread(&serial::receive_thread_function, this);
}

serial::~serial()
{
  // Stop the receive thread
  m_stop_thread.store(true, std::memory_order_release);

  if (m_receive_thread.joinable()) {
    m_receive_thread.join();
  }

  // Close the file descriptor
  if (m_fd != -1) {
    ::close(m_fd);
  }
}
void serial::receive_thread_function()
{
  constexpr int select_timeout_ms = 100;

  while (!m_stop_thread.load(std::memory_order_acquire)) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(m_fd, &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = select_timeout_ms * 1000;

    int result = ::select(m_fd + 1, &read_fds, nullptr, nullptr, &timeout);

    if (result > 0 && FD_ISSET(m_fd, &read_fds)) {
      std::array<hal::byte, 256> temp_buffer;
      ssize_t bytes_read = ::read(m_fd, temp_buffer.data(), temp_buffer.size());

      if (bytes_read > 0) {
        usize current_cursor = m_receive_cursor.load(std::memory_order_acquire);
        usize buffer_size = m_receive_buffer.size();

        for (ssize_t i = 0; i < bytes_read; ++i) {
          m_receive_buffer[current_cursor] = temp_buffer[i];
          current_cursor = (current_cursor + 1) % buffer_size;
        }

        m_receive_cursor.store(current_cursor, std::memory_order_release);
      }
    }
  }
}

void serial::driver_configure(hal::v5::serial::settings const& p_settings)
{

  // Configure basic terminal settings
  struct termios tty;

  if (::tcgetattr(m_fd, &tty) != 0) {
    ::close(m_fd);
    throw hal::operation_not_permitted(nullptr);
  }

  // Set up raw mode
  ::cfmakeraw(&tty);

  // Enable receiver and set local mode
  tty.c_cflag |= (CLOCAL | CREAD);

  // Set 8 data bits
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  // Set baud rate
  speed_t speed = baud_rate_to_speed(p_settings.baud_rate);

  if (speed == B0) {
    throw hal::operation_not_supported(this);
  }

  ::cfsetospeed(&tty, speed);
  ::cfsetispeed(&tty, speed);

  // Configure stop bits
  if (p_settings.stop == hal::v5::serial::settings::stop_bits::one) {
    tty.c_cflag &= ~CSTOPB;  // 1 stop bit
  } else {
    tty.c_cflag |= CSTOPB;  // 2 stop bits
  }

  // Configure parity
  switch (p_settings.parity) {
    case hal::v5::serial::settings::parity::none:
      tty.c_cflag &= ~PARENB;  // Disable parity
      break;
    case hal::v5::serial::settings::parity::even:
      tty.c_cflag |= PARENB;   // Enable parity
      tty.c_cflag &= ~PARODD;  // Even parity
      break;
    case hal::v5::serial::settings::parity::odd:
      tty.c_cflag |= PARENB;  // Enable parity
      tty.c_cflag |= PARODD;  // Odd parity
      break;
    case hal::v5::serial::settings::parity::forced1:
    case hal::v5::serial::settings::parity::forced0:
      // These are not commonly supported on Darwin/macOS
      throw hal::operation_not_supported(nullptr);
  }

  // Apply the settings
  if (::tcsetattr(m_fd, TCSANOW, &tty) != 0) {
    throw hal::operation_not_permitted(nullptr);
  }
}

void serial::driver_write(std::span<hal::byte const> p_data)
{
  if (p_data.empty()) {
    return;
  }

  usize total_written = 0;
  while (total_written < p_data.size()) {
    hal::isize bytes_written = ::write(
      m_fd, p_data.data() + total_written, p_data.size() - total_written);

    if (bytes_written < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Would block - try again
        continue;
      } else {
        throw hal::io_error(nullptr);
      }
    }

    total_written += static_cast<usize>(bytes_written);
  }
}

std::span<hal::byte const> serial::driver_receive_buffer()
{
  return std::span<hal::byte const>(m_receive_buffer);
}

usize serial::driver_cursor()
{
  return m_receive_cursor.load(std::memory_order_acquire);
}

// Add these function implementations:

void serial::set_dtr(bool p_state)
{
  int status;
  if (::ioctl(m_fd, TIOCMGET, &status) != 0) {
    throw hal::operation_not_permitted(this);
  }

  if (p_state) {
    status |= TIOCM_DTR;  // Set DTR bit
  } else {
    status &= ~TIOCM_DTR;  // Clear DTR bit
  }

  if (::ioctl(m_fd, TIOCMSET, &status) != 0) {
    throw hal::operation_not_permitted(this);
  }
}

void serial::set_rts(bool p_state)
{
  int status;
  if (::ioctl(m_fd, TIOCMGET, &status) != 0) {
    throw hal::operation_not_permitted(this);
  }

  if (p_state) {
    status |= TIOCM_RTS;  // Set RTS bit
  } else {
    status &= ~TIOCM_RTS;  // Clear RTS bit
  }

  if (::ioctl(m_fd, TIOCMSET, &status) != 0) {
    throw hal::operation_not_permitted(this);
  }
}

serial::control_signals serial::get_control_signals()
{
  int status;
  if (::ioctl(m_fd, TIOCMGET, &status) != 0) {
    throw hal::operation_not_permitted(this);
  }

  return control_signals{ .dtr = (status & TIOCM_DTR) != 0,
                          .rts = (status & TIOCM_RTS) != 0,
                          .dsr = (status & TIOCM_DSR) != 0,
                          .cts = (status & TIOCM_CTS) != 0 };
}

void serial::set_control_signals(bool p_dtr_state, bool p_rts_state)
{
  int status;
  if (::ioctl(m_fd, TIOCMGET, &status) != 0) {
    throw hal::operation_not_permitted(this);
  }

  // Clear both bits first
  status &= ~(TIOCM_DTR | TIOCM_RTS);

  // Set the requested states
  if (p_dtr_state) {
    status |= TIOCM_DTR;
  }
  if (p_rts_state) {
    status |= TIOCM_RTS;
  }

  if (::ioctl(m_fd, TIOCMSET, &status) != 0) {
    throw hal::operation_not_permitted(this);
  }
}

class modem_dtr_output_pin : public hal::output_pin
{
public:
  modem_dtr_output_pin(hal::v5::strong_ptr_only_token,
                       hal::v5::strong_ptr<mac::serial> p_manager)
    : m_manager(p_manager)
  {
  }

private:
  void driver_configure(settings const& p_settings) override
  {
    if (p_settings.open_drain) {
      hal::safe_throw(hal::operation_not_supported(this));
    }
    // we can ignore everything else as we do not have such control. The only
    // thing we need to signal to the caller is that this operation is not
    // possible. As such, the current sequence of execution cannot continue
    // without intervention.
  }

  void driver_level(bool p_high) override
  {
    m_manager->set_dtr(p_high);
  }

  bool driver_level() override
  {
    return m_manager->get_control_signals().dtr;
  }

  hal::v5::strong_ptr<mac::serial> m_manager;
};

class modem_rts_output_pin : public hal::output_pin
{
public:
  modem_rts_output_pin(hal::v5::strong_ptr_only_token,
                       hal::v5::strong_ptr<mac::serial> p_manager)
    : m_manager(p_manager)
  {
  }

  modem_rts_output_pin(modem_rts_output_pin const&) = delete;
  modem_rts_output_pin& operator=(modem_rts_output_pin const&) = delete;
  modem_rts_output_pin(modem_rts_output_pin&&) = delete;
  modem_rts_output_pin& operator=(modem_rts_output_pin&&) = delete;

private:
  void driver_configure(settings const& p_settings) override
  {
    if (p_settings.open_drain) {
      hal::safe_throw(hal::operation_not_supported(this));
    }
    // we can ignore everything else as we do not have such control. The only
    // thing we need to signal to the caller is that this operation is not
    // possible. As such, the current sequence of execution cannot continue
    // without intervention.
  }

  void driver_level(bool p_high) override
  {
    m_manager->set_rts(p_high);
  }

  bool driver_level() override
  {
    return m_manager->get_control_signals().rts;
  }

  hal::v5::strong_ptr<mac::serial> m_manager;
};

hal::v5::strong_ptr<hal::output_pin> acquire_output_pin(
  std::pmr::polymorphic_allocator<> p_allocator,
  hal::v5::strong_ptr<mac::serial> p_manager,
  mac::modem_out p_pin)
{
  switch (p_pin) {
    case mac::modem_out::dtr: {
      return hal::v5::make_strong_ptr<modem_dtr_output_pin>(p_allocator,
                                                            p_manager);
    }
    case mac::modem_out::rts: {
      return hal::v5::make_strong_ptr<modem_rts_output_pin>(p_allocator,
                                                            p_manager);
    }
  }
}
}  // namespace hal::mac::inline v1
