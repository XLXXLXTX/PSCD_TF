//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de interfaz de la clase LindaDriver que permite el acceso y uso
//          de los servidores que integran el sistema Linda a los procesos que lo requieran 
// File:    LindaDriver.hpp
// Encode:  UTF-8
// EOF:     LF
//--------------------------------------------------------------------------------------------

#ifndef LINDA_DRIVER
#define LINDA_DRIVER
#include <iostream>
#include <string>
#include "../Tupla/Tupla.hpp"
#include "../Socket/Socket.hpp"

using namespace std;

const int MESSAGE_SIZE = 200; //mensajes de no más 201 caracteres

class LindaDriver{
private:
    int socket_despliegue; //sockets de conexión
	int socket_LS1;
	int socket_LS2;
	int socket_LS3;

	// Usamos punteros ya que el socket lo queremos crear según las direcciones
	// que tenemos del servidor de despliegue y de este obtenemos las de los
	// servidores que integran el sistema Linda
	Socket* chan;
	Socket* LS1;
	Socket* LS2;
	Socket* LS3;
public:
	//constructor -----------------------------
	LindaDriver(string SERVER_ADDRESS, string SERVER_PORT);
	//destructor -----------------------------
	~LindaDriver();
	//operadores -----------------------------

	//Pre:  ---
	//Post: Post Note, siendo "p" la tupla
	void PN(Tupla t);

	//Pre:  "p" y "t" tienen el mismo tamaño
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

	// Función auxiliar de cierre de los sockets que tenemos abiertos
	void cierreSockets();

};
#endif