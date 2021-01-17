//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de implementación de la clase LindaDriver que permite el acceso y uso
//          de los servidores que integran el sistema Linda a los procesos que lo requieran 
// File:    LindaDriver.cpp
// Encode:  UTF-8
// EOF:     LF
//--------------------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <ctime>
#include <thread>
#include "LindaDriver.hpp"

using namespace std;

LindaDriver::LindaDriver(string SERVER_ADDRESS, string SERVER_PORT){
    chan = new Socket(SERVER_ADDRESS, stoi(SERVER_PORT));
    
    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    do {
        // Conexión con el servidor
        socket_despliegue = chan->Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_despliegue == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while (socket_despliegue == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if (socket_despliegue == -1) {
        cerr << "Error conexión a despliegue\n";
        exit(2);
    }
    char buffer[MESSAGE_SIZE];  // buffer donde almacenamos lo que leemos
    int read_bytes = chan->Recv(socket_despliegue, buffer, MESSAGE_SIZE);
    if (read_bytes == -1) {     //caso error del Recv
        cerr << "Error PN read_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        chan->Close(socket_despliegue);
    }

    string MENS_FIN = "END_OF_SERVICE";
    int send_bytes  = chan->Send(socket_despliegue, MENS_FIN);  // Envíamos el string
    if(send_bytes == -1){   //caso error del Send
        cerr << "Error cerrar socket_despliegue send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chan->Close(socket_despliegue);
        exit(1);
    }
    chan->Close(socket_despliegue);

    char* servidor = strtok (buffer,":");   // Dirección IP del primer LindaServer
    int puerto = atoi(strtok (NULL,":"));   // Puerto del primer LindaServer
    LS1 = new Socket(servidor, puerto);     // LS1 es el socket que conecta con el primer puerto

    servidor = strtok (NULL,":");            // Dirección IP del segundo LindaServer
    puerto = atoi(strtok (NULL,":"));        // Puerto del segundo LindaServer
    LS2 = new Socket(servidor, puerto);      // LS1 es el socket que conecta con el segundo puerto

    servidor = strtok (NULL,":");            // Dirección IP del tercer LindaServer
    puerto = atoi(strtok (NULL,":"));        // Puerto del tercer LindaServer
    LS3 = new Socket(servidor, puerto);      // LS1 es el socket que conecta con el tercer puerto

    count = 0;
    do {
        // Conexión con los servidores
        socket_LS1 = LS1->Connect();
        socket_LS2 = LS2->Connect();
        socket_LS3 = LS3->Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if ( (socket_LS1 == -1) || (socket_LS2 == -1) || (socket_LS3 == -1) ) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while (((socket_LS1 == -1) || (socket_LS2 == -1) || (socket_LS3 == -1)) && count < MAX_ATTEMPS);

    
    // Chequeamos si se han realizado las conexiones
    if ((socket_LS1 == -1) || (socket_LS2 == -1) || (socket_LS3 == -1)) {
        cerr << "Error conexión a servidores Linda\n";
        exit(2);
    }
}

LindaDriver::~LindaDriver() {
    cierreSockets();    //función auxiliar para cerrar los sockets
    // Eliminamos los sockets creados en el constructor
    delete LS1;
    delete LS2;
    delete LS3;
}

void LindaDriver::PN(Tupla t){
    int socket_fd;
    Socket* LS;

    // Asignamos a socket_fd la conexión del socket con el servidor indicado y a LS dicho socket
    switch (t.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS=LS1;
            break;  // Terminamos los casos de tamaño 1, 2 y 3
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS=LS2;
            break;  // Terminamos los casos de tamaño 4 y 5
        case 6:
            socket_fd = socket_LS3;
            LS=LS3;
            break;  // Terminamos los casos de tamaño 6  
        default:    // Caso tamaño de tupla no admitido, cerramos los sockets para evitar errores
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit\n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "PN:" + t.to_string() + ":";   // Adaptamos la tupla a string
    int send_bytes = LS->Send(socket_fd, mensaje);  // Envíamos el string
    
    if (send_bytes == -1) {  // Caso error del Send
        cerr << "Error PN send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }

    string respuesta;   // buffer donde almacenamos lo que leemos
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);

    if (read_bytes == -1) { // Caso error del Recv
        cerr << "Error PN read_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

}

void LindaDriver::RN(Tupla p, Tupla& t){
    int socket_fd;
    Socket* LS;

    // Asignamos a socket_fd la conexión del socket con el servidor indicado y a LS dicho socket
    switch (p.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS = LS1;
            break;  // Terminamos los casos de tamaño 1, 2 y 3
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS = LS2;
            break;  // Terminamos los casos de tamaño 4 y 5
        case 6:
            socket_fd = socket_LS3;
            LS = LS3;
            break;  // Terminamos los casos de tamaño 6
        default:    //caso tamaño de tupla no admitido, cerramos los sockets para evitar errores
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "RN:" + p.to_string() + ":";   // Adaptamos la tupla a string
    int send_bytes = LS->Send(socket_fd, mensaje);  // Envíamos el string
    if (send_bytes == -1) { //caso error del Send
        cerr << "Error RN send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }

    string respuesta;   // buffer donde almacenamos lo que leemos
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);
    if (read_bytes == -1) {  //caso error del Recv
        cerr << "Error RN read_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

    t.from_string(respuesta);   // Damos valor a t
}

void LindaDriver::RN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2){
    int socket_fd;
    Socket* LS;

    //asignamos a socket_fd la conexión del socket con el servidor indicado y a LS dicho socket
    switch (p1.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS = LS1;
            break;  //terminamos los casos de tamaño 1, 2 y 3
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS = LS2;
            break;  //terminamos los casos de tamaño 4 y 5
        case 6:
            socket_fd = socket_LS3;
            LS = LS3;
            break;  //terminamos los casos de tamaño 6
        default:    //caso tamaño de tupla no admitido, cerramos los sockets para evitar errores
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "RN_2:" + p1.to_string() + ":" + p2.to_string() + ":";	        // Adaptamos las tuplas
    int send_bytes = LS->Send(socket_fd, mensaje);	// Envíamos el string
    if (send_bytes == -1) {  //caso error del Send
        cerr << "Error RN_2 send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }

	char respuesta[MESSAGE_SIZE];
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);
    if (read_bytes == -1) {   //caso error del Recv
        cerr << "Error RN_2 read_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

    char* tupla = strtok (respuesta,":");   //asignamos a tupla los caracteres previos al separador ":"
    t1.from_string(tupla);      // Damos valor a t1
    tupla = strtok (NULL,":");  //asignamos a tupla los siguientes caracteres previos al separador ":"
    t2.from_string(tupla);		// Damos valor a t2
}

void LindaDriver::RDN(Tupla p, Tupla& t){
    int socket_fd;
    Socket* LS;
    //asignamos a socket_fd la conexión del socket con el servidor indicado y a LS dicho socket
    switch (p.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS=LS1;
            break;  //terminamos los casos de tamaño 1, 2 y 3
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS=LS2;
            break;  //terminamos los casos de tamaño 4 y 5
        case 6:
            socket_fd = socket_LS3;
            LS=LS3;
            break;  //terminamos los casos de tamaño 6
        default: //caso tamaño de tupla no admitido, cerramos los sockets para evitar errores
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "RDN:" + p.to_string() + ":";	    // Adaptamos la tupla a string
    int send_bytes = LS->Send(socket_fd, mensaje);	// Envíamos el string
    if (send_bytes == -1) {   //caso error del Send
        cerr << "Error RDN send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }

    string respuesta;   // buffer donde almacenamos lo que leemos
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);
    if (read_bytes == -1) { //caso error del Recv
        cerr << "Error RDN read_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

    t.from_string(respuesta);   // Damos valor a t
}

void LindaDriver::RDN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2) {
    int socket_fd;
    Socket* LS;
    // A signamos a socket_fd la conexión del socket con el servidor indicado y a LS dicho socket
    switch (p1.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS = LS1;
            break;  // Terminamos los casos de tamaño 1, 2 y 3
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS = LS2;
            break;  // Terminamos los casos de tamaño 4 y 5
        case 6:
            socket_fd = socket_LS3;
            LS = LS3;
            break;  // Terminamos los casos de tamaño 6
        default:    // Caso tamaño de tupla no admitido, cerramos los sockets para evitar errores
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "RDN_2:" + p1.to_string() + ":" + p2.to_string() + ":";    // Adaptamos las tuplas
    int send_bytes = LS->Send(socket_fd, mensaje);  // Envíamos el string
    if (send_bytes == -1) {  // Caso error del Send
        cerr << "Error RDN_2 send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }
    
	char respuesta[MESSAGE_SIZE]; // buffer donde almacenamos lo que leemos
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);
    if(read_bytes == -1) {   // Caso error del Recv
        cerr << "Error RDN_2 read_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

    char* tupla = strtok (respuesta,":");   // Asignamos a tupla los caracteres previos al separador ":"
    t1.from_string(tupla);      // Damos valor a t1
    tupla = strtok (NULL,":");  // Asignamos a tupla los siguientes caracteres previos al separador ":"
    t2.from_string(tupla);		// Damos valor a t2
}

// Función auxiliar de cierre de los sockets que tenemos abiertos
void LindaDriver::cierreSockets() {
    string MENS_FIN = "END_OF_SERVICE";
    int send_bytes  = LS1->Send(socket_LS1, MENS_FIN);	// Envíamos el string
    if (send_bytes == -1) {   // Caso error del Send
        cerr << "Error cerrar socket_LS1 send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        LS1->Close(socket_LS1);
        LS2->Close(socket_LS2);
        LS3->Close(socket_LS3);
        exit(1);
    }
    LS1->Close(socket_LS1); // Cerramos el primer socket

    send_bytes = LS2->Send(socket_LS2, MENS_FIN);   // Envíamos el string
    if (send_bytes == -1) { // Caso error del Send
        cerr << "Error cerrar socket_LS2 send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        LS2->Close(socket_LS2);
        LS3->Close(socket_LS3);
        exit(1);
    }
    LS2->Close(socket_LS2); // Cerramos el segundo socket

    send_bytes = LS3->Send(socket_LS3, MENS_FIN);   // Envíamos el string
    if (send_bytes == -1) {   // caso error del Send
        cerr << "Error cerrar socket_LS3 send_bytes: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        LS3->Close(socket_LS3);
        exit(1);
    }
    LS3->Close(socket_LS3);  // Cerramos el tercer socket
}