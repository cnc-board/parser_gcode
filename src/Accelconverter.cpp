/*
 * Accelconverter.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: julien
 */

#include <cmath>
#include <iomanip>
#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"
#include "../headers/Zynqcomm.hpp"
#include "../headers/Gcode.h"


Accel_converter::Accel_converter(float Pulse_per_mm_X, float Pulse_per_mm_Y,
								float Pulse_per_mm_Z, float Pulse_per_mm_A, Limit_machine limit_machine) :
								_Pulse_per_mm_X(Pulse_per_mm_X), _Pulse_per_mm_Y(Pulse_per_mm_Y),
								_Pulse_per_mm_Z(Pulse_per_mm_Z), _Pulse_per_mm_A(Pulse_per_mm_A), _Limite_machine(limit_machine)
{
	_acc_tick.x = acc_mm_to_tick(_Limite_machine.Acc_mm_max_X, x);
	_acc_tick.y = acc_mm_to_tick(_Limite_machine.Acc_mm_max_Y, y);
	_acc_tick.z = acc_mm_to_tick(_Limite_machine.Acc_mm_max_Z, z);
	_acc_tick.a = acc_mm_to_tick(_Limite_machine.Acc_mm_max_A, a);

	_vit_tick.x = acc_mm_to_tick(_Limite_machine.V_mm_max_X, x);
	_vit_tick.y = acc_mm_to_tick(_Limite_machine.V_mm_max_Y, y);
	_vit_tick.z = acc_mm_to_tick(_Limite_machine.V_mm_max_Z, z);
	_vit_tick.a = acc_mm_to_tick(_Limite_machine.V_mm_max_A, a);

}

int64_t Accel_converter::mmToTick(double val,  Axis axe) {
	int64_t ret;
	switch(axe)
	{
	case x:
		ret = (int64_t)((double)_Tick_per_pulse*_Pulse_per_mm_X*val);

		if(ret / (_Tick_per_pulse*_Pulse_per_mm_X) != val)
		{
			cout << "Débordement !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		}
		break;
	case y:
		ret = (int64_t)((double)_Tick_per_pulse*_Pulse_per_mm_Y*val);
		break;
	case z:
		ret = (int64_t)((double)_Tick_per_pulse*_Pulse_per_mm_Z*val);
		break;
	case a:
		ret = (int64_t)((double)_Tick_per_pulse*_Pulse_per_mm_A*val);
		break;
	}
	return ret;
}


Accel_converter::~Accel_converter() {
	// TODO Auto-generated destructor stub
}

uint64_t Accel_converter::acc_mm_to_tick(float mm_per_second_square,  Axis axe)
{
	uint64_t ret;
	switch(axe)
	{
	case x:
		ret = round((uint64_t)(_Tick_per_pulse * mm_per_second_square * _Pulse_per_mm_X) / (25000000.0 * 25000000.0));
		break;
	case y:
		ret = round((uint64_t)(_Tick_per_pulse * mm_per_second_square * _Pulse_per_mm_Y) / (25000000.0 * 25000000.0));
		break;
	case z:
		ret = round((uint64_t)(_Tick_per_pulse * mm_per_second_square * _Pulse_per_mm_Z) / (25000000.0 * 25000000.0));
		break;
	case a:
		ret = round((uint64_t)(_Tick_per_pulse * mm_per_second_square * _Pulse_per_mm_A) / (25000000.0 * 25000000.0));
		break;
	}
	return ret;
}

uint64_t Accel_converter::vit_mm_to_tick(float mm_per_second,  Axis axe)
{
	uint64_t ret;
	mm_per_second=abs(mm_per_second);
	switch(axe)
	{
	case x:
		ret = round((uint64_t)(((double)mm_per_second * (double)(_Pulse_per_mm_X * (double)_Tick_per_pulse)) / 25000000.0));
		break;
	case y:
		ret = round((uint64_t)(((double)mm_per_second * (double)(_Pulse_per_mm_Y * (double)_Tick_per_pulse)) / 25000000.0));
		break;
	case z:
		ret = round((uint64_t)(((double)mm_per_second * (double)(_Pulse_per_mm_Z * (double)_Tick_per_pulse)) / 25000000.0));
		break;
	case a:
		ret = round((uint64_t)(((double)mm_per_second * (double)(_Pulse_per_mm_A * (double)_Tick_per_pulse)) / 25000000.0));
		break;
	}
	return ret;
}

void Accel_converter::profileGenerator(Movement_Vector Dvect, Gcode::Class_EtatMachine* Etat) {
	static float _vitesse_X, _vitesse_Y, _vitesse_Z, _vitesse_A;
	static int nb = 0;
	typedef enum axes{X,Y,Z,A} biggest_axes;
	struct {
		uint64_t		acc;
		uint64_t 		vit;
		uint64_t		acc_dist;
		uint64_t		acc_temps;
		int64_t 		dist;
		biggest_axes	name_of_axis;
	}static axe_prioritaire;

	struct {
		uint64_t x = 0;
		uint64_t y = 0;
		uint64_t z = 0;
		uint64_t a = 0;
	}static _acc_temps;

	struct {
		uint64_t x = 0;
		uint64_t y = 0;
		uint64_t z = 0;
		uint64_t a = 0;
	}static _acc_distance;

	int32_t accel_x,accel_y,accel_z;

	uint64_t epoch_begin;
	uint64_t epoch_end_accel;
	uint64_t epoch_end_conti;
	uint64_t epoch_end;


	double facteur_reduc_acc_x; double facteur_reduc_acc_z;
	double facteur_reduc_acc_y; double facteur_reduc_acc_a;

#if DISPLAY_GENE_VECTOR == 1
	cout << endl << "Calcul du vecteur N°" << nb << "  -----------------------------------------------------" << endl << endl;
	nb++;
#endif

	if(_vitesse_X != Etat->VitesseDeplacement_X || _vitesse_Y != Etat->VitesseDeplacement_Y
			|| _vitesse_Z != Etat->VitesseDeplacement_Z || _vitesse_A != Etat->VitesseDeplacement_A)
	{	//todo factoriser "if"

		_vitesse_X = Etat->VitesseDeplacement_X; _vitesse_Y = Etat->VitesseDeplacement_Y;
		_vitesse_Z = Etat->VitesseDeplacement_Z; _vitesse_A = Etat->VitesseDeplacement_A;

		_acc_temps.x = 0;
		_acc_temps.y = 0;
		_acc_temps.z = 0;
		_acc_temps.a = 0;


		// Axe : Y
		_vit_tick.x = vit_mm_to_tick(Etat->VitesseDeplacement_X, x); //Vitesse : changement d'unité (mm/s to tick / tick)
		if(_acc_tick.x == 0) {_acc_temps.x = 0;}
		else { _acc_temps.x =  round((double)(_vit_tick.x / _acc_tick.x));}	//Calcul du temps de l'accélération jusqu'a la vitesse voulue (en ticks)
		//_vit_tick.x = _acc_tick.x * _acc_temps.x;



		// Axe : Y
		_vit_tick.y = vit_mm_to_tick(Etat->VitesseDeplacement_Y, y); //Vitesse : changement d'unité (mm/s to tick / tick)
		if(_acc_tick.y == 0) {_acc_temps.y = 0;}
		else { _acc_temps.y =  round((double)(_vit_tick.y / _acc_tick.y));}	//Calcul du temps de l'accélération jusqu'a la vitesse voulue (en ticks)


		// Axe : Z
		_vit_tick.z = vit_mm_to_tick(Etat->VitesseDeplacement_Z, z); //Vitesse : changement d'unité (mm/s to tick / tick)
		if(_acc_tick.z == 0) {_acc_temps.z = 0;}
		else { _acc_temps.z =  round((double)(_vit_tick.z / _acc_tick.z));}	//Calcul du temps de l'accélération jusqu'a la vitesse voulue (en ticks)


		// Axe : A
		_vit_tick.a = vit_mm_to_tick(Etat->VitesseDeplacement_A, a); //Vitesse : changement d'unité (mm/s to tick / tick)
		if(_acc_tick.a == 0) {_acc_temps.a = 0;}
		else { _acc_temps.a =  round((double)(_vit_tick.a / _acc_tick.a));}	//Calcul du temps de l'accélération jusqu'a la vitesse voulue (en ticks)



#if DISPLAY_GENE_VECTOR == 1
		cout << endl;
		cout << "Changement de vitesse, re-calul des constantes :" << endl;
		cout << "Axe                        : X" << endl;
		cout << "Accélération de            : " << _acc_tick.x << endl;
		cout << "Vitesse cible              : " << _vit_tick.x << endl;
		cout << "Temps de l'accélération    : " << _acc_temps.x << endl;
		//cout << "Temps de l'accélération f  : " << setprecision(16) << acc_temps<< endl;
		cout << "déplacement                : " << Dvect.Dep_x << endl;
		cout << "distance d'acc             : " << (_acc_tick.x / 2) * _acc_temps.x * _acc_temps.x << endl;

		cout << "Axe                        : Y" << endl;
		cout << "Accélération de            : " << _acc_tick.y << endl;
		cout << "Vitesse cible              : " << _vit_tick.y << endl;
		cout << "Temps de l'accélération    : " << _acc_temps.y << endl;
		cout << "déplacement                : " << Dvect.Dep_y << endl;
		cout << "distance d'acc             : " << (_acc_tick.y / 2) * _acc_temps.y * _acc_temps.y << endl;

		cout << "Axe                        : Z" << endl;
		cout << "Accélération de            : " << _acc_tick.z << endl;
		cout << "Vitesse cible              : " << _vit_tick.z << endl;
		cout << "Temps de l'accélération    : " << _acc_temps.z << endl;

		cout << "Axe                        : A" << endl;
		cout << "Accélération de            : " << _acc_tick.a << endl;
		cout << "Vitesse cible              : " << _vit_tick.a << endl;
		cout << "Temps de l'accélération    : " << _acc_temps.a << endl;
#endif
	}


	//Recherche de l'axe prioritaire (temps d'accélération le plus long)
	if(_acc_temps.y > _acc_temps.x){
		if(_acc_temps.z > _acc_temps.y)
		{
#if DISPLAY_GENE_VECTOR == 1
			cout << "Axe prioritaire : Z" << endl;
#endif
			axe_prioritaire.name_of_axis = Z;
			axe_prioritaire.acc_temps = _acc_temps.z;
			axe_prioritaire.acc = _acc_tick.z;
			axe_prioritaire.vit = _vit_tick.z;
			axe_prioritaire.dist = Dvect.Dep_z;
		}
		else
		{
#if DISPLAY_GENE_VECTOR == 1
			cout << "Axe prioritaire : Y" << endl;
#endif
			axe_prioritaire.name_of_axis = Y;
			axe_prioritaire.acc_temps = _acc_temps.y;
			axe_prioritaire.acc = _acc_tick.y;
			axe_prioritaire.vit = _vit_tick.y;
			axe_prioritaire.dist = Dvect.Dep_y;
		}
	}
	else if(_acc_temps.z > _acc_temps.x){

#if DISPLAY_GENE_VECTOR == 1
		cout << "Axe prioritaire : Z" << endl;
#endif
		axe_prioritaire.name_of_axis = Z;
		axe_prioritaire.acc_temps = _acc_temps.z;
		axe_prioritaire.acc = _acc_tick.z;
		axe_prioritaire.vit = _vit_tick.z;
		axe_prioritaire.dist = Dvect.Dep_z;
	}
	else {
#if DISPLAY_GENE_VECTOR == 1
		cout << "Axe prioritaire : X" << endl;
#endif
		axe_prioritaire.name_of_axis = X;
		axe_prioritaire.acc_temps = _acc_temps.x;
		axe_prioritaire.acc = _acc_tick.x;
		axe_prioritaire.vit = _vit_tick.x;
		axe_prioritaire.dist = Dvect.Dep_x;
	}

	axe_prioritaire.acc_dist = (axe_prioritaire.acc / 2) * axe_prioritaire.acc_temps * axe_prioritaire.acc_temps;


#if DISPLAY_GENE_VECTOR == 1
	cout << endl << "Calcul des facteurs de réduction des accélérations : " << endl;
#endif

	facteur_reduc_acc_x = (double)((double)_vit_tick.x / (double)((_acc_tick.x==0?1:_acc_tick.x) * (axe_prioritaire.acc_temps==0?1:axe_prioritaire.acc_temps)));
	facteur_reduc_acc_y = (double)((double)_vit_tick.y / (double)((_acc_tick.y==0?1:_acc_tick.y) * (axe_prioritaire.acc_temps==0?1:axe_prioritaire.acc_temps)));
	facteur_reduc_acc_z = (double)((double)_vit_tick.z / (double)((_acc_tick.z==0?1:_acc_tick.z) * (axe_prioritaire.acc_temps==0?1:axe_prioritaire.acc_temps)));
	facteur_reduc_acc_a = (double)((double)_vit_tick.a / (double)((_acc_tick.a==0?1:_acc_tick.a) * (axe_prioritaire.acc_temps==0?1:axe_prioritaire.acc_temps)));

#if DISPLAY_GENE_VECTOR == 1
	cout << "facteur axe X : " << facteur_reduc_acc_x <<endl;
	cout << "facteur axe Y : " << facteur_reduc_acc_y <<endl;
	cout << "facteur axe Z : " << facteur_reduc_acc_z <<endl;
	cout << "facteur axe A : " << facteur_reduc_acc_a <<endl;
#endif

	epoch_begin=epoch_present;

	if((uint64_t)abs(axe_prioritaire.dist)>axe_prioritaire.acc_dist*2)
		{
			switch(axe_prioritaire.name_of_axis)
					{
					case X:
						accel_x=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;
						accel_y=Dvect.Dep_y==0?0:(int32_t)((double)_acc_tick.y * (Dvect.Dep_y>0?facteur_reduc_acc_y:-facteur_reduc_acc_y));
						accel_z=Dvect.Dep_z==0?0:(int32_t)((double)_acc_tick.z * (Dvect.Dep_z>0?facteur_reduc_acc_z:-facteur_reduc_acc_z));
						break;
					case Y:
						accel_y=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;
						accel_x=Dvect.Dep_x==0?0:(int32_t)((double)_acc_tick.x * (Dvect.Dep_x>0?facteur_reduc_acc_x:-facteur_reduc_acc_x));
						accel_z=Dvect.Dep_z==0?0:(int32_t)((double)_acc_tick.z * (Dvect.Dep_z>0?facteur_reduc_acc_z:-facteur_reduc_acc_z));
						break;
					case Z:
						accel_z=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;
						accel_x=Dvect.Dep_x==0?0:(int32_t)((double)_acc_tick.x * (Dvect.Dep_x>0?facteur_reduc_acc_x:-facteur_reduc_acc_x));
						accel_y=Dvect.Dep_y==0?0:(int32_t)((double)_acc_tick.y * (Dvect.Dep_y>0?facteur_reduc_acc_y:-facteur_reduc_acc_y));
						break;
					case A:
						break;
					}


			epoch_end_accel = epoch_begin + axe_prioritaire.acc_temps;
			epoch_end_conti = epoch_end_accel + (double)(abs(axe_prioritaire.dist) - 2 * axe_prioritaire.acc_dist) / (double)axe_prioritaire.vit; //peut etre une erreure ??
			epoch_end = epoch_end_conti + axe_prioritaire.acc_temps;
			epoch_present = epoch_end;

			Accel_vectors.push_back(new Accel_vector(accel_x,accel_y,accel_z,0,epoch_begin,epoch_end_accel));
			Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_end_accel,epoch_end_conti));
			Accel_vectors.push_back(new Accel_vector(-accel_x,-accel_y,-accel_z,0,epoch_end_conti,epoch_end));

		}
	else
	{
					switch(axe_prioritaire.name_of_axis)
							{
							case X:
								accel_x=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;//todo gestion si deplacement x00y00z00
								accel_y=Dvect.Dep_y==0?0:(int32_t)((double)_acc_tick.y * (Dvect.Dep_y>0?facteur_reduc_acc_y:-facteur_reduc_acc_y));
								accel_z=Dvect.Dep_z==0?0:(int32_t)((double)_acc_tick.z * (Dvect.Dep_z>0?facteur_reduc_acc_z:-facteur_reduc_acc_z));
								break;
							case Y:
								accel_y=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;
								accel_x=Dvect.Dep_x==0?0:(int32_t)((double)_acc_tick.x * (Dvect.Dep_x>0?facteur_reduc_acc_x:-facteur_reduc_acc_x));
								accel_z=Dvect.Dep_z==0?0:(int32_t)((double)_acc_tick.z * (Dvect.Dep_z>0?facteur_reduc_acc_z:-facteur_reduc_acc_z));
								break;
							case Z:
								accel_z=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;
								accel_x=Dvect.Dep_x==0?0:(int32_t)((double)_acc_tick.x * (Dvect.Dep_x>0?facteur_reduc_acc_x:-facteur_reduc_acc_x));
								accel_y=Dvect.Dep_y==0?0:(int32_t)((double)_acc_tick.y * (Dvect.Dep_y>0?facteur_reduc_acc_y:-facteur_reduc_acc_y));
								break;
							case A:
								break;

							}

					uint64_t time_accel=0;

					time_accel = sqrt((double)((double)(abs(axe_prioritaire.dist)) / (double)axe_prioritaire.acc));

					epoch_end_accel = epoch_begin + time_accel;
					epoch_end = epoch_end_accel + time_accel;
					epoch_present = epoch_end;
					Accel_vectors.push_back(new Accel_vector(accel_x,accel_y,accel_z,0,epoch_begin,epoch_end_accel));
					Accel_vectors.push_back(new Accel_vector(-accel_x,-accel_y,-accel_z,0,epoch_end_accel,epoch_end));
	}
}



bool Accel_converter::generate_tick_vector(Gcode::TabEtatMachine & tabetat) {
	bool first=true;
	Movement_Vector vect(0,0,0,0);
	Movement_Vector prevvect(0,0,0,0);
	unsigned int iter=0;
	Accel_vectors.push_back(new Accel_vector(0,0,0,0,0,epoch_present+=1000));//empty begin vector
	for(Gcode::TabEtatMachine::iterator it = tabetat.begin(); it != tabetat.end(); it++) {
	    if((*it)->Deplacement==false) continue;
	    if(first)
	    {
	    	vect.SetVector(	mmToTick((*it)->PosOutil_X, x),mmToTick((*it)->PosOutil_Y, y),
	    					mmToTick((*it)->PosOutil_Z, z),mmToTick((*it)->PosOutil_A, a));
	    }
	    else
	    {
	    	vect.SetVector(	mmToTick((*it)->PosOutil_X, x) - prevvect.Dep_x, mmToTick((*it)->PosOutil_Y, y) - prevvect.Dep_y,
	    		    		mmToTick((*it)->PosOutil_Z, z) - prevvect.Dep_z, mmToTick((*it)->PosOutil_A, a) - prevvect.Dep_a);
	    }
	    	prevvect.SetVector(	mmToTick((*it)->PosOutil_X, x),mmToTick((*it)->PosOutil_Y, y),
					mmToTick((*it)->PosOutil_Z, z),mmToTick((*it)->PosOutil_A, a));
	    profileGenerator(vect, (*it));
	    first=false;
	    iter++;
	}
	Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_present,epoch_present+1000));//empty end vector
	Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_present+1000,epoch_present+2000));//empty end vector
	cout << " Done" << endl;
	return false;
}



void Accel_converter::print()
{
	/*uint64_t v = 0, p = 0, Ttick_to_acceldeccel=0, speed=0, Dtick_to_acceldeccel=0;


	do
	{
		Ttick_to_acceldeccel++;
		speed+=accelDeccel;
		Dtick_to_acceldeccel+=speed;
	} while (speed<Vmax);


	for(int64_t time_accel=0; dist_accel<target; time_accel+=1)
		{
			speed+=accelDeccel;
			dist_accel+=speed;
		}


		deltap=(a*dt^2)/2
		dt²=sqrt((deltap/a)*2)
		do
		{
			time_accel++;
			speed+=accelDeccel;
			dist_accel+=speed;
		} while (dist_accel<target);

	do{
		v = v + 36;
		p = p + v;
	}while(1);
	*/
	int i;
	for (i = 0; i < (int)Accel_vectors.size(); i++)
	{
		Accel_vectors[i]->printvect();
	}
}

void Accel_converter::printcsv() {
	int i;
	uint32_t j;
	cout << "CSV" << endl;
	for (i = 0; i < (int)Accel_vectors.size(); i++)
		{
			for (j = 0; j < Accel_vectors[i]->epoch_stop-Accel_vectors[i]->epoch_start; ++j) {
				cout << Accel_vectors[i]->Accel_x << ',' << Accel_vectors[i]->Accel_y << endl;
			}
		}
}

void Accel_converter::sendVectors(Zynq_comm & comm) {
	int i=0;
	char first=1;
	for(TabAccelVector::iterator it = Accel_vectors.begin(); it != Accel_vectors.end(); it++) {
		//cout << "send vector ok !!!!!" <<endl;
		comm.transmit_vector(*(*it));
		i++;
		if(i>5)
		{
			i=0;
			while(comm.get_fifo_fill()>70)
			{
				if(first==1)
				{
					comm.execute_fifo_list();
					first=0;
				}
				cout << "\rFIFO fill : " << comm.get_fifo_fill() << "%" <<flush;
				usleep(250000);
			}
			cout << "\rFIFO fill : " << comm.get_fifo_fill() << "%" <<flush;
		}
	}
	cout << "End transmit" << endl;
}
