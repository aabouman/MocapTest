#pragma once

#include <string>
#include <Arduino.h>

#include "pose.hpp"

namespace rexquad {

/**
 * @brief Make sure the message starts with the byte `msg_id`.
 * 
 * If the message does not start with a byte matching `msg_id`, find the byte with
 * `msg_id`, shift the data, and report how many extra bytes need to be read to 
 * complete the message.
 * 
 * TODO: Handle the case when the message ID is not found at all.
 * 
 * @param buf Data buffer
 * @param len Length of data buffer
 * @param msg_id Expected first byte
 * @return int Number of extra bytes to read. If 0, the message is valid.
 */
int VerifyRead(char* buf, int len, uint8_t msg_id);

template <class SerialClass>
void PrintPose(SerialClass Serial, const PoseMsg& msg) {
  Serial.print("  x = [");
  Serial.print(msg.x, 3);
  Serial.print(", ");
  Serial.print(msg.y, 3);
  Serial.print(", ");
  Serial.print(msg.z, 3);
  Serial.print("]\n");
  Serial.print("  q = [");
  Serial.print(msg.qw, 3);
  Serial.print(", ");
  Serial.print(msg.qx, 3);
  Serial.print(", ");
  Serial.print(msg.qy, 3);
  Serial.print(", ");
  Serial.print(msg.qz, 3);
  Serial.print("]\n");
}

template <class Serial>
int ReadMessageWithID(Serial serial, char* buf, int buflen, int msg_len, int msg_id) {
  int bytes_available = std::min(serial.available(), buflen);
  if (bytes_available >= msg_len) {
    int bytes_received = serial.readBytes(buf, bytes_available);
    if (bytes_received > 0) {

      // Find start bit
      int start_index;
      uint8_t byte;
      for (start_index = 0; start_index < bytes_received; ++start_index) {
        byte  = buf[start_index];
        if (byte == msg_id) {
          break;
        }
      }

      // How many bytes of the message did we receive?
      int received_length = bytes_received - start_index;  

      // Shift the data over to the start of the buffer
      //   discards all bytes before the MsgID
      if (start_index > 0) {
        for (int i = 0; i < received_length; ++i) {
          buf[i] = buf[i + start_index];
        }
      }
      return received_length;
    }
    return 0;
  }
  return 0;
}

/**
 * @brief Prints a message at a given rate
 *
 * # Usage
 * Initialize the printer as a global variable (one for each rate-limited message you want to send).
 * Call the `print` or `println` methods as you would. Note that right now you can't string multiple
 * print statements back-to-back.
 */
class PrintAtRate {
  public:
    PrintAtRate(float rate)
        : delay_ms_(static_cast<int>(1000.0 / rate)),
          timestamp_last_print_ms_(-delay_ms_) {}
    void print(const std::string& msg);
    void println(const std::string& msg);

  private:
    int delay_ms_;
    int timestamp_last_print_ms_;
};

/**
 * @brief Prints the rate at which this method is called
 */
void print_rate();

/**
 * @brief Give a user a prompt over a Serial connection and store the response 
 * 
 * Will loop indefinitely until the user provides a response.
 * 
 * @param prompt A prompt to provide the user for context about what input is needed
 * @return The user's response, entered in the command line. Does not include a terminating 
 * newline character.
 */
template <class SerialClass>
String GetUserResponse(SerialClass Serial, String prompt) {
  Serial.println(prompt);
  String user_response = Serial.readStringUntil('\n');
  while (user_response.length() == 0) {
    user_response = Serial.readStringUntil('\n');
  }
  return user_response;
}

void RatePrinter();

}  // namespace rexquad