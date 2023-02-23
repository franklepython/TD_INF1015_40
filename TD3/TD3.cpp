//*** Solutionnaire version 2, sans les //[ //] au bon endroit car le code est assez diff�rent du code fourni.
#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de cha�nes qui sont consid�r�es non s�curitaires.

#include "structures.hpp"      // Structures de donn�es pour la collection de films en m�moire.

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de m�moire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "debogage_memoire.hpp"        // Ajout des num�ros de ligne des "new" dans le rapport de fuites.  Doit �tre apr�s les include du syst�me, qui peuvent utiliser des "placement new" (non support� par notre ajout de num�ros de lignes).
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


//TODO: Une fonction pour ajouter un Film � une ListeFilms, le film existant d�j�; on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit doubler la taille du tableau allou�, avec au minimum un �l�ment, dans le cas o� la capacit� est insuffisante pour ajouter l'�l�ment.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y avait dans l'ancien, et �liminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
//[
void ListeFilms::changeDimension(int nouvelleCapacite)
{
	Film** nouvelleListe = new Film * [nouvelleCapacite];

	if (elements != nullptr) {  // Noter que ce test n'est pas n�cessaire puique nElements sera z�ro si elements est nul, donc la boucle ne tentera pas de faire de copie, et on a le droit de faire delete sur un pointeur nul (�a ne fait rien).
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

//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en param�tre un pointeur vers le film � enlever.  L'ordre des films dans la liste n'a pas � �tre conserv�.
//[
// On a juste fait une version const qui retourne un span non const.  C'est valide puisque c'est la struct qui est const et non ce qu'elle pointe.  �a ne va peut-�tre pas bien dans l'id�e qu'on ne devrait pas pouvoir modifier une liste const, mais il y aurais alors plusieurs fonctions � �crire en version const et non-const pour que �a fonctionne bien, et ce n'est pas le but du TD (il n'a pas encore vraiment de mani�re propre en C++ de d�finir les deux d'un coup).
span<Film*> ListeFilms::enSpan() const { return span(elements, nElements); }

void ListeFilms::enleverFilm(const Film* film)
{
	for (Film*& element : enSpan()) {  // Doit �tre une r�f�rence au pointeur pour pouvoir le modifier.
		if (element == film) {
			if (nElements > 1)
				element = elements[nElements - 1];
			nElements--;
			return;
		}
	}
}
//]

//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouv�.  Devrait utiliser span.
//[
// Voir la NOTE ci-dessous pourquoi Acteur* n'est pas const.  Noter que c'est valide puisque c'est la struct uniquement qui est const dans le param�tre, et non ce qui est point� par la struct.
span<Acteur*> spanListeActeurs(const ListeActeurs& liste) { return span(liste.elements, liste.nElements); }

//NOTE: Doit retourner un Acteur modifiable, sinon on ne peut pas l'utiliser pour modifier l'acteur tel que demand� dans le main, et on ne veut pas faire �crire deux versions.
Acteur* ListeFilms::trouverActeur(const string& nomActeur) const
{
	for (const Film* film : enSpan()) {
		for (Acteur* acteur : spanListeActeurs(film->acteurs)) {
			if (acteur->nom == nomActeur)
				return acteur;
		}
	}
	return nullptr;
}
//]

//TODO: Compl�ter les fonctions pour lire le fichier et cr�er/allouer une ListeFilms.  La ListeFilms devra �tre pass�e entre les fonctions, pour v�rifier l'existence d'un Acteur avant de l'allouer � nouveau (cherch� par nom en utilisant la fonction ci-dessus).
Acteur* lireActeur(istream& fichier//[
	, ListeFilms& listeFilms//]
)
{
	Acteur acteur = {};
	acteur.nom = lireString(fichier);
	acteur.anneeNaissance = lireUint16(fichier);
	acteur.sexe = lireUint8(fichier);
	//[
	Acteur* acteurExistant = listeFilms.trouverActeur(acteur.nom);
	if (acteurExistant != nullptr)
		return acteurExistant;
	else {
		cout << "Cr�ation Acteur " << acteur.nom << endl;
		return new Acteur(acteur);
	}
	//]
	return {}; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait d�j�.  Pour fins de d�bogage, affichez les noms des acteurs cr�es; vous ne devriez pas voir le m�me nom d'acteur affich� deux fois pour la cr�ation.
}

Film* lireFilm(istream& fichier//[
	, ListeFilms& listeFilms//]
)
{
	Film film = {};
	film.titre = lireString(fichier);
	film.realisateur = lireString(fichier);
	film.anneeSortie = lireUint16(fichier);
	film.recette = lireUint16(fichier);
	film.acteurs.nElements = lireUint8(fichier);  //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de r�allocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour r�utiliser cette r�allocation.
	//[
	Film* filmp = new Film(film); //NOTE: On aurait normalement fait le "new" au d�but de la fonction pour directement mettre les informations au bon endroit; on le fait ici pour que le code ci-dessus puisse �tre directement donn� aux �tudiants sans qu'ils aient le "new" d�j� �crit.
	cout << "Cr�ation Film " << film.titre << endl;
	filmp->acteurs.elements = new Acteur * [filmp->acteurs.nElements];
	/*
	//]
	for (int i = 0; i < film.acteurs.nElements; i++) {
		//[
	*/
	for (Acteur*& acteur : spanListeActeurs(filmp->acteurs)) {
		acteur =
			//]
			lireActeur(fichier//[
				, listeFilms//]
			); //TODO: Placer l'acteur au bon endroit dans les acteurs du film.
			//TODO: Ajouter le film � la liste des films dans lesquels l'acteur joue.
		//[
		acteur->joueDans.ajouterFilm(filmp);
		//]
	}
	//[
	return filmp;
	//]
	return {}; //TODO: Retourner le pointeur vers le nouveau film.
}

ListeFilms::ListeFilms(const string& nomFichier) : possedeLesFilms_(true)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);

	//TODO: Cr�er une liste de films vide.
	//[
	/*
	//]
	for (int i = 0; i < nElements; i++) {
		//[
	*/
	for ([[maybe_unused]] int i : range(nElements)) { //NOTE: On ne peut pas faire un span simple avec spanListeFilms car la liste est vide et on ajoute des �l�ments � mesure.
		ajouterFilm(
			//]
			lireFilm(fichier//[
				, *this  //NOTE: L'utilisation explicite de this n'est pas dans la mati�re indiqu�e pour le TD2.
				//]
			)//[
		)
			//]
			; //TODO: Ajouter le film � la liste.
	}

	//[
	/*
	//]
	return {}; //TODO: Retourner la liste de films.
	//[
	*/
	//]
}

//TODO: Une fonction pour d�truire un film (rel�cher toute la m�moire associ�e � ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film d�truit des films dans lesquels jouent les acteurs.  Pour fins de d�bogage, affichez les noms des acteurs lors de leur destruction.
//[
void detruireActeur(Acteur* acteur)
{
	cout << "Destruction Acteur " << acteur->nom << endl;
	delete acteur;
}
bool joueEncore(const Acteur* acteur)
{
	return acteur->joueDans.size() != 0;
}
void detruireFilm(Film* film)
{
	for (Acteur* acteur : spanListeActeurs(film->acteurs)) {
		acteur->joueDans.enleverFilm(film);
		if (!joueEncore(acteur))
			detruireActeur(acteur);
	}
	cout << "Destruction Film " << film->titre << endl;
	delete[] film->acteurs.elements;
	delete film;
}
//]

//TODO: Une fonction pour d�truire une ListeFilms et tous les films qu'elle contient.
//[
//NOTE: Attention que c'est difficile que �a fonctionne correctement avec le destructeur qui d�truit la liste.  Mon ancienne impl�mentation utilisait une m�thode au lieu d'un destructeur.  Le probl�me est que la mati�re pour le constructeur de copie/move n'est pas dans le TD2 mais le TD3, donc si on copie une liste (par exemple si on la retourne de la fonction creerListe) elle sera incorrectement copi�e/d�truite.  Ici, creerListe a �t� converti en constructeur, qui �vite ce probl�me.
ListeFilms::~ListeFilms()
{
	if (possedeLesFilms_)
		for (Film* film : enSpan())
			detruireFilm(film);
	delete[] elements;
}
//]

void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).
//[
void afficherFilm(const Film& film)
{
	cout << "Titre: " << film.titre << endl;
	cout << "  R�alisateur: " << film.realisateur << "  Ann�e :" << film.anneeSortie << endl;
	cout << "  Recette: " << film.recette << "M$" << endl;

	cout << "Acteurs:" << endl;
	for (const Acteur* acteur : spanListeActeurs(film.acteurs))
		afficherActeur(*acteur);
}
//]

ostream& operator << (ostream& o, const Film& film) {

	o << "Titre: " << film.titre << endl;
	o << "  R�alisateur: " << film.realisateur << "  Ann�e :" << film.anneeSortie << endl;
	o << "  Recette: " << film.recette << "M$" << endl;
	o << "Acteurs:" << endl;

	for (const Acteur* acteur : spanListeActeurs(film.acteurs))
		afficherActeur(*acteur);

	return o;
};



void afficherListeFilms(const ListeFilms& listeFilms)
{
	//TODO: Utiliser des caract�res Unicode pour d�finir la ligne de s�paration (diff�rente des autres lignes de s�parations dans ce progamme).
	static const string ligneDeSeparation = //[
		"\033[32m????????????????????????????????????????\033[0m\n";
	/*
	//]
	{};
//[ */
//]
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	//[
	/*//]
	for (int i = 0; i < listeFilms.nElements; i++) {
		//[*/
	for (const Film* film : listeFilms.enSpan()) {
		//]
		//TODO: Afficher le film.
		//[
		afficherFilm(*film);
		//]
		cout << ligneDeSeparation;
	}
}

void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur)
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre � nullptr).
	const Acteur* acteur = //[
		listeFilms.trouverActeur(nomActeur);
	/* //]
	nullptr;
//[ */
//]
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		afficherListeFilms(acteur->joueDans);
}

int main()
{
#ifdef VERIFICATION_ALLOCATION_INCLUS
	bibliotheque_cours::VerifierFuitesAllocations verifierFuitesAllocations;
#endif
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par d�faut.

	int* fuite = new int; //TODO: Enlever cette ligne apr�s avoir v�rifi� qu'il y a bien un "Detected memory leak" de "4 bytes" affich� dans la "Sortie", qui r�f�re � cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m????????????????????????????????????????\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions �crites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la m�moire n�cessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de d�bogage dans votre fonction lireActeur).
	ListeFilms listeFilms("films.bin");

	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	//TODO: Afficher le premier film de la liste.  Devrait �tre Alien.
	//[
	afficherFilm(*listeFilms.enSpan()[0]);
	//]

	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	//[
	afficherListeFilms(listeFilms);
	//]

	//TODO: Modifier l'ann�e de naissance de Benedict Cumberbatch pour �tre 1976 (elle �tait 0 dans les donn�es lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	//[
	listeFilms.trouverActeur("Benedict Cumberbatch")->anneeNaissance = 1976;
	//]

	cout << ligneDeSeparation << "Liste des films o� Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films o� Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	//[
	afficherFilmographieActeur(listeFilms, "Benedict Cumberbatch");
	//]

	//TODO: D�truire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) d�truire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	//[
	detruireFilm(listeFilms.enSpan()[0]);
	listeFilms.enleverFilm(listeFilms.enSpan()[0]);
	//]

	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	//[
	afficherListeFilms(listeFilms);
	//]

	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non ex�cut�es dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas ex�cut�e, si finalement vous pensez qu'elle ne sont pas utiles.
	//[
	// Les lignes � mettre ici d�pendent de comment ils ont fait leurs fonctions.  Dans mon cas:
	//listeFilms.enleverFilm(nullptr); // Enlever un film qui n'est pas dans la liste (clairement que nullptr n'y est pas).
	//afficherFilmographieActeur(listeFilms, "N'existe pas"); // Afficher les films d'un acteur qui n'existe pas.
	//]

	//TODO: D�truire tout avant de terminer le programme.  L'objet verifierFuitesAllocations devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
//[
//]
}