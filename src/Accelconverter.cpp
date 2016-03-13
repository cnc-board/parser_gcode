/*
 * Accelconverter.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: julien
 */

#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"
Accel_converter::Accel_converter() {
	calculateAccelTick();
}

int64_t Accel_converter::mmToTick(double val) {
	return val*convertFactorMmToTick;
}

Accel_converter::~Accel_converter() {
	// TODO Auto-generated destructor stub
}

DeplacementVector::DeplacementVector(int64_t x,int64_t y,int64_t z,int64_t a):Dep_x(x),Dep_y(y),Dep_z(z),Dep_a(a){};

Accel_vector::Accel_vector(int32_t Accel_x,int32_t Accel_y,int32_t Accel_z,int32_t Accel_a,uint64_t epoch_start,uint64_t epoch_stop)//,uint32_t epoch_stop
			:
		Accel_x(Accel_x), Accel_y(Accel_y), Accel_z(Accel_z), Accel_a(Accel_a), epoch_start(
				epoch_start),epoch_stop(epoch_stop) { //, epoch_stop(epoch_stop)
}
;

void Accel_converter::profileGenerator(DeplacementVector Dvect) {
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
	if(abs(bigestcoord)>Dtick_to_acceldeccel*2)
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
		uint64_t dist_accel=0;
		uint64_t speed=0;

		uint64_t target=abs(bigestcoord)/2;
		/*for(time_accel=0;dist_accel<target;time_accel+=1) 			//todo calcul de i proper todo verif si movement < accelDeccel;
		{
			speed+=accelDeccel;
			dist_accel+=speed;
		}*/
		do
		{
			time_accel++;
			speed+=accelDeccel;
			dist_accel+=speed;
		} while (dist_accel<target);
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

void Accel_converter::calculateAccelTick() {
	/*int64_t i;

	for(i=0;i<=Vmax-accelDeccel;i+=accelDeccel)
		{
			Ttick_to_acceldeccel++;
			Dtick_to_acceldeccel+=i;
		}*/
	//uint64_t time_accel=0;
			//uint64_t dist_accel=0;
			uint64_t speed=0;
			do
			{
				Ttick_to_acceldeccel++;
				speed+=accelDeccel;
				Dtick_to_acceldeccel+=speed;
			} while (speed<Vmax);
		if(speed!=Vmax)Vmax=speed;
		cout << "Vmax is " << Vmax << endl;
		cout << "TAccelTick is " << Ttick_to_acceldeccel << endl;
		cout << "DAccelTick is " << Dtick_to_acceldeccel << endl;
}

bool Accel_converter::generate_tick_vector(Gcode::TabEtatMachine & tabetat) {
	bool first=true;
	DeplacementVector vect(0,0,0,0);
	DeplacementVector prevvect(0,0,0,0);

	Accel_vectors.push_back(new Accel_vector(0,0,0,0,0,epoch_present+=1000));//empty begin vector

	for(Gcode::TabEtatMachine::iterator it = tabetat.begin(); it != tabetat.end(); it++) {
	    if((*it)->Deplacement==false) continue;
	    if(first)
	    {
	    	vect.SetVector(	mmToTick((*it)->PosOutil_X),mmToTick((*it)->PosOutil_Y),
	    					mmToTick((*it)->PosOutil_Z),mmToTick((*it)->PosOutil_A));
	    }
	    else
	    {
	    	vect.SetVector(	mmToTick((*it)->PosOutil_X)-prevvect.Dep_x,mmToTick((*it)->PosOutil_Y)-prevvect.Dep_y,
	    		    		mmToTick((*it)->PosOutil_Z)-prevvect.Dep_z,mmToTick((*it)->PosOutil_A)-prevvect.Dep_a);
	    }
	    	prevvect.SetVector(	mmToTick((*it)->PosOutil_X),mmToTick((*it)->PosOutil_Y),
					mmToTick((*it)->PosOutil_Z),mmToTick((*it)->PosOutil_A));
	    profileGenerator(vect);
	    first=false;

	    //Phase accel
	    //Phase maintien
	    //Phase deccel


	}
	Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_present,epoch_present+1000));//empty end vector
	Accel_vectors.push_back(new Accel_vector(0,0,0,0,epoch_present+1000,epoch_present+2000));//empty end vector

	return false;
}

void Accel_vector::printvect() {
	cout << "Accel x : "<< Accel_x << " Accel y : "<< Accel_y<< " Accel z : "<< Accel_z << " Epoch start : " << epoch_start << " Epoch stop : " << epoch_stop << endl;
}

void Accel_converter::afficher()
{
	int i;
	for (i = 0; i < (int)Accel_vectors.size(); i++)
	{
		Accel_vectors[i]->printvect();
	}

}

void DeplacementVector::SetVector(int64_t x, int64_t y, int64_t z, int64_t a) {
	Dep_x=x;
	Dep_y=y;
	Dep_z=z;
	Dep_a=a;
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
	for(TabAccelVector::iterator it = Accel_vectors.begin(); it != Accel_vectors.end(); it++) {
		comm.transmit_vector(*(*it));
	}
}
