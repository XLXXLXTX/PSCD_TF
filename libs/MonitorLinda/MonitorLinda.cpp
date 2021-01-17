//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de implementación de las operaciones del monitor
//          para el manejo de espacios de tuplas 
// File:    MonitorLinda.cpp
// Encode:  UTF-8
// EOF:     LF
//--------------------------------------------------------------------------------------------

#include <iostream>
#include "MonitorLinda.hpp"

MonitorLinda::MonitorLinda() {
    // Inicializamos variables
    srand (time(NULL));
    lectPend  = 0;
    lectTotal = 0;
    escTotal  = 0;
    borradas  = 0;
};

MonitorLinda::~MonitorLinda() {};

void MonitorLinda::INFO(string& s) {
    unique_lock<mutex> lck(mtxML);
    // Devolvemos en el string toda la informaci�n sobre el estado del espacio de tuplas separada por comas
    s = to_string(espacioTup.size()) + "," + to_string(lectPend) + "," + to_string(lectTotal) + "," + to_string(borradas) + "," + to_string(escTotal);
};

void MonitorLinda::PN(Tupla t) {
    unique_lock<mutex> lck(mtxML);  
    if (espacioTup.size() > 0) { // Si ya hay alguna tupla
        typename list<Tupla>::iterator it = espacioTup.begin();
        int aleat = rand() % espacioTup.size();
        advance(it, aleat);
        espacioTup.insert(it, t); // La colocamos en un punto aleatorio de la lista
    } else {
        espacioTup.push_front(t); // Si no hab�a tuplas, la ponemos en primer lugar
    }

    escTotal++;
    hayTupla.notify_all(); // Avisamos a los procesos bloqueados
};

void MonitorLinda::RN(Tupla p, Tupla& t) {
    unique_lock<mutex> lck(mtxML);
    bool esta = false;
    do {    // Mientras no encontramos un match
        if (espacioTup.size() > 0) {    // Si ya hay alguna tupla
            typename list<Tupla>::iterator it = espacioTup.begin();
            int aleat = rand() % espacioTup.size();
            advance(it, aleat);

            // El iterador comienza en un punto aleatorio de la lista
            typename list<Tupla>::iterator fin   = it;
            typename list<Tupla>::iterator fuera = espacioTup.end();

            do {    // Buscamos hasta encontrar match o hacer un ciclo entero en la lista
                if (it == fuera) it = espacioTup.begin();
                if ((*it).match(p)) {       // Hay match
                    for (int i = 0; i < t.size(); i++) {    // Leemos la tupla en t
                        t.set(i, (*it).get(i));
                    }
                    espacioTup.erase(it);   // La borramos del espacio
                    esta = true;
                } else {
                    it++;
                }
            } while (it != fin && !esta);
        }
        if (!esta) {    // Si hemos recorrido toda la lista sin encontrarla, nos bloqueamos
            lectPend++;
            hayTupla.wait(lck);
            lectPend--;
        }
    } while (!esta);

    borradas++;
    lectTotal++;
};

void MonitorLinda::RDN(Tupla p, Tupla& t) {
    unique_lock<mutex> lck(mtxML);
    bool esta = false;
    do {    // Mientras no encontramos un match
        if (espacioTup.size() > 0) {    // Si ya hay alguna tupla
            typename list<Tupla>::iterator it = espacioTup.begin();
            int aleat = rand() % espacioTup.size();
            advance(it, aleat);

            // El iterador comienza en un punto aleatorio de la lista
            typename list<Tupla>::iterator fin   = it;
            typename list<Tupla>::iterator fuera = espacioTup.end();

            do {    // Buscamos hasta encontrar match o hacer un ciclo entero en la lista
                if (it == fuera) it = espacioTup.begin();
                if ((*it).match(p)) {   // Hay match
                    for (int i = 0; i < t.size(); i++) {    // Leemos la tupla en t
                        t.set(i, (*it).get(i));
                    }
                    esta = true;
                } else {
                    it++;
                }
            } while (it != fin && !esta);
        }
        if (!esta) {    // Si hemos recorrido toda la lista sin encontrarla, nos bloqueamos
            lectPend++;
            hayTupla.wait(lck);
            lectPend--;
        }
    }while(!esta);

    lectTotal++;
};

void MonitorLinda::RN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2) {
    unique_lock<mutex> lck(mtxML);
    string var = "";    // Para guardar la posible variable común
    int cont   = 0;     // Contador de variables comunes
    int vars1[t1.size()] = {0}; // Para guardar las posiciones de las variables comunes de p1
    int vars2[t2.size()] = {0}; // Para guardar las posiciones de las variables comunes de p2
    bool esta = false;
    bool doblematch = false;
    typename list<Tupla>::iterator it1;
    typename list<Tupla>::iterator it2;
   
    // Localiza las variables comunes
    for (int i = 0; i < t1.size(); i++) {
        var = p1.get(i);
        if (var[0] == '?' && isalpha(var[1]) && isupper(var[1])) {  // Busca en p1 una variable
            for (int j = 0; j < t2.size(); j++) {
                if (var == p2.get(j) && vars2[j] == 0) {    // Busca en p2 todas las variables iguales a la encontrada en p1
                    // Marca las posiciones de la variable común
                    cont++;
                    vars1[i] = cont;
                    vars2[j] = cont;
                }
            }
        }
    }

    do {    // Mientras no encontramos un match doble
        if (espacioTup.size()>1) {   // Si ya hay alguna tupla
            it1 = espacioTup.begin();
            int aleat = rand() % espacioTup.size();
            advance(it1, aleat);

            if (it1 == espacioTup.end()) it1 = espacioTup.begin();
            // El iterador comienza en un punto aleatorio de la lista
            typename list<Tupla>::iterator fin1 = it1;
            typename list<Tupla>::iterator fuera = espacioTup.end();

            do {    // Buscamos hasta encontrar match o hacer un ciclo entero en la lista
                if (it1 == fuera) it1 = espacioTup.begin(); // Si nos salimos, volvemos al inicio
                else if ((*it1).match(p1)) {    // Hay match
                    for (int i = 0; i < t1.size(); i++) {   // Leemos la tupla en t1
                        t1.set(i, (*it1).get(i));
                    }
                    esta = true;
                } else {    // Seguimos iterando
                    it1++;
                }
            } while (it1 != fin1 && !esta);

            while (esta) {  // Si hemos encontrado match para p1
                esta = false;

                // Modificar t2 con p2 y los nuevos valores de las variables comunes
                for (int i = 0; i < t1.size(); i++) {
                    t2.set(i, p2.get(i));
                }

                for (int i = 0; i < t1.size(); i++) {
                    if (vars1[i] != 0) {
                        var = t1.get(i); // Valor de la variable común
                        for (int j = 0; j < t2.size(); j++) {
                            if (vars1[i] == vars2[j]) {
                                t2.set(j, var); // Pone el valor de la variable com�n correspondiente
                            }
                        }
                    }
                }
                it2 = espacioTup.begin();
                int aleat = rand() % espacioTup.size();
                advance(it2, aleat);

                // El iterador comienza en un punto aleatorio de la lista
                typename list<Tupla>::iterator fin2 = it2;
                typename list<Tupla>::iterator fuera = espacioTup.end();
                do {    // Buscamos hasta encontrar match o hacer un ciclo entero en la lista
                    if (it2 == fuera) it2 = espacioTup.begin(); // Si nos salimos, volvemos al inicio
                    else if ((*it2).match(t2) && it1 != it2) {  // Si hacemos match con una tupla que no sea t1
                        for (int i = 0; i < t2.size(); i++) {   // Leemos la tupla en t2
                            t2.set(i, (*it2).get(i));
                        }
                        doblematch = true;
                    } else { // Seguimos iterando
                        it2++;
                    }
                } while (it2 != fin2 && !doblematch);

                if(!doblematch) {   // Si aún no hemos encontrado doblematch
                    it1++;
                    while (it1 != fin1 && !esta) { // Buscamos hasta encontrar match o hacer un ciclo entero en la lista
                        if(it1 == fuera) it1 = espacioTup.begin();  // Si nos salimos, volvemos al inicio
                        else if ((*it1).match(p1)) { // Hay match
                            for (int i = 0; i < t1.size(); i++) {   // Leemos la tupla en t1
                                t1.set(i, (*it1).get(i));
                            }
                            esta = true;
                        } else { // Seguimos iterando
                            it1++;
                        }
                    }
                }
            }
        }
        if (!doblematch) {  // Si no hemos encontrado doblematch, nos bloqueamos
            lectPend += 2;
            hayTupla.wait(lck);
            esta = false;
            lectPend -= 2;
        }
    } while (!doblematch);
    // Tuplas asignadas
    
    // Borrar ambas del espacio
    espacioTup.erase(it1);
    espacioTup.erase(it2);

    borradas  += 2;
    lectTotal += 2;
};

void MonitorLinda::RDN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2) {
    unique_lock<mutex> lck(mtxML);
    string var = "";    // Para guardar la posible variable com�n
    int cont   = 0;     // Contador de variables comunes
    int vars1[t1.size()] = {0}; // Para guardar las posiciones de las variables comunes de p1
    int vars2[t2.size()] = {0}; // Para guardar las posiciones de las variables comunes de p2
    bool esta = false;
    bool doblematch = false;
    typename list<Tupla>::iterator it1;
    typename list<Tupla>::iterator it2;
    
    // Localiza las variables comunes
    for (int i = 0; i < t1.size(); i++) {
        var = p1.get(i);
        if (var[0] == '?' && isalpha(var[1]) && isupper(var[1])) {  // Busca en p1 una variable
            for (int j = 0; j < t2.size(); j++) {
                if (var == p2.get(j) && vars2[j] == 0) {// Busca en p2 todas las variables iguales a la encontrada en p1
                    // Marca las posiciones de la variable com�n
                    cont++;
                    vars1[i] = cont;
                    vars2[j] = cont;
                }
            }
        }
    }

    do {    // Mientras no encontramos un match doble
        if(espacioTup.size()>1) {   // Si ya hay alguna tupla
            it1 = espacioTup.begin();
            int aleat = rand() % espacioTup.size();
            advance(it1, aleat);

            if (it1 == espacioTup.end()) it1 = espacioTup.begin();
            // El iterador comienza en un punto aleatorio de la lista
            typename list<Tupla>::iterator fin1 = it1;
            typename list<Tupla>::iterator fuera = espacioTup.end();

            do {    // Buscamos hasta encontrar match o hacer un ciclo entero en la lista
                if (it1 == fuera) it1 = espacioTup.begin(); // Si nos salimos, volvemos al inicio
                else if ((*it1).match(p1)) {    // Hay match
                    for (int i = 0; i < t1.size(); i++) {   // Leemos la tupla en t1
                        t1.set(i, (*it1).get(i));
                    }
                    esta = true;
                } else { // Seguimos iterando
                    it1++;
                }
            } while (it1 != fin1 && !esta);

            while (esta) { // Si hemos encontrado match para p1
                esta = false;

                // Modificar t2 con p2 y los nuevos valores de las variables comunes
                for (int i = 0; i < t1.size(); i++) {
                    t2.set(i, p2.get(i));
                }

                for (int i = 0; i < t1.size(); i++) {
                    if (vars1[i] != 0) {
                        var = t1.get(i); // Valor de la variable común
                        for (int j = 0; j < t2.size(); j++) {
                            if (vars1[i] == vars2[j]) {
                                t2.set(j, var); // Pone el valor de la variable común correspondiente
                            }
                        }
                    }
                }

                it2 = espacioTup.begin();
                int aleat = rand() % espacioTup.size();
                advance(it2, aleat);

                // El iterador comienza en un punto aleatorio de la lista
                typename list<Tupla>::iterator fin2 = it2;
                typename list<Tupla>::iterator fuera = espacioTup.end();
                do {    // Buscamos hasta encontrar match o hacer un ciclo entero en la lista
                    if(it2 == fuera) it2 = espacioTup.begin();  // Si nos salimos, volvemos al inicio
                    else if ((*it2).match(t2) && it1 != it2) {  // Si hacemos match con una tupla que no sea t1
                        for (int i = 0; i < t2.size(); i++) {   // Leemos la tupla en t2
                            t2.set(i, (*it2).get(i));
                        }
                        doblematch = true;
                    } else {    // Seguimos iterando
                        it2++;
                    }
                } while (it2!=fin2 && !doblematch);

                if(!doblematch) { // Si aún no hemos encontrado doblematch
                    it1++;
                    while(it1 != fin1 && !esta) {   // Buscamos hasta encontrar match o hacer un ciclo entero en la lista
                        if (it1 == fuera) it1 = espacioTup.begin(); // Si nos salimos, volvemos al inicio
                        else if ((*it1).match(p1)) {    // Hay match
                            for (int i = 0; i < t1.size(); i++) {   // Leemos la tupla en t1
                                t1.set(i, (*it1).get(i));
                            }
                            esta = true;
                        } else {    // Seguimos iterando
                            it1++;
                        }
                    }
                }
            }
        }
        if (!doblematch) { // Si no hemos encontrado doblematch, nos bloqueamos
            lectPend += 2;
            hayTupla.wait(lck);
            esta = false;
            lectPend -= 2;
        }
    } while (!doblematch);
    // Tuplas asignadas

    lectTotal+=2;
};