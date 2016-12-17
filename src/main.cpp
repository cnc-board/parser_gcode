#include "../headers/Gcode.h"
#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"
#include "../headers/SignalHandler.h"
#include "../headers/Limit_machine.h"
#include "../headers/Zynqcomm.hpp"



int main(int argc, char **argv)
{

	Zynq_comm comm(false);
	SignalHandler sighand;
	sighand.register_exitSignalHandler(&comm);
	InitMachine EtatInit;

	EtatInit.ModeDistance = Absolues;
	EtatInit.PosOutil_A = 0;
	EtatInit.PosOutil_X = 0;
	EtatInit.PosOutil_Y = 0;
	EtatInit.PosOutil_Z = 0;
	EtatInit.PosPiece_X = 10;
	EtatInit.PosPiece_Y = 20;
	EtatInit.vitesse_deplacement_broche_G00 = 20;
	EtatInit.vitesse_deplacement_broche_G01 = 50;
	EtatInit.vitesse_deplacement_axe_z_G00 = 100;
	EtatInit.vitesse_deplacement_axe_z_G01 = 50;

	Limit_machine lim_machine(20,100,100,200,100,100,100,50);


	Gcode Prog1 = Gcode(argv[1], EtatInit);

	Prog1.parser();


	Accel_converter conv(400, 127.896942801, 400, 0.0, lim_machine); //127.896942801 pulse / mm
	conv.generate_tick_vector(Prog1._TabEtatMachine);
	//conv.print();
	cout << "Cycle time : " << (conv.Accel_vectors[conv.Accel_vectors.size()-1]->epoch_stop)/(25000000*60)<< " minutes" << endl;
	comm.execute_reset_off();
	conv.sendVectors(comm);
	//comm.execute_fifo_list();

	return 0;
}
