/**
* Un programme qui organise dans une bibliotheque des livres, des films et des FilmLivres sous forme d'Items.
* \file   td3.cpp
* \author Ammah et Trahan
* \date   22 mars 2023
* Créé le 7 mars 2023
*/


#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS
#include "structures.hpp"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "debogage_memoire.hpp"
#include <iomanip>
#include <forward_list>


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

void ListeFilms::changeDimension(int nouvelleCapacite)
{
	Film** nouvelleListe = new Film * [nouvelleCapacite];

	if (elements != nullptr) {  
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

span<Film*> ListeFilms::enSpan() const { return span(elements, nElements); }

void ListeFilms::enleverFilm(const Film* film)
{
	for (Film*& element : enSpan()) {  
		if (element == film) {
			if (nElements > 1)
				element = elements[nElements - 1];
			nElements--;
			return;
		}
	}
}


template <class T>
span<shared_ptr<T>> Liste<T>::spanListeActeurs() const { return span(elements_.get(), nElements_); }


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
	return {}; 
}
Film* lireFilm(istream& fichier, ListeFilms& listeFilms) {
	Film film = {};
	Film* pointeurFilm = new Film;
	pointeurFilm->modifierTitre(lireString(fichier));
	pointeurFilm->realisateur_ = lireString(fichier);
	pointeurFilm->modifierAnneeSortie(lireUint16(fichier));
	pointeurFilm->recette_ = lireUint16(fichier);
	pointeurFilm->acteurs_.modifierNElements(lireUint8(fichier));
	pointeurFilm->acteurs_.modifierCapacite(pointeurFilm->acteurs_.accesNElements());

	cout << "Création Film " << film.accesTitre() << endl;
	pointeurFilm->acteurs_.modifierElements(make_unique<shared_ptr<Acteur>[]>(pointeurFilm->acteurs_.accesNElements()));

	for (shared_ptr<Acteur>& acteur : pointeurFilm->acteurs_.spanListeActeurs())
		acteur = lireActeur(fichier, listeFilms);
	return pointeurFilm;
}

ListeFilms::ListeFilms(const string& nomFichier) : possedeLesFilms_(true)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);
	for ([[maybe_unused]] int i : range(nElements)) {
		ajouterFilm(lireFilm(fichier, *this));
	}
}

void detruireFilm(Film* film) {
	cout << "Destruction Film " << film->accesTitre() << endl;
	delete film;
}

ListeFilms::~ListeFilms() {
	if (possedeLesFilms_)
		for (Film* film : enSpan())
			detruireFilm(film);
	delete[] elements;
}


ostream& operator<< (ostream& os, const Acteur& acteur)
{
	return os << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}


// TD4 partie 3
ostream& operator<< (ostream& os, const Affichable& affichable)
{
	affichable.afficher(os);
	return os;
}

void Item::afficher(ostream& os) const
{
	os << endl;
}

void Film::afficherCourt(ostream& os) const
{
	os << titre_ << ", par " << realisateur_ << endl;
}


void Livre::afficherCourt(ostream& os) const
{
	os << titre_ << ", de " << auteur_ << endl;
}

void FilmLivre::afficherCourt(ostream& os) const
{
	os << titre_ << ", de " << auteur_ << ", par " << realisateur_ << endl;
}


template <typename TypeDeListe>
void afficherListeItems(const TypeDeListe& listeItems)
{
	static const string ligneDeSeparation =
		"\033[32m────────────────────────────────────────\033[0m\n";

	{};

	cout << ligneDeSeparation;
	for (const auto& item : listeItems) {
		item->afficherCourt(cout);
		cout << ligneDeSeparation;

	}
}


template <class T>
shared_ptr<T> Liste<T>::operator[](int index)
{
	return spanListeActeurs()[index];
}

void transfererFilms(vector<shared_ptr<Item>>& bibliothequeItems, ListeFilms& listeFilms) {
	for (Film* film : listeFilms.enSpan()) {
		bibliothequeItems.push_back(make_shared<Film>(*film));
	}
}

void ajouterLivre(vector<shared_ptr<Item>>& bibliothequeItems, string fichierLivre) {

	ifstream fichier(fichierLivre);

	string ligne;
	while (getline(fichier, ligne)) {
		stringstream ss(ligne);
		string titre;
		int anneeSortie;
		string auteur;
		int nMillionsDeCopiesVendues;
		int nPages;

		ss >> quoted(titre) >> anneeSortie >> quoted(auteur) >> nMillionsDeCopiesVendues >> nPages;

		Livre livre(titre, anneeSortie, auteur, nMillionsDeCopiesVendues, nPages);

		shared_ptr<Livre> ptrLivre = make_shared<Livre>(livre);

		bibliothequeItems.push_back(ptrLivre);
	}


	fichier.close();
};


void copierPtrItems(forward_list<shared_ptr<Item>>& listLieSimple, vector<shared_ptr<Item>> const &bibliothequeItems) { //1.1
	for (int i = bibliothequeItems.size() - 1; i >= 0; i--) {
		listLieSimple.push_front(bibliothequeItems[i]);
	}
}

void copierListEnOrdreInverse(forward_list<shared_ptr<Item>> listeOrdreOriginal, forward_list<shared_ptr<Item>>& listeOrdreInverse) {
	
	while (listeOrdreOriginal.empty() != true) {
		listeOrdreInverse.push_front(listeOrdreOriginal.front());
		listeOrdreOriginal.pop_front();
	 }
}

void copierListeOriginale(forward_list<shared_ptr<Item>> const& listeOriginal, forward_list<shared_ptr<Item>>& copieListeOriginale) {

	auto itTemporaire = copieListeOriginale.before_begin();

	for (auto it = listeOriginal.begin(); it != listeOriginal.end(); ++it, ++itTemporaire) {
		copieListeOriginale.insert_after(itTemporaire, *(it));
	}
}

void inverserListVect(forward_list<shared_ptr<Item>> const& listeOriginal, vector<shared_ptr<Item>>& vectListOriginaleIverse) {

	for (auto it = listeOriginal.begin(); it != listeOriginal.end(); ++it) {
		vectListOriginaleIverse.insert(vectListOriginaleIverse.begin(), *it);
	}
}


int main()
{
#ifdef VERIFICATION_ALLOCATION_INCLUS
	bibliotheque_cours::VerifierFuitesAllocations verifierFuitesAllocations;
#endif
	bibliotheque_cours::activerCouleursAnsi();
	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	ListeFilms listeFilms("films.bin");

	vector<shared_ptr<Item>> bibliothequeItems;

	transfererFilms(bibliothequeItems, listeFilms);
	ajouterLivre(bibliothequeItems, "livresbiblio.txt");

	Film* ptrFilmLeHobbit = dynamic_cast<Film*>(bibliothequeItems[4].get());
	Livre* ptrLivreLeHobbit = dynamic_cast<Livre*>(bibliothequeItems[9].get());

	FilmLivre filmLivre(*ptrFilmLeHobbit, *ptrLivreLeHobbit);

	bibliothequeItems.push_back(make_shared<FilmLivre>(filmLivre));

	afficherListeItems(bibliothequeItems);

	forward_list<shared_ptr<Item>> listeOrdreOriginal; // 1.1

	copierPtrItems(listeOrdreOriginal, bibliothequeItems);

	cout << ligneDeSeparation << endl;
	afficherListeItems(listeOrdreOriginal);
	
	forward_list<shared_ptr<Item>> listeOrdreInverse; // 1.2

	copierListEnOrdreInverse(listeOrdreOriginal, listeOrdreInverse);

	cout << ligneDeSeparation << endl;
	afficherListeItems(listeOrdreInverse);

	forward_list<shared_ptr<Item>> copieListeOriginale; // 1.3

	copierListeOriginale(listeOrdreOriginal, copieListeOriginale);

	cout << ligneDeSeparation << endl;
	afficherListeItems(copieListeOriginale);

	vector<shared_ptr<Item>> vectListOriginaleInverse; // 1.4

	inverserListVect(listeOrdreOriginal, vectListOriginaleInverse); // complexite de type O(n2)

	cout << ligneDeSeparation << endl;
	afficherListeItems(vectListOriginaleInverse);
	
	//cout << "The wanted list is: " << vectListOriginaleIverse.empty() << endl; // test pour verif sur le list

	//copy_if(listeOrdreOriginal.begin(), listeOrdreOriginal.end(), back_inserter(films), [](const std::shared_ptr<Item>& item) { return dynamic_cast<Film*>(item.get()) != nullptr; });


	return 0;
}
