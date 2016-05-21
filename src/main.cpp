#include "../headers/Gcode.h"
#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"
#include "../headers/SignalHandler.h"
#include "../headers/Limit_machine.h"




int main(int argc, char **argv)
{

	Spi_comm comm(true);
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
	EtatInit.V_Avance_Defaut_A = 0;
	EtatInit.V_Avance_Defaut_X = 50;
	EtatInit.V_Avance_Defaut_Y = 50;
	EtatInit.V_Avance_Defaut_Z = 200;
	EtatInit.V_Rapide_Defaut_A = 0;
	EtatInit.V_Rapide_Defaut_X = 50;
	EtatInit.V_Rapide_Defaut_Y = 50;
	EtatInit.V_Rapide_Defaut_Z = 120;

	Limit_machine lim_machine(100,100,100,100,10,10,50,5);


	Gcode Prog1 = Gcode(argv[1], EtatInit);

	Prog1.parser();


	Accel_converter conv(127, 127, 127, 0.0, lim_machine); //127.896942801 pulse / mm
	conv.generate_tick_vector(Prog1._TabEtatMachine);
	cout << "Cycle time : " << (conv.Accel_vectors[conv.Accel_vectors.size()-1]->epoch_stop)/(25000000*60)<< " minutes" << endl;
	comm.execute_reset_off();
	conv.sendVectors(comm);

	return 0;
}
