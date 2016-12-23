/*
 * Zynqcomm.h
 *
 *  Created on: Mar 9, 2016
 *      Author: julien
 */

#ifndef ZYNQCOMM_H_
#define ZYNQCOMM_H_
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include "Accelconverter.h"
#include "Position_Vector.hpp"

#define R_0 0x00
#define R_1 0x01
#define R_2 0x02
#define R_3 0x03
#define R_4 0x04
#define R_control R_0
#define R_position R_4
#define bit_write_fifo 31
#define bit_startcycle 30

class Zynq_comm {
public:
Zynq_comm(bool simulate);
virtual ~Zynq_comm();
void transmit_vector(Accel_vector & vect);
void execute_fifo_list();
void execute_reset_off();
void send_reset_FPGA();
unsigned int get_fifo_fill();
Position_Vector get_axe_position();
private:
int comm_FD=-1;
bool Simulation_mode=false;
uint32_t* generator_addr = NULL;
int check_comm();
};

#endif /* ZYNQCOMM_H_ */
