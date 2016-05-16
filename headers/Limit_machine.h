/*
 * Limitmachine.h
 *
 *  Created on: May 16, 2016
 *      Author: maxime
 */
#include "../headers/Limit_machine.h"

#ifndef HEADERS_LIMIT_MACHINE_H_
#define HEADERS_LIMIT_MACHINE_H_

class Limit_machine {
public:
	Limit_machine(float Vitesse_max_X, float Vitesse_max_Y, float Vitesse_max_Z, float Vitesse_max_A,
			float Accel_max_X, float Accel_max_Y, float Accel_max_Z, float Accel_max_A) :
				V_mm_max_X(Vitesse_max_X), V_mm_max_Y(Vitesse_max_Y),V_mm_max_Z(Vitesse_max_Z),V_mm_max_A(Vitesse_max_A),
				Acc_mm_max_X(Accel_max_X),Acc_mm_max_Y(Accel_max_Y),Acc_mm_max_Z(Accel_max_Z),Acc_mm_max_A(Accel_max_A){}

	float V_mm_max_X; //En mm/s
	float V_mm_max_Y;
	float V_mm_max_Z;
	float V_mm_max_A;

	float Acc_mm_max_X; //En mm/s/s
	float Acc_mm_max_Y;
	float Acc_mm_max_Z;
	float Acc_mm_max_A;


private:



	/*void compute_Vmm_and_Accmm_to_Vtick_and_Acctick();

	uint64_t _Vitesse_max_tick_per_tick_X; //todo faire constructeur pour remplire variables
	uint64_t _Vitesse_max_tick_per_tick_Y;
	uint64_t _Vitesse_max_tick_per_tick_Z;
	uint64_t _Vitesse_max_tick_per_tick_A;
*/

};

#endif /* HEADERS_LIMIT_MACHINE_H_ */
