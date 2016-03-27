/*
 * Movement_Vector.h
 *
 *  Created on: Mar 15, 2016
 *      Author: julien
 */

#ifndef HEADERS_MOVEMENT_VECTOR_H_
#define HEADERS_MOVEMENT_VECTOR_H_

#include <inttypes.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;


class Movement_Vector {
public:
	int64_t Dep_x;
	int64_t Dep_y;
	int64_t Dep_z;
	int64_t Dep_a;
	bool isAbsolute = false;
	Movement_Vector(int64_t x,int64_t y,int64_t z,int64_t a);
	void SetVector(int64_t x,int64_t y,int64_t z,int64_t a);
};


#endif /* HEADERS_MOVEMENT_VECTOR_H_ */
