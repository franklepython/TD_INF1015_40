/**
* Un programme qui défénit les structures qui organisent dans une bibliotheque des livres, des films et des FilmLivres sous forme d'Items.
* \file   structures.hpp
* \author Ammah et Trahan
* \date   22 mars 2023
* Créé le 7 mars 2023
*/


#pragma once



// Structures mémoires pour une collection d'Items.

#include <string>
#include <memory>
#include <cassert>
#include "gsl/span"
using gsl::span;
using namespace std;

struct Film; struct Acteur;

class ListeFilms {
public:
	ListeFilms() = default;
	ListeFilms(const std::string& nomFichier);
	ListeFilms(const ListeFilms& l) { assert(l.elements == nullptr); } 
	~ListeFilms();

	void ajouterFilm(Film* film);
	void enleverFilm(const Film* film);

	shared_ptr<Acteur> trouverActeur(const std::string& nomActeur); 
	span<Film*> enSpan() const;
	int size() const { return nElements; }

	Film*& operator[](int const index);

private:
	void changeDimension(int nouvelleCapacite);

	int capacite = 0, nElements = 0;
	Film** elements = nullptr; 
	bool possedeLesFilms_ = false; 
};

template<class T >
class Liste {
public:

	Liste() = default;

	void modifierCapacite(int newCapacite) {
		capacite_ = newCapacite;
	}

	void modifierNElements(int newNElements) {
		nElements_ = newNElements;
	}

	void modifierElements(unique_ptr<shared_ptr<T>[]> newElements) {
		elements_ = move(newElements);
	}

	void CreationPointeursElementsString(string* textes) {
		modifierElements(make_unique<shared_ptr<string>[]>(accesNElements()));
		for (int i = 0; i < accesNElements(); i++)
			accesElements()[i] = make_shared<string>(textes[i]);
	}

	int accesCapacite() const {
		return capacite_;
	}

	int accesNElements() const {
		return nElements_;
	}

	auto accesElements() const {
		return elements_.get();
	}

	Liste(int tailleTableauElements) : capacite_(tailleTableauElements), nElements_(tailleTableauElements) {
		elements_ = make_unique<shared_ptr<T>[]>(nElements_);
	}

	Liste(const Liste<T>& autre) : capacite_(autre.capacite_), nElements_(autre.nElements_) {
		elements_ = make_unique<shared_ptr<T>[]>(autre.nElements_);
		for (int j = 0; j < autre.nElements_; j++) {
			elements_[j] = autre.elements_[j];
		}
	}

	span<shared_ptr<T>> spanListeActeurs() const;

	shared_ptr<T> operator [](int index);

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




class Affichable {
public:
	virtual void afficher(ostream& o) const = 0;
	virtual ~Affichable() = default;
};

class Item : public Affichable
{
public:
	//Pour qu'on garde le titre et l'annee de sortie en private
	string accesTitre() const {
		return titre_;
	}

	int accesAnneeSortie() const {
		return anneeSortie_;
	}
	void modifierAnneeSortie(int nouvelleAnneeSortie) {
		anneeSortie_ = nouvelleAnneeSortie;
	}

	void modifierTitre(string nouveauTitre) {
		titre_ = nouveauTitre;
	}

	void afficher(ostream& os) const override;
	Item() = default;
	Item(string titre, int anneeSortie) : titre_(titre), anneeSortie_(anneeSortie) {};
	virtual ~Item() = default;

private:
	string titre_ = "default";
	int anneeSortie_ = 0;
};

class Film : virtual public Item
{

public:
	void afficher(ostream& os) const override;
	Film() = default;
	Film(string titre, int anneeSortie, string realisateur, int recette, ListeActeurs acteurs) : realisateur_(realisateur),
		recette_(recette), acteurs_(acteurs), Item(titre, anneeSortie) {};

	ListeActeurs acteurs_;

	friend ostream& operator<< (std::ostream& o, const Film& film);
	friend Film* lireFilm(istream& fichier, ListeFilms& listeFilms);
	friend shared_ptr<Acteur> ListeFilms::trouverActeur(const string& nomActeur);

private:
	string realisateur_ = "";
	int recette_ = 0;

};


class Livre : virtual public Item
{
public:
	void afficher(ostream& os) const override;
	Livre() = default;
	Livre(string titre, int anneSortie, string auteur, int nMillionsDeCopiesVendues, int nPages) : auteur_(auteur), nMillionsDeCopiesVendues_(nMillionsDeCopiesVendues), nPages_(nPages),
		Item(titre, anneSortie) {};

private:
	string auteur_ = "";
	int nMillionsDeCopiesVendues_ = 0, nPages_ = 0;
};

class FilmLivre : public Film, public Livre {

public:
	FilmLivre(Film film, Livre livre) : Film(film), Livre(livre), Item(film.accesTitre(), film.accesAnneeSortie()) {};
	void afficher(ostream& os) const override;
};

struct Acteur
{
	string nom = ""; int anneeNaissance = 0; char sexe = ' ';
};

