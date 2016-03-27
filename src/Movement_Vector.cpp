/*
 * Movement_Vector.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: julien
 */

#include "../headers/Movement_Vector.h"

Movement_Vector::Movement_Vector(int64_t x,int64_t y,int64_t z,int64_t a):Dep_x(x),Dep_y(y),Dep_z(z),Dep_a(a){};

void Movement_Vector::SetVector(int64_t x, int64_t y, int64_t z, int64_t a) {
	Dep_x=x;
	Dep_y=y;
	Dep_z=z;
	Dep_a=a;
}
