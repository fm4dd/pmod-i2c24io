## Pi-TCA6424A

Linux 'C' Control Code for designs based on the I2C 24-bit digital expander IC Texas Instruments TCA6424A.
Implemented and tested on a Raspberry Pi, using the PMOD2RPI interface board and a I2C24IO PMOD module. 

### Usage
```
pi@pi-ms05:~/pmod2rpi/pi-tca6424a $ ./settca6424a
Usage: settca6424a [-b i2c-bus] [-d] [-i] -o [<24-bit output>, or all=0/1] [-r] [-v]

Command line parameters have the following format:
   -b   I2C bus to query, Example: -b /dev/i2c-1 (default)
   -d   dump the complete sensor register map content
   -i   print sensor information
   -o   24-bit binary output string, or all=0/1 to set all pins at once
        -o 111111110000000011111111 24 x 0/1 sets individual pins
        -o all=1 sets all pins 'on', -o all=0 sets all pins 'off'
   -r   reset sensor
   -h   display this message
   -v   enable debug output

Usage examples:
./settca6424a -b /dev/i2c-0 -i
./settca6424a -o all=1
./settca6424a -o 010101010101010101010101
```

### Examples

Enable all pins as output, and set the output level as '0':
```
pi@pi-ms05:~/pmod2rpi/pi-tca6424a $ ./settca6424a -o all=0
```
Enable all pins as output, and set the output level as '1':
```
pi@pi-ms05:~/pmod2rpi/pi-tca6424a $ ./settca6424a -o all=1
```
Enable all pins as output, and set individual pins according to the 24-bit pattern '111111110000000010101010':
```
pi@pi-ms05:~/pmod2rpi/pi-tca6424a $ ./settca6424a -o 111111110000000010101010 -v
Debug: ts=[1632130627] date=Mon Sep 20 18:37:07 2021
Debug: Get pin gpio22: [direction 2]
Debug: Get pin gpio22: [act_state 1]
Debug: Set pin gpio22: [output '1']
Debug: I2C bus device: [/dev/i2c-1]
Debug: Sensor address: [0x22]
Debug: Got data @addr: [0x22]
Debug: Setting all ports as output complete
Debug: port0 data str: [11111111] hex [0xFF]
Debug: port1 data str: [00000000] hex [0x00]
Debug: port2 data str: [10101010] hex [0xAA]
Debug: Setting pin output as [111111110000000010101010] complete
```
Small one-liner to set pins as output, and alternate the pins output level:
``` 
pi@pi-ms05:~/pmod2rpi/pi-tca6424a $ while true; do ./settca6424a -o 101010101010101010101010; \
> sleep 0.5; ./settca6424a -o 010101010101010101010101; sleep 0.5; done
```
Set a specific pin value, and dump the TCA6424A register values:
```
pi@pi-ms05:~/pmod2rpi/pi-tca6424a $ ./settca6424a -o 111111110000000010101010
pi@pi-ms05:~/pmod2rpi/pi-tca6424a $ ./settca6424a -d
------------------------------------------------------
Texas Instruments TCA6424A register dump:
------------------------------------------------------
 reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
------------------------------------------------------
[0x00] FF 00 AA -- FF 00 AA -- 00 00 00 -- 00 00 00 --
```