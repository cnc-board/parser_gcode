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
#include "../headers/Limit_machine.h"

#define FREQUENCE_FPGA 25000000
using namespace std;

class Accel_vector;
class Movement_Vector;

class Spi_comm;
class Accel_converter {
private:
	uint64_t epoch_present=0;
	typedef enum axes{x,y,z,a} Axis;

	int64_t mmToPulse(double val,  Axis axe);

	int64_t mm_per_second_square_to_pulse_per_tick_square(float mm_per_second_square, Axis axe);

	int64_t pulse_per_tick_square_X;
	int64_t pulse_per_tick_square_Y;
	int64_t pulse_per_tick_square_Z;
	int64_t pulse_per_tick_square_A;

	double _convertFactorMmToTick_X;
	double _convertFactorMmToTick_Y;
	double _convertFactorMmToTick_Z;
	double _convertFactorMmToTick_A;

	Limit_machine _Limite_machine;




	void profileGenerator(Movement_Vector Dvect, Gcode::Class_EtatMachine* Etat);

	int32_t accelDeccel=1548;//TODO find a spot to store accel/deccel/vmax params
	uint64_t Vmax=0x4816f0068; //TODO create profileGen class

	struct Axe { // temps et distance d'accélération en tick
		struct {
			uint64_t Accel_X = 0;
			uint64_t Accel_Y = 0;
			uint64_t Accel_Z = 0;
			uint64_t Accel_A = 0;
		} Temps;
		struct {
			uint64_t Accel_X = 0;
			uint64_t Accel_Y = 0;
			uint64_t Accel_Z = 0;
			uint64_t Accel_A = 0;
		} Distance;
	};

	Axe _axe;

	uint64_t Ttick_to_accel=0;
			uint64_t Dtick_to_accel=0;

public:
	TabAccelVector Accel_vectors;
	Accel_converter(double convertFactorMmToTick_X, double convertFactorMmToTick_Y, double convertFactorMmToTick_Z, double convertFactorMmToTick_A, Limit_machine limit_machine);
	void calculateAccelTick();
	virtual ~Accel_converter();
	bool generate_tick_vector(Gcode::TabEtatMachine& tabetat);
	void print();
	void printcsv();
	void sendVectors(Spi_comm & comm);

};


#endif /* ACCELCONVERTER_H_ */
