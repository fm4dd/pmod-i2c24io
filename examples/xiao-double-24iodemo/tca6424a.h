/* ------------------------------------------------------------ *
 * file:        tca6424a.h                                      *
 * purpose:     header for tca6424a.cpp and xiao-pmod1-24iodemo *
 *                                                              *
 * author:      09/24/2021 FM4DD                                *
 * ------------------------------------------------------------ */
#include <Arduino.h>           // Arduino default library
#include <Wire.h>              // I2C

// Macro for printing binary numbers with leading zeros
// Example: Serial.print("data value: "); PRINTBIN(data); Serial.println();
// will print "data value: 000000010"
#define PRINTBIN(Num) for (uint32_t t = (1UL<< (sizeof(Num)*8)-1); t; t >>= 1) Serial.write(Num  & t ? '1' : '0');

#define I2CBUS        "/dev/i2c-1" // Raspi default I2C bus
#define I2CADDR            "0x22"  // tca6424a addr is 0x22 / 0x23
#define CHIPNAME      "gpiochip0"  // Raspberry PI GPIO chip
#define PMOD              "pmod4"  // gpiod signal consumer
#define RESET_PIN             22   // PMOD-4 pin-1 "Reset" (ops=1)

/* ------------------------------------------------------------ *
 * INPUT registers can only be read, any write is ignored.      *
 * ------------------------------------------------------------ */
#define INPUT_PORT0_NOAI     0x00 // read-only, no increment
#define INPUT_PORT1_NOAI     0x01 // read-only, no increment
#define INPUT_PORT2_NOAI     0x02 // read-only, no increment

/* ------------------------------------------------------------ *
 * OUTPUT registers can be set or read for status.              *
 * ------------------------------------------------------------ */
#define OUTPUT_PORT0_NOAI    0x04 // read-write, no increment
#define OUTPUT_PORT1_NOAI    0x05 // read-write, no increment
#define OUTPUT_PORT2_NOAI    0x06 // read-write, no increment

/* ------------------------------------------------------------ *
 * For input ports, the polarity register can invert the signal *
 * ------------------------------------------------------------ */
#define POLARITY_PORT0_NOAI  0x08 // read-write, no increment
#define POLARITY_PORT1_NOAI  0x09 // read-write, no increment
#define POLARITY_PORT2_NOAI  0x0a // read-write, no increment

/* ------------------------------------------------------------ *
 * Configuration registers set each pin as input (1) output (0) *
 * ------------------------------------------------------------ */
#define CONFIG_PORT0_NOAI    0x0c // read-write, no increment
#define CONFIG_PORT1_NOAI    0x0d // read-write, no increment
#define CONFIG_PORT2_NOAI    0x0e // read-write, no increment

/* ------------------------------------------------------------ *
 * Access register with autoincrement to get all 3 ports in one *
 * E.g. read or write of 3 bytes would program the complete IO. *
 * ------------------------------------------------------------ */
#define INPUT_PORT0_AI       0x80 // read-only, autoincrement
#define INPUT_PORT1_AI       0x81 // read-only, autoincrement
#define INPUT_PORT2_AI       0x82 // read-only, autoincrement

#define OUTPUT_PORT0_AI      0x84 // read-write, autoincrement
#define OUTPUT_PORT1_AI      0x85 // read-write, autoincrement
#define OUTPUT_PORT2_AI      0x86 // read-write, autoincrement

#define POLARITY_PORT0_AI    0x88 // read-write, autoincrement
#define POLARITY_PORT1_AI    0x89 // read-write, autoincrement
#define POLARITY_PORT2_AI    0x8a // read-write, autoincrement

#define CONFIG_PORT0_AI      0x8c // read-write, autoincrement
#define CONFIG_PORT1_AI      0x8d // read-write, autoincrement
#define CONFIG_PORT2_AI      0x8e // read-write, autoincrement

/* ------------------------------------------------------------ *
 * external function prototypes for I2C bus communication       *
 * ------------------------------------------------------------ */
class TCA6424A {
  private:
  uint8_t _deviceAddr1;
  uint8_t _resetLine1;
  uint8_t _deviceAddr2;
  uint8_t _resetLine2;
private:
  void writePort(uint8_t port, uint8_t data);// write data to output port
public:
   TCA6424A(uint8_t address1, uint8_t reset1, 
            uint8_t address2, uint8_t reset2);
   void begin();                            // start I2C
   void reset(uint8_t module);              // reset the device
   void setall(uint8_t level);              // set all port pins output=0/1
   void move1(uint8_t mode, int milli);     // move 1 bit through all pins
   void move1in8(uint8_t mode, int milli);  // move 1 bit through each port
   void move1in4(uint8_t mode, int milli);  // move 2 bits 10001 on each port
   void alternate(int milli);               // alternate neighbor pins
   void incr1in8(uint8_t mode, int milli);  // increment pins from 0..8
   void decr1in8(uint8_t mode, int milli);  // decrement pins from 8..0
//   void center12(uint8_t mode, int milli);  // increment 12led rows
};
