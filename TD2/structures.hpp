#pragma once
// Structures mémoires pour une collection de films.

#include <string>

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
public:
	ListeFilms() = default;
	~ListeFilms();

	void ajouterFilm(Film* film);
	void enleverFilm(Film* ptrVersFilm);

	Acteur* trouverActeur(const string nomActeur) const;
	Acteur* lireActeur(istream& fichier);

	Film* lireFilm(istream& fichier);
	ListeFilms listeFilms(string nomFichier);

	void relacherMemoireActeurs(const string nomFilm);
	void enleverFilmDesListes(const string nomFilm);
	void detruireFilm(const string nomFilm);

	void detruireListeFilms();

	void afficherActeur(const Acteur& acteur) const;
	void afficherActeursFilm(const Film* film) const;
	void afficherListeFilms() const;
	void afficherFilmographieActeur(const string& nomActeur) const;

private:
	int capacite_=0, nElements_=0;
	Film** elements;
};


template <class T>
class Liste {
public:
	Liste() = default;
	~Liste() = default;
	
	void modifierCapacite(int newCapacite)
	{
		capacite_ = newCapacite;
	}
	void modifierNElements(int newNElements)
	{
		nElements_ = newNElements;
	}
	void modifierElements(unique_ptr<shared_ptr<T>[]> newElements)
	{
		elements_ = move(newElements);
	}

	int accesCapacite() const {
		return capacite_;
	}
	int accesNelements() const {
		return nElements_
	}
	auto accesElements() const {
		return elements_.get();
	}
	span<shared_ptr<T>>::spanListeActeurs() const;
	Liste<T>(const Liste<T>& autre)
	{
		*this = autre;
	}
	Liste<T>& operator=(const Liste<T>& autre)
	{
		if (this != &autre)
		{
			capacite_ = autre.capacite_;
			nElements_ = autre.nElements_;
			elements_ = make_unique<shared_ptr<T>[]>(nElements_);
			for (int i = 0; i < nElements_; i++)
			{
				elements_[i] = autre.elements_[i];
			}
		}
		return *this;
	}
	shared_ptr<T>::operator[](int index);


private:
	int capacite_ = 0, nElements_ = 0;
	unique_ptr<shared_ptr<T>[]> elements_;
};
using ListeActeurs = Liste<Acteur>;


struct Film
{
	std::string titre="", realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
};

struct Acteur
{
	std::string nom=""; int anneeNaissance=0; char sexe=' ';
	ListeFilms joueDans;
	ListeFilms afficherFilmographieActeur;

};

