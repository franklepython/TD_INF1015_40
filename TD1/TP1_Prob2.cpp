/**
* Programme qui calcule le nombre de mois pour repayer de l'argent prêtée avec intérêt.
* \file   TP1_Prob2.cpp
* \author  Ammah et Trahan
* \date   29 janvier 2023
* Créé le 24 janvier 2023
*/

#include <iostream>
#include <cmath>
using namespace std;


int main()
{
	int mois;
	int SommeArgentPretee;
	cout << "Quelle est la somme d'argent pretee: ";
	cin >> SommeArgentPretee;
	while (SommeArgentPretee <= 0 || SommeArgentPretee == INFINITY) {
		cout << "Entrez un nombre positif:";
		cin >> SommeArgentPretee;
	}
	int MontantParMois;
	cout << "Quel est le montant rembourse chaque mois: ";
	cin >> MontantParMois;
	while (MontantParMois <= 0 || MontantParMois == INFINITY) {
		cout << "Entrez un nombre positif:";
		cin >> MontantParMois;
	}
	int TauxAnnuel;
	int TauxMensuel;
	cout << "Quel est le taux annuel: ";
	cin >> TauxAnnuel;
	while (TauxAnnuel < 0 || TauxAnnuel > 100) {
		cout << "Entrer un nombre entre 0 et 100: ";
		cin >> TauxAnnuel;
	}
	TauxMensuel = TauxAnnuel / 12;

	double SommeRestanteAPayer = SommeArgentPretee;
	double InteretAccumuler = TauxAnnuel;
	for (mois = 0; SommeRestanteAPayer > 0; mois++) {
		InteretAccumuler += SommeRestanteAPayer * TauxMensuel / 100;
		SommeRestanteAPayer += SommeRestanteAPayer * TauxMensuel / 100;
		SommeRestanteAPayer -= MontantParMois;
	}
	cout << "ca va prendre " << mois << " mois pour payer la somme d'argent pretee et l'interet acumuler est " << InteretAccumuler << endl;
	return 0;
}

