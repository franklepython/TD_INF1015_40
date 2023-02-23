/**
* Un programme qui permet de calculer une valeur approchée de pi par la 
méthode de Monte‐Carlo basée sur les probabilités. 
* \file   TP1_Prob4.cpp
* \author Ammah et Trahan
* \date   29 janvier 2023
* Créé le 25 janvier 2023
*/

#include <cstdlib>  
#include <ctime> 
#include<cmath>
#include <random>
#include<iostream>
#include<iomanip>

using namespace std;


default_random_engine generator;
uniform_real_distribution<double> distribution(-1, 1);

const double pi = 3.141593;

void ApproximerPi(int nIterations) {
	double nDePointsDansLeCercle = 0.0;
	double nDePointsEnDehorsDuCercle = 0.0;

	for (int i = 0; i < nIterations; i++) {
		double x = distribution(generator);
		double y = distribution(generator);

		if ((x * x + y * y) < 1) {
			nDePointsDansLeCercle += 1;
		}
		else {
			nDePointsEnDehorsDuCercle += 1;
		}
	}

	double nDePointsTot = nDePointsDansLeCercle + nDePointsEnDehorsDuCercle;

	double piApproxime = (nDePointsDansLeCercle / nDePointsTot) * 4;

	double ecartRelatif = abs(pi - piApproxime);

	std::cout << std::setprecision(6) << std::fixed << piApproxime << " est la valeur approchee de pi.";
	cout << " L'ecart entre " << pi << " et " << piApproxime << " est de ";
	cout << setprecision(6) << ecartRelatif << endl;
}

int main() {

	cout << "Quelle est le nombre d'iterations souhaitees?: ";
	int nIterations;
	cin >> nIterations;

	ApproximerPi(nIterations);

	return 0;
}