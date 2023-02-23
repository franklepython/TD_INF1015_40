/**
* Un programme qui saisit un tableau d’entiers de taille 10 et qui l’affiche de 
telle sorte que tous les entiers pairs se retrouvent avant les entiers impairs.
* \file   TP1_Prob5.cpp
* \author Ammah et Trahan
* \date   29 janvier 2023
* Créé le 26 janvier 2023
*/

#include<iostream>
using namespace std;


void makeEvenNumbersFirst(int initialTable[10]) {

    int numberOfEvenNumbers = 0;
    int newTable[10];

    for (int i = 0; i < 10; i++) {
        if (initialTable[i] % 2 == 0) {
            newTable[numberOfEvenNumbers] = initialTable[i];
            numberOfEvenNumbers += 1;
        }
    }

    for (int i = 0; i < 10; i++) {
        if (initialTable[i] % 2 != 0) {
            newTable[numberOfEvenNumbers] = initialTable[i];
            numberOfEvenNumbers += 1;
        }
    }

    for (int i = 0; i < 10; i++) {
        cout << newTable[i] << " ";
    }
}



int main() {
    int initialTable[10];

    for (int i = 0; i < 10; i++) {
        int inputNum;
        cout << "Veillez entrer une valeur: " << endl;
        cin >> inputNum;
        initialTable[i] = inputNum;
    }

    makeEvenNumbersFirst(initialTable);
    return 0;
}