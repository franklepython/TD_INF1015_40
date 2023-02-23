/**
* Programme qui lit un fichier .txt et le transforme en structure
* \file   TP1_Prob6.cpp
* \author  et Durant
* \date   29 janvier 2023
* Créé le 24 janvier 2023
*/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int const nombreMotsDict = 4;
const string nomDict = "dictionnaire.txt";

struct DictionnaireDefinitions {
	string nom;
	string genre;
	string definition;
};

int main() {
	ifstream file(nomDict);
	DictionnaireDefinitions dictionnaireFinal[nombreMotsDict];
	string nom, genre, definition;
	int indexNomPlusLong = 0;

	for (int i = 0; i < nombreMotsDict; i++) {
		getline(file, nom, '\t');
		getline(file, genre, '\t');
		getline(file, definition);
		dictionnaireFinal[i] = { nom, genre, definition };
		cout << dictionnaireFinal[i].nom << endl;
		if (nom.length() > dictionnaireFinal[indexNomPlusLong].nom.length()) {
			indexNomPlusLong = i;

		}

	}
	cout << dictionnaireFinal[indexNomPlusLong].nom << " (" << dictionnaireFinal[indexNomPlusLong].genre << ") : " << dictionnaireFinal[indexNomPlusLong].definition << endl;

	return 0;
}
