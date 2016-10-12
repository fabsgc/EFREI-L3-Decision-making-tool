#ifndef STRUCTURES_HPP_INCLUDED
#define STRUCTURES_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <list>
#include <map>
#include <algorithm>

using namespace std;

typedef pair<int, list<int>> Variable;
typedef vector<Variable> ListeVariables;

enum contrainteType {DIFFERENT = 0, EGALE = 1, SOMME = 2, INFERIEUR = 3, SUPERIEUR = 4};
enum constructionArbreType {DOMAINE_PETIT = 0, DOMAINE_GRAND = 1, CONTRAINTES_PLUS = 2, CONTRAINTES_MOINS = 3};

struct Noeud{
    int valeur;
    Noeud* precedent;
    list<Noeud*> suivants;
};

struct Arbre{
    Noeud* sommet;
};

struct Contrainte{
    list<int> variables;
    int type;
    int valeur;
};

struct Donnees{
    int nVariables;
    int nContraintes;
    int triType;
    ListeVariables variables;
    list<Contrainte> contraintes;
    Arbre arbre;
    list<map<int,Noeud*>> noeudsSolution;
};

typedef pair<int,int> CoupleVariables;
typedef map<CoupleVariables, list<Contrainte>> CouplesVariables;

#endif // STRUCTURES_HPP_INCLUDED
