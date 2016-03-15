/*
 * Accelconverter.h
 *
 *  Created on: Feb 29, 2016
 *      Author: julien
 */

#ifndef ACCELCONVERTER_H_
#define ACCELCONVERTER_H_
#include <inttypes.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "../headers/Gcode.h"
class Accel_vector;
class DeplacementVector;
typedef std::vector<Accel_vector*> TabAccelVector;
class Spi_comm;
class Accel_converter {
private:
	TabAccelVector Accel_vectors;
	uint64_t epoch_present=0;
	int64_t mmToTick(double val);
	void profileGenerator(DeplacementVector Dvect);
	double convertFactorMmToTick=127.896942801*8796093022208.0;
	int32_t accelDeccel=1548;//TODO find a spot to store accel/deccel/vmax params
	uint64_t Vmax=0x4816f0068; //TODO create profileGen class
	uint64_t Ttick_to_acceldeccel=0;
	uint64_t Dtick_to_acceldeccel=0;

public:
	Accel_converter();
	void calculateAccelTick();
	virtual ~Accel_converter();
	bool generate_tick_vector(Gcode::TabEtatMachine& tabetat);
	void afficher();
	void printcsv();
	void sendVectors(Spi_comm & comm);

};

class DeplacementVector {
public:
	int64_t Dep_x;
	int64_t Dep_y;
	int64_t Dep_z;
	int64_t Dep_a;
	DeplacementVector(int64_t x,int64_t y,int64_t z,int64_t a);
	void SetVector(int64_t x,int64_t y,int64_t z,int64_t a);
private:
};

class Accel_vector {
public:
Accel_vector(int32_t Accel_x,int32_t Accel_y,int32_t Accel_z,int32_t Accel_a,uint64_t epoch_start,uint64_t epoch_stop);//,uint32_t epoch_stop);
//private:
	int32_t Accel_x;
	int32_t Accel_y;
	int32_t Accel_z;
	int32_t Accel_a;
	uint64_t epoch_start;
	uint64_t epoch_stop;
	void printvect();
};


#endif /* ACCELCONVERTER_H_ */
