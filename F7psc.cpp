#include "F7psc.h"

void lectureFichier(Donnees& donnees)
{
    int variable = 0;
    int contrainteType = 0;
    string nom;

    cout << "Nom du fichier : ";
    cin >> nom;
    cout << endl;

    string nomFichier = nom + ".txt";
    ifstream fichier(nomFichier);

    fichier >> donnees.nVariables;
    fichier >> variable;

    while (variable != -1)
    {
        list<int> valeurs;
        int valeur;

        fichier >> valeur;

        while(valeur != -1)
        {
            valeurs.push_back(valeur);
            fichier >> valeur;
        }

        donnees.variables.push_back(make_pair(variable, valeurs));

        fichier >> variable;
    }

    fichier >> donnees.nContraintes;
    fichier >> contrainteType;

    while (contrainteType != -1)
    {
        Contrainte contrainte;
        contrainte.type = contrainteType;

        int valeur;
        list<int> valeurs;

        if(contrainteType == SOMME)
        {
            fichier >> contrainte.valeur;
        }

        fichier >> valeur;

        while(valeur != -1)
        {
            contrainte.variables.push_back(valeur);
            fichier >> valeur;
        }

        donnees.contraintes.push_back(contrainte);

        fichier >> contrainteType;
    }

    afficherVariables(donnees);
}

/**
 * @brief Utilisation de la solution naïve pour créer l'arbre des solutions (fonction récursive) (profondeur d'abord)
 * @param Donnees &donnees : les données provenant du fichier (variables et contraintes)
 * @param Noeud* noeudParent : noeud parent qui a été traité juste avant
 * @param map<int, Noeud*> chemin : chemin complet de la racine jusqu'à une feuille
 * @param int variableCourante : quelle variable nous sommes en train de traiter
 * @return voids
*/

void parcoursPronfondeurNaif(Donnees &donnees, Noeud * noeudParent, map<int, Noeud*> chemin, int variableCourante)
{
    list<int> domaineValeurs = donnees.variables[variableCourante].second;
    int nomVariable = donnees.variables[variableCourante].first;

    for(auto itValeurs = domaineValeurs.begin(); itValeurs != domaineValeurs.end(); itValeurs++)
    {
        Noeud* noeudEnfant = new Noeud;
        noeudEnfant->precedent = noeudParent;
        noeudEnfant->valeur = *itValeurs;
        noeudParent->suivants.push_back(noeudEnfant);
        chemin[nomVariable] = noeudEnfant;

        //si on a atteint la feuille de l'arbre, alors on fait les différents tests
        if(variableCourante == donnees.nVariables-1)
        {
            bool solution = true;

            //on parcours la liste des contraintes
            for(auto itContrainte = donnees.contraintes.begin(); itContrainte != donnees.contraintes.end(); itContrainte++)
            {
                //On récupère la liste des valeurs des variables concernées par cette contrainte
                list<int> variablesValeurs = listeVariablesToListeValeurs(itContrainte->variables, chemin);

                switch(itContrainte->type)
                {
                    case DIFFERENT:
                        if(!differents(variablesValeurs))
                            solution = false;
                    break;

                    case EGALE:
                        if(!egales(variablesValeurs))
                            solution = false;
                    break;

                    case SUPERIEUR:
                        if(!superieur(variablesValeurs, itContrainte->valeur))
                            solution = false;
                    break;

                    case INFERIEUR:
                        if(!inferieur(variablesValeurs, itContrainte->valeur))
                            solution = false;
                    break;

                    case SOMME:
                        if(!somme(variablesValeurs, itContrainte->valeur))
                            solution = false;
                    break;
                }
            }

            if(solution)
            {
                donnees.noeudsSolution.push_back(chemin);
                cout << "Le chemin " << afficherChemin(chemin) << " respecte les contraintes" << endl;
            }
        }
        else
        {
            //sinon, on continue l'algorithme pour la variable suivante
            int nouvelleVariableCourante = variableCourante + 1;
            parcoursPronfondeurNaif(donnees, noeudEnfant, chemin, nouvelleVariableCourante);
        }
    }
}

/**
 * @brief Utilisation de la solution par réduction de domaine pour créer l'arbre des solutions (fonction récursive) (profondeur d'abord)
 * @param Donnees &donnees : les données provenant du fichier (variables et contraintes)
 * @param Noeud* noeudParent : noeud parent qui a été traité juste avant
 * @param map<int, Noeud*> chemin : chemin complet de la racine jusqu'à une feuille
 * @param int variableCourante : quelle variable nous sommes en train de traiter
 * @return voids
*/

void parcoursPronfondeurReductionDomaine(Donnees &donnees, Noeud * noeudParent, map<int, Noeud*> chemin, int variableCourante, map<int, ListeVariables> listeVariablesTri)
{
    list<int> domaineValeurs;
    int nomVariable;

    if((donnees.triType == CONTRAINTES_PLUS || donnees.triType == CONTRAINTES_MOINS) && existe(listeVariablesTri, variableCourante))
    {
        domaineValeurs = listeVariablesTri[variableCourante][variableCourante].second;
        nomVariable = listeVariablesTri[variableCourante][variableCourante].first;
    }
    else
    {
        domaineValeurs = donnees.variables[variableCourante].second;
        nomVariable = donnees.variables[variableCourante].first;
    }

    for(auto itValeurs = domaineValeurs.begin(); itValeurs != domaineValeurs.end(); itValeurs++)
    {
        Noeud* noeudEnfant = new Noeud;
        noeudEnfant->precedent = noeudParent;
        noeudEnfant->valeur = *itValeurs;
        noeudParent->suivants.push_back(noeudEnfant);
        chemin[nomVariable] = noeudEnfant;

        //On réorganise les variables pour savoir laquelle traiter maintenant
        //Cependant, comme à chaque traitement de variables, la liste triée sera la même, on ne le fait qu'une fois
        if(variableCourante < donnees.nVariables-1)
        {
            if((donnees.triType == CONTRAINTES_PLUS || donnees.triType == CONTRAINTES_MOINS) && !existe(listeVariablesTri, variableCourante))
            {
                Donnees donneesTmp = donnees;

                if(donnees.triType == CONTRAINTES_PLUS)
                    variableMoinsContraignante(donneesTmp, chemin);
                else
                    variablePlusContraignante(donneesTmp, chemin);

                listeVariablesTri[variableCourante] = donneesTmp.variables;
            }
        }

        //si on a atteint la feuille de l'arbre, alors on fait les différents tests
        if(variableCourante == donnees.nVariables-1)
        {
            bool solution = true;

            //on parcours la liste des contraintes
            for(auto itContrainte = donnees.contraintes.begin(); itContrainte != donnees.contraintes.end(); itContrainte++)
            {
                //On récupère la liste des valeurs des variables concernées par cette contrainte
                list<int> variablesValeurs = listeVariablesToListeValeurs(itContrainte->variables, chemin);

                switch(itContrainte->type)
                {
                    case SOMME:
                        if(!somme(variablesValeurs, itContrainte->valeur))
                            solution = false;
                    break;
                }
            }

            if(solution)
                donnees.noeudsSolution.push_back(chemin);
        }
        else
        {
            //Pour chaque valeur du domaine de la variable en cours, on regarde si elle respecte les contraintes qu'on peut lui appliquer
            //Si la valeur ne respecte pas les conditions, alors on ne continue pas ce chemin et on passe au suivant
            bool solution = true;

            //on parcours la liste des contraintes
            for(auto itContrainte = donnees.contraintes.begin(); itContrainte != donnees.contraintes.end(); itContrainte++)
            {
                //On récupère la liste des valeurs des variables concernées par cette contrainte
                list<int> variablesValeurs = listeVariablesToListeValeurs(itContrainte->variables, chemin);

                //On vérifie si le chemin contient toutes les variables nécessaires pour appliquer la contrainte
                switch(itContrainte->type)
                {
                    case DIFFERENT:
                        if(!differents(variablesValeurs))
                            solution = false;
                    break;

                    case EGALE:
                        if(!egales(variablesValeurs))
                            solution = false;
                    break;

                    case SUPERIEUR:
                        if(!superieur(variablesValeurs, itContrainte->valeur))
                            solution = false;
                    break;

                    case INFERIEUR:
                        if(!inferieur(variablesValeurs, itContrainte->valeur))
                            solution = false;
                    break;

                    case SOMME:
                        if(variablesValeurs.size() == itContrainte->variables.size() && !somme(variablesValeurs, itContrainte->valeur))
                            solution = false;
                    break;
                }
            }

            string cheminString = afficherChemin(chemin);

            if(solution == true)
            {
                cout << "Le chemin partiel " << cheminString << " respecte les contraintes" << endl;
                int nouvelleVariableCourante = variableCourante + 1;
                parcoursPronfondeurReductionDomaine(donnees, noeudEnfant, chemin, nouvelleVariableCourante, listeVariablesTri);
            }
            else{
                cout << "Le chemin partiel " << cheminString << " ne respecte pas les contraintes" << endl;
            }
        }
    }
}

/**
 * @brief A partir d'une liste de variables récupère leurs valeurs dans le chemin qui a été traité dans l'arbre des solutions
 * @param list<int>& variables : liste des variables
 * @param map<int, Noeud*>& chemin : noeuds de l'arbre déjà traités et donc liste des valeurs des variables
 * @return list<int>
*/

list<int> listeVariablesToListeValeurs(list<int>& variables, map<int, Noeud*>& chemin)
{
    list<int> valeurs;

    for(auto it = variables.begin(); it != variables.end(); it++)
    {
        if(existe(chemin, *it))
            valeurs.push_back(chemin[*it]->valeur);
    }

    return valeurs;
}

/**
 * @brief Creation de la solution
 * @param Donnees &donnees : données d'entrée
 * @return void
*/

void creerSolution()
{
    Donnees donnees;
    lectureFichier(donnees);

    int choix = 1;
    bool coherence = true;
    donnees.arbre.sommet = new Noeud;
    map<int, Noeud*> chemin;

    cout << "# Utiliser la coherence d'arete (0/1) : " << endl;
    cout << "#Votre choix : ";
    cin >> choix;

    if(choix == 1)
        coherence = ac3(donnees);

    if(coherence == false)
        return;

    cout << "# Quel algorithme : " << endl;
    cout << " - 1.Naif" << endl;
    cout << " - 2.Reduction des domaines de valeurs" << endl;
    cout << endl << "#Votre choix : ";
    cin >> choix;

    switch(choix)
    {
        case 1:
        {
            cout << endl << "###########################" << endl;
            cout << "Parcours en profondeur naif" << endl;
            cout << "###########################" << endl << endl;

            parcoursPronfondeurNaif(donnees, donnees.arbre.sommet, chemin, 0);
        }
        break;

        case 2:
        {
            cout << "# Quel methode de tri :" << endl;
            cout << " - 1.Variable la plus contrainte" << endl;
            cout << " - 2.Variable la moins contrainte" << endl;
            cout << " - 3.Variable la plus contraignante" << endl;
            cout << " - 4.Variable la moins contraignante" << endl;
            cout << " - 5.Aucune" << endl;
            cout << endl << "#Votre choix : ";
            cin >> choix;

            switch(choix)
            {
                case 1 :
                    donnees.triType = DOMAINE_PETIT;
                    variablePlusPetitDomaine(donnees);
                break;

                case 2 :
                    donnees.triType = DOMAINE_GRAND;
                    variablePlusGrandDomaine(donnees);
                break;

                case 3 :
                    donnees.triType = CONTRAINTES_PLUS;
                    variablePlusContraignante(donnees, chemin);
                break;

                case 4 :
                    donnees.triType = CONTRAINTES_MOINS;
                    variableMoinsContraignante(donnees, chemin);
                break;
            }

            cout << endl << "################################################" << endl;
            cout << "Parcours en profondeur avec reduction de domaine" << endl;
            cout << "################################################" << endl << endl;

            map<int, ListeVariables> listeVariablesTri;
            parcoursPronfondeurReductionDomaine(donnees, donnees.arbre.sommet, chemin, 0, listeVariablesTri);
        }
        break;
    }

    afficherSolutions(donnees);

    system("PAUSE");
    system("CLS");
}

/**
 * @brief contrainte "différents deux à deux"
 * @param list<int> liste
 * @return bool
*/

bool differents(list<int> liste)
{
    for(auto i1 = liste.begin(); i1 != liste.end(); i1++)
    {
        auto i2 = i1;

        for(auto i3 = ++i2; i3 != liste.end(); i3++)
        {
            if(*i1 == *i3)
                return false;
        }
    }

    return true;
}

/**
 * @brief contrainte "égaux deux à deux"
 * @param list<int> liste
 * @return bool
*/

bool egales(list<int> liste)
{
    for(auto i1 = liste.begin(); i1 != liste.end(); i1++)
    {
        auto i2 = i1;

        for(auto i3 = ++i2; i3 != liste.end(); i3++)
        {
            if(*i1 != *i3)
                return false;
        }
    }

    return true;
}

/**
 * @brief contrainte "somme"
 * @param list<int> liste
 * @return bool
*/

bool somme(list<int> liste, int valeur)
{
    int somme = 0;

    for(int i: liste)
    {
        somme += i;
    }

    if(somme == valeur)
        return true;

    return false;
}

/**
 * @brief contrainte "superieur à"
 * @param list<int> liste
 * @return bool
*/

bool superieur(list<int> liste, int valeur)
{
    for(int i:  liste)
    {
        if(i <= valeur)
            return false;
    }

    return true;
}

/**
 * @brief contrainte "inférieur à"
 * @param list<int> liste
 * @return bool
*/

bool inferieur(list<int> liste, int valeur)
{
    for(int i:  liste)
    {
        if(i >= valeur)
            return false;
    }

    return true;
}

/**
 * @brief trie la liste des variables en fonction de la taille du domaine de chacune dans l'ordre croissant
 * @param Donnees& donnees
 * @return void
*/

void variablePlusPetitDomaine(Donnees& donnees)
{
    cout << "##############################" << endl;
    cout << "Variable au plus petit domaine" << endl;
    cout << "##############################" << endl << endl;

    sort(donnees.variables.begin(), donnees.variables.end(), TriAscendant);
    afficherVariables(donnees);
}

/**
 * @brief trie la liste des variables en fonction de la taille du domaine de chacune dans l'ordre décroissant
 * @param Donnees& donnees
 * @return void
*/

void variablePlusGrandDomaine(Donnees& donnees)
{
    cout << "##############################" << endl;
    cout << "Variable au plus grand domaine" << endl;
    cout << "##############################" << endl << endl;

    sort(donnees.variables.begin(), donnees.variables.end(), TriDescendant);
    afficherVariables(donnees);
}

/**
 * @brief classe de trie ascendant permettant de classer les variables non assignées en fonction du nombre de contraintes qu'elles ont en commun avec des variables non assignées
*/

class TriAscendantContrainte {
    Donnees &donnees;
    ListeVariables &variablesTmp;

    public:
        TriAscendantContrainte(Donnees& d, ListeVariables vTmp) : donnees{d}, variablesTmp{vTmp} {}

        /**
         * @brief fonction de trie ascendant permettant de classer les variables non assignées en
                  fonction du nombre de contraintes qu'elles ont en commun avec des variables non assignées
         * @param Variable & lhs : élément gauche
         * @param Variable & rhs : élément droite
         * @return bool
        */

        bool operator()(Variable & lhs, Variable & rhs)
        {
            int nombreContraintesLhs = nombreContraintesVariables(lhs, donnees, variablesTmp);
            int nombreContraintesRhs = nombreContraintesVariables(rhs, donnees, variablesTmp);
            return nombreContraintesLhs < nombreContraintesRhs;
        }
};

/**
 * @brief classe de trie descendant permettant de classer les variables non assignées en fonction du nombre de contraintes qu'elles ont en commun avec des variables non assignées
*/

class TriDescendantContrainte {
    Donnees &donnees;
    ListeVariables &variablesTmp;

    public:
        TriDescendantContrainte(Donnees& d, ListeVariables vTmp) : donnees{d}, variablesTmp{vTmp} {}

        /**
         * @brief fonction de trie ascendant permettant de classer les variables non assignées en
                  fonction du nombre de contraintes qu'elles ont en commun avec des variables non assignées
         * @param Variable & lhs : élément gauche
         * @param Variable & rhs : élément droite
         * @return bool
        */

        bool operator()(Variable & lhs, Variable & rhs)
        {
            int nombreContraintesLhs = nombreContraintesVariables(lhs, donnees, variablesTmp);
            int nombreContraintesRhs = nombreContraintesVariables(rhs, donnees, variablesTmp);
            return nombreContraintesLhs > nombreContraintesRhs;
        }
};

/**
 * @brief trie la liste des variables en fonction du nombre de contraintes qu'elles ont en commun avec des variables non assignées
 * @param Donnees& donnees : la liste des variables qu'il va falloir trier
 * @param map<int, Noeud*> chemin : cette liste contient les variables qui ont déjà été assignées. On en a besoin car il ne faut pas réorganiser ces variables
          elle doivent toujours être au début, sinon, le programme risque d'échouer
 * @return void
*/

void variablePlusContraignante(Donnees & donnees, map<int, Noeud*>& chemin)
{
    cout << "##############################" << endl;
    cout << "Variable la plus contraignante" << endl;
    cout << "##############################" << endl << endl;

    ListeVariables variablesTriees;
    ListeVariables variablesTmp = donnees.variables;

    supprimerVariableAssignees(variablesTriees, variablesTmp, chemin);
    sort(variablesTmp.begin(), variablesTmp.end(), TriAscendantContrainte(donnees, variablesTmp));
    variablesTriees.insert(variablesTriees.end(), variablesTmp.begin(), variablesTmp.end());

    afficherVariables(donnees);
}

/**
 * @brief trie la liste des variables en fonction du nombre de contraintes qu'elles ont en commun avec des variables non assignées
 * @param Donnees& donnees : la liste des variables qu'il va falloir trier
 * @param map<int, Noeud*> chemin : cette liste contient les variables qui ont déjà été assignées. On en a besoin car il ne faut pas réorganiser ces variables
          elle doivent toujours être au début, sinon, le programme risque d'échouer
 * @return void
*/

void variableMoinsContraignante(Donnees & donnees, map<int, Noeud*>& chemin)
{
    cout << "###############################" << endl;
    cout << "Variable la moins contraignante" << endl;
    cout << "###############################" << endl << endl;

    ListeVariables variablesTriees;
    ListeVariables variablesTmp = donnees.variables;

    supprimerVariableAssignees(variablesTriees, variablesTmp, chemin);
    sort(variablesTmp.begin(), variablesTmp.end(), TriDescendantContrainte(donnees, variablesTmp));
    variablesTriees.insert(variablesTriees.end(), variablesTmp.begin(), variablesTmp.end());

    afficherVariables(donnees);
}

/**
 * @brief dans une liste, supprime les variables déjà assignées
 * @param Donnees& donnees : la liste des variables qu'il va falloir trier : liste des variables
 * @param map<int, Noeud*> chemin : cette liste contient les variables qui ont déjà été assignées
 * @return void
*/

void supprimerVariableAssignees(ListeVariables & variablesTriees, ListeVariables & variablesTmp, map<int, Noeud*>& chemin)
{
    for(auto it = variablesTmp.begin(); it != variablesTmp.end(); it++)
    {
        if(existe(chemin, it->first))
        {
            variablesTriees.push_back(*it);
            variablesTmp.erase(it);
        }
    }
}

/**
 * @brief fonction de trie ascendant sur vector de pair
 * @param Variable & lhs : élément gauche
 * @param Variable & rhs : élément droite
 * @return bool
*/

bool TriAscendant(Variable & lhs, Variable & rhs)
{
    return lhs.second.size() < rhs.second.size();
}

/**
 * @brief fonction de trie descendant sur vector de pair
 * @param Variable & lhs : élément gauche
 * @param Variable & rhs : élément droite
 * @return bool
*/

bool TriDescendant(Variable & lhs, Variable & rhs)
{
    return lhs.second.size() > rhs.second.size();
}

/**
 * @brief Pour une variable non assignée, détermine le nombre de contraintes qu'elle possède en relation avec d'autres variables non assignées
 * @param Variable & variable : la variable à traiter
 * @param Donnees& donnees : contient la liste des contraintes
 * @param ListeVariables & variablesTmp : liste des variables non assignées
 * @return int
*/

int nombreContraintesVariables(Variable & variable, Donnees& donnees, ListeVariables & variablesTmp)
{
    int nombreContraintes = 0;

    //on calcule le nombre de contraintes que possèd it en relation avec des variables aussi non assignées
    for(auto itContrainte = donnees.contraintes.begin(); itContrainte != donnees.contraintes.end(); itContrainte++)
    {
        //si la contrainte concerne cette variable
        if(existe(itContrainte->variables, variable.first))
        {
            //on verifie si la contrainte concerne au moins une autre variable non assignée
            for(auto i : variablesTmp)
            {
                if(i.first != variable.first && existe(itContrainte->variables, i.first))
                {
                    nombreContraintes++;
                    break;
                }
            }
        }
    }

    return nombreContraintes;
}

bool ac3(Donnees& donnees)
{
    cout << "#################" << endl;
    cout << "Coherence d'arete" << endl;
    cout << "#################" << endl << endl;

    CouplesVariables couplesVariables;

    //On créé d'abord la queue qui contient tous les couples de variables à traiter
    for(auto x = donnees.variables.begin(); x != donnees.variables.end(); x++)
    {
        for(auto y = donnees.variables.begin(); y != donnees.variables.end(); y++)
        {
            if(x->first != y->first)
            {
                //on trouve toutes les contraintes binaires qui comprennent le couple (x,y)
                for(auto contrainte: donnees.contraintes)
                {
                    //on ne prend que des contraintes binaires
                    if(contrainte.type == DIFFERENT || contrainte.type == EGALE)
                    {
                        //on regarde si la contrainte donnée impacte sur les variables x et y
                        if(existe(contrainte.variables, x->first) && existe(contrainte.variables, y->first))
                            couplesVariables[make_pair(x->first,y->first)].push_back(contrainte);
                    }
                }
            }
        }
    }

    cout << "# liste des couples de variables a traiter : " << endl;

    for(auto coupleVariables : couplesVariables)
    {
        cout << coupleVariables.first.first << "/" << coupleVariables.first.second;
        cout << " avec " << coupleVariables.second.size() << " contrainte(s)" << endl;
    }

    cout << endl;

    do
    {
        CoupleVariables couple = couplesVariables.begin()->first;
        list<Contrainte> contraintes = couplesVariables.begin()->second;
        couplesVariables.erase(couple);

        int varX = couple.first;
        int varY = couple.second;

        //on regarde si le domaine de valeur de x a été modifié
        if(ac3Reduction(donnees, varX, varY, contraintes))
        {
            //si son domaine de valeur est vide, on arrête tout
            if(donnees.variables[varX-1].second.size() == 0)
            {
                cout << "## Ce systeme ne possede aucune solution" << endl;
                return false;
            }

            //on cherche à ajouter tous les voisins de X (tous les z qui ont des contraintes avec x)
            for(auto z = donnees.variables.begin(); z != donnees.variables.end(); z++)
            {
                if(z->first != varY && z->first != varX)
                {
                    //on trouve toutes les contraintes binaires qui comprennent le couple (z,x)
                    for(auto contrainte: donnees.contraintes)
                    {
                        //on ne prend que des contraintes binaires
                        if(contrainte.type == DIFFERENT || contrainte.type == EGALE)
                        {
                            //on regarde si la contrainte donnée impacte sur les variables x et y
                            if(existe(contrainte.variables, varX) && existe(contrainte.variables, z->first))
                            {
                                couplesVariables[make_pair(z->first,varX)].push_back(contrainte);
                            }
                        }
                    }
                }
            }
        }

    } while(couplesVariables.size() > 0);

    cout << "## Domaines de valeurs des variables : " << endl;
    afficherVariables(donnees);

    return true;
}

/**
 * @brief Implémentation de la réduction d'arc
 * @param Donnees& donnees
 * @param int varX : variable x du couple à réduire
 * @param int varY : variable y du couple à réduire
 * @param list<Contrainte>& contraintes : liste des contraintes à vérifier sur ce couple
 * @return void
*/

bool ac3Reduction(Donnees& donnees, int varX, int varY, list<Contrainte>& contraintes)
{
    cout << "reduction d'arc de : " << varX << "/" << varY << endl;
    bool change = false;

    list<int> VX = donnees.variables[varX-1].second;
    list<int> VY = donnees.variables[varY-1].second;

    //on parcourt toutes les valeurs possibles de x
    for(auto vx : VX)
    {
        //on part du principe qu'aucune valeur de y permet de valider les contraintes
        bool contraintesRespectees = false;

        for(auto vy : VY)
        {
            bool contrainteRespectee = true;

            for(auto contrainte : contraintes)
            {
                list<int> variablesValeurs = {vx,vy};

                switch(contrainte.type)
                {
                    case DIFFERENT:
                        if(!differents(variablesValeurs))
                        {
                            contrainteRespectee = false;
                            break;
                        }

                    break;

                    case EGALE:
                        if(!egales(variablesValeurs))
                        {
                            contrainteRespectee = false;
                            break;
                        }
                    break;
                }
            }

            if(contrainteRespectee)
            {
                contraintesRespectees = true;
                cout << " - le couple de valeurs " << vx << "/" << vy << " respecte les contraintes" << endl;
                break;
            }
            else
            {
                cout << " - le couple de valeurs " << vx << "/" << vy << " ne respecte pas les contraintes" << endl;
            }
        }

        if(!contraintesRespectees)
        {
            cout << " # suppression de la valeur " << vx << " dans le domaine de " << varX << endl;
            donnees.variables[varX-1].second.remove(vx);
            change = true;
            continue;
        }
    }

    cout << endl;

    return change;
}

/**
 * @brief Affiche la liste des variables et leurs domaines de valeurs
 * @param Donnees& donnees
 * @return void
*/

void afficherVariables(Donnees& donnees)
{
    cout << endl << "###################" << endl;
    cout << "Liste des variables" << endl;
    cout << "###################" << endl << endl;

    for(auto variable : donnees.variables)
    {
        cout << " - " << variable.first << " : ";

        for(auto valeur : variable.second)
        {
            cout << valeur << " ";
        }

        cout << endl;
    }

    cout << endl;
}

/**
 * @brief Affiche la liste des solutions
 * @param Donnees& donnees
 * @return void
*/

void afficherSolutions(Donnees& donnees)
{
    cout << endl << "#####################" << endl;
    cout << "Solutions du probleme" << endl;
    cout << "#####################" << endl << endl;

    if(donnees.noeudsSolution.size() > 0)
    {
        for(map<int,Noeud*> noeudSolution: donnees.noeudsSolution)
        {
            for(pair<int,Noeud*> solution: noeudSolution)
            {
                cout << solution.second->valeur << " ";
            }

            cout << endl;
        }
    }
    else
    {
        cout << "Aucune solution" << endl;
    }

    cout << endl;
}

/**
 * @brief Affiche un chemin de traite dans l'arbre des solutions
 * @param map<int, Noeud*>& chemin
 * @return string
*/

string afficherChemin(map<int, Noeud*>& chemin)
{
    string result = "";

    for(pair<int, Noeud*> noeud : chemin)
    {
        result += var2Str(noeud.second->valeur) + " ";
    }

    return result;
}
