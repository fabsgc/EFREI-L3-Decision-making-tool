#ifndef FONCTIONS_H_INCLUDED
#define FONCTIONS_H_INCLUDED

#include "F7structures.h"

using namespace std;

/**
 * @brief Permet de savoir une clef existe dans une map
 * @param map<int, T> mymap
 * @param int x
 * @return bool
*/

template<typename T>
bool existe(map<int, T> mymap, int x){
    if (mymap.find(x) != mymap.end())
        return true;

    return false;
}

template<typename T>
bool existe(list<T> mylist, T x){
    if (find(mylist.begin(), mylist.end(), x) != mylist.end())
        return true;

    return false;
}

/**
 * @brief Convertie le type donné en paramètre en string
 * @param T x
 * @return string
*/
template<typename T>
std::string var2Str(T x)
{
    stringstream type;
    type << x;
    return type.str();
}

#endif // FONCTIONS_H_INCLUDED
