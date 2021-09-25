/* ------------------------------------------------------------ *
 * file:        tca6424a.h                                      *
 * purpose:     header file for settca6424a.c & i2c_tca6424a.c  *
 *                                                              *
 * author:      09/04/2021 Frank4DD                             *
 * ------------------------------------------------------------ */

#define I2CBUS        "/dev/i2c-1" // Raspi default I2C bus
#define I2CADDR            "0x22"  // tca6424a addr is 0x22 / 0x23
#define CHIPNAME      "gpiochip0"  // Raspberry PI GPIO chip
#define PMOD              "pmod4"  // gpiod signal consumer
#define RESET_PIN             22   // PMOD-4 pin-1 "Reset" (ops=1)


/* ------------------------------------------------------------ *
 * INPUT registers can only be read, any write is ignored.      *
 * ------------------------------------------------------------ */
#define INPUT_PORT0_NOAI_ADDR     0x00 // read-only, no increment
#define INPUT_PORT1_NOAI_ADDR     0x01 // read-only, no increment
#define INPUT_PORT2_NOAI_ADDR     0x02 // read-only, no increment

/* ------------------------------------------------------------ *
 * OUTPUT registers can be set or read for status.              *
 * ------------------------------------------------------------ */
#define OUTPUT_PORT0_NOAI_ADDR    0x04 // read-write, no increment
#define OUTPUT_PORT1_NOAI_ADDR    0x05 // read-write, no increment
#define OUTPUT_PORT2_NOAI_ADDR    0x06 // read-write, no increment

/* ------------------------------------------------------------ *
 * For input ports, the polarity register can invert the signal *
 * ------------------------------------------------------------ */
#define POLARITY_PORT0_NOAI_ADDR  0x08 // read-write, no increment
#define POLARITY_PORT1_NOAI_ADDR  0x09 // read-write, no increment
#define POLARITY_PORT2_NOAI_ADDR  0x0a // read-write, no increment

/* ------------------------------------------------------------ *
 * Configuration registers set each pin as input (1) output (0) *
 * ------------------------------------------------------------ */
#define CONFIG_PORT0_NOAI_ADDR    0x0c // read-write, no increment
#define CONFIG_PORT1_NOAI_ADDR    0x0d // read-write, no increment
#define CONFIG_PORT2_NOAI_ADDR    0x0e // read-write, no increment

/* ------------------------------------------------------------ *
 * Access register with autoincrement to get all 3 ports in one *
 * E.g. read or write of 3 bytes would program the complete IO. *
 * ------------------------------------------------------------ */
#define INPUT_PORT0_AI_ADDR       0x80 // read-only, autoincrement
#define INPUT_PORT1_AI_ADDR       0x81 // read-only, autoincrement
#define INPUT_PORT2_AI_ADDR       0x82 // read-only, autoincrement

#define OUTPUT_PORT0_AI_ADDR      0x84 // read-write, autoincrement
#define OUTPUT_PORT1_AI_ADDR      0x85 // read-write, autoincrement
#define OUTPUT_PORT2_AI_ADDR      0x86 // read-write, autoincrement

#define POLARITY_PORT0_AI_ADDR    0x88 // read-write, autoincrement
#define POLARITY_PORT1_AI_ADDR    0x89 // read-write, autoincrement
#define POLARITY_PORT2_AI_ADDR    0x8a // read-write, autoincrement

#define CONFIG_PORT0_AI_ADDR      0x8c // read-write, autoincrement
#define CONFIG_PORT1_AI_ADDR      0x8d // read-write, autoincrement
#define CONFIG_PORT2_AI_ADDR      0x8e // read-write, autoincrement

/* ------------------------------------------------------------ *
 * global variables                                             *
 * ------------------------------------------------------------ */
int i2cfd;               // I2C file descriptor
int verbose;             // debug flag, 0 = normal, 1 = debug mode
struct gpiod_chip *chip; // gpiod chip object for GPIO control

/* ------------------------------------------------------------ *
 * external function prototypes for I2C bus communication       *
 * ------------------------------------------------------------ */
extern void get_i2cbus(char*, char*);          // get the I2C bus file handle
extern int tca6424a_dump();                    // dump the register map data
extern int tca6424a_reset();                   // reset the device
extern void tca6424a_info();                   // print port information
extern void tca6424a_allout();                 // all ports and pins as output
extern void tca6424a_setall();                 // set all port pins output=0/1
extern void tca6424a_setout(char *pindata);    // set individual pins as output
extern void tca6424a_setpins(char *pindata);   // set individual pins 0/1
extern void writePort(char port, char data);   // write a byte to output port 1,2 or 3
extern int delay(long msec);                   // create a Arduino-style delay
