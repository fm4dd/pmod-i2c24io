/* ------------------------------------------------------- *
 * PMOD-I2C24IO driver code tca6424a.cpp 2021-09 @FM4DD    *
 *                                                         *
 * This file has the functions to control the tca6424a     *
 * 24-bit digital IO expander circuit over the I2C bus     *
 * ------------------------------------------------------- */
#include "tca6424a.h"

TCA6424A::TCA6424A(uint8_t address1, uint8_t reset1, uint8_t address2, uint8_t reset2) {
  _deviceAddr1 = address1;
  _resetLine1 = reset1;
  _deviceAddr2 = address2;
  _resetLine2 = reset2;
}

/* ------------------------------------------------------- */
/* begin() sets RESET line high, initializes I2C and       */
/* configures the IC for digital out on all 24 pins.       */
/* ------------------------------------------------------- */
void TCA6424A::begin() {
  pinMode(_resetLine1, OUTPUT);
  digitalWrite(_resetLine1, HIGH);
  pinMode(_resetLine2, OUTPUT);
  digitalWrite(_resetLine2, HIGH);
  uint8_t reg = CONFIG_PORT0_AI; // use auto-increment register addr
  uint8_t conf_data = 0x00;      // switch default 0xFF input to 0x00 output
  Wire.beginTransmission(_deviceAddr1); // start data sending
  Wire.write(reg);               // send address of port0 config register
  Wire.write(conf_data);         // send 0x00 to config port0 as output
  Wire.write(conf_data);         // send 0x00 to config port1 as output
  Wire.write(conf_data);         // send 0x00 to config port2 as output
  Wire.endTransmission();        // stop transmitting
  Wire.beginTransmission(_deviceAddr2); // start data sending
  Wire.write(reg);               // send address of port0 config register
  Wire.write(conf_data);         // send 0x00 to config port0 as output
  Wire.write(conf_data);         // send 0x00 to config port1 as output
  Wire.write(conf_data);         // send 0x00 to config port2 as output
  Wire.endTransmission();        // stop transmitting
}

/* ------------------------------------------------------- */
/* reset() sets RESET line low, and high again             */
/* ------------------------------------------------------- */
void TCA6424A::reset(uint8_t module) {
  if(module == 0) {
    digitalWrite(_resetLine1, LOW);
    delay(50);
    digitalWrite(_resetLine1, HIGH);
  }
  else {
    digitalWrite(_resetLine2, LOW);
    delay(50);
    digitalWrite(_resetLine2, HIGH);
  }
}

/* ------------------------------------------------------- */
/* setall() set all ports to the same bit pattern, e.g.    */
/* 0x00 all on, 0xFF all off.                              */
/* ------------------------------------------------------- */
void TCA6424A::setall(uint8_t out_data) {
  writePort(0, out_data);
  writePort(1, out_data);
  writePort(2, out_data);
}

  /* ------------------------------------------------------- */
  /* alternate() Switch on every second LED on the expander  */
  /* and alternate the LED with it's neighbor LED.           */
  /* ------------------------------------------------------- */
void TCA6424A::alternate(int millisec) {
  uint8_t out_data = 0xAA;
  writePort(0, out_data);
  writePort(1, out_data);
  writePort(2, out_data);
  delay(millisec);
  out_data = 0x55;
  writePort(0, out_data);
  writePort(1, out_data);
  writePort(2, out_data);
  delay(millisec); 
}

/* ------------------------------------------------------- */
/* Single LED walkthrough - 1..24 Runs single LED light    */
/* around the circle.                                      */
/* ------------------------------------------------------- */
void TCA6424A::move1(uint8_t mode, int millisec) {
  uint8_t out_data;
  
  if(mode) setall(0xFF);                  // all LEDs off
  else setall(0x00);                      // all LEDs on
  for(uint8_t i=0; i<8; i++) {            // cycle port bits
    out_data = 0x01 << i;                 // 0x01 = 00000001
    if(mode) out_data = ~out_data;        // send port data
    writePort(0, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Delete the last LED before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(0, out_data);

  for(uint8_t i=0; i<8; i++) {            // cycle port bits
    out_data = 1 << i;
    if(mode) out_data = ~out_data;        // send port data
    writePort(1, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Delete the last LED before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(1, out_data);

  for(uint8_t i=0; i<8; i++) {            // cycle port bits
    out_data = 1 << i;
    if(mode) out_data = ~out_data;        // send port data
    writePort(2, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Delete the last LED before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(2, out_data);
}

/* ------------------------------------------------------- */
/* Single LED walkthrough - 1..8 Runs single LED light     */
/* in each port. Looks as if 3 lights move in a circle.    */
/* ------------------------------------------------------- */
void TCA6424A::move1in8(uint8_t mode, int millisec) {
  uint8_t out_data;
  
  if(mode) setall(0xFF);                  // all LEDs off
  else setall(0x00);                      // all LEDs on
  for(uint8_t i=0; i<8; i++) {            // cycle port bits
    out_data = 0x01 << i;                 // 0x01 = 00000001
    if(mode) out_data = ~out_data;        // send port data
    writePort(0, out_data);               // write data to port
    writePort(1, out_data);               // write data to port
    writePort(2, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Delete the last LED before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(0, out_data);
  writePort(1, out_data);
  writePort(2, out_data);
}

/* ------------------------------------------------------- */
/* Increment Pattern LED1 - LED12 - LED123 ..LED12345678   */
/* in each port.                                           */
/* ------------------------------------------------------- */
void TCA6424A::incr1in8(uint8_t mode, int millisec) {
  uint8_t out_data, data = 0x00;

  if(mode) setall(0xFF);                  // all LEDs off
  else setall(0x00);                      // all LEDs on
  for(uint8_t i=0; i<8; i++) {            // cycle port bits
    data = data | 0x01 << i;
    if(mode) out_data = ~data;
    else out_data = data;
    writePort(0, out_data);               // write data to port
    writePort(1, out_data);               // write data to port
    writePort(2, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Reset all before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(0, out_data);
  writePort(1, out_data);
  writePort(2, out_data);
}

/* ------------------------------------------------------- */
/* Decrement Pattern LED12345678 ..LED123 - LED12 - LED1   */
/* in each port. TODO: switch direction right to left!!!   */
/* ------------------------------------------------------- */
void TCA6424A::decr1in8(uint8_t mode, int millisec) {
  uint8_t out_data, data = 0x00;

  if(mode) setall(0x00);                  // all LEDs off
  else setall(0xFF);                      // all LEDs on
  for(uint8_t i=0; i<8; i++) {            // cycle port bits
    //Serial.print("old data value: ");
    //PRINTBIN(data);
    //Serial.println();
    data = data | 1 << i;
    //Serial.print("new data value: ");
    //PRINTBIN(data);
    //Serial.println();
    if(mode) out_data = data;
    else out_data = ~data;
    writePort(0, out_data);               // write data to port
    writePort(1, out_data);               // write data to port
    writePort(2, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Reset all before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(0, out_data);
  writePort(1, out_data);
  writePort(2, out_data);
}

/* ------------------------------------------------------- */
/* Single LED walkthrough - 1..4 Runs two LED lights       */
/* in each port. Looks as if 6 lights move in a circle.    */
/* ------------------------------------------------------- */
void TCA6424A::move1in4(uint8_t mode, int millisec) {
  uint8_t out_data;
  
  if(mode) setall(0xFF);                  // all LEDs off
  else setall(0x00);                      // all LEDs on
  for(uint8_t i=0; i<4; i++) {            // cycle port bits
    out_data = 0x11 << i;                 // 0x11 = 00010001
    if(mode) out_data = ~out_data;        // send port data
    writePort(0, out_data);               // write data to port
    writePort(1, out_data);               // write data to port
    writePort(2, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Delete the last LED before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(0, out_data);
  writePort(1, out_data);
  writePort(2, out_data);
}

/* ------------------------------------------------------- */
/* writePort() writes a data byte to output port 0,1,2     */
/* ------------------------------------------------------- */
void TCA6424A::writePort(uint8_t port, uint8_t data) {
  uint8_t reg;
  if(port == 0) reg = OUTPUT_PORT0_NOAI; // P0 output reg
  if(port == 1) reg = OUTPUT_PORT1_NOAI; // P1 output reg
  if(port == 2) reg = OUTPUT_PORT2_NOAI; // P2 output reg
  Wire.beginTransmission(_deviceAddr1);  // start I2C addr
  Wire.write(reg);                       // send register
  Wire.write(data);                      // send output data
  Wire.endTransmission();                // end transmission
  Wire.beginTransmission(_deviceAddr2);  // start I2C addr
  Wire.write(reg);                       // send register
  Wire.write(data);                      // send output data
  Wire.endTransmission();                // end transmission
}
