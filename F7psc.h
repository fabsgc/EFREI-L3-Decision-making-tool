#ifndef PSC_H_INCLUDED
#define PSC_H_INCLUDED

#include "F7fonctions.h"

using namespace std;

void lectureFichier(Donnees& donnees);
void creerSolution();
bool differents(list<int> liste);
bool egales(list<int> liste);
bool differents(list<int> liste);
bool somme(list<int> liste, int valeur);
bool inferieur(list<int> liste, int valeur);
bool superieur(list<int> liste, int valeur);
void parcoursPronfondeurNaif(Donnees& donnees, Noeud* noeudParent, map<int, Noeud*> chemin, int variableCourante);
void parcoursPronfondeurReductionDomaine(Donnees& donnees, Noeud* noeudParent, map<int, Noeud*> chemin, int variableCourante, map<int, ListeVariables> listeVariablesTri);
list<int> listeVariablesToListeValeurs(list<int>& variables, map<int, Noeud*>& chemin);

void variablePlusPetitDomaine(Donnees& donnees);
void variablePlusGrandDomaine(Donnees& donnees);
bool TriAscendant(Variable& lhs, Variable& rhs);
bool TriDescendant(Variable& lhs, Variable& rhs);
void variablePlusContraignante(Donnees& donnees, map<int, Noeud*>& chemin);
void variableMoinsContraignante(Donnees& donnees, map<int, Noeud*>& chemin);
int nombreContraintesVariables(Variable& variable, Donnees& donnees, ListeVariables& variablesTmp);
void supprimerVariableAssignees(ListeVariables& variablesTriees, ListeVariables& variablesTmp, map<int, Noeud*>& chemin);

bool ac3(Donnees& donnees);
bool ac3Reduction(Donnees& donnees, int varX, int varY, list<Contrainte>& contraintes);

void afficherVariables(Donnees& donnees);
void afficherSolutions(Donnees& donnees);
string afficherChemin(map<int, Noeud*>& chemin);

#endif // PSC_H_INCLUDED
