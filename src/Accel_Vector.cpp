/*
 * Accel_Vector.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: julien
 */

#include "../headers/Accel_Vector.h"

Accel_vector::Accel_vector(int32_t Accel_x,int32_t Accel_y,int32_t Accel_z,int32_t Accel_a,uint64_t epoch_start,uint64_t epoch_stop)//,uint32_t epoch_stop
			:
		Accel_x(Accel_x), Accel_y(Accel_y), Accel_z(Accel_z), Accel_a(Accel_a), epoch_start(
				epoch_start),epoch_stop(epoch_stop) { //, epoch_stop(epoch_stop)
};

void Accel_vector::printvect() {
	cout << "Accel x : "<< Accel_x << " Accel y : "<< Accel_y<< " Accel z : "<< Accel_z << " Epoch start : " << epoch_start << " Epoch stop : " << epoch_stop << endl;
}
