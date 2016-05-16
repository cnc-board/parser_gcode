#ifndef HEADERS_GCODE_H_
#define HEADERS_GCODE_H_

#pragma once

#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include "Err.h"

using namespace std;


enum ChoixModeDistances
{
	Nondef, Absolues, Relatives
};
enum enuMoteurBroche
{
	MarcheSensHoraire, MarcheSensAntiHoraire, Arret
};

struct InitMachine
{
	double PosOutil_X, PosOutil_Y, PosOutil_Z, PosOutil_A;//Position de l'outil avant début cycle
	double PosPiece_X, PosPiece_Y;//Position de la pièce à usiner

	float V_Rapide_Defaut_X, V_Rapide_Defaut_Y, V_Rapide_Defaut_Z, V_Rapide_Defaut_A;//Vitesse rapide par defaut
	float V_Avance_Defaut_X, V_Avance_Defaut_Y, V_Avance_Defaut_Z, V_Avance_Defaut_A;//Vitesse d'avance par defaut

	ChoixModeDistances ModeDistance;// Mode : non défini, Absolu ou relatif
};


class Gcode
{
public:
	//****************************** Constructeurs / Destructeur ****************************
	Gcode(string FileName, InitMachine InitMachine); // Initialiser les arguments !!! (via un fichier ou autre !!) OBLIGATOIRE !!
	~Gcode();

	//****************************************************************************************

	class Class_EtatMachine
	{
	public:
		Class_EtatMachine(const Gcode &Source);
		void Copy(const Class_EtatMachine &Etat);

		bool Deplacement;
		double PosOutil_X, PosOutil_Y, PosOutil_Z, PosOutil_A;
		float VitesseDeplacement_X, VitesseDeplacement_Y, VitesseDeplacement_Z, VitesseDeplacement_A;
		enuMoteurBroche MoteurBroche;
		ChoixModeDistances ModeDistance;
		int PauseProgramme;
		bool FinProgramme;

	private:
	};

	typedef std::vector<Class_EtatMachine*> TabEtatMachine;



	//************************************** Méthodes *************************************
	void SetPositionOutil(); //A coder !
	void parser();//Parse, créer le tableau d'états machine et bientôt le code machine
	void afficher();
TabEtatMachine _TabEtatMachine;// Tableau des états machine après parsage
private:

	//************************************** Arguments ************************************

	string _FileName; //Adresse et fichier contenant le Gcode

	double _PosOutil_X, _PosOutil_Y, _PosOutil_Z, _PosOutil_A;//Position de l'outil avant début cycle
	double _PosPiece_X, _PosPiece_Y;//Position de la pièce à usiner

	float _V_Rapide_Defaut_X, _V_Rapide_Defaut_Y, _V_Rapide_Defaut_Z, _V_Rapide_Defaut_A;//Vitesse rapide par defaut
	float _V_Avance_Defaut_X, _V_Avance_Defaut_Y, _V_Avance_Defaut_Z, _V_Avance_Defaut_A;//Vitesse d'avance par defaut

	enuMoteurBroche _MoteurDefaut;//Moteur : Marche sens horaire, anti horaire ou arret
	ChoixModeDistances _ModeDistance;// Mode : non défini, Absolu ou relatif

	void(Gcode::*instructionsG[100])(string line);//tableau de pointeur sur fonction pour instruction G
	void(Gcode::*instructionsM[100])(string line);//tableau de pointeur sur fonction pour instruction M



	//************************************** Méthodes *************************************


	void GestionSuppressionCommentaire(string& line);
	bool RechercheValeur(char index, const string& line, float& value);

	void Deplacement_G00_G01(string line, bool G00);
	void DeplacementRapide(string line);			//Ne recherche pas la vitesse !!
	void DeplacementVitesseDavance(string line);	//Ne recherche pas la vitesse !!
	void PauseProgramme(string line);
	void ModeDistances(string line, bool absolue);
	void ModeDistancesAbsolues(string line);
	void ModeDistancesRelatives(string line);
	void FinProgramme(string line);
	void MarcheBroche(string line, enuMoteurBroche M_A);
	void MarcheBrocheSensHoraire(string line);
	void MarcheBrocheSensAntiHoraire(string line);
	void ArretBroche(string line);




};


#endif /* HEADERS_GCODE_H_ */
