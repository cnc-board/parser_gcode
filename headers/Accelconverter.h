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
	uint32_t epoch_present=0;
	int32_t mmToTick(double val);
	void profileGenerator(DeplacementVector Dvect);
	double convertFactorMmToTick=100.0;
	int32_t accelDeccel=10;//TODO find a spot to store accel/deccel/vmax params
	int32_t Vmax=100; //TODO create profileGen class
	int32_t Ttick_to_acceldeccel=0;
	int32_t Dtick_to_acceldeccel=0;

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
	int32_t Dep_x;
	int32_t Dep_y;
	int32_t Dep_z;
	int32_t Dep_a;
	DeplacementVector(int32_t x,int32_t y,int32_t z,int32_t a);
	void SetVector(int32_t x,int32_t y,int32_t z,int32_t a);
private:
};

class Accel_vector {
public:
Accel_vector(int32_t Accel_x,int32_t Accel_y,int32_t Accel_z,int32_t Accel_a,uint32_t epoch_start,uint32_t epoch_stop);//,uint32_t epoch_stop);
//private:
	int32_t Accel_x;
	int32_t Accel_y;
	int32_t Accel_z;
	int32_t Accel_a;
	uint32_t epoch_start;
	void printvect();
	uint32_t epoch_stop;
};


#endif /* ACCELCONVERTER_H_ */
