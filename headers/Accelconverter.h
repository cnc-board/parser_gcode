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
#include <math.h>
#include "../headers/Gcode.h"
#include "../headers/Accel_Vector.h"
#include "../headers/Movement_Vector.h"
using namespace std;

class Accel_vector;
class Movement_Vector;

class Spi_comm;
class Accel_converter {
private:
	uint64_t epoch_present=0;
	int64_t mmToTickX(double val);
	int64_t mmToTickY(double val);
	int64_t mmToTickZ(double val);
	int64_t mmToTickA(double val);
	void profileGenerator(Movement_Vector Dvect);
	double _convertFactorMmToTick_X, _convertFactorMmToTick_Y, _convertFactorMmToTick_Z, _convertFactorMmToTick_A;
	int32_t accelDeccel=1548;//TODO find a spot to store accel/deccel/vmax params
	uint64_t Vmax=0x4816f0068; //TODO create profileGen class
	uint64_t Ttick_to_acceldeccel=0;
	uint64_t Dtick_to_acceldeccel=0;

public:
	TabAccelVector Accel_vectors;
	Accel_converter(double convertFactorMmToTick_X, double convertFactorMmToTick_Y, double convertFactorMmToTick_Z, double convertFactorMmToTick_A);
	void calculateAccelTick();
	virtual ~Accel_converter();
	bool generate_tick_vector(Gcode::TabEtatMachine& tabetat);
	void print();
	void printcsv();
	void sendVectors(Spi_comm & comm);

};


#endif /* ACCELCONVERTER_H_ */
