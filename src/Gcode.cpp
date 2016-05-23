#include "../headers/Gcode.h"
#include "../headers/Err.h"
#include <string>
#include <cmath>




Gcode::Gcode(string FileName, InitMachine InitMachine)
{
	int i;
	for (i = 0; i < 100; i++)
	{
		instructionsG[i] = NULL;
		instructionsM[i] = NULL;
	}

	instructionsG[0] = &Gcode::DeplacementRapide;
	instructionsG[1] = &Gcode::DeplacementVitesseDavance;
	instructionsG[4] = &Gcode::PauseProgramme;
	instructionsG[90] = &Gcode::ModeDistancesAbsolues;
	instructionsG[91] = &Gcode::ModeDistancesRelatives;

	instructionsM[2] = &Gcode::FinProgramme;
	instructionsM[3] = &Gcode::MarcheBrocheSensHoraire;
	instructionsM[4] = &Gcode::MarcheBrocheSensAntiHoraire;
	instructionsM[5] = &Gcode::ArretBroche;


	_FileName = FileName; //Adresse et fichier contenant le Gcode

	_PosOutil_X = InitMachine.PosOutil_X;
	_PosOutil_Y = InitMachine.PosOutil_Y;
	_PosOutil_Z = InitMachine.PosOutil_Z;
	_PosOutil_A = InitMachine.PosOutil_A;
	_PosPiece_X = InitMachine.PosPiece_X;
	_PosPiece_Y = InitMachine.PosPiece_Y;


	_vitesse_deplacement_broche_G00 = InitMachine.vitesse_deplacement_broche_G00;
	_vitesse_deplacement_broche_G01 = InitMachine.vitesse_deplacement_broche_G01;
	_vitesse_deplacement_axe_z_G00 = InitMachine.vitesse_deplacement_axe_z_G00;
	_vitesse_deplacement_axe_z_G01 = InitMachine.vitesse_deplacement_axe_z_G01;


	_MoteurDefaut = Arret;
	_ModeDistance = InitMachine.ModeDistance;
}

Gcode::~Gcode()
{
	int i;
	for (i = 0; i < (int)_TabEtatMachine.size(); i++)
	{
		delete(_TabEtatMachine[i]);
		_TabEtatMachine[i] = NULL;
	}
}

void Gcode::afficher()
{
	int i;
	for (i = 0; i < (int)_TabEtatMachine.size(); i++)
	{
		cout<<"pos X : "<<_TabEtatMachine[i]->PosOutil_X<<"  pos Y : "<<_TabEtatMachine[i]->PosOutil_Y<<endl;
	}

}

void Gcode::parser()
{
	string line;
	errors err = NoErr;
	ifstream fichier(_FileName, ios::in);
	int Nb = 0;
	int Nline = 0;

	if (fichier)
	{

		while (getline(fichier, line))
		{
			try
			{
				Nline++;
				GestionSuppressionCommentaire(line);

				if (line == "" || line[0] == ' ')
					continue;

				switch (line[0])
				{
				case 'M':
						try
						{
							Nb = std::stoi(line.substr(1, 2));
						}
						catch (...)
						{
							throw Err(CmdInconnue);
						}
						if (instructionsM[Nb] != NULL)
							(this->*instructionsM[Nb])(line);
						else
							throw Err(CmdInconnue);

						break;

				case 'G':
						try
						{
							Nb = stoi(line.substr(1, 2));
						}
						catch (...)
						{
							throw Err(CmdInconnue);
						}
						if (instructionsG[Nb] != NULL)
							(this->*instructionsG[Nb])(line);
						else
							throw Err(CmdInconnue);

						break;

				default:
						err = CmdInconnue;
						break;
				}

					//GestionErr(Nline, err, line);
					//err = NoErr;

				}
			catch(Err& e)
			{
				switch (e._err)
					{
					case NoErr:
						break;
					case CanNotOpenFile:
						cout << "CanNotOpenFile" << endl;
						break;
					case geneBin:
						break;
					case CmdInconnue:
						cout << "Ligne : " << Nline << "  commande inconnue            :   " << line << endl;
						break;
					case MissingParam:
						cout << "Ligne : " << Nline << "  Paramettre(s) manquant(s)    :   " << line << endl;
						break;
					case ModeDistanceNonDef:
						cout << "Mode distance non defini !" << line << endl;
						break;
					case MvSur3ou4AxesNonPrisEnCharge:
						cout << "Mouvement sur plus de deux axes simultanément non pris en charge" << endl;
						break;
					case DeplacementDeZero:
						cout << "Déplacement null (0 mm sur tous les axes)" << endl;
					default:
						break;
					}
			}


		}//While

	}
	else
	{
		cout << "CanNotOpenFile" << endl; // A finir de gérer !!!!!
	}


}

void Gcode::GestionSuppressionCommentaire(string& line)
{
	size_t posParOpen;
	size_t posParClose;
	size_t posPointVirgule;
	string commentaireParenthese;
	string commentairePointVirgule;
	int i;
	bool test = true;

	//Recherche commentaires entre "()"
	posParOpen = line.find_first_of('(', 0);
	posParClose = line.find_first_of(')', 0);

	//if
	i = posParOpen + 1;
	do
	{
		if (line.find_first_of('(', i) < posParClose)
		{
			posParClose = line.find_first_of(')', posParClose + 1);
			i = line.find_first_of('(', i) + 1;
		}
		else
			test = false;
	} while (posParClose != string::npos && test);



	while (posParOpen != string::npos && posParClose != string::npos && posParOpen < posParClose)
	{
		commentaireParenthese = line.substr(posParOpen + 1, posParClose - posParOpen - 1);
		// réliser ici la gestion des commentaires (). commentaireParenthese contient le commentaire

		line = line.substr(0, posParOpen) + line.substr(posParClose + 1, line.length()); //suppression du commentaire entre ()

		posParOpen = line.find_first_of('(', 0);
		posParClose = line.find_first_of(')', 0);
	}



	//Recherche commentaire après ";"
	posPointVirgule = line.find_first_of(';', 0);

	if (posPointVirgule != string::npos)
	{
		commentairePointVirgule = line.substr(posPointVirgule + 1, line.length());
		// réliser ici la gestion des commentaires après ;. commentairePointVirgule contient le commentaire

		line = line.substr(0, posPointVirgule);//suppression du commentaire après ;
	}
}

bool Gcode::RechercheValeur(char index, const string& line, float& value)
{
	size_t offset;
	size_t nbr;
	float _value;

	offset = line.find_first_of(index, 0);

	if (offset != string::npos)
	{
		for (nbr = 0; (offset + 1 + nbr) < line.length() && line[offset + 1 + nbr] != ' '; nbr++)
			if (nbr > 30) { return false; }

		try
		{
			_value = stof(line.substr(offset + 1, nbr));

		}
		catch (std::exception e)
		{
			return false;
		}

		value = _value;
		return true;
	}
	else
	{
		return false;
	}
}



// *************************************************************************************************
//**************************************** Instructions M ******************************************
// *************************************************************************************************


void Gcode::FinProgramme(string line)
{
	//Création de l'ete courant (initialisation par def dans le constructeur)
	//S'il éxiste déja un etat dans le tableau, copy dans l'etat courant
	Class_EtatMachine * EtatMachine = new Class_EtatMachine(*this);
	int test = _TabEtatMachine.size();
	if (test != 0)
		EtatMachine->Copy(*_TabEtatMachine[test - 1]);

	//Parametrage et non action de déplacement
	EtatMachine->Deplacement = false;

	//Indique la fin du programme
	EtatMachine->FinProgramme = true;
	cout << "Fin programme" << endl;

	//Ajout de l'état courant dans la liste
	_TabEtatMachine.push_back(EtatMachine);
}

void Gcode::MarcheBroche(string line, enuMoteurBroche M_A)
{
	//Création de l'ete courant (initialisation par def dans le constructeur)
	//S'il éxiste déja un etat dans le tableau, copy dans l'etat courant
	Class_EtatMachine * EtatMachine = new Class_EtatMachine(*this);
	int test = _TabEtatMachine.size();
	if (test != 0)
		EtatMachine->Copy(*_TabEtatMachine[test - 1]);

	//Parametrage et non action de déplacement
	EtatMachine->Deplacement = false;



	//Marche/Arret broche sans horaire/antihoraire
	EtatMachine->MoteurBroche = M_A;

	if (M_A == MarcheSensHoraire)
		cout << "Marche moteur broche sens    :   horaire" << endl;
	else if (M_A == MarcheSensAntiHoraire)
		cout << "Marche moteur broche sens    :   anti horaire" << endl;
	else if (M_A == Arret)
		cout << "Arret moteur" << endl;



	//Ajout de l'état courant dans la liste
	_TabEtatMachine.push_back(EtatMachine);
}
void Gcode::MarcheBrocheSensHoraire(string line)
{
	MarcheBroche(line, MarcheSensHoraire);
}
void Gcode::MarcheBrocheSensAntiHoraire(string line)
{
	MarcheBroche(line, MarcheSensAntiHoraire);
}
void Gcode::ArretBroche(string line)
{
	MarcheBroche(line, Arret);
}




// *************************************************************************************************
//**************************************** Instructions G ******************************************
// *************************************************************************************************

void Gcode::Deplacement_G00_G01(string line, bool G00)
{
	//errors err = NoErr;
	float Xvalue = 0, Yvalue = 0, Zvalue = 0, Avalue = 0;

	double alpha;

	//Création de l'ete courant (initialisation par def dans le constructeur)
	//S'il éxiste déja un etat dans le tableau, copy dans l'etat courant
	Class_EtatMachine * EtatMachine = new Class_EtatMachine(*this);
	int test = _TabEtatMachine.size();
	if (test != 0)
		EtatMachine->Copy(*_TabEtatMachine[test - 1]);

	Class_EtatMachine* dernier_etat = NULL;
	if(_TabEtatMachine.size() != 0) //S'il il y a un autre etat_machine dans le tableau
		{
			//Récupération de l'état précédent
			dernier_etat = _TabEtatMachine[_TabEtatMachine.size()-1];
		}


	//Action de déplacement broche (pas de parametrage)
	EtatMachine->Deplacement = true;

	//Récupérartion des paramettres de la fonction GXX
	bool testX = RechercheValeur('X', line, Xvalue);
	bool testY = RechercheValeur('Y', line, Yvalue);
	bool testZ = RechercheValeur('Z', line, Zvalue);
	bool testA = RechercheValeur('A', line, Avalue);

	if (!(testX || testY || testZ || testA))
		throw Err(MissingParam);

	if((testX && testY) && (testZ || testA))
		throw Err(MvSur3ou4AxesNonPrisEnCharge);

	//Modification de la position de l'outil
	if (EtatMachine->ModeDistance == Absolues)
	{
		if (testX) { EtatMachine->PosOutil_X = Xvalue; }
		if (testY) { EtatMachine->PosOutil_Y = Yvalue; }
		if (testZ) { EtatMachine->PosOutil_Z = Zvalue; }
		if (testA) { EtatMachine->PosOutil_A = Avalue; }
	}
	else if (EtatMachine->ModeDistance == Relatives)
	{
		if (testX) { EtatMachine->PosOutil_X += Xvalue; }
		if (testY) { EtatMachine->PosOutil_Y += Yvalue; }
		if (testZ) { EtatMachine->PosOutil_Z += Zvalue; }
		if (testA) { EtatMachine->PosOutil_A += Avalue; }
	}
	else
		throw Err(ModeDistanceNonDef);


	if(dernier_etat->PosOutil_X == EtatMachine->PosOutil_X && dernier_etat->PosOutil_Y == EtatMachine->PosOutil_Y &&
	   dernier_etat->PosOutil_Z == EtatMachine->PosOutil_Z && dernier_etat->PosOutil_A == EtatMachine->PosOutil_A)
		throw Err(DeplacementDeZero);




	if(testX || testY) //calcul de l'angle formé par le déplacement
	{

		if(dernier_etat != NULL) //S'il il y a un autre etat machine dans le tableau (obtention du déplacement par rapport à ce dernier)
		{
			alpha = atan((EtatMachine->PosOutil_Y - dernier_etat->PosOutil_Y) / (EtatMachine->PosOutil_X - dernier_etat->PosOutil_X));
		}
		else
		{
			alpha = atan(EtatMachine->PosOutil_Y / EtatMachine->PosOutil_X);
		}
	}


	//Modification vitesse d'avance
	if (testZ)
	{
		EtatMachine->VitesseDeplacement_X = 0;
		EtatMachine->VitesseDeplacement_Y = 0;
		EtatMachine->VitesseDeplacement_Z = G00?_vitesse_deplacement_axe_z_G00:_vitesse_deplacement_axe_z_G01;
		EtatMachine->VitesseDeplacement_A = 0;
	}
	else if (testA)
	{
		//A faire
	}
	else
	{
		EtatMachine->VitesseDeplacement_X = (float)cos(alpha) * (G00==true?_vitesse_deplacement_broche_G00:_vitesse_deplacement_broche_G01);
		EtatMachine->VitesseDeplacement_Y = (float)sin(alpha) * (G00==true?_vitesse_deplacement_broche_G00:_vitesse_deplacement_broche_G01);
		EtatMachine->VitesseDeplacement_Z = 0;
		EtatMachine->VitesseDeplacement_A = 0;
	}






	//Ajout de l'état courant dans la liste
	_TabEtatMachine.push_back(EtatMachine);


	//Faire ici la génération du code machine !!!!!

}
void Gcode::DeplacementRapide(string line)
{
	Deplacement_G00_G01(line, true);
}
void Gcode::DeplacementVitesseDavance(string line)
{
	Deplacement_G00_G01(line, false);
}

void Gcode::PauseProgramme(string line)
{
	float temps;

	//Création de l'ete courant (initialisation par def dans le constructeur)
	//S'il éxiste déja un etat dans le tableau, copy dans l'etat courant

	Class_EtatMachine * EtatMachine = new Class_EtatMachine(*this);
	int test = _TabEtatMachine.size();
	if (test != 0)
		EtatMachine->Copy(*_TabEtatMachine[test - 1]);

	//Parametrage et non action de déplacement
	EtatMachine->Deplacement = false;

	if (RechercheValeur('P', line, temps))
	{
		EtatMachine->PauseProgramme = 3;
		cout << "Pause du programme pendant   :   " << temps << "s" << endl;
	}
	else
		throw Err(MissingParam);

	//Ajout de l'état courant dans la liste
	_TabEtatMachine.push_back(EtatMachine);
}

void Gcode::ModeDistances(string line, bool absolue)
{
		//Création de l'ete courant (initialisation par def dans le constructeur)
		//S'il éxiste déja un etat dans le tableau, copy dans l'etat courant
		Class_EtatMachine * EtatMachine = new Class_EtatMachine(*this);
		int test = _TabEtatMachine.size();
		if (test != 0)
			EtatMachine->Copy(*_TabEtatMachine[test - 1]);

		//Parametrage et non action de déplacement
		EtatMachine->Deplacement = false;

		//Modification du mode de distance vers position Absolue
		if(absolue)
		{
			EtatMachine->ModeDistance = Absolues;
			cout << "Mode distances               :   absolues" << endl;
		}
		else
		{
			EtatMachine->ModeDistance = Relatives;
			cout << "Mode distances               :   relatives" << endl;
		}



		//Ajout de l'état courant dans la liste
		_TabEtatMachine.push_back(EtatMachine);

}
void Gcode::ModeDistancesAbsolues(string line)
{
	ModeDistances(line, true);
}
void Gcode::ModeDistancesRelatives(string line)
{
	ModeDistances(line, false);
}
