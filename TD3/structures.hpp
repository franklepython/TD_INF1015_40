#pragma once
// Structures m�moires pour une collection de films.

#include <string>
#include <cassert>
#include "gsl/span"
#include <memory>
using gsl::span;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront d�fini apr�s.

class ListeFilms {
public:
	ListeFilms() = default;
	ListeFilms(const std::string& nomFichier);
	ListeFilms(const ListeFilms& l) { assert(l.elements == nullptr); } // Pas demand� dans l'�nonc�, mais on veut s'assurer qu'on ne fait jamais de copie de liste, car la copie par d�faut ne fait pas ce qu'on veut.  Donc on ne permet pas de copier une liste non vide (la copie de liste vide est utilis�e dans la cr�ation d'un acteur).
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
	bool possedeLesFilms_ = false; // Les films seront d�truits avec la liste si elle les poss�de.
};

struct ListeActeurs {
	int capacite, nElements;
	unique_ptr<Acteur* [] > elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du r�alisateur (on suppose qu'il n'y a qu'un r�alisateur).
	int anneeSortie, recette; // Ann�e de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;

	friend ostream& operator << (ostream& o, const Film& film);
};

struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};