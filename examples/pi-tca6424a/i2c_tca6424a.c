/* ------------------------------------------------------------ *
 * file:        i2c_tca6424a.c                                   *
 * purpose:     Extract data from MEMSIC TCA6424A sensor modules *
 *              Functions for I2C bus communication, get and    *
 *              set sensor register data. Ths file belongs to   *
 *              the pi-tca6424a package. Functions are called    *
 *              from gettca6424a.c, globals are in tca6424a.h.    *
 *                                                              *
 * Requires:	I2C development packages i2c-tools libi2c-dev   *
 *                                                              *
 * author:      13/09/2021 Frank4DD                             *
 * ------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <errno.h>    
#include <gpiod.h>
#include "tca6424a.h"

/* ------------------------------------------------------------ *
 * Global variables                                             *
 * ------------------------------------------------------------ */
struct gpiod_chip *chip;     // gpiod chip object for GPIO control
int i2cfd;                   // I2C file descriptor

/* ------------------------------------------------------------ *
 * set_resetpin() - set the reset pin as output with signal = 1 *
 * ------------------------------------------------------------ */
void set_resetpin() {
   chip = gpiod_chip_open_by_name(CHIPNAME);
   if (!chip) {
      printf("Error failed to open GPIO chip [%s].\n", CHIPNAME);
      exit(-1);
   }

   struct gpiod_line *reset;
   reset = gpiod_chip_get_line(chip, RESET_PIN);
   if (!reset) {
      printf("Error failed to open GPIO pin [%d].\n", RESET_PIN);
      exit(-1);
   }

   /* --------------------------------------------------------- *
    * Check reset line pin configuration
    * --------------------------------------------------------- */
   int direction = gpiod_line_direction(reset);
   if(verbose == 1) printf("Debug: Get pin gpio%02d: [direction %d]\n", RESET_PIN, direction);
   int act_state = gpiod_line_active_state(reset);
   if(verbose == 1) printf("Debug: Get pin gpio%02d: [act_state %d]\n", RESET_PIN, act_state);

   int ret = gpiod_line_request_output(reset, PMOD, 1);  // output with signal=1
   if (ret != 0) {
      printf("Error failed to set GPIO pin [%d] output = '1'.\n", RESET_PIN);
      exit(-1);
   }
   if(verbose == 1) printf("Debug: Set pin gpio%02d: [output '1']\n", RESET_PIN);
}

/* ------------------------------------------------------------ *
 * get_i2cbus() - Enables the I2C bus communication. RPi 2,3,4  *
 * use /dev/i2c-1, RPi 1 used i2c-0, NanoPi Neo also uses i2c-0 *
 * ------------------------------------------------------------ */
void get_i2cbus(char *i2cbus, char *i2caddr) {
   set_resetpin();                 // set resetpin=1 to enable IC

   if((i2cfd = open(i2cbus, O_RDWR)) < 0) {
      printf("Error failed to open I2C bus [%s].\n", i2cbus);
      exit(-1);
   }
   if(verbose == 1) printf("Debug: I2C bus device: [%s]\n", i2cbus);
   /* --------------------------------------------------------- *
    * Set I2C device (TCA6424A I2C address is 0x22 or 0x23)     *
    * --------------------------------------------------------- */
   int addr = (int)strtol(i2caddr, NULL, 16);
   if(verbose == 1) printf("Debug: Sensor address: [0x%02X]\n", addr);

   if(ioctl(i2cfd, I2C_SLAVE, addr) != 0) {
      printf("Error can't find sensor at address [0x%02X].\n", addr);
      exit(-1);
   }
   if(verbose == 1) printf("Debug: Got data @addr: [0x%02X]\n", addr);
}

/* --------------------------------------------------------------- *
 * tca6424a_dump() dumps the complete register map data (16 bytes) *
 * --------------------------------------------------------------- */
int tca6424a_dump() {
   printf("------------------------------------------------------\n");
   printf("Texas Instruments TCA6424A register dump:\n");
   printf("------------------------------------------------------\n");
   printf(" reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
   printf("------------------------------------------------------\n");

   /* ------------------------------------------------------ *
    * Read 3 bytes input port register data starting at 0x80 *
    * ------------------------------------------------------ */
   char reg = 0x80;
   if(write(i2cfd, &reg, 1) != 1) {
      printf("Error: I2C write failure for register 0x%02X\n", reg);
      exit(-1);
   }

   char input[3] = {0};  // 3 bytes register data buffer
   if(read(i2cfd, &input, 3) != 3) {
      printf("Error: I2C read failure for register 0x%02X\n", reg);
      exit(-1);
   }

   /* ------------------------------------------------------ *
    * Read 3 bytes output port register data starting 0x84   *
    * ------------------------------------------------------ */
   reg = 0x84;
   if(write(i2cfd, &reg, 1) != 1) {
      printf("Error: I2C write failure for register 0x%02X\n", reg);
      exit(-1);
   }

   char output[3] = {0};  // 3 bytes register data buffer
   if(read(i2cfd, &output, 3) != 3) {
      printf("Error: I2C read failure for register 0x%02X\n", reg);
      exit(-1);
   }

   /* ------------------------------------------------------ *
    * Read 3 bytes polarity port register data starting 0x88  *
    * ------------------------------------------------------ */
   reg = 0x88;
   if(write(i2cfd, &reg, 1) != 1) {
      printf("Error: I2C write failure for register 0x%02X\n", reg);
      exit(-1);
   }

   char polarity[3] = {0};  // 3 bytes register data buffer
   if(read(i2cfd, &polarity, 3) != 3) {
      printf("Error: I2C read failure for register 0x%02X\n", reg);
      exit(-1);
   }

   /* ------------------------------------------------------ *
    * Read 3 bytes config port register data starting 0x8c   *
    * ------------------------------------------------------ */
   reg = 0x8c;
   if(write(i2cfd, &reg, 1) != 1) {
      printf("Error: I2C write failure for register 0x%02X\n", reg);
      exit(-1);
   }

   char config[3] = {0};  // 3 bytes register data buffer
   if(read(i2cfd, &config, 3) != 3) {
      printf("Error: I2C read failure for register 0x%02X\n", reg);
      exit(-1);
   }

   printf("[0x00] %02X %02X %02X ", input[0], input[1], input[2]);
   printf("-- %02X %02X %02X ", output[0], output[1], output[2]);
   printf("-- %02X %02X %02X ", polarity[0], polarity[1], polarity[2]);
   printf("-- %02X %02X %02X --\n", config[0], config[1], config[2]);
   exit(0);
}

/* --------------------------------------------------------------- *
 * tca6424a_reset() resets the IC and clears config settings       *
 * --------------------------------------------------------------- */
int tca6424a_reset() {
   if(verbose == 1) printf("Debug: IC Reset complete\n");
   
   /* ------------------------------------------------------------ *
    * After a reset, the IC needs at leat 2ms to boot up.          *
    * ------------------------------------------------------------ */
   usleep(2 * 1000);
   exit(0);
}

/* --------------------------------------------------------------- *
 * tca6424a_info() prints the IC port configuration                *
 * --------------------------------------------------------------- */
void tca6424a_info() {
}

/* --------------------------------------------------------------- *
 * tca6424a_allout() sets all ports and pins as output             *
 * --------------------------------------------------------------- */
void tca6424a_allout() {
   char data[4];
   data[0] = CONFIG_PORT0_AI_ADDR;
   data[1] = 0x00;
   data[2] = 0x00;
   data[3] = 0x00;
   if(write(i2cfd, data, 4) != 4) {
      printf("Error: I2C write failure for register 0x%02X\n", data[0]);
      exit(-1);
   }
   if(verbose == 1) printf("Debug: Setting all ports as output complete\n");
}

/* --------------------------------------------------------------- *
 * tca6424a_setall() sets all ports and pins output = '0' or '1'   *
 * --------------------------------------------------------------- */
void tca6424a_setall(int on) {
   char data[4];
   data[0] = OUTPUT_PORT0_AI_ADDR;

   if(on == 0) { data[1] = 0x00; data[2] = 0x00; data[3] = 0x00; }
   else if(on == 1) { data[1] = 0xFF; data[2] = 0xFF; data[3] = 0xFF; }
   else {
      printf("Error: Pin signal level [%d], can only be '0' or '1'\n", on);
      exit(-1);
   }

   if(write(i2cfd, data, 4) != 4) {
      printf("Error: I2C write failure for register 0x%02X\n", data[0]);
      exit(-1);
   }
   if(verbose == 1) printf("Debug: Setting all output as [%d] complete\n", on);
}

void tca6424a_setpins(char *pindata) {
   char data[4];
   data[0] = OUTPUT_PORT0_AI_ADDR;

   char port0_datastr[9], port1_datastr[9], port2_datastr[9];
   // sety port 0 data
   strncpy(port0_datastr, pindata, 8);
   port0_datastr[8] = '\0';
   data[1] = strtol(port0_datastr, NULL, 2);
   if(verbose == 1) printf("Debug: port0 data str: [%s] hex [0x%02X]\n", port0_datastr, data[1]);
   // sety port 1 data
   strncpy(port1_datastr, &pindata[8], 8);
   port1_datastr[8] = '\0';
   data[2] = strtol(port1_datastr, NULL, 2);
   if(verbose == 1) printf("Debug: port1 data str: [%s] hex [0x%02X]\n", port1_datastr, data[2]);
   // sety port 2 data
   strncpy(port2_datastr, &pindata[16], 8);
   port2_datastr[8] = '\0';
   data[3] = strtol(port2_datastr, NULL, 2);
   if(verbose == 1) printf("Debug: port2 data str: [%s] hex [0x%02X]\n", port2_datastr, data[3]);

   if(write(i2cfd, data, 4) != 4) {
      printf("Error: I2C write failure for register 0x%02X\n", data[0]);
      exit(-1);
   }
   if(verbose == 1) printf("Debug: Setting pin output as [%s] complete\n", pindata);

}

void tca6424a_setout(char *pindata) {
}


/* ------------------------------------------------------- */
/* writePort() writes a data byte to output port 0,1,2     */
/* ------------------------------------------------------- */
void writePort(char port, char data) {
   char out_data[2];
   if(port == 0) out_data[0] = OUTPUT_PORT0_NOAI_ADDR; // P0 output reg
   if(port == 1) out_data[0] = OUTPUT_PORT1_NOAI_ADDR; // P1 output reg
   if(port == 2) out_data[0] = OUTPUT_PORT2_NOAI_ADDR; // P2 output reg
   out_data[1] = data;

   if(write(i2cfd, out_data, 2) != 2) {
      printf("Error: I2C write failure for register 0x%02X\n", out_data[0]);
      exit(-1);
   }
}

/* ------------------------------------------------------- */
/* delay() Sleep for the requested number of milliseconds. */
/* ------------------------------------------------------- */
int delay(long msec) {
   struct timespec ts;
   int res;

   if (msec < 0) { errno = EINVAL; return -1; }

   ts.tv_sec = msec / 1000;
   ts.tv_nsec = (msec % 1000) * 1000000;

   do { res = nanosleep(&ts, &ts); }
   while (res && errno == EINTR);

   return res;
}
