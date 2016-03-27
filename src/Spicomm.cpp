/*
 * Spicomm.cpp
 *
 *  Created on: Mar 9, 2016
 *      Author: julien
 */
#include "../headers/Spicomm.h"

#include <string.h>

Spi_comm::Spi_comm() {
	cout << "spi_const" << endl;
#ifndef DEBUG_SPI
	int ret;
	static uint8_t mode = 3;
	static uint8_t bits = 8;
	static uint32_t speed = 1000000;
	fd = open("/dev/spidev32766.2", O_RDWR);
	if (fd < 0) {
		cerr << "can't open device" << endl;
		exit(-1);
	}

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1) {
		cerr << "can't set spi mode" << endl;
		exit(-1);
	}

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1) {
		cerr << "can't get spi mode" << endl;
		exit(-1);
	}

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		cerr << "can't set bits per word" << endl;
		exit(-1);
	}

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1) {
		cerr << "can't get bits per word" << endl;
		exit(-1);
	}

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		cerr << "can't set max speed hz" << endl;
		exit(-1);
	}

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		cerr << "can't get max speed hz" << endl;
		exit(-1);
	}
#endif

}

Spi_comm::~Spi_comm() {
	cout << "spicomm dest" << endl;
	close(fd);
}

void Spi_comm::transmit_vector(Accel_vector& vect) {
	char tx[1+2+2+2+2+6];
	char rx[1+2+2+2+2+6];
	tx[0]=0xA0;

	tx[1]=vect.Accel_x&0xFF;
	tx[2]=(vect.Accel_x>>8)&0xFF;
	tx[3]=vect.Accel_y&0xFF;
	tx[4]=(vect.Accel_y>>8)&0xFF;
	tx[5]=vect.Accel_z&0xFF;
	tx[6]=(vect.Accel_z>>8)&0xFF;
	tx[7]=vect.Accel_a&0xFF;
	tx[8]=(vect.Accel_a>>8)&0xFF;


	tx[9]=vect.epoch_stop&0xFF;
	tx[10]=(vect.epoch_stop>>8)&0xFF;
	tx[11]=(vect.epoch_stop>>16)&0xFF;
	tx[12]=(vect.epoch_stop>>24)&0xFF;
	tx[13]=(vect.epoch_stop>>32)&0xFF;
	tx[14]=(vect.epoch_stop>>40)&0xFF;

	transfert(tx,rx,1+2+2+2+2+6);


}

void Spi_comm::execute_fifo_list() {
	char tx[1];
		char rx[1];
		tx[0]=0xB2;
		transfert(tx,rx,1);
}

void Spi_comm::execute_reset_off() {
	char tx[1];
		char rx[1];
		tx[0]=0xB3;
		transfert(tx,rx,1);
}

void Spi_comm::transfert(char* tx, char* rx,uint32_t lenght) {

	struct spi_ioc_transfer tr[lenght];
	//memset(&tr, 0, sizeof(tr));
	for (uint32_t i = 0; i <lenght; i++) {
		memset(&(tr[i]), 0, sizeof(tr[i]));
		tr[i].tx_buf=(unsigned long)(&(tx[i]));
		tr[i].rx_buf=(unsigned long)(&(rx[i]));
		tr[i].len=1;
		//tr[i].speed_hz=1000000;
		//tr[i].delay_usecs=0;
		//tr[i].bits_per_word=8;
		//tr[i].cs_change=1;

	}
	//cout << " lenght : " << tr.len << endl;
//#ifdef DEBUG_SPI
/*	for (uint32_t ret = 0; ret < lenght; ret++) {
			if (!(ret % 6))
				puts("");
			printf("%.2X ", tx[ret]);
		}
	cout << endl;*/
//#else
	//tr.tx_buf=(__u64)tx;

	/*struct spi_ioc_transfer tr = {
			tx_buf : (__u64)tx,
			rx_buf : (__u64)rx,
			len : lenght,
			speed_hz : 0,
			delay_usecs : 0,
			bits_per_word : 0,
//			cs_change : NULL,
//			tx_nbits : NULL,
//			rx_nbits : NULL,
//			pad : NULL
	};*/

	int ret = ioctl(fd, SPI_IOC_MESSAGE(lenght), tr);
		if (ret < 0)
		{
			perror("spi error");
			cout << "ret : " << ret << endl;
			exit(-2);
		}



//#endif


}

unsigned int Spi_comm::get_fifo_fill() {
	char tx[2];
	char rx[2];
	tx[0]=0xA1;
	transfert(tx,rx,2);
	return (rx[1]*100)/255;
}
