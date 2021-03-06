/*
 * Spicomm.h
 *
 *  Created on: Mar 9, 2016
 *      Author: julien
 */

#ifndef SPICOMM_H_
#define SPICOMM_H_
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "Accelconverter.h"

//#define DEBUG_SPI

class Spi_comm {
public:
	Spi_comm(bool simulate);
	virtual ~Spi_comm();
	void transmit_vector(Accel_vector & vect);
	int fd;
	void execute_fifo_list();
	void execute_reset_off();
	void send_reset_FPGA();
	unsigned int get_fifo_fill();
private:
	bool Simulation_mode=false;
	void transfert(char* tx, char* rx,uint32_t lenght);
};

#endif /* SPICOMM_H_ */
