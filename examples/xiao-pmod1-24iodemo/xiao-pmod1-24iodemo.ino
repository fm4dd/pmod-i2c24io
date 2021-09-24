// --------------------------------------
// xiao-pmod1-24iodemo
//
// A demo program for the TI TCA6424A-based
// I2C24IO PMOD connected to a PMOD2RPI 
// interface board's coonnector PMOD1. It
// produces various light patterns.
//
// I2C24IO PMOD connected to PMOD1 upper row
// I2C24IO PMOD SW1 ON/ON/ON = PullUp + 0x22
// PMOD1 XIAO I2C pin assignment SW3 ON/ON
// -----------------------------------------
// pmod1-1 = XIAO gpio D7  / pin J4-1 = RESET
// pmod1-2 = XIAO gpio D10 / pin J4-4 = INT
// pmod1-3 = XIAO gpio D9  / pin J4-3 = SCL
// pmod1-4 = XIAO gpio D8  / pin J4-2 = SDA
//
// I2C24IO PMOD connected to PMOD1 lower row
// PMOD1 XIAO I2C pin assignment SW3 ON/OFF
// -----------------------------------------
// pmod1-5 = XIAO gpio D3  / pin J4-1 = RESET
// pmod1-6 = XIAO gpio D2  / pin J4-4 = INT
// pmod1-7 = XIAO gpio D5  / pin J4-3 = SCL
// pmod1-8 = XIAO gpio D4  / pin J4-2 = SDA
// -----------------------------------------
// Note:
// PMOD2RPI SW3 flips pmod1-3/4 with pmod1-7/8
// -----------------------------------------
// RESET = 7 if connected to higher row and sW3 OFF
// RESET = 3 if connected to lower row and SW3 ON
// -----------------------------------------
#include "tca6424a.h"   // local code for the TCA6424A IC
#define I2CADDR1 0x22
#define RESET1 3

/* ------------------------------------------------- */
/* initialize TCA6424A IC with I2C address and reset */
/* ------------------------------------------------- */
TCA6424A i2c24io_a(I2CADDR1, RESET1);

/* ------------------------------------------------- *
 * global variables                                  *
 * ------------------------------------------------- */
int verbose = 0; // debug flag, 0 = normal, 1 = debug mode
uint8_t i = 0;

void setup() {
  Wire.begin();
  if(verbose) {
    Serial.begin(9600);
    while (!Serial) {} // wait for serial port to connect. Needed for native USB port only
    Serial.println("xiao-pmod1-24iodemo starting...");
  }
  i2c24io_a.begin();
}
  
void loop(){
  if(verbose) Serial.println("Set I2C24IO PMOD port 0-2 output 0 - lights all LED");
  i2c24io_a.setall(0x00);
  delay(500);
  i2c24io_a.setall(0xFF);
  delay(500);
  i2c24io_a.setall(0x00);
  delay(500);
  i2c24io_a.setall(0xFF);
  delay(500);

  for(i=0; i<6; i++)  i2c24io_a.move1(1, 50);
  for(i=0; i<5; i++)  i2c24io_a.move1(0, 50);
  for(i=0; i<16; i++) i2c24io_a.move1in8(1, 50);
  for(i=0; i<6; i++)  i2c24io_a.move1in8(0, 50);
    for(i=0; i<4; i++) { 
    i2c24io_a.incr1in8(1, 50);
    i2c24io_a.incr1in8(0, 50);
  }
  for(i=0; i<24; i++) i2c24io_a.move1in4(1, 50);
  for(i=0; i<10; i++) i2c24io_a.move1in4(0, 50);
  for(i=0; i<10; i++) i2c24io_a.alternate(200);
  delay(500);
}
