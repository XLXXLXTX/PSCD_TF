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
#include "Socket/Socket.hpp"

using namespace std;

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres

class LindaDriver : public Socket{
private:
	string SERVER_ADDRESS;	//dirección ip
    int socket_fd; //socket de conexión
public:
	//constructores -----------------------------
	LindaDriver(string SERVER_ADDRESS, string SERVER_PORT);
	//destructor -----------------------------
	~LindaDriver();
	//operadores -----------------------------

	//PostNote
	void PN(Tupla t);

	//Pre:  "p" y "t" tienen el mismo dtamaño
	//Post: Remove Note, siendo "p" el patrón y "t" la tupla
	void RN(Tupla p, Tupla& t);

	//Pre:  "p1" y "t1" tienen el mismo tamaño
	//      "p2" y "t2" tienen el mismo tamaño
	//Post: Remove Notes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
	void RN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2);

	//Pre:  "p" y "t" tienen el mismo tamaño
	//Post: ReaD Note, siendo "p" el patrón y "t" la tupla
	void RDN(Tupla p, Tupla& t);

	//Pre:  "p1" y "t1" tienen el mismo tamaño
	//      "p2" y "t2" tienen el mismo tamaño
	//Post: ReaD Notes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
	void RDN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2);

};
#endif