//*****************************************************************
// File:   LindaDriver.cpp
// Author: PSCD-Unizar
// Date:   Diciembre 2020
// Coms:   Implementación del Linda Driver
//*****************************************************************

#include <cassert>
#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include "LindaDriver.hpp"

using namespace std;

//constructores -----------------------------
LindaDriver::LindaDriver(string SERVER_ADDRESS, string SERVER_PORT) : Socket(SERVER_ADDRESS, stoi(SERVER_PORT)){

    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    do {
        // Conexión con el servidor
        socket_fd = Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_fd == -1) {
        cerr << "Error chan LindaDriver" << endl;
    }
}

//destructor -----------------------------
LindaDriver::~LindaDriver() {
    string MENS_FIN="END OF SERVICE";

    int send_bytes = Send(socket_fd, MENS_FIN);	// Envíamos el string
    if(send_bytes == -1){
        cerr << "Error PN send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }
    Close(socket_fd);
}

//operadores -----------------------------
void LindaDriver::PN(Tupla t){
    string mensaje = "PN:" + t.to_string() + ":";	    // Adaptamos la tupla a string
    int send_bytes = Send(socket_fd, mensaje);	// Envíamos el string
    
    if(send_bytes == -1){
        cerr << "Error PN send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }

    string respuesta;
    int read_bytes = Recv(socket_fd, respuesta, MESSAGE_SIZE);

    if(read_bytes == -1) {
        cerr << "Error PN read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        Close(socket_fd);
    }

}
void LindaDriver::RN(Tupla p, Tupla& t){
    string mensaje = "RN:" + p.to_string() + ":";	    // Adaptamos la tupla a string
    int send_bytes = Send(socket_fd, mensaje);	// Envíamos el string

    if(send_bytes == -1) {
        cerr << "Error RN send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }

    string respuesta;
    int read_bytes = Recv(socket_fd, respuesta, MESSAGE_SIZE);

    if(read_bytes == -1) {
        cerr << "Error RN read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        Close(socket_fd);
    }

    t.from_string(respuesta);					// Damos valor a t

}
void LindaDriver::RN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2){
    string mensaje = "RN_2:" + p1.to_string() + ":" + p2.to_string() + ":";	        // Adaptamos las tupla
    int send_bytes = Send(socket_fd, mensaje);	// Envíamos el string
    
    if(send_bytes == -1) {
        cerr << "Error RN_2 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }

	char respuesta[MESSAGE_SIZE];
    int read_bytes = Recv(socket_fd, respuesta, MESSAGE_SIZE);
    
    if(read_bytes == -1) {
        cerr << "Error RN_2 read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        Close(socket_fd);
    }
    char* tupla = strtok (respuesta,":");
    t1.from_string(tupla);			// Damos valor a t1
    tupla = strtok (NULL,":");
    t2.from_string(tupla);		// Damos valor a t2
}

void LindaDriver::RDN(Tupla p, Tupla& t){
    string mensaje = "RDN:" + p.to_string() + ":";	    // Adaptamos la tupla a string
    int send_bytes = Send(socket_fd, mensaje);	// Envíamos el string
    
    if(send_bytes == -1) {
        cerr << "Error RDN send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }

    string respuesta;
    int read_bytes = Recv(socket_fd, respuesta, MESSAGE_SIZE);
    
    if(read_bytes == -1) {
            cerr << "Error RDN read_bytes: " << strerror(errno) << endl;
            // Cerramos los sockets
            Close(socket_fd);
        }
    t.from_string(respuesta);					// Damos valor a t
}

void LindaDriver::RDN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2){
    string mensaje = "RDN_2:" + p1.to_string() + ":" + p2.to_string() + ":";	        // Adaptamos las tupla
    int send_bytes = Send(socket_fd, mensaje);	// Envíamos el string
    
    if(send_bytes == -1) {
        cerr << "Error RDN_2 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }
    
	char respuesta[MESSAGE_SIZE];
    int read_bytes = Recv(socket_fd, respuesta, MESSAGE_SIZE);
    
    if(read_bytes == -1) {
        cerr << "Error RDN_2 read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        Close(socket_fd);
    }
    char* tupla = strtok (respuesta,":");
    t1.from_string(tupla);			// Damos valor a t1
    tupla = strtok (NULL,":");
    t2.from_string(tupla);		// Damos valor a t2
}