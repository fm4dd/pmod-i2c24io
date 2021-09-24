// --------------------------------------
// xiao-pmod1-i2c
//
// A modified i2c-scanner.ino program to test
// the TI TCA6424A-based I2C24IO PMOD connected
// to a PMOD2RPI PMOD interface board's PMOD1
//
// This sketch tests for response on the I2C
// device address 0x22 and 0x23
//
// I2C24IO PMOD connected to PMOD1 lower row
// I2C24IO PMOD SW1 ON/ON/ON = PullUp + 0x22
// PMOD1 XIAO I2C pin assignment SW3 ON/ON
// -----------------------------------------
// pmod1-1 = XIAO gpio D7  / pin J4-1 = RESET
// pmod1-2 = XIAO gpio D10 / pin J4-4 = INT
// pmod1-3 = XIAO gpio D9  / pin J4-3 = SCL
// pmod1-4 = XIAO gpio D8  / pin J4-2 = SDA
//
// I2C24IO PMOD connected to PMOD1 upper row
// PMOD1 XIAO I2C pin assignment SW3 ON/OFF
// -----------------------------------------
// pmod1-5 = XIAO gpio D3  / pin J2-4 = LD4
// pmod1-6 = XIAO gpio D2  / pin J2-3 = LD5
// pmod1-7 = XIAO gpio D5  / pin J2-6 = LD6
// pmod1-8 = XIAO gpio D4  / pin J2-5 = LD7
// -----------------------------------------
// Note:
// PMOD2RPI SW3 flips pmod1-3/4 with pmod1-7/8 

#include <Wire.h>
byte addr1 = 0x22;
byte addr2 = 0x23;
byte addr = 0x0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) {} // wait for serial port to connect. Needed for native USB port only
  Serial.println("xiao-pmod1-i2c identify I2C24IO PMOD");
  byte error;
  pinMode(7, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite( 7, HIGH); // RESET lower row
  digitalWrite(10, HIGH); // INT lower row
  digitalWrite( 8, HIGH); // RESET upper row
  digitalWrite( 3, HIGH); // INT upper row
  
  Serial.print("Test address 0x22: ");
  Wire.beginTransmission(addr1);
  error = Wire.endTransmission();
  if (error == 0){
    Serial.println("ACK - I2C device found at address 0x22");
    addr = addr1;
  }
  else if (error==4) {
    Serial.println("Unknown error at address 0x22");
  }
  else Serial.println("No device");
  delay(200);
  Serial.print("Test address 0x23: ");
  Wire.beginTransmission(addr2);
  error = Wire.endTransmission();
  if (error == 0){
    Serial.println("ACK - I2C device found at address 0x23");
    addr = addr2;
  }
  else if (error==4) {
    Serial.println("Unknown error at address 0x23");
  }
  else Serial.println("No device");
  if (addr == 0x0) {
    Serial.println("No device found, exiting program...");
    exit(0);
  }
  Serial.print("xiao-pmod1-i2c found I2C24IO PMOD at 0x");
  Serial.print(addr, HEX);
  Serial.println();
  Serial.println("Set I2C24IO PMOD port 0-2 as output");
  byte port0_conf_reg = 0x8c;   // use the auto-increment register addr
  byte port_conf_data = 0x00;   // switch default 0xFF input to 0x00 output
  Wire.beginTransmission(addr); // start data sending
  Wire.write(port0_conf_reg);   // send address of port0 config register
  Wire.write(port_conf_data);   // send 0x00 to config port0 as output
  Wire.write(port_conf_data);   // send 0x00 to config port1 as output
  Wire.write(port_conf_data);   // send 0x00 to config port2 as output
  Wire.endTransmission();       // stop transmitting
  delay(200);
  Serial.println("Set I2C24IO PMOD port 0-2 output 0 - lights all LED");
  byte port0_out_reg = 0x84;    // use the auto-increment register addr
  byte port_out_data = 0x00;    // send 0x00 since LED light at 0-signal
  Wire.beginTransmission(addr); // start data sending
  Wire.write(port0_out_reg);    // send address of port0 output register
  Wire.write(port_out_data);    // send 0x00 to set port0 output 00000000
  Wire.write(port_out_data);    // send 0x00 to set port0 output 00000000
  Wire.write(port_out_data);    // send 0x00 to set port0 output 00000000
  Wire.endTransmission();       // stop transmitting
}
void loop(){}
