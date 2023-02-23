/**
* Programme qui calcule la vitesse et la hauteur d'une balle apres un certain nombre de rebond. 
* \file   TP1_Prob3.cpp
* \author  Ammah et Trahan
* \date   29 janvier 2023
* Créé le 24 janvier 2023
*/

#include <iostream>
#include <cmath>
using namespace std;

int main() {
	int i;
	double hi;
	double vi;
	double hiPlus1;
	double viPlus1;
	const double constanteGravite = 9.81;

	
	
	int hauteurInitiale;
	cout << "Quelle est la hauteur initiale: ";
	cin >> hauteurInitiale;
	while (hauteurInitiale <= 0 || hauteurInitiale == INFINITY) {
		cout << "Entrez une hauteur initiale positive: ";
		cin >> hauteurInitiale;
	}
	int nombreDeRebonds;
	cout << "Quel est le nombre de rebonds au bout duquel vous souhaitez connaître la hauteur de la balle: ";
	cin >> nombreDeRebonds;
	while (nombreDeRebonds <= 0 || nombreDeRebonds == INFINITY) {
		cout << "Entrez un nombre de rebond positif:";
		cin >> nombreDeRebonds;
	}
	double coefficientDeRebond;
	cout << "Quel est le coefficient de rebonds que vous souhaitez utiliser: ";
	cin >> coefficientDeRebond;
	while (coefficientDeRebond < 0 || coefficientDeRebond > 1) {
		cout << "Entrer un nombre entre 0 et 1: ";
		cin >> coefficientDeRebond;
	}


	hi = hauteurInitiale;
	const double constanteGravite = 9.81;
	for (int i = 0; nombreDeRebonds > i; i++) {
		if (i != 0) {
			hi = hiPlus1;
			vi = viPlus1;
		}
		vi = sqrt(2 * constanteGravite * hi);
		viPlus1 = coefficientDeRebond * vi;
		hiPlus1 = (viPlus1 * viPlus1) / (2 * constanteGravite);
	}

	cout << "Apres " << nombreDeRebonds << " rebonds, la balle sera a la hauteur " << hiPlus1 << " et sera a la vitesse de " << viPlus1 << " m/s." << endl;
	return 0;
}

