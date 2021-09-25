/* ------------------------------------------------------------ *
 * file:        demotca6424a.c                                  *
 * purpose:     Demo program for the 24-IO I2C TCA6426A port    *
 *              expander module I2C24IO PMOD on a Raspberry Pi  *
 *                                                              *
 * return:      0 on success, and -1 on errors.                 *
 *                                                              *
 * requires:	I2C library headers and GPIOD library           *
 *              e.g. sudo apt install libi2c-dev libgpiod-dev   *
 *                                                              *
 * compile:	gcc -o demotca6424a i2c_tca6424a.c \            *
 *              demotca6424a.c                                  *
 *                                                              *
 * author:      09/25/2021 Frank4DD                             *
 * ------------------------------------------------------------ */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include "tca6424a.h"

/* ------------------------------------------------------------ *
 * Global variables and defaults                                *
 * ------------------------------------------------------------ */
int verbose = 0;
int outflag = 0;
int argflag = 0;             // 1=dump, 2=info, 3=reset, 4=set
char i2c_bus[256] = I2CBUS;  // set I2C bus default value for RPi
char i2caddr[256] = I2CADDR; // set default I2C address as 0x22
char outset[25] = "";        // output signal argument string

/* ------------------------------------------------------------ *
 * print_usage() prints the programs commandline instructions.  *
 * ------------------------------------------------------------ */
void usage() {
   static char const usage[] = "Usage: demotca6424a [-a addr] [-b i2c-bus] [-v]\n\
\n\
Command line parameters have the following format:\n\
   -a   module I2C bus address in hex, Example: -a 0x22 (default)\n\
   -b   I2C bus to query, Example: -b /dev/i2c-1 (default)\n\
   -h   display this message\n\
   -v   enable debug output\n\
\n\
\n\
Usage examples:\n\
./demotca6424a -a 0x23 -b /dev/i2c-0\n\
./demotca6424a -v\n\\n";
   printf(usage);
}

/* ------------------------------------------------------------ *
 * parseargs() checks the commandline arguments with C getopt   *
 * ------------------------------------------------------------ */
void parseargs(int argc, char* argv[]) {
   int arg;
   opterr = 0;

//   if(argc == 1) { usage(); exit(-1); }

   while ((arg = (int) getopt (argc, argv, "a:b:hv")) != -1) {
      switch (arg) {
         // arg -a + I2C address, type: string, example "0x23"
         case 'a':
            if(verbose == 1) printf("Debug: arg -a, value %s\n", optarg);
            if (strlen(optarg) != 4 || optarg[0] != '0' || optarg[1] != 'x') {
               printf("Error: Cannot get valid -a I2C address argument.\n");
               exit(-1);
            }
            strncpy(i2caddr, optarg, sizeof(i2caddr));
            break;

         // arg -b + I2C bus device name, type: string, example: "/dev/i2c-1"
         case 'b':
            if(verbose == 1) printf("Debug: arg -b, value %s\n", optarg);
            if (strlen(optarg) >= sizeof(i2c_bus)) {
               printf("Error: I2C bus argument to long.\n");
               exit(-1);
            }
            strncpy(i2c_bus, optarg, sizeof(i2c_bus));
            break;

         // arg -h usage, type: flag, optional
         case 'h':
            usage(); exit(0);
            break;

         // arg -v verbose
         case 'v':
            verbose = 1; break;

         case '?':
            if(isprint (optopt))
               printf ("Error: Unknown option `-%c'.\n", optopt);
            else
               printf ("Error: Unknown option character `\\x%x'.\n", optopt);
            usage();
            exit(-1);
            break;

         default:
            usage();
            break;
      }
   }
}

/* ------------------------------------------------------- */
/* Single LED walkthrough - 1..24 Runs single LED light    */
/* around the circle.                                      */
/* ------------------------------------------------------- */
void move1(char mode, int millisec) {
  char out_data;
  
  if(mode) tca6424a_setall(1);            // all LEDs off
  else tca6424a_setall(0);                // all LEDs on
  for(char i=0; i<8; i++) {               // cycle port bits
    out_data = 0x01 << i;                 // 0x01 = 00000001
    if(mode) out_data = ~out_data;        // send port data
    writePort(0, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Delete the last LED before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(0, out_data);

  for(char i=0; i<8; i++) {               // cycle port bits
    out_data = 1 << i;
    if(mode) out_data = ~out_data;        // send port data
    writePort(1, out_data);               // write data to port
    delay(millisec);                      // change speed
  }
  // Delete the last LED before moving on
  if(mode)out_data = 0xFF;                // all port LED off
  else out_data = 0x00;                   // all port LED on
  writePort(1, out_data);

  for(char i=0; i<8; i++) {               // cycle port bits
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
void move1in8(char mode, int millisec) {
  char out_data;
  
  if(mode) tca6424a_setall(1);            // all LEDs off
  else tca6424a_setall(0);                // all LEDs on
  for(char i=0; i<8; i++) {               // cycle port bits
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
void incr1in8(char mode, int millisec) {
  char out_data, data = 0x00;

  if(mode) tca6424a_setall(1);             // all LEDs off
  else tca6424a_setall(0);                 // all LEDs on
  for(char i=0; i<8; i++) {                // cycle port bits
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
/* Single LED walkthrough - 1..4 Runs two LED lights       */
/* in each port. Looks as if 6 lights move in a circle.    */
/* ------------------------------------------------------- */
void move1in4(char mode, int millisec) {
  char out_data;
  
  if(mode) tca6424a_setall(1);            // all LEDs off
  else tca6424a_setall(0);                // all LEDs on
  for(char i=0; i<4; i++) {               // cycle port bits
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
/* alternate() Switch on every second LED on the expander  */
/* and alternate the LED with it's neighbor LED.           */
/* ------------------------------------------------------- */
void alternate(int millisec) {
  char out_data = 0xAA;
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

int main(int argc, char *argv[]) {
   int i = 0;

   /* ---------------------------------------------------------- *
    * Process the cmdline parameters                             *
    * ---------------------------------------------------------- */
   parseargs(argc, argv);

   /* ----------------------------------------------------------- *
    * get current time (now), output at program start if verbose  *
    * ----------------------------------------------------------- */
   time_t tsnow = time(NULL);
   if(verbose == 1) printf("Debug: ts=[%lld] date=%s", (long long) tsnow, ctime(&tsnow));

   /* ----------------------------------------------------------- *
    * Open the I2C bus connecting to the i2c address 0x22 or 0x23 *
    * ----------------------------------------------------------- */
   get_i2cbus(i2c_bus, i2caddr);

   /* ----------------------------------------------------------- *
    *   Run the demo in a forever loop                            *
    * ----------------------------------------------------------- */
   tca6424a_allout();
   while(1) {
      tca6424a_setall(0);
      delay(500);
      tca6424a_setall(1);
      delay(500);
      tca6424a_setall(0);
      delay(500);
      tca6424a_setall(1);
      delay(500);
      for(i=0; i<6; i++) move1(1, 50);
      for(i=0; i<5; i++) move1(0, 50);
      for(i=0; i<16; i++) move1in8(1, 50);
      for(i=0; i<6; i++)  move1in8(0, 50);
      for(i=0; i<4; i++) { 
         incr1in8(1, 50);
         incr1in8(0, 50);
      }
      for(i=0; i<24; i++) move1in4(1, 50);
      for(i=0; i<10; i++) move1in4(0, 50);
      for(i=0; i<10; i++) alternate(200);
      delay(500);
   }
   exit(0);
}
