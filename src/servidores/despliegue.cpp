//********************************************************************************************
// File:   despliegue.cpp
// Author: Pablo Bueno, Santiago Illa, Luis Palazón, Carlos Paesa, Javier Pardos y Héctor Toral
// Date:   enero 2021
// Coms:   Fichero de implementación de un servidor de despliegue que comunica a otros 
//         procesos la localización de los servidores que integran el sistema Linda
//***********************************************************cd *********************************
#include <iostream>
#include <thread>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include "../../librerias/Socket/Socket.hpp"

using namespace std;

const char FICHERO[] = "servidoresLinda.txt";
const int MAX_CONNECT = 100;

//-------------------------------------------------------------
// Pre: el socket tiene abierta ya la conexion mediante client_fd
//      message contiene la localización de los servidores Linda
// Post: comunica por el socket la localización de los servidores Linda
//       Termina al recibir MENS_FIN o MENS_CIERRE (en el caso segundo bloquea nuevas 
//       conexiones al servidor de despliegue)
void servCliente(Socket& soc, int client_fd, string& message, bool& terminar) {
	int length = 100;
	char buffer[length];
    char MENS_FIN[]="END_OF_SERVICE";
    char MENS_CIERRE[]="CLOSE_SERVER";
    bool out = false;
    while(!out) {
        int send_bytes = soc.Send(client_fd, message);
        if(send_bytes == -1) {
            string mensError(strerror(errno));
            cerr << "Error: fallo al enviar mensaje en despliegue: " + mensError + "\n";
            // Cerramos los sockets
            soc.Close(client_fd);
            exit(1);
        }

        // Recibimos el mensaje del cliente
        int rcv_bytes = soc.Recv(client_fd,buffer,length);
        if (rcv_bytes <= 0) {
            string mensError(strerror(errno));
            cerr << "Error: fallo al recibir mensaje en despliegue: " + mensError + "\n";
        }

        if(0 == strcmp(buffer, MENS_CIERRE)) { // Si recibimos "CLOSE_SERVER"
			terminar=true;
            out=true;
		} else if(0 == strcmp(buffer,MENS_FIN)) { // Si recibimos "END_OF_SERVICE"
            out=true;
        }
    }

	// Cerramos cliente
	cout << "Cliente desconectado del despliegue" << endl;
	soc.Close(client_fd);
}
//-------------------------------------------------------------
int main(int argc, char* argv[]) {
    // Puerto donde escucha el proceso servidor
    int SERVER_PORT = atoi(argv[1]); //normalmente será un parámetro de invocación

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);

    // Bind
    int socket_fd = chan.Bind();
    if (socket_fd == -1) {
        cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
        exit(1);
    }

    // Listen
    int error_code = chan.Listen(MAX_CONNECT);
    if (error_code == -1) {
        cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    thread cliente[MAX_CONNECT];
    int client_fd[MAX_CONNECT];
    ifstream f(FICHERO);
    // El fichero contiene tres líneas, cada una con una dirección y un puerto separadas por ":"
    // correspondientes al primer, segundo y tercer servidor que integran el sistema Linda. Ejemplo de formato:
    //
    // 155.210.152.183:2120
    // 155.210.152.183:2121
    // 155.210.152.183:2122

    if (f.is_open()) {
        string temp;
        f >> temp;
        string message = temp + ":";
        f >> temp;
        message = message + temp + ":";
        f >> temp;
        message = message + temp + ":";
	
        int clientesActuales=0;
        bool terminar = false;
        while(clientesActuales<MAX_CONNECT && !terminar) {
            // Accept
            client_fd[clientesActuales] = chan.Accept();

            if(client_fd[clientesActuales] == -1) {
                cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
                // Cerramos el socket
                chan.Close(socket_fd);
                exit(1);
            }

            cliente[clientesActuales] = thread(&servCliente, ref(chan), client_fd[clientesActuales], ref(message), ref(terminar));
            cout << "Nuevo cliente " + to_string(clientesActuales) + " aceptado" + "\n";
            usleep(1000);
            clientesActuales++;
        }

        for (int i=0; i<clientesActuales; i++) {
            cliente[i].join();
        }

        // Cerramos el socket del servidor
        error_code = chan.Close(socket_fd);
        if (error_code == -1) {
            cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
        }
    }

    return error_code;
}
//-------------------------------------------------------------
