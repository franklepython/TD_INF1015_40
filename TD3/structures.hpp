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
	shared_ptr<Acteur> trouverActeur(const std::string& nomActeur) const;
	span<Film*> enSpan() const;
	int size() const { return nElements; }

	Film*& operator[](int const index);
	Film*& operator[](int const index) const;
	Film* rechercheCritereFilm(const auto& critereDeRecherche) const;

private:
	void changeDimension(int nouvelleCapacite);

	int capacite = 0, nElements = 0;
	Film** elements = nullptr; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
	bool possedeLesFilms_ = false; // Les films seront détruits avec la liste si elle les possède.
};

template<typename T>
class Liste {
public:

	Liste() = default;
	Liste(int tailleTableauElements) : capacite(tailleTableauElements), nElements(tailleTableauElements), elements(make_unique<shared_ptr<T>[]>(tailleTableauElements)) {};
	Liste(Liste& autre) : capacite(autre.capacite), nElements(autre.nElements), elements(make_unique<shared_ptr<T>[]>(autre.capacite)) {

		for (int i = 0; i < autre.nElements; i++) {
			elements[i] = make_shared<T>(*autre.elements[i]);
		}

	}

	void modifierElements(const shared_ptr<T> const ptr, const int const index) {
		if (index < capacite) {
			elements[index] = ptr;
		}
	}

	int capacite = 0, nElements = 0;
	unique_ptr<shared_ptr<T>[]> elements;
private:
};

using ListeActeurs = Liste<Acteur>;



struct Film
{
	Film() = default;

	string titre = "aucun", realisateur = "aucun"; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie = 0, recette = 0; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
	friend std::ostream& operator<< (std::ostream& o, const Film& film);

	//Film(const Film& autreFilm);
};

struct Acteur
{
	std::string nom = "aucun"; int anneeNaissance = 0; char sexe = 0;
	//ListeFilms joueDans;
};