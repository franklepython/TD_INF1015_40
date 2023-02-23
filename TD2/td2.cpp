/**
* Un programme qui organise des films et des acteurs à l'aide de pointeurs à partir d'objets de types ListeFilms 
* \file   td2.cpp
* \author Ammah et Trahan
* \date   12 février 2023
* Créé le 4 février 2023
*/


#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

#include "cppitertools/range.hpp"
#include "gsl/span"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).

using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{

UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}

//TODO: Une fonction pour ajouter un Film à une ListeFilms, le film existant déjà; on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit doubler la taille du tableau alloué, avec au minimum un élément, dans le cas où la capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y avait dans l'ancien, et éliminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ListeFilms::ajouterFilm(Film* film) {
	if (film == nullptr) {
		return;
	}

	if (capacite_ != 0) {
		if (nElements_ = capacite_) {
			const int tailleNouveauTableau = capacite_ * 2;

			Film** nouveauTableau = new Film * [tailleNouveauTableau];

			for (int i = 0; i < capacite_; i++) {
				nouveauTableau[i] = elements[i];
				elements[i] = nullptr;
			}
			delete[] elements;

			capacite_ = tailleNouveauTableau;
			nouveauTableau[capacite_ + 1] = film;
			elements = nouveauTableau;
		}
		else {
			elements[nElements_ + 1] = film;
		}
		nElements_ += 1;
	}
}

//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
void ListeFilms::enleverFilm(Film* ptrVersFilm) {
	for (int i = 0; i < nElements_; i++) {
		if (elements[i] = ptrVersFilm) {
			elements[i] = nullptr;
			delete elements[i];

			for (int j = i; j < nElements_; j++) {
				elements[j] = elements[j + 1];
			}
		}
	}
}
//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.

Acteur* ListeFilms::trouverActeur(const string nomActeur) const {
	bool acteurTrouve = false;
	span<Film*> spanPtrsVersFilms = span(elements, nElements_);

	for (Film* film : spanPtrsVersFilms) {
		for (Acteur* acteur : gsl::span(film->acteurs.elements, film->acteurs.nElements)) {
			if (acteur->nom == nomActeur) {
				return acteur;
			}
		}
	}

	if (acteurTrouve = false) {
		return nullptr;
	}
}
//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.  La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).

Acteur* ListeFilms:: lireActeur(istream& fichier) {
	Acteur acteur = {};
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);

	Acteur* acteurPointeur = trouverActeur(acteur.nom);
	if (acteurPointeur != nullptr) {
		return acteurPointeur;
	}
	else {
		cout << "L'acteur n'existe pas, il est cree!" << acteur.nom << endl;
		return new Acteur(acteur);
	}//TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
}

Film* ListeFilms::lireFilm(istream& fichier) {
	Film film = {};
	film.titre = lireString(fichier);
	film.realisateur = lireString(fichier);
	film.anneeSortie = lireUint16(fichier);
	film.recette = lireUint16(fichier);
	film.acteurs.nElements = lireUint8(fichier);  //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.

	film.acteurs.capacite = film.acteurs.nElements;
	film.acteurs.elements = new Acteur * [film.acteurs.nElements];
	Film* filmPointeur = new Film(film);

	for (int i = 0; i < film.acteurs.nElements; i++) {
		filmPointeur->acteurs.elements[i] = lireActeur(fichier);//TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		ajouterFilm(filmPointeur);

		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.

	}
	return { filmPointeur }; //TODO: Retourner le pointeur vers le nouveau film.
}

ListeFilms ListeFilms::listeFilms(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);

	ListeFilms listeCreer;


	//TODO: Créer une liste de films vide.
	capacite_ = 0;
	nElements_ = 0;
	elements = new Film * [capacite_];
	for (int i = 0; i < nElements_; i++) {
		ajouterFilm(lireFilm(fichier)); //TODO: Ajouter le film à la liste.
	}
}

//TODO: Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.

void ListeFilms::relacherMemoireActeurs(const string nomFilm) {
	for (int i = 0; i < nElements_; i++) {
		if (elements[i]->titre == nomFilm) {
			Film film = *elements[i];

			for (int j = 0; j < film.acteurs.nElements; j++) {
				if (film.acteurs.elements[j]->joueDans.nElements_ == 1) {
					cout << film.acteurs.elements[j]->nom << " est detruit. ";
					film.acteurs.elements[j]->joueDans.~ListeFilms();
					delete film.acteurs.elements[j]->joueDans.elements;
					film.acteurs.elements[j] = nullptr;
					delete film.acteurs.elements[j];
					int indexActeurEnleve = j;
					for (int k = j; k < film.acteurs.nElements; k++) {
						film.acteurs.elements[k] = film.acteurs.elements[k + 1];
					}

					film.acteurs.nElements -= 1;
				}
			}
		}
	}
}

void ListeFilms::enleverFilmDesListes(string nomFilm) {
	for (int i = 0; i < nElements_; i++) {
		if (elements[i]->titre == nomFilm) {
			Film film = *elements[i];

			const int indexDuPointeurVersFilm = i;

			for (int j = 0; j < film.acteurs.nElements; j++) {
				if (film.acteurs.elements[j]->joueDans.elements[j] == elements[indexDuPointeurVersFilm]) {
					delete film.acteurs.elements[j]->joueDans.elements[j];
					film.acteurs.elements[j]->joueDans.elements[j] = nullptr;
					film.acteurs.elements[j]->joueDans.nElements_ -= 1;
				}
			}
		}
	}
}

void ListeFilms::detruireFilm(const string nomFilm) {

	relacherMemoireActeurs(nomFilm);

	for (int i = 0; i < nElements_; i++) {
		if (elements[i]->titre == nomFilm) {
			delete elements[i];
			elements[i] = nullptr;
			for (int j = i; j < nElements_; j++) {
				elements[j] = elements[j + 1];
			}
		}
	}
	enleverFilmDesListes(nomFilm);
}

//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.
void ListeFilms::detruireListeFilms() {
	for (int i = 0; i < nElements_; i++) {
		delete elements[i];
	}
	delete[] elements;
	elements = nullptr;
	nElements_ = 0;
}

void ListeFilms::afficherActeur(const Acteur& acteur) const {
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).
void ListeFilms::afficherActeursFilm(const Film* film) const {

	cout << "Le nom du film est " << film->titre << endl;
	cout << "Les acteurs sont ";

	for (int i = 0; i < film->acteurs.nElements; i++) {
		afficherActeur(*film->acteurs.elements[i]);
	}
}



void ListeFilms::afficherListeFilms() const
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = {};
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	span<Film*> spanListeFilms(elements, nElements_);
	for (Film* film : spanListeFilms) {
		//TODO: Afficher le film.
		cout << film->titre << endl;
		cout << ligneDeSeparation;
	}
}

void ListeFilms::afficherFilmographieActeur(const string& nomActeur) const
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	const Acteur* acteur = trouverActeur(nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		(acteur->joueDans).afficherListeFilms();
}



int main()
{
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	int* fuite = new int; //TODO: Enlever cette ligne après avoir vérifié qu'il y a bien un "Fuite detectee" de "4 octets" affiché à la fin de l'exécution, qui réfère à cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilm;
	listeFilm.listeFilms("films.bin");

	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	cout << ligneDeSeparation << "Le premier film de la liste est:" << listeFilm.afficherActeursFilm(listeFilm.elements[0]) << endl;
	//afficherActeursFilm(listeFilms.elements[0]);
	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	listeFilm.afficherListeFilms();
	//TODO: Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	Acteur* benedictPointeur = listeFilm.trouverActeur("Benedict Cumberbatch");
	benedictPointeur->anneeNaissance = 1976;

	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	listeFilm.afficherFilmographieActeur("Benedict Cumberbatch");
	//TODO: Détruire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	listeFilm.detruireFilm("Alien");
	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	listeFilm.afficherListeFilms();
	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.

	//TODO: Détruire tout avant de terminer le programme.  La bibliothèque de verification_allocation devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	listeFilm.detruireListeFilms();
}