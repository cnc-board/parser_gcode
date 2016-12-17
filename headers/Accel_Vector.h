/*
 * Accel_Vector.h
 *
 *  Created on: Mar 15, 2016
 *      Author: julien
 */


#ifndef ACCELVECTOR_H_
#define ACCELVECTOR_H_

#include <inttypes.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;


class Accel_vector {
public:
Accel_vector(int16_t Accel_x,int16_t Accel_y,int16_t Accel_z,int16_t Accel_a,uint64_t epoch_start,uint64_t epoch_stop);//,uint32_t epoch_stop);
//private:
	int16_t Accel_x;
	int16_t Accel_y;
	int16_t Accel_z;
	int16_t Accel_a;
	uint64_t epoch_start;
	uint64_t epoch_stop;
	void printvect();
};

typedef std::vector<Accel_vector*> TabAccelVector;

#endif /* ACCELVECTOR_H_ */
