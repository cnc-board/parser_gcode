/*
 * Accelconverter.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: julien
 */

#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"
Accel_converter::Accel_converter(double convertFactorMmToTick_X, double convertFactorMmToTick_Y,
								double convertFactorMmToTick_Z, double convertFactorMmToTick_A) :
								_convertFactorMmToTick_X(convertFactorMmToTick_X), _convertFactorMmToTick_Y(convertFactorMmToTick_Y),
								_convertFactorMmToTick_Z(convertFactorMmToTick_Z), _convertFactorMmToTick_A(convertFactorMmToTick_A)
{

}

int64_t Accel_converter::mmToTickX(double val) {
	return val*_convertFactorMmToTick_X;
}

int64_t Accel_converter::mmToTickY(double val) {
	return val*_convertFactorMmToTick_Y;
}

int64_t Accel_converter::mmToTickZ(double val) {
	return val*_convertFactorMmToTick_Z;
}

int64_t Accel_converter::mmToTickA(double val) {
	return val*_convertFactorMmToTick_A;
}

Accel_converter::~Accel_converter() {
	// TODO Auto-generated destructor stub
}

void Accel_converter::profileGenerator(Movement_Vector Dvect) {
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
	if((uint64_t)abs(bigestcoord)>Dtick_to_acceldeccel*2)
		{
			epoch_end_accel=epoch_present+=Ttick_to_acceldeccel;
			switch(baxes)
					{
					case x:
						accel_x=Dvect.Dep_x>0?accelDeccel:-accelDeccel;
						accel_y=Dvect.Dep_y==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_y/(double)(abs(Dvect.Dep_x))));
						accel_z=Dvect.Dep_z==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_z/(double)(abs(Dvect.Dep_x))));
						epoch_end_conti=epoch_present+=(double)((double)(abs(Dvect.Dep_x)-2*Dtick_to_acceldeccel)/(double)Vmax);//-2*Ttick_to_acceldeccel;
						//epoch_end_conti=epoch_present+=Dvect.Dep_y-2*Ttick_to_acceldeccel;

						break;
					case y:
						accel_y=Dvect.Dep_y>0?accelDeccel:-accelDeccel;
						accel_x=Dvect.Dep_x==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_x/(double)(abs(Dvect.Dep_y))));
						accel_z=Dvect.Dep_z==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_z/(double)(abs(Dvect.Dep_y))));
						epoch_end_conti=epoch_present+=(double)((double)(abs(Dvect.Dep_y)-2*Dtick_to_acceldeccel)/(double)Vmax);

						break;
					case z:
						accel_z=Dvect.Dep_z>0?accelDeccel:-accelDeccel;
						accel_x=Dvect.Dep_x==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_x/(double)(abs(Dvect.Dep_z))));
						accel_y=Dvect.Dep_y==0?0:(double)((double)accelDeccel*(double)((double)Dvect.Dep_y/(double)(abs(Dvect.Dep_z))));
						epoch_end_conti=epoch_present+=(double)((double)(abs(Dvect.Dep_z)-2*Dtick_to_acceldeccel)/(double)Vmax);

						break;
					case a:
						break;

					}
			epoch_end=epoch_present+=Ttick_to_acceldeccel;
			Accel_vectors.push_back(new Accel_vector(accel_x,accel_y,accel_z,0,epoch_begin,epoch_end_accel));
			Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_end_accel,epoch_end_conti));
			Accel_vectors.push_back(new Accel_vector(-accel_x,-accel_y,-accel_z,0,epoch_end_conti,epoch_end));

		}
	else
	{
		uint64_t time_accel=0;
		//uint64_t dist_accel=0;
		//uint64_t speed=0;

		uint64_t target=abs(bigestcoord)/2;
		/*for(time_accel=0;dist_accel<target;time_accel+=1) 			//todo calcul de i proper todo verif si movement < accelDeccel;
		{
			speed+=accelDeccel;
			dist_accel+=speed;
		}*/


		//deltap=(a*dt^2)/2
		//dt²=sqrt((deltap/a)*2)
		/*do
		{
			time_accel++;
			speed+=accelDeccel;
			dist_accel+=speed;
		} while (dist_accel<target);*/
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
	    	vect.SetVector(	mmToTickX((*it)->PosOutil_X),mmToTickY((*it)->PosOutil_Y),
	    					mmToTickZ((*it)->PosOutil_Z),mmToTickA((*it)->PosOutil_A));
	    }
	    else
	    {
	    	vect.SetVector(	mmToTickX((*it)->PosOutil_X)-prevvect.Dep_x,mmToTickY((*it)->PosOutil_Y)-prevvect.Dep_y,
	    		    		mmToTickZ((*it)->PosOutil_Z)-prevvect.Dep_z,mmToTickA((*it)->PosOutil_A)-prevvect.Dep_a);
	    }
	    	prevvect.SetVector(	mmToTickX((*it)->PosOutil_X),mmToTickY((*it)->PosOutil_Y),
					mmToTickZ((*it)->PosOutil_Z),mmToTickA((*it)->PosOutil_A));
	    profileGenerator(vect);
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
