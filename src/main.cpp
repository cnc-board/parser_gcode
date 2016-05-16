#include "../headers/Gcode.h"
#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"
#include "../headers/SignalHandler.h"
int main(int argc, char **argv)
{

	Spi_comm comm(false);
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
	EtatInit.V_Rapide_Defaut_X = 120;
	EtatInit.V_Rapide_Defaut_Y = 120;
	EtatInit.V_Rapide_Defaut_Z = 120;

	Limit_machine lim_machine;
	lim_machine.Accel_max_X = 5;//mm/s/s
	lim_machine.Accel_max_Y = 5;
	lim_machine.Accel_max_Z = 5;
	lim_machine.Accel_max_A = 5;
	lim_machine.Vitesse_max_X = 30; //mm/s
	lim_machine.Vitesse_max_Y = 30;
	lim_machine.Vitesse_max_Z = 30;
	lim_machine.Vitesse_max_A = 30;


	Gcode Prog1 = Gcode(argv[1], EtatInit);

	Prog1.parser();






	Accel_converter conv(127.896942801*8796093022208.0, 127.896942801*8796093022208.0, 127.896942801*8796093022208.0, 0.0); //FactorMmToPulse
	conv.generate_tick_vector(Prog1._TabEtatMachine);
	cout << "Cycle time : " << (conv.Accel_vectors[conv.Accel_vectors.size()-1]->epoch_stop)/(25000000*60)<< " minutes" << endl;
	comm.execute_reset_off();
	conv.sendVectors(comm);

	return 0;
}
