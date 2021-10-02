## XIAO-DOUBLE-24IODEMO

Arduino Sketch for designs based on the I2C 24-bit digital expander IC Texas Instruments TCA6424A.
Implemented and tested on a PMOD2RPI interface board with two I2C24IO PMOD modules driven by the Seeeduino XIAO (Cortex M0+)  

### Usage

Running the sketch will output various LED patterns on both modules in parallel:

1. ON/OFF for all pins on the module
2. Moving a single bit through each of the 24 pins
3. Moving 3 bits, one for each of the 3 output ports
4. Increment pins from 0 until all port pins are ON/OFF
5. Moving 6 bits, two for each of the 3 output ports
6. Alternate the level for neighporing pins ON/OFF

Two I2C24IO PMOD's connected to the RPi PMOD Interface board connector PMOD1 and PMOD2, running the "xiao-double-i2c24io" example program on the Seeeduino XIAO (Cortex M0):

<img src="https://github.com/fm4dd/pmod-i2c24io/blob/master/images/t6-v10/pmod2rpi-xiao-i2c24io-double-v10.png" width="640px">
