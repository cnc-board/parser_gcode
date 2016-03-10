#include "../headers/Gcode.h"
#include "../headers/Accelconverter.h"
#include "../headers/Spicomm.h"

int main()
{
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

	Gcode Prog1 = Gcode("test.txt", EtatInit);

	Prog1.parser();
	cout << endl << endl << "position des axes : " << endl << endl;

	Accel_converter conv;
	conv.generate_tick_vector(Prog1._TabEtatMachine);
	conv.afficher();
	Spi_comm comm;
	conv.sendVectors(comm);
	comm.execute_fifo_list();
	conv.printcsv();

	return 0;
}
