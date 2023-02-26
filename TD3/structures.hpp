#include <memory>
#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <cassert>
#include "gsl/span"
using gsl::span;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
public:
	ListeFilms() = default;
	ListeFilms(const std::string& nomFichier);
	ListeFilms(const ListeFilms& l) { assert(l.elements == nullptr); } // Pas demandé dans l'énoncé, mais on veut s'assurer qu'on ne fait jamais de copie de liste, car la copie par défaut ne fait pas ce qu'on veut.  Donc on ne permet pas de copier une liste non vide (la copie de liste vide est utilisée dans la création d'un acteur).
	~ListeFilms();
	void ajouterFilm(Film* film);
	void enleverFilm(const Film* film);
	Acteur* trouverActeur(const std::string& nomActeur) const;
	span<Film*> enSpan() const;
	int size() const { return nElements; }

private:
	void changeDimension(int nouvelleCapacite);

	int capacite = 0, nElements = 0;
	Film** elements = nullptr; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
	bool possedeLesFilms_ = false; // Les films seront détruits avec la liste si elle les possède.
};

class ListeActeurs {

public:
	ListeActeurs(int tailleTableauElements) : capacite(tailleTableauElements), elements(make_unique<Acteur* []>(tailleTableauElements)) {};
	int capacite = 0, nElements = 0;
	//Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
	unique_ptr<Acteur* []> elements;

private:
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
	friend std::ostream& operator<< (std::ostream& o, const Film& film);
};

struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};
