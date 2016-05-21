/*
 * Accelconverter.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: julien
 */

#include <cmath>
#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"
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

uint64_t Accel_converter::mmToTick(double val,  Axis axe) {
	long double ret;
	switch(axe)
	{
	case x:
		ret = (uint64_t)((long double)val*(long double)_Pulse_per_mm_X*(long double)_Tick_per_pulse);
		if(ret / ((long double)_Pulse_per_mm_X*(long double)_Tick_per_pulse) != (long double)val)
		{
			cout << "Débordement !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		}
		break;
	case y:
		ret = val*_Pulse_per_mm_X*_Tick_per_pulse;
		break;
	case z:
		ret = val*_Pulse_per_mm_X*_Tick_per_pulse;
		break;
	case a:
		ret = val*_Pulse_per_mm_X*_Tick_per_pulse;
		break;
	}
	return (uint64_t)ret;
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
		uint64_t 		temps_mouvement;
		biggest_axes	name_of_axis;
	}axe_prioritaire;


	int32_t accel_x,accel_y,accel_z;

	uint64_t epoch_begin;
	uint64_t epoch_end_accel;
	uint64_t epoch_end_conti;
	uint64_t epoch_end;


	uint64_t temps_mouvement_X;
	uint64_t temps_mouvement_Y;
	uint64_t temps_mouvement_Z;
	uint64_t temps_mouvement_A;
	double facteur_reduc_acc_x; double facteur_reduc_acc_z;
	double facteur_reduc_acc_y; double facteur_reduc_acc_a;

	cout << endl << "Calcul du vecteur N°" << nb << "  -----------------------------------------------------" << endl << endl;
	nb++;

	if(_vitesse_X != Etat->VitesseDeplacement_X || _vitesse_Y != Etat->VitesseDeplacement_Y
			|| _vitesse_Z != Etat->VitesseDeplacement_Z || _vitesse_A != Etat->VitesseDeplacement_A)
	{	//todo factoriser "if"

		_vitesse_X = Etat->VitesseDeplacement_X; _vitesse_Y = Etat->VitesseDeplacement_Y;
		_vitesse_Z = Etat->VitesseDeplacement_Z; _vitesse_A = Etat->VitesseDeplacement_A;

		_acc_distance.x = 0; _acc_temps.x = 0;
		_acc_distance.y = 0; _acc_temps.y = 0;
		_acc_distance.z = 0; _acc_temps.z = 0;
		_acc_distance.a = 0; _acc_temps.a = 0;


		// Axe : X
		_vit_tick.x = vit_mm_to_tick(Etat->VitesseDeplacement_X, x); //Vitesse : changement d'unité (mm/s to tick / tick)
		if(_acc_tick.x == 0) {_acc_temps.x = 0;}
		else { _acc_temps.x =  round((double)(_vit_tick.x / _acc_tick.x));}	//Calcul du temps de l'accélération jusqu'a la vitesse voulue (en ticks)
		_acc_distance.x = (_acc_tick.x / 2) * _acc_temps.x * _acc_temps.x;//Calul de la distance parcourue pendant la phase d'accélération (en tiks)

		// Axe : Y
		_vit_tick.y = vit_mm_to_tick(Etat->VitesseDeplacement_Y, y); //Vitesse : changement d'unité (mm/s to tick / tick)
		if(_acc_tick.y == 0) {_acc_temps.y = 0;}
		else { _acc_temps.y =  round((double)(_vit_tick.y / _acc_tick.y));}	//Calcul du temps de l'accélération jusqu'a la vitesse voulue (en ticks)
		_acc_distance.y = (_acc_tick.y / 2) * _acc_temps.y * _acc_temps.y;//Calul de la distance parcourue pendant la phase d'accélération (en tiks)

		// Axe : Z
		_vit_tick.z = vit_mm_to_tick(Etat->VitesseDeplacement_Z, z); //Vitesse : changement d'unité (mm/s to tick / tick)
		if(_acc_tick.z == 0) {_acc_temps.z = 0;}
		else { _acc_temps.z =  round((double)(_vit_tick.z / _acc_tick.z));}	//Calcul du temps de l'accélération jusqu'a la vitesse voulue (en ticks)
		_acc_distance.z = (_acc_tick.z / 2) * _acc_temps.z * _acc_temps.z;//Calul de la distance parcourue pendant la phase d'accélération (en tiks)

		// Axe : A
		_vit_tick.a = vit_mm_to_tick(Etat->VitesseDeplacement_A, a); //Vitesse : changement d'unité (mm/s to tick / tick)
		if(_acc_tick.a == 0) {_acc_temps.a = 0;}
		else { _acc_temps.a =  round((double)(_vit_tick.a / _acc_tick.a));}	//Calcul du temps de l'accélération jusqu'a la vitesse voulue (en ticks)
		_acc_distance.a = (_acc_tick.a / 2) * _acc_temps.a * _acc_temps.a;//Calul de la distance parcourue pendant la phase d'accélération (en tiks)


#if DISPLAY_GENE_VECTOR == 1
		cout << endl;
		cout << "Changement de vitesse, re-calul des constantes :" << endl;
		cout << "Axe                        : X" << endl;
		cout << "Accélération de            : " << _acc_tick.x << endl;
		cout << "Vitesse cible              : " << _vit_tick.x << endl;
		cout << "Temps de l'accélération    : " << _acc_temps.x << endl;
		cout << "Distance parcourue         : " << _acc_distance.x << endl<<endl;

		cout << "Axe                        : Y" << endl;
		cout << "Accélération de            : " << _acc_tick.y << endl;
		cout << "Vitesse cible              : " << _vit_tick.y << endl;
		cout << "Temps de l'accélération    : " << _acc_temps.y << endl;
		cout << "Distance parcourue         : " << _acc_distance.y << endl<<endl;

		cout << "Axe                        : Z" << endl;
		cout << "Accélération de            : " << _acc_tick.z << endl;
		cout << "Vitesse cible              : " << _vit_tick.z << endl;
		cout << "Temps de l'accélération    : " << _acc_temps.z << endl;
		cout << "Distance parcourue         : " << _acc_distance.z << endl<<endl;

		cout << "Axe                        : A" << endl;
		cout << "Accélération de            : " << _acc_tick.a << endl;
		cout << "Vitesse cible              : " << _vit_tick.a << endl;
		cout << "Temps de l'accélération    : " << _acc_temps.a << endl;
		cout << "Distance parcourue         : " << _acc_distance.a << endl<<endl;
#endif
	}


	//Calcul le temps que pouraient mettre chaque axe à bouger s'ils étaient seuls (sans facteur de correction)
	if((uint64_t)abs(Dvect.Dep_x)>_acc_distance.x*2) //si on peut faire 3 phases (acc ---- desc)
	{
		temps_mouvement_X = _vit_tick.x>0?_acc_temps.x * 2 + (abs(Dvect.Dep_x) / _vit_tick.x - _acc_temps.x * 2):0;
	}
	else
	{
		temps_mouvement_X =  _acc_tick.x>0?sqrt((double)(abs(Dvect.Dep_x) / _acc_tick.x)) * 2:0;
	}

	if((uint64_t)abs(Dvect.Dep_y)>_acc_distance.y*2) //si on peut faire 3 phases (acc ---- desc)
	{
		temps_mouvement_Y = _acc_temps.y * 2 + (abs(Dvect.Dep_y) / _vit_tick.y - _acc_temps.y * 2);
	}
	else
	{
		temps_mouvement_Y = sqrt((double)(abs(Dvect.Dep_y) / _acc_tick.y)) * 2;
	}

	if((uint64_t)abs(Dvect.Dep_z)>_acc_distance.z*2) //si on peut faire 3 phases (acc ---- desc)
	{
		temps_mouvement_Z = _acc_temps.z * 2 + (abs(Dvect.Dep_z) / _vit_tick.z - _acc_temps.z * 2);
	}
	else
	{
		temps_mouvement_Z = sqrt((double)(abs(Dvect.Dep_z) / _acc_tick.z)) * 2;
	}

	if((uint64_t)abs(Dvect.Dep_a)>_acc_distance.a*2) //si on peut faire 3 phases (acc ---- desc)
	{
		temps_mouvement_A =_vit_tick.a>0? _acc_temps.a * 2 + (abs(Dvect.Dep_a) / _vit_tick.a - _acc_temps.a * 2):0;
	}
	else
	{
		temps_mouvement_A =_acc_tick.a>0? sqrt((double)(abs(Dvect.Dep_a) / _acc_tick.a)) * 2:0;
	}

	cout << "Temps du mouvement sur l'axe X : " << temps_mouvement_X << endl;
	cout << "Temps du mouvement sur l'axe Y : " << temps_mouvement_Y << endl;
	cout << "Temps du mouvement sur l'axe Z : " << temps_mouvement_Z << endl;
	cout << "Temps du mouvement sur l'axe A : " << temps_mouvement_A << endl;

	//Recherche de l'axe prioritaire (temps de mouvement le plus long)
	if(temps_mouvement_Y > temps_mouvement_X){
		if(temps_mouvement_Z > temps_mouvement_Y)
		{
			cout << "Axe prioritaire : Z" << endl;
			axe_prioritaire.name_of_axis = Z;
			axe_prioritaire.acc_dist = _acc_distance.z;
			axe_prioritaire.acc_temps = _acc_temps.z;
			axe_prioritaire.acc = _acc_tick.z;
			axe_prioritaire.vit = _vit_tick.z;
			axe_prioritaire.dist = Dvect.Dep_z;
			axe_prioritaire.temps_mouvement = temps_mouvement_Z;

			//baxes=z;
			//bigestcoord=Dvect.Dep_z;
		}
		else
		{
			cout << "Axe prioritaire : Y" << endl;
			axe_prioritaire.name_of_axis = Y;
			axe_prioritaire.acc_dist = _acc_distance.y;
			axe_prioritaire.acc_temps = _acc_temps.y;
			axe_prioritaire.acc = _acc_tick.y;
			axe_prioritaire.vit = _vit_tick.y;
			axe_prioritaire.dist = Dvect.Dep_y;
			axe_prioritaire.temps_mouvement = temps_mouvement_Y;

			//baxes=y;
			//bigestcoord=Dvect.Dep_y;
		}
	}
	else if(temps_mouvement_Z > temps_mouvement_X){
		cout << "Axe prioritaire : Z" << endl;
		axe_prioritaire.name_of_axis = Z;
		axe_prioritaire.acc_dist = _acc_distance.z;
		axe_prioritaire.acc_temps = _acc_temps.z;
		axe_prioritaire.acc = _acc_tick.z;
		axe_prioritaire.vit = _vit_tick.z;
		axe_prioritaire.dist = Dvect.Dep_z;
		axe_prioritaire.temps_mouvement = temps_mouvement_Z;

		//baxes=z;
		//bigestcoord=Dvect.Dep_z;
	}
	else {
		cout << "Axe prioritaire : X" << endl;
		axe_prioritaire.name_of_axis = X;
		axe_prioritaire.acc_dist = _acc_distance.x;
		axe_prioritaire.acc_temps = _acc_temps.x;
		axe_prioritaire.acc = _acc_tick.x;
		axe_prioritaire.vit = _vit_tick.x;
		axe_prioritaire.dist = Dvect.Dep_x;
		axe_prioritaire.temps_mouvement = temps_mouvement_X;
	}


	cout << endl << "Calcul des facteurs de réduction des accélérations : " << endl;

	facteur_reduc_acc_x = (double)((double)(4 * abs(Dvect.Dep_x)) / (double)(_acc_tick.x==0?1:_acc_tick.x * axe_prioritaire.temps_mouvement * axe_prioritaire.temps_mouvement));
	facteur_reduc_acc_y = (double)((double)(4 * abs(Dvect.Dep_y)) / (double)(_acc_tick.y==0?1:_acc_tick.y * axe_prioritaire.temps_mouvement * axe_prioritaire.temps_mouvement));
	facteur_reduc_acc_z = (double)((double)(4 * abs(Dvect.Dep_z)) / (double)(_acc_tick.z==0?1:_acc_tick.z * axe_prioritaire.temps_mouvement * axe_prioritaire.temps_mouvement));
	facteur_reduc_acc_a = (double)((double)(4 * abs(Dvect.Dep_a)) / (double)(_acc_tick.a==0?1:_acc_tick.a * axe_prioritaire.temps_mouvement * axe_prioritaire.temps_mouvement));

	cout << "facteur axe X : " << facteur_reduc_acc_x <<endl;
	cout << "facteur axe Y : " << facteur_reduc_acc_y <<endl;
	cout << "facteur axe Z : " << facteur_reduc_acc_z <<endl;
	cout << "facteur axe A : " << facteur_reduc_acc_a <<endl;

	epoch_begin=epoch_present;

	if((uint64_t)abs(axe_prioritaire.dist)>axe_prioritaire.acc_dist*2)
		{

		// A finir !!!! changer les variable par la structure axeprioritaire
		// Simplifier en ajoutant peut être directement les structure des axe choisies (_acc et _vit ??)

			epoch_present+=axe_prioritaire.acc_temps;

			epoch_end_accel=epoch_present;
			switch(axe_prioritaire.name_of_axis)
					{
					case X:
						accel_x=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;
						accel_y=Dvect.Dep_y==0?0:(int32_t)((double)_acc_tick.y * facteur_reduc_acc_y);
						accel_z=Dvect.Dep_z==0?0:(int32_t)((double)_acc_tick.z * facteur_reduc_acc_z);

						epoch_present += (double)(abs(axe_prioritaire.dist) - 2 * axe_prioritaire.acc_dist) / (double)axe_prioritaire.vit; //peut etre une erreure ??
						epoch_end_conti=epoch_present;

						break;
					case Y:

						accel_y=Dvect.Dep_y>0?accelDeccel:-accelDeccel;
						accel_x=Dvect.Dep_x==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_x/(double)(abs(Dvect.Dep_y))));
						accel_z=Dvect.Dep_z==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_z/(double)(abs(Dvect.Dep_y))));
						epoch_end_conti=epoch_present+=(double)((double)(abs(Dvect.Dep_y)-2*Dtick_to_accel)/(double)Vmax);

						break;
					case Z:
						accel_z=Dvect.Dep_z>0?accelDeccel:-accelDeccel;
						accel_x=Dvect.Dep_x==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_x/(double)(abs(Dvect.Dep_z))));
						accel_y=Dvect.Dep_y==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_y/(double)(abs(Dvect.Dep_z))));
						epoch_end_conti=epoch_present+=(double)((double)(abs(Dvect.Dep_z)-2*Dtick_to_accel)/(double)Vmax);

						break;
					case A:
						break;

					}
			epoch_present += axe_prioritaire.acc_temps;
			epoch_end=epoch_present;
			Accel_vectors.push_back(new Accel_vector(accel_x,accel_y,accel_z,0,epoch_begin,epoch_end_accel));
			Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_end_accel,epoch_end_conti));
			Accel_vectors.push_back(new Accel_vector(-accel_x,-accel_y,-accel_z,0,epoch_end_conti,epoch_end));

		}
	else
	{
		uint64_t time_accel=0;

		time_accel = sqrt((double)((double)(abs(axe_prioritaire.dist)) / (double)axe_prioritaire.acc));
		epoch_present+=time_accel;

		epoch_end_accel = epoch_present;
					switch(axe_prioritaire.name_of_axis)
							{
							case X:

								accel_x=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;//todo gestion si deplacement x00y00z00
								accel_y=Dvect.Dep_y==0?0:(int32_t)((double)_acc_tick.y * facteur_reduc_acc_y);
								accel_z=Dvect.Dep_z==0?0:(int32_t)((double)_acc_tick.z * facteur_reduc_acc_z);
								break;
							case Y:
								accel_y=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;
								accel_x=Dvect.Dep_x==0?0:(int32_t)((double)_acc_tick.x * facteur_reduc_acc_x);
								accel_z=Dvect.Dep_z==0?0:(int32_t)((double)_acc_tick.z * facteur_reduc_acc_z);
								break;
							case Z:
								accel_z=axe_prioritaire.dist>0?axe_prioritaire.acc:-axe_prioritaire.acc;
								accel_x=Dvect.Dep_x==0?0:(int32_t)((double)_acc_tick.x * facteur_reduc_acc_x);
								accel_y=Dvect.Dep_y==0?0:(int32_t)((double)_acc_tick.y * facteur_reduc_acc_y);
								break;
							case A:
								break;

							}
					epoch_present+=time_accel;
					epoch_end = epoch_present;
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
	Gcode::TabEtatMachine::iterator it2 = tabetat.begin();
	for(Gcode::TabEtatMachine::iterator it = tabetat.begin(); it != tabetat.end(); it++) {
	    if((*it)->Deplacement==false) continue;
	    if(first)
	    {
	    	cout << "mmToTick((*it)->PosOutil_X, x) - 2^64 : " << mmToTick((*it)->PosOutil_X, x) - pow(2,64) << endl << endl;
	    	vect.SetVector(	mmToTick((*it)->PosOutil_X, x),mmToTick((*it)->PosOutil_Y, y),
	    					mmToTick((*it)->PosOutil_Z, z),mmToTick((*it)->PosOutil_A, a));
	    }
	    else
	    {
	    	vect.SetVector(	mmToTick((*it)->PosOutil_X - (*it2)->PosOutil_X, x),mmToTick((*it)->PosOutil_Y - (*it2)->PosOutil_Y, y),
	    		    		mmToTick((*it)->PosOutil_Z - (*it2)->PosOutil_Z, z),mmToTick((*it)->PosOutil_A - (*it2)->PosOutil_A, a));
	    	it2++;
	    }
	    	prevvect.SetVector(	mmToTick((*it)->PosOutil_X, x),mmToTick((*it)->PosOutil_Y, y),
					mmToTick((*it)->PosOutil_Z, z),mmToTick((*it)->PosOutil_A, a));
	    profileGenerator(vect, (*it));
	    first=false;
	    iter++;
    	//cout << "\rgenerated vectors : " << (int)((float)((iter*100.0)/tabetat.size())) << "%" << flush;
	}
	Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_present,epoch_present+1000));//empty end vector
	Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_present+1000,epoch_present+2000));//empty end vector
	cout << " Done" << endl;
	return false;
}



void Accel_converter::print()
{
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

void Accel_converter::sendVectors(Spi_comm & comm) {
	int i=0;
	char first=1;
	for(TabAccelVector::iterator it = Accel_vectors.begin(); it != Accel_vectors.end(); it++) {
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
