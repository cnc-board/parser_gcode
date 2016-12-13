
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include "Accelconverter.h"

#define R_0 0x00
#define R_1 0x01
#define R_2 0x02
#define R_3 0x03
#define R_control R_0
#define bit_write_fifo 31
#define bit_startcycle 30
int open_driver_comm();

void transmit_vector(Accel_vector& vect);

void execute_fifo_list();

void execute_reset_off();

void send_reset_FPGA();

unsigned int get_fifo_fill();

int check_comm();

int close_driver_comm();
