//********************************************************************************************
// File:   LindaDriver.cpp
// Author: Pablo Bueno, Santiago Illa, Luis Palazón, Carlos Paesa, Javier Pardos y Héctor Toral
// Date:   enero 2021
// Coms:   Fichero de implementación de la clase LindaDriver que permite el acceso y uso
//         de los servidores que integran el sistema Linda a los procesos que lo requieran 
//********************************************************************************************
#include <cassert>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include "LindaDriver.hpp"

using namespace std;

//constructor -----------------------------
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
    } while(socket_despliegue == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_despliegue == -1) {
        cerr << "Error conexión a despliegue" << endl;
        exit(2);
    }
    char buffer[MESSAGE_SIZE];
    int read_bytes = chan->Recv(socket_despliegue, buffer, MESSAGE_SIZE);
    if(read_bytes == -1) {
        cerr << "Error PN read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        chan->Close(socket_despliegue);
    }

    string MENS_FIN="END_OF_SERVICE";
    int send_bytes = chan->Send(socket_despliegue, MENS_FIN);	// Envíamos el string
    if(send_bytes == -1){
        cerr << "Error cerrar socket_despliegue send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        chan->Close(socket_despliegue);
        exit(1);
    }
    chan->Close(socket_despliegue);

    char* servidor = strtok (buffer,":");
    int puerto = atoi(strtok (NULL,":"));
    LS1 = new Socket(servidor, puerto);
    servidor = strtok (NULL,":");
    puerto = atoi(strtok (NULL,":"));
    LS2 = new Socket(servidor, puerto);
    servidor = strtok (NULL,":");
    puerto = atoi(strtok (NULL,":"));
    LS3 = new Socket(servidor, puerto);

    count = 0;
    do {
        // Conexión con los servidores
        socket_LS1 = LS1->Connect();
        socket_LS2 = LS2->Connect();
        socket_LS3 = LS3->Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if( (socket_LS1 == -1) || (socket_LS2 == -1) || (socket_LS3 == -1) ) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(((socket_LS1 == -1) || (socket_LS2 == -1) || (socket_LS3 == -1)) && count < MAX_ATTEMPS);

    
    // Chequeamos si se han realizado las conexiones
    if((socket_LS1 == -1) || (socket_LS2 == -1) || (socket_LS3 == -1)) {
        cerr << "Error conexión a servidores Linda" << endl;
        exit(2);
    }
}

//destructor -----------------------------
LindaDriver::~LindaDriver() {
    cierreSockets();
    delete LS1;
    delete LS2;
    delete LS3;
}

//operadores -----------------------------

//Pre:  ---
//Post: Post Note, siendo "p" la tupla
void LindaDriver::PN(Tupla t){
    int socket_fd;
    Socket* LS;
    switch (t.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS=LS1;
            break;
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS=LS2;
            break;
        case 6:
            socket_fd = socket_LS3;
            LS=LS3;
            break;   
        default:
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "PN:" + t.to_string() + ":";	    // Adaptamos la tupla a string
    int send_bytes = LS->Send(socket_fd, mensaje);	// Envíamos el string
    
    if(send_bytes == -1){
        cerr << "Error PN send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }

    string respuesta;
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);

    if(read_bytes == -1) {
        cerr << "Error PN read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

}

//Pre:  "p" y "t" tienen el mismo tamaño
//Post: Remove Note, siendo "p" el patrón y "t" la tupla
void LindaDriver::RN(Tupla p, Tupla& t){
    int socket_fd;
    Socket* LS;
    switch (p.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS=LS1;
            break;
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS=LS2;
            break;
        case 6:
            socket_fd = socket_LS3;
            LS=LS3;
            break;   
        default:
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "RN:" + p.to_string() + ":";	    // Adaptamos la tupla a string
    int send_bytes = LS->Send(socket_fd, mensaje);	// Envíamos el string
    if(send_bytes == -1) {
        cerr << "Error RN send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }

    string respuesta;
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);
    if(read_bytes == -1) {
        cerr << "Error RN read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

    t.from_string(respuesta);					// Damos valor a t
}

//Pre:  "p1" y "t1" tienen el mismo tamaño
//      "p2" y "t2" tienen el mismo tamaño
//Post: Remove Notes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
void LindaDriver::RN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2){
    int socket_fd;
    Socket* LS;
    switch (p1.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS=LS1;
            break;
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS=LS2;
            break;
        case 6:
            socket_fd = socket_LS3;
            LS=LS3;
            break;   
        default:
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "RN_2:" + p1.to_string() + ":" + p2.to_string() + ":";	        // Adaptamos las tuplas
    int send_bytes = LS->Send(socket_fd, mensaje);	// Envíamos el string
    if(send_bytes == -1) {
        cerr << "Error RN_2 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }

	char respuesta[MESSAGE_SIZE];
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);
    if(read_bytes == -1) {
        cerr << "Error RN_2 read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

    char* tupla = strtok (respuesta,":");
    t1.from_string(tupla);			// Damos valor a t1
    tupla = strtok (NULL,":");
    t2.from_string(tupla);		// Damos valor a t2
}

//Pre:  "p" y "t" tienen el mismo tamaño
//Post: ReaD Note, siendo "p" el patrón y "t" la tupla
void LindaDriver::RDN(Tupla p, Tupla& t){
    int socket_fd;
    Socket* LS;
    switch (p.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS=LS1;
            break;
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS=LS2;
            break;
        case 6:
            socket_fd = socket_LS3;
            LS=LS3;
            break;   
        default:
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "RDN:" + p.to_string() + ":";	    // Adaptamos la tupla a string
    int send_bytes = LS->Send(socket_fd, mensaje);	// Envíamos el string
    if(send_bytes == -1) {
        cerr << "Error RDN send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }

    string respuesta;
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);
    if(read_bytes == -1) {
        cerr << "Error RDN read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

    t.from_string(respuesta);					// Damos valor a t
}

//Pre:  "p1" y "t1" tienen el mismo tamaño
//      "p2" y "t2" tienen el mismo tamaño
//Post: ReaD Notes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
void LindaDriver::RDN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2){
    int socket_fd;
    Socket* LS;
    switch (p1.size()) {
        case 1:
        case 2:
        case 3:
            socket_fd = socket_LS1;
            LS=LS1;
            break;
        case 4:
        case 5:
            socket_fd = socket_LS2;
            LS=LS2;
            break;
        case 6:
            socket_fd = socket_LS3;
            LS=LS3;
            break;   
        default:
            cerr << "Error tamaño de tupla no admitido, cerramos sockets y exit \n";
            cierreSockets();
            exit(5);
    }

    string mensaje = "RDN_2:" + p1.to_string() + ":" + p2.to_string() + ":";// Adaptamos las tuplas
    int send_bytes = LS->Send(socket_fd, mensaje);	// Envíamos el string
    if(send_bytes == -1) {
        cerr << "Error RDN_2 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        cierreSockets();
        exit(3);
    }
    
	char respuesta[MESSAGE_SIZE];
    int read_bytes = LS->Recv(socket_fd, respuesta, MESSAGE_SIZE);
    if(read_bytes == -1) {
        cerr << "Error RDN_2 read_bytes: " << strerror(errno) << endl;
        // Cerramos los sockets
        cierreSockets();
        exit(4);
    }

    char* tupla = strtok (respuesta,":");
    t1.from_string(tupla);			// Damos valor a t1
    tupla = strtok (NULL,":");
    t2.from_string(tupla);		// Damos valor a t2
}

// Función auxiliar de cierre de los sockets que tenemos abiertos
void LindaDriver::cierreSockets() {
    string MENS_FIN="END_OF_SERVICE";
    int send_bytes = LS1->Send(socket_LS1, MENS_FIN);	// Envíamos el string
    if(send_bytes == -1){
        cerr << "Error cerrar socket_LS1 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        LS1->Close(socket_LS1);
        LS2->Close(socket_LS2);
        LS3->Close(socket_LS3);
        exit(1);
    }
    LS1->Close(socket_LS1);

    send_bytes = LS2->Send(socket_LS2, MENS_FIN);	// Envíamos el string
    if(send_bytes == -1){
        cerr << "Error cerrar socket_LS2 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        LS2->Close(socket_LS2);
        LS3->Close(socket_LS3);
        exit(1);
    }
    LS2->Close(socket_LS2);

    send_bytes = LS3->Send(socket_LS3, MENS_FIN);	// Envíamos el string
    if(send_bytes == -1){
        cerr << "Error cerrar socket_LS3 send_bytes: " << strerror(errno) << endl;
        // Cerramos el socket
        LS3->Close(socket_LS3);
        exit(1);
    }
    LS3->Close(socket_LS3);
}