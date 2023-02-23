/**
*  Un programme qui lit du clavier un nombre entier et qui détermine si ce nombre est premier ou non.
* \file   TP1_Prob1.cpp
* \author Ammah et Trahan
* \date   29 janvier 2023
* Créé le 25 janvier 2023
*/

#include<iostream>
using namespace std;
#include <cmath>

bool estPremier(int nombreEntier) {
    if (nombreEntier == 0 || nombreEntier == 1 || nombreEntier == 2) {
        cout << nombreEntier << " n'est pas un nombre premier.";
        return false;
    }

    if (nombreEntier % 2 == 0) {
        cout << nombreEntier << " n'est pas un nombre premier car il est divisible par 2.";
        return false;
    }

    for (int i = 3; i < sqrt(nombreEntier); i++) {
        if (i % 2 != 0) {
            int nbEntierDiviseParI = nombreEntier % i;
            if (nbEntierDiviseParI == 0) {
                cout << nombreEntier << " n'est pas un nombre premier car il est divisible par " << i << ".";
                return false;
            }
        }
    }


    cout << nombreEntier << " est un nombre premier.";

    return true;
}


int main() {
    int nombreEntier;
    cout << "Entrer un nombre entier: ";
    cin >> nombreEntier;
    estPremier(nombreEntier);

    return 0;
}