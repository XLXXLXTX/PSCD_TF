//*****************************************************************
// File:   mainLinda.hpp
// Author: PSCD-Unizar
// Date:   noviembre 2020
// Coms:   
//*****************************************************************

#ifndef LINDA_DRIVER
#define LINDA_DRIVER
#include <iostream>
#include <string>
#include "Tupla.hpp"

using namespace std;

class LindaDriver {
public:
	//constructores -----------------------------
	LindaDriver(...);
	//destructor -----------------------------
	~LindaDriver();
	//operadores -----------------------------

	//PostNote
	void PN(const Tupla t);

	//Pre:  "p" y "t" tienen el mismo tamaño
	//Post: Remove Note, siendo "p" el patrón y "t" la tupla
	void RN(const Tupla p, Tupla& t);

	//Pre:  "p1" y "t1" tienen el mismo tamaño
	//      "p2" y "t2" tienen el mismo tamaño
	//Post: Remove Notes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
	void RN_2(const Tupla p1, const Tupla p2, Tupla& t1, Tupla& t2);

	//Pre:  "p" y "t" tienen el mismo tamaño
	//Post: ReaD Note, siendo "p" el patrón y "t" la tupla
	void RDN(const Tupla p, Tupla& t);

	//Pre:  "p1" y "t1" tienen el mismo tamaño
	//      "p2" y "t2" tienen el mismo tamaño
	//Post: ReaD Notes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
	void RDN_2(const Tupla p1, const Tupla p2, Tupla& t1, Tupla& t2);

	//...
private:
	//...
};
#endif