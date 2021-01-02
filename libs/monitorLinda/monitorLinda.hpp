// Fichero: MonitorLinda.hpp
// Fecha: Enero 2021
// Autores: ...
// Coms: Fichero de interfaz del monitor del espacio de tuplas

#ifndef MONITORLINDA_HPP
#define MONITORLINDA_HPP

#include <mutex>
#include <condition_variable>
#include <string>
#include <list>
#include <iterator>
#include <cstdlib>
#include <ctime>
#include "Tupla.hpp"
using namespace std;

class MonitorLinda {
    public:
    MonitorLinda(); // Constructor

    ~MonitorLinda(); // Destrutor

    void PN(Tupla t); // PostNote
    void RN(Tupla p, Tupla& t);  // RemoveNote
    void RDN(Tupla p, Tupla& t); // ReadNote
    void RN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2); // Remove doble
    void RDN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2); // Read doble
    
    private:
    int tamTup;
    mutex mtxML; // Mutex para la exclusión mutua
    condition_variable hayTupla; // Variable condición para los procesos en espera de leer una tupla con cierto patrón
    list<Tupla> espacioTup; // Espacio de tuplas
};
#endif