/* ------------------------------------------------------------ *
 * file:        settca6424a.c                                   *
 * purpose:     port control and data extraction program        *
 *              for the 24-IO I2C TCA6426A port expander        *
 *                                                              *
 * return:      0 on success, and -1 on errors.                 *
 *                                                              *
 * requires:	I2C library headers and GPIOD library           *
 *              e.g. sudo apt install libi2c-dev libgpiod-dev   *
 *                                                              *
 * compile:	gcc -o settca6424a i2c_tca6424a.c settca6424a.c *
 *                                                              *
 * example:	./settca6424a -o 010011110011010101011111       *
 *                                                              *
 * author:      09/09/2021 Frank4DD                             *
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
   static char const usage[] = "Usage: settca6424a [-a addr] [-b i2c-bus] [-d] [-i] -o [<24-bit output>, or all=0/1] [-r] [-v]\n\
\n\
Command line parameters have the following format:\n\
   -a   module I2C bus address in hex, Example: -a 0x22 (default)\n\
   -b   I2C bus to query, Example: -b /dev/i2c-1 (default)\n\
   -d   dump the complete sensor register map content\n\
   -i   print IC port information\n\
   -o   24-bit binary output string, or all=0/1 to set all pins at once\n\
        -o 111111110000000011111111 24 x 0/1 sets individual pins\n\
        -o all=1 sets all pins 'on', -o all=0 sets all pins 'off'\n\
   -r   reset the IC\n\
   -h   display this message\n\
   -v   enable debug output\n\
\n\
\n\
Usage examples:\n\
./settca6424a -a 0x23 -b /dev/i2c-0 -d\n\
./settca6424a -o all=1\n\
./settca6424a -o 010101010101010101010101\n\n";
   printf(usage);
}

/* ------------------------------------------------------------ *
 * parseargs() checks the commandline arguments with C getopt   *
 * -d = argflag 1     -i = argflag 2                            *
 * -r = argflag 3     -o = argflag 4                            *
 * ------------------------------------------------------------ */
void parseargs(int argc, char* argv[]) {
   int arg;
   opterr = 0;

   if(argc == 1) { usage(); exit(-1); }

   while ((arg = (int) getopt (argc, argv, "a:b:dio:rhv")) != -1) {
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

         // arg -d dumps the complete register map data
         case 'd':
            if(verbose == 1) printf("Debug: arg -d\n");
            argflag = 1;
            break;

         // arg -i prints sensor information
         case 'i':
            if(verbose == 1) printf("Debug: arg -i\n");
            argflag = 2;
            break;

         // arg -o sets output pin signal, type: string values "all=1"
         case 'o':
            if(verbose == 1) printf("Debug: arg -o, value %s\n", optarg);
            argflag = 4;
            if (strlen(optarg) >= sizeof(outset)) {
               printf("Error: output set argument to long.\n");
               exit(-1);
            }
            strncpy(outset, optarg, sizeof(outset));
            break;

         // arg -r
         // optional, resets sensor
         case 'r':
            if(verbose == 1) printf("Debug: arg -r\n");
            argflag = 3;
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

int main(int argc, char *argv[]) {
   int res = -1;       // res = function retcode: 0=OK, -1 = Error

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
    *  "-d" dump the register map content and exit the program    *
    * ----------------------------------------------------------- */
    if(argflag == 1) {
      res = tca6424a_dump();
      if(res != 0) {
         printf("Error: could not dump the register maps.\n");
         exit(-1);
      }
      exit(0);
   }

   /* ----------------------------------------------------------- *
    *  "-i" print port information and exit the program           *
    * ----------------------------------------------------------- */
    if(argflag == 2) {
      tca6424a_info();
      exit(0);
   }

   /* ----------------------------------------------------------- *
    *  "-r" reset the circuit and exit the program                *
    * ----------------------------------------------------------- */
    if(argflag == 3) {
      res = tca6424a_reset();
      if(res != 0) {
         printf("Error: could not reset the IC.\n");
         exit(-1);
      }
      exit(0);
   }

   /* ----------------------------------------------------------- *
    *  "-o" set the ouput signal and exit the program             *
    * ----------------------------------------------------------- */
   if(argflag == 4) {
      tca6424a_allout();
      if(strcmp(outset, "all=0") == 0) tca6424a_setall(0);
      if(strcmp(outset, "all=1") == 0) tca6424a_setall(1);
      if(outset[0] == '0' || outset[0] == '1') {
         int outset_len = strlen(outset);
         if(outset_len != 24) {
            printf("Error: arg needs 24 values, '0' or '1'. (got %d)\n", outset_len);
            exit(-1);
         }
         tca6424a_setpins(outset);
      }
      exit(0);
   }
}
