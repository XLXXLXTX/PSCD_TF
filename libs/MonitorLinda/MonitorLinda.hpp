//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de interfaz de la clase MonitorLinda
//          para el manejo de espacios de tuplas 
// File:    MonitorLinda.hpp
// Encode:  UTF-8
// EOF:     LF
//--------------------------------------------------------------------------------------------

#ifndef MONITORLINDA_HPP
#define MONITORLINDA_HPP

#include <mutex>
#include <condition_variable>
#include <list>
#include <iterator>
#include <ctime>
#include <cstdlib>
#include <string>
#include "../Tupla/Tupla.hpp"

using namespace std;

class MonitorLinda {
    public:
        // Constructor de la clase
        MonitorLinda();

        // Destructor de la clase
        ~MonitorLinda();

        // Pre:  ---
        // Post: Devuelve en <info> datos sobre el estado del espacio de tuplas
        void INFO(string& info);
        
        // Pre:  ---
        // Post: La tupla t ha sido añadida al espacio interno de tuplas de la clase
        void PN(Tupla t);

        // Pre:  p, t son tuplas del mismo tamaño
        // Post: En t hay una tupla que hace match con p, leída del espacio de tuplas interno a la clase.
        //       Además ha sido borrada del espacio.
        void RN(Tupla p, Tupla& t);

        // Pre:  p, t son tuplas del mismo tamaño
        // Post: En t hay una tupla que hace match con p, leída del espacio de tuplas interno a la clase.
        void RDN(Tupla p, Tupla& t);

        // Pre:  p1, p2, t1, t2 son tuplas del mismo tamaño
        // Post: En t1 hay una tupla que hace match con p1 y en t2 hay una tupla que hace match con p2,
        //       ambas leídas del espacio de tuplas interno a la clase. Además han sido borradas del espacio.
        void RN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2);

        // Pre:  p1, p2, t1, t2 son tuplas del mismo tamaño
        // Post: En t1 hay una tupla que hace match con p1 y en t2 hay una tupla que hace match con p2,
        //       ambas leídas del espacio de tuplas interno a la clase.
        void RDN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2); 

    private:
        mutex mtxML;                    // Mutex para la exclusión mutua
        condition_variable hayTupla;    // Variable condición para los procesos en espera de leer una tupla 
        list<Tupla> espacioTup;         // Espacio de tuplas

        int lectPend;   // número de procesos bloqueados en RDN, RN, RDN_2 o RN_2
        int lectTotal;  // número total de tuplas leídas y borradas
        int borradas;   // número total de tuplas borradas
        int escTotal;   // número total de tuplas añadidas
};

#endif  // MONITORLINDA_HPP