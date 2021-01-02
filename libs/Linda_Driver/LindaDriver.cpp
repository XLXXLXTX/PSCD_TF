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
LindaDriver::LindaDriver(string SERVER_ADDRESS, string SERVER_PORT) : Socket(SERVER_ADDRESS, stoi(SERVER_ADDRESS)){

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
LindaDriver::~LindaDriver()//Hacer destructor
{
    Close(socket_fd);
    exit(1);
}

//operadores -----------------------------
void LindaDriver::PN(Tupla t){
    string mensaje = "PN: " + t.to_string();	    // Adaptamos la tupla a string
    int send_bytes = Send(socket_fd, mensaje);	// Envíamos el string
    
    if(send_bytes == -1){
        cerr << "Error PN send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }

}
void LindaDriver::RN(Tupla p, Tupla& t){
    string mensaje = "RN: " + p.to_string();	    // Adaptamos la tupla a string
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
    string mensaje = "RN_2: " + p1.to_string();	        // Adaptamos la primera tupla
    int send_bytes1 = Send(socket_fd, mensaje);	// Envíamos el string

    mensaje = "RN_2: " + p2.to_string();	        // Adaptamos la segunda tupla
    int send_bytes2 = Send(socket_fd, mensaje);	// Envíamos el string
    
    if((send_bytes1 == -1) || (send_bytes2 == -1)) {
        cerr << "Error RN_2 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }

    string respuesta1, respuesta2;
    int read_bytes1 = Recv(socket_fd, respuesta1, MESSAGE_SIZE);
    int read_bytes2 = Recv(socket_fd, respuesta2, MESSAGE_SIZE);
    
    if((read_bytes1 == -1) || (read_bytes2 == -1)) {
        cerr << "Error RN_2 read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        Close(socket_fd);
    }
    
    t1.from_string(respuesta1);					// Damos valor a t1
    t2.from_string(respuesta2);					// Damos valor a t2
}

void LindaDriver::RDN(Tupla p, Tupla& t){
    string mensaje = "RDN: " + p.to_string();	    // Adaptamos la tupla a string
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
    string mensaje = "RN_2: " + p1.to_string();                     //Pasamos la primera tupla a string
    int send_bytes1 = Send(socket_fd, mensaje);	            // Envíamos el string
    mensaje = "RN_2: " + p2.to_string();                            //Pasamos la segunda tupla a string
    int send_bytes2 = Send(socket_fd, mensaje);	            // Envíamos el string
    
    if(send_bytes1 == -1 || send_bytes2 == -1) {
        cerr << "Error RDN_2 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        Close(socket_fd);
        exit(1);
    }

    string respuesta1, respuesta2;
    int read_bytes1 = Recv(socket_fd, respuesta1, MESSAGE_SIZE);
    int read_bytes2 = Recv(socket_fd, respuesta2, MESSAGE_SIZE);
    
    if((read_bytes1 == -1) || (read_bytes2 == -1)) {
        cerr << "Error RDN_2 read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        Close(socket_fd);
    }
    t1.from_string(respuesta1);					// Damos valor a t1
    t2.from_string(respuesta2);					// Damos valor a t2
}