/**
* Un programme qui organise des films et des acteurs_ à l'aide de pointeurs à partir d'objets de types ListeFilms
* \file   td3.cpp
* \author Ammah et Trahan
* \date   5 mars 2023
* Créé le 16 février 2023
*/


#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include "cppitertools/range.hpp"
#include "gsl/span"
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
//[
void ListeFilms::changeDimension(int nouvelleCapacite)
{
	Film** nouvelleListe = new Film * [nouvelleCapacite];

	if (elements != nullptr) {  // Noter que ce test n'est pas nécessaire puique nElements sera zéro si elements est nul, donc la boucle ne tentera pas de faire de copie, et on a le droit de faire delete sur un pointeur nul (ça ne fait rien).
		nElements = min(nouvelleCapacite, nElements);
		for (int i : range(nElements))
			nouvelleListe[i] = elements[i];
		delete[] elements;
	}

	elements = nouvelleListe;
	capacite = nouvelleCapacite;
}

void ListeFilms::ajouterFilm(Film* film)
{
	if (nElements == capacite)
		changeDimension(max(1, capacite * 2));
	elements[nElements++] = film;
}

//]

//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
// On a juste fait une version const qui retourne un span non const.  C'est valide puisque c'est la struct qui est const et non ce qu'elle pointe.  Ça ne va peut-être pas bien dans l'idée qu'on ne devrait pas pouvoir modifier une liste const, mais il y aurais alors plusieurs fonctions à écrire en version const et non-const pour que ça fonctionne bien, et ce n'est pas le but du TD (il n'a pas encore vraiment de manière propre en C++ de définir les deux d'un coup).
span<Film*> ListeFilms::enSpan() const { return span(elements, nElements); }

void ListeFilms::enleverFilm(const Film* film)
{
	for (Film*& element : enSpan()) {  // Doit être une référence au pointeur pour pouvoir le modifier.
		if (element == film) {
			if (nElements > 1)
				element = elements[nElements - 1];
			nElements--;
			return;
		}
	}
}

//Initialisation du span de listeacteurs_
template <class T>
span<shared_ptr<T>> Liste<T>::spanListeActeurs() const { return span(elements_.get(), nElements_); }

//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.
shared_ptr<Acteur> ListeFilms::trouverActeur(const string& nomActeur) 
{
	for (const Film* film : enSpan()) {
		for (shared_ptr<Acteur> acteur : film->acteurs_.spanListeActeurs()) {
			if (acteur->nom == nomActeur)
				return acteur;
		}
	}
	return nullptr;
}


//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.  La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).
shared_ptr<Acteur> lireActeur(istream& fichier//[
	, ListeFilms& listeFilms//]
)
{
	Acteur acteur = {};
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);
	//[
	shared_ptr<Acteur> acteurExistant = listeFilms.trouverActeur(acteur.nom);
	if (acteurExistant != nullptr)
		return acteurExistant;
	else {
		cout << "Création Acteur " << acteur.nom << endl;
		return make_shared<Acteur>(acteur);
	}
	//]
	return {}; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs_ crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
}
Film* lireFilm(istream& fichier, ListeFilms& listeFilms) {
	//Création film qu'on retourne directement.
	Film film = {};
	//Création d'un nouvau pointeur vers le film qu'on va associer les valeurs qu'on va lire dans le fichier.
	Film* pointeurFilm = new Film;
	//Lecture du fichier avec le Film* qui s'associe tout les valeurs appropriées.
	pointeurFilm->titre_ = lireString(fichier);
	pointeurFilm->realisateur_ = lireString(fichier);
	pointeurFilm->anneeSortie_ = lireUint16(fichier);
	pointeurFilm->recette_ = lireUint16(fichier);
	//On utilise les méthodes de Liste<Acteur> pour changer le nombre d'éléments et le stocker dans les attributs de la liste.
	pointeurFilm->acteurs_.modifierNElements(lireUint8(fichier));
	pointeurFilm->acteurs_.modifierCapacite(pointeurFilm->acteurs_.accesNElements());

	//Création du nouveau film et nous utilisons le pointeur film qui a récupéré les informations du fichier pour créer les attributs de ce nouveau film.
	cout << "Création Film " << film.titre_ << endl;
	pointeurFilm->acteurs_.modifierElements(make_unique<shared_ptr<Acteur>[]>(pointeurFilm->acteurs_.accesNElements()));

	//On change les Pointeurs d'acteurs_ pour ceux du fichier dans notre pointeur film.
	for (shared_ptr<Acteur>& acteur : pointeurFilm->acteurs_.spanListeActeurs())
		acteur = lireActeur(fichier, listeFilms);
	return pointeurFilm;
}

//Constructeur d'une ListeFilms (fait par François)
ListeFilms::ListeFilms(const string& nomFichier) : possedeLesFilms_(true)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);
	for ([[maybe_unused]] int i : range(nElements)) {
		ajouterFilm(lireFilm(fichier, *this));
	}
}

//Destruction d'un film (fait par François)
void detruireFilm(Film* film) {
	cout << "Destruction Film " << film->titre_ << endl;
	delete film;
}

//Une fonction pour détruire une ListeFilms et tous les films qu'elle contient. (Faite par François)
ListeFilms::~ListeFilms() {
	if (possedeLesFilms_)
		for (Film* film : enSpan())
			detruireFilm(film);
	delete[] elements;
}

//Surcharge d'operateur pour un Acteur avec <<.
ostream& operator << (ostream& o, const Acteur& acteur) {
	o << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
	return o;
}
//Surcharge d'operateur pour un pointeur vers un Acteur avec <<.
ostream& operator << (ostream& o, const Acteur* acteurPointeur) {
	o << "  " << acteurPointeur->nom << ", " << acteurPointeur->anneeNaissance << " " << acteurPointeur->sexe << endl;
	return o;
}

//Surcharge d'operateur pour un film avec <<.
ostream& operator << (ostream& o, const Film& film) {

	o << "titre: " << film.titre_ << endl;
	o << "Réalisateur: " << film.realisateur_ << "Année: " << film.anneeSortie_ << endl;
	o << "recette: " << film.recette_ << "M$" << endl;
	o << "acteurs: " << endl;

	//TODO: Trouver une maniere de display la liste des acteurs_ du film
	for (shared_ptr<Acteur> acteur : film.acteurs_.spanListeActeurs()) {
		cout << acteur;
	}
	return o;
}

//Surcharge d'operateur pour un pointeur vers un Film avec <<
ostream& operator << (ostream& o, const Film* filmPointeur) {
	o << "titre: " << filmPointeur->titre_ << endl;
	o << "Réalisateur: " << filmPointeur->realisateur_ << "Année: " << filmPointeur->anneeSortie_ << endl;
	o << "recette_: " << filmPointeur->recette_ << "M$" << endl;
	o << "acteurs: " << endl;

	for (shared_ptr<Acteur> acteur : filmPointeur->acteurs_.spanListeActeurs()) {
		cout << acteur;
	}
	return o;
}


//Surcharge d'operateur pour une liste de films avec <<.
ostream& operator << (ostream& o, const ListeFilms& listeFilms) {
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme). Fait par professeur.
	static const string ligneDeSeparation = //[
		"\033[32m────────────────────────────────────────\033[0m\n";
	//]
	{};

	o << ligneDeSeparation;

	//Utilise le span pour passer un film à la fois.
	for (const Film* filmPointeur : listeFilms.enSpan()) {

		//Affiche le film à l'aide du ostream pour film fait plus haut.
		cout << filmPointeur;
		//Affiche une ligne de séparation.
		o << ligneDeSeparation;
	}
	return o;
}

Film*& ListeFilms::operator[](int const index) {
	assert(index >= 0 && index <= nElements);
	return elements[index];
}

Film*& ListeFilms::operator[](int const index) const {
	assert(index >= 0 && index <= nElements);
	return elements[index];
}

//Recherche d'un film à l'aide d'un critère 
Film* ListeFilms::rechercheCritereFilm(const auto& critereDeRecherche) const {
	for (auto&& film : enSpan()) {
		if (critereDeRecherche(film)) {
			return film;
		}
	}
	return nullptr;
}

//Chapitre 10, Lambda et afficahge de la recherche
void ListeFilms::retourRechercheCritereFilm(const ListeFilms& listeFilms) {
	Film* filmCritere;
	filmCritere = listeFilms.rechercheCritereFilm([](auto film) {
		//choisir le critere ici!
		return film->recette_ == 955;
		}
	);
	if (filmCritere == nullptr) {
		cout << "Aucun film possède le critère rechercheé" << endl;
	}
	else {
		cout << "Le film suivant possède le critère recherché: " << endl;
		cout << *filmCritere;
	}
}

//surcharge d'operateur pour qu'on puisse utiliser l'index d'un acteur
template <class T>
shared_ptr<T> Liste<T>::operator[](int index)
{
	return spanListeActeurs()[index];
}

void transfererFilms(vector<shared_ptr<Item>>& vecteurItem, ListeFilms &listeFilms) {
	for (Film* film : listeFilms.enSpan()) {

		shared_ptr<Item> ptrFilm = make_shared<Item>(*film);
		vecteurItem.push_back(ptrFilm);
	}
}

int main()
{
#ifdef VERIFICATION_ALLOCATION_INCLUS
	bibliotheque_cours::VerifierFuitesAllocations verifierFuitesAllocations;
#endif
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	//TODO: Enlever cette ligne après avoir vérifié qu'il y a bien un "Detected memory leak" de "4 bytes" affiché dans la "Sortie", qui réfère à cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs_ sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilms("films.bin");

	
	vector<shared_ptr<Item>> vecteurItem;
	transfererFilms(vecteurItem, listeFilms);





}
