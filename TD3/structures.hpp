#pragma once



// Structures mémoires pour une collection de films.

#include <string>
#include <memory>
#include <cassert>
#include "gsl/span"
using gsl::span;
using namespace std;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
public:
	ListeFilms() = default;
	ListeFilms(const std::string& nomFichier);
	ListeFilms(const ListeFilms& l) { assert(l.elements == nullptr); } // Pas demandé dans l'énoncé, mais on veut s'assurer qu'on ne fait jamais de copie de liste, car la copie par défaut ne fait pas ce qu'on veut.  Donc on ne permet pas de copier une liste non vide (la copie de liste vide est utilisée dans la création d'un acteur).
	~ListeFilms();

	void ajouterFilm(Film* film);
	void enleverFilm(const Film* film);

	shared_ptr<Acteur> trouverActeur(const std::string& nomActeur); // methode const n'est plus const
	span<Film*> enSpan() const;
	int size() const { return nElements; }

	Film*& operator[](int const index);
	Film*& operator[](int const index) const;

	//CH.10
	Film* rechercheCritereFilm(const auto& critereDeRecherche) const;
	void retourRechercheCritereFilm(const ListeFilms& listeFilms);

private:
	void changeDimension(int nouvelleCapacite);

	int capacite = 0, nElements = 0;
	Film** elements = nullptr; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
	bool possedeLesFilms_ = false; // Les films seront détruits avec la liste si elle les possède.
};

//Chapitre 6 & chapitre 9.
template<class T >
class Liste {
public:
	
	Liste() = default;
	//~Liste() = default;

	// Méthode pour modifier la capacité de la la liste
	void modifierCapacite(int newCapacite) {
		capacite_ = newCapacite;
	}

	// Méthode pour modifier le nombre d'élément de la la liste
	void modifierNElements(int newNElements) {
		nElements_ = newNElements;
	}

	// Méthode pour modifier un élément de la la liste
	void modifierElements(unique_ptr<shared_ptr<T>[]> newElements) {
		elements_ = move(newElements);
	}
	
	void CreationPointeursElementsString(string* textes) {
		modifierElements(make_unique<shared_ptr<string>[]>(accesNElements()));
		for (int i = 0; i < accesNElements(); i++)
			accesElements()[i] = make_shared<string>(textes[i]);
	}

	//Méthode pour avoir la capacité actuelle de la liste
	int accesCapacite() const {
		return capacite_;
	}

	//Méthode pour avoir le nombre d'éléments actuel de la liste
	int accesNElements() const {
		return nElements_;
	}

	//Méthode pour avoir un élément de la liste (dans notre cas un shared_ptr)
	auto accesElements() const {
		return elements_.get();
	}
	//Constructeur de liste avec la taille du tableau et les différents types
	Liste(int tailleTableauElements) : capacite_(tailleTableauElements), nElements_(tailleTableauElements) {
	elements_ = make_unique<shared_ptr<T>[]>(nElements_);
	}
	
	Liste(const Liste<T>& autre) : capacite_(autre.capacite_), nElements_(autre.nElements_) {
		elements_ = make_unique<shared_ptr<T>[]>(autre.nElements_);
		for (int j = 0;  j < autre.nElements_; j++) {
			elements_[j] = autre.elements_[j];
		}
	}
	
	//Méthode pour le span d'une liste d'acteur en particulier.
	span<shared_ptr<T>> spanListeActeurs() const;
	
	//Déclaration de la surcharge de l'opérateur [] dans une liste
	shared_ptr<T> operator [](int index);

	//Méthode pour modifier un éléments d'une liste avec son index
	void modifierElementsIndex(const shared_ptr<T> ptr, int const index) {
		if (index < capacite_) {
			elements_[index] = ptr;
		}
	}

private:
	int capacite_ = 0, nElements_ = 0;
	unique_ptr<shared_ptr<T>[]> elements_;
};

using ListeActeurs = Liste<Acteur>;

class Item
{
public:

	//Item() = default;
	//Item(string titre, int anneeSortie): titre_(titre), anneeSortie_(anneeSortie) {};
	string titre_ = "default";
	int anneeSortie_ = 0;

private:

};

class Film: public Item
{

public:

	//Film() = default;
	//Film(string realisateur, int recette, ListeActeurs acteurs): realisateur_(realisateur), recette_(recette), acteurs_(acteurs),  Item() {};
	ListeActeurs acteurs_;

	friend ostream& operator<< (std::ostream& o, const Film& film);
	friend Film* lireFilm(istream& fichier, ListeFilms& listeFilms);
	friend shared_ptr<Acteur> ListeFilms::trouverActeur(const string& nomActeur);

	string realisateur_ = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int recette_ = 0; // Année de sortie et recette globale du film en millions de dollars
};


class Livre : public Item
{
public:
	//Livre() = default;
	//Livre(string auteur, int nMillionsDeCopiesVendues, int nPages) : auteur_(auteur), nMillionsDeCopiesVendues_(nMillionsDeCopiesVendues), nPages_(nPages),
	//Item() {};
	string auteur_ = "";
	int nMillionsDeCopiesVendues_ = 0, nPages_ = 0;
};


struct Acteur
{
	string nom = ""; int anneeNaissance = 0; char sexe = ' ';
};