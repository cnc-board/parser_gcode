#include "../headers/Gcode.h"


Gcode::Class_EtatMachine::Class_EtatMachine(const Gcode &Source)
{
	Deplacement = false;
	PosOutil_X = Source._PosOutil_X;
	PosOutil_Y = Source._PosOutil_Y;
	PosOutil_Z = Source._PosOutil_Z;
	PosOutil_A = Source._PosOutil_A;
	VitesseDeplacement_X = Source._V_Avance_Defaut_X;
	VitesseDeplacement_Y = Source._V_Avance_Defaut_Y;
	VitesseDeplacement_Z = Source._V_Avance_Defaut_Z;
	VitesseDeplacement_A = Source._V_Avance_Defaut_A;
	MoteurBroche = Source._MoteurDefaut;
	ModeDistance = Source._ModeDistance;
	PauseProgramme = 0;
	FinProgramme = false;
}




void Gcode::Class_EtatMachine::Copy(const Class_EtatMachine &Etat)
{
	Deplacement = Etat.Deplacement;
	PosOutil_X = Etat.PosOutil_X;
	PosOutil_Y = Etat.PosOutil_Y;
	PosOutil_Z = Etat.PosOutil_Z;
	PosOutil_A = Etat.PosOutil_A;
	VitesseDeplacement_X = Etat.VitesseDeplacement_X;
	VitesseDeplacement_Y = Etat.VitesseDeplacement_Y;
	VitesseDeplacement_Z = Etat.VitesseDeplacement_Z;
	VitesseDeplacement_A = Etat.VitesseDeplacement_A;
	MoteurBroche = Etat.MoteurBroche;
	ModeDistance = Etat.ModeDistance;

	// Pour qu'un etat "pause" ne se propage pas
	// lors de la création d'un autre état.
	PauseProgramme = 0; // Important !!


	FinProgramme = Etat.FinProgramme;
}
