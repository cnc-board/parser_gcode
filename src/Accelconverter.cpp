/*
 * Accelconverter.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: julien
 */

#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"
#include "../headers/Gcode.h"

Accel_converter::Accel_converter(double convertFactorMmToTick_X, double convertFactorMmToTick_Y,
								double convertFactorMmToTick_Z, double convertFactorMmToTick_A, Limit_machine limit_machine) :
								_convertFactorMmToTick_X(convertFactorMmToTick_X), _convertFactorMmToTick_Y(convertFactorMmToTick_Y),
								_convertFactorMmToTick_Z(convertFactorMmToTick_Z), _convertFactorMmToTick_A(convertFactorMmToTick_A), _Limite_machine(limit_machine)
{

}

int64_t Accel_converter::mmToPulse(double val,  Axis axe) {
	int64_t ret;
	switch(axe)
	{
	case x:
		ret = val*_convertFactorMmToTick_X;
		break;
	case y:
		ret = val*_convertFactorMmToTick_Y;
		break;
	case z:
		ret = val*_convertFactorMmToTick_Z;
		break;
	case a:
		ret = val*_convertFactorMmToTick_A;
		break;
	}
	return ret;
}


Accel_converter::~Accel_converter() {
	// TODO Auto-generated destructor stub
}

int64_t Accel_converter::mm_per_second_square_to_pulse_per_tick_square(float mm_per_second_square,  Axis axe)
{
	int64_t tick_per_second_square;

	tick_per_second_square = mmToPulse((double) mm_per_second_square, axe);

	return tick_per_second_square / FREQUENCE_FPGA;

}



void Accel_converter::profileGenerator(Movement_Vector Dvect, Gcode::Class_EtatMachine* Etat) {
	uint64_t speed=0;
	static float _vitesse_X, _vitesse_Y, _vitesse_Z, _vitesse_A;

	if(_vitesse_X != Etat->VitesseDeplacement_X || _vitesse_Y != Etat->VitesseDeplacement_Y
			|| _vitesse_Z != Etat->VitesseDeplacement_Z || _vitesse_A != Etat->VitesseDeplacement_A)
	{	//todo factoriser "if"

		do
		{
			_axe.Temps.Accel_X ++;
			speed += _Limite_machine.Acc_mm_max_X;
			_axe.Distance.Accel_X += speed;
		} while (speed < Etat->VitesseDeplacement_X);
		//if(speed!=Vmax)Vmax=speed;

		cout << "Vitesse de l'axe X (tick / tick) is" << Etat->VitesseDeplacement_X << endl;
		cout << "Temps AccelTick is " << _axe.Temps.Accel_X << endl;
		cout << "Distance AccelTick is " << _axe.Distance.Accel_X << endl<<endl;

		do
		{
			_axe.Temps.Accel_Y ++;
			speed += _Limite_machine.Acc_mm_max_Y;
			_axe.Distance.Accel_Y += speed;
		} while (speed < Etat->VitesseDeplacement_Y);
				//if(speed!=Vmax)Vmax=speed;

		cout << "Vitesse de l'axe Y (tick / tick) is" << Etat->VitesseDeplacement_Y << endl;
		cout << "Temps AccelTick is " << _axe.Temps.Accel_Y << endl;
		cout << "Distance AccelTick is " << _axe.Distance.Accel_Y << endl<<endl;

		do
		{
			_axe.Temps.Accel_Z ++;
			speed += _Limite_machine.Acc_mm_max_Z;
			_axe.Distance.Accel_Z += speed;
		} while (speed < Etat->VitesseDeplacement_Z);
				//if(speed!=Vmax)Vmax=speed;

		cout << "Vitesse de l'axe Z (tick / tick) is" << Etat->VitesseDeplacement_Z << endl;
		cout << "Temps AccelTick is " << _axe.Temps.Accel_Z << endl;
		cout << "Distance AccelTick is " << _axe.Distance.Accel_Z << endl<<endl;

		do
		{
			_axe.Temps.Accel_A ++;
			speed += _Limite_machine.Acc_mm_max_A;
			_axe.Distance.Accel_A += speed;
		} while (speed < Etat->VitesseDeplacement_A);
				//if(speed!=Vmax)Vmax=speed;

		cout << "Vitesse de l'axe A (tick / tick) is" << Etat->VitesseDeplacement_A << endl;
		cout << "Temps AccelTick is " << _axe.Temps.Accel_A << endl;
		cout << "Distance AccelTick is " << _axe.Distance.Accel_A << endl<<endl;

	}





	typedef enum axes{x,y,z,a} biggest_axes;
	int64_t bigestcoord=Dvect.Dep_x;
	biggest_axes baxes=x;
	int32_t accel_x,accel_y,accel_z;
	uint64_t epoch_begin,epoch_end_accel,epoch_end_conti,epoch_end;
	epoch_begin=epoch_present;

	if(abs(Dvect.Dep_y)>abs(Dvect.Dep_x)){//todo : WRONG SELECT MAX
		if(abs(Dvect.Dep_z)>abs(Dvect.Dep_y))
		{
			baxes=z;
			bigestcoord=Dvect.Dep_z;
		}
		else
		{
			baxes=y;
			bigestcoord=Dvect.Dep_y;
		}
	}
	else if(abs(Dvect.Dep_z)>abs(Dvect.Dep_x)){
		baxes=z;
		bigestcoord=Dvect.Dep_z;
	}
	if((uint64_t)abs(bigestcoord)>Dtick_to_accel*2)
		{
			epoch_end_accel=epoch_present+=Ttick_to_accel;
			switch(baxes)
					{
					case x:
						accel_x=Dvect.Dep_x>0?accelDeccel:-accelDeccel;
						accel_y=Dvect.Dep_y==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_y/(double)(abs(Dvect.Dep_x))));
						accel_z=Dvect.Dep_z==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_z/(double)(abs(Dvect.Dep_x))));
						epoch_end_conti=epoch_present+=(double)((double)(abs(Dvect.Dep_x)-2*Dtick_to_accel)/(double)Vmax);//-2*Ttick_to_accel;
						//epoch_end_conti=epoch_present+=Dvect.Dep_y-2*Ttick_to_accel;

						break;
					case y:
						accel_y=Dvect.Dep_y>0?accelDeccel:-accelDeccel;
						accel_x=Dvect.Dep_x==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_x/(double)(abs(Dvect.Dep_y))));
						accel_z=Dvect.Dep_z==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_z/(double)(abs(Dvect.Dep_y))));
						epoch_end_conti=epoch_present+=(double)((double)(abs(Dvect.Dep_y)-2*Dtick_to_accel)/(double)Vmax);

						break;
					case z:
						accel_z=Dvect.Dep_z>0?accelDeccel:-accelDeccel;
						accel_x=Dvect.Dep_x==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_x/(double)(abs(Dvect.Dep_z))));
						accel_y=Dvect.Dep_y==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_y/(double)(abs(Dvect.Dep_z))));
						epoch_end_conti=epoch_present+=(double)((double)(abs(Dvect.Dep_z)-2*Dtick_to_accel)/(double)Vmax);

						break;
					case a:
						break;

					}
			epoch_end=epoch_present+=Ttick_to_accel;
			Accel_vectors.push_back(new Accel_vector(accel_x,accel_y,accel_z,0,epoch_begin,epoch_end_accel));
			Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_end_accel,epoch_end_conti));
			Accel_vectors.push_back(new Accel_vector(-accel_x,-accel_y,-accel_z,0,epoch_end_conti,epoch_end));

		}
	else
	{
		uint64_t time_accel=0;
		uint64_t target=abs(bigestcoord)/2;


		time_accel=sqrt((double)((double)target/(double)accelDeccel)*2.0);

		epoch_end_accel=epoch_present+=time_accel;
					switch(baxes)
							{
							case x:
								accel_x=Dvect.Dep_x>0?accelDeccel:-accelDeccel;//todo gestion si deplacement x00y00z00
								accel_y=Dvect.Dep_y==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_y/(double)(abs(Dvect.Dep_x))));
								accel_z=Dvect.Dep_z==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_z/(double)(abs(Dvect.Dep_x))));
								break;
							case y:
								accel_y=Dvect.Dep_y>0?accelDeccel:-accelDeccel;
								accel_x=Dvect.Dep_x==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_x/(double)(abs(Dvect.Dep_y))));
								accel_z=Dvect.Dep_z==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_z/(double)(abs(Dvect.Dep_y))));
								break;
							case z:
								accel_z=Dvect.Dep_z>0?accelDeccel:-accelDeccel;
								accel_x=Dvect.Dep_x==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_x/(double)(abs(Dvect.Dep_z))));
								accel_y=Dvect.Dep_y==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_y/(double)(abs(Dvect.Dep_z))));
								break;
							case a:
								break;

							}
					epoch_end=epoch_present+=time_accel;
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
	    	vect.SetVector(	mmToPulse((*it)->PosOutil_X, x),mmToPulse((*it)->PosOutil_Y, y),
	    					mmToPulse((*it)->PosOutil_Z, z),mmToPulse((*it)->PosOutil_A, a));
	    }
	    else
	    {
	    	vect.SetVector(	mmToPulse((*it)->PosOutil_X, x)-prevvect.Dep_x,mmToPulse((*it)->PosOutil_Y, y)-prevvect.Dep_y,
	    		    		mmToPulse((*it)->PosOutil_Z, z)-prevvect.Dep_z,mmToPulse((*it)->PosOutil_A, a)-prevvect.Dep_a);
	    }
	    	prevvect.SetVector(	mmToPulse((*it)->PosOutil_X, x),mmToPulse((*it)->PosOutil_Y, y),
					mmToPulse((*it)->PosOutil_Z, z),mmToPulse((*it)->PosOutil_A, a));
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
