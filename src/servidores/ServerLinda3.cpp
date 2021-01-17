//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de implementación del tercero de los servidores que integran el sistema Linda
// 		    Contiene en su espacio de tuplas las de tamaño 6
// File:    ServerLinda3.cpp
// Encode:  UTF-8
// EOF:     LF
// Compile with make:
//      make:  make -f Makefile_ServersLinda
//      clean: make -f Makefile_ServersLinda clean
// Run with bash: ./runServers.bash
// Run manually:  ./ServerLinda3 <PORT>
//--------------------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <sstream>
#include <cstring> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../libs/Socket/Socket.hpp"
#include "../../libs/MonitorLinda/MonitorLinda.hpp"

using namespace std;

const int MAX_CONNECT = 100;

// Pre:  ---
// Post: Devuelve cuantas subcadenas de caracteres hay separadas por comas
int largo(string cad);

// Permite gestionar mediante threads a clientes que deseen usar espacios de tuplas de tamaño 6 del servicio Linda
void servCliente(Socket& soc, int client_fd, MonitorLinda& ML, bool& terminar);

int main(int numArg, char* args[]) {
	int error_code = 0;

    if (numArg < 2) {           // Falta de parámetros
        cerr << "ERROR: Faltan parámetros.\n       ./ServerLinda3 <PUERTO>\n";
        error_code = -2;
    } else if (numArg > 2) {    // Exceso de parámetros
        cerr << "ERROR: Exceso de parámetros.\n       ./ServerLinda3 <PUERTO>\n";
        error_code = 2;
    } else {
		// Dirección del servidor y tamaño de las tuplas
		string SERVER_ADDRESS = "localhost";
		int SERVER_PORT 	  = atoi(args[1]);
		int client_fd[MAX_CONNECT];

		cout << "Iniciando servidor Linda nº3 en el puerto " + to_string(SERVER_PORT) + "\n";
		// Socket para comunicación
		Socket socket(SERVER_PORT);

		// Bind 
		int socket_fd =socket.Bind();
		if (socket_fd == -1) {
			string mensError(strerror(errno));
			cerr << "error: failure at Bind(): " + mensError + "\n";
			exit(1);
		}

		// Listen
		int error_code = socket.Listen(MAX_CONNECT);
		if (error_code == -1) {
			string mensError(strerror(errno));
			cerr << "error: failure at Listen(): " + mensError + "\n";
			// Cerramos el socket
			socket.Close(socket_fd);
			exit(1);
		}

		// Monitores para el manejo del espacio de tuplas
		MonitorLinda ML6;

		thread cliente[MAX_CONNECT];
		int clientesActuales = 0;
		bool terminar = false;
		while(clientesActuales<MAX_CONNECT && !terminar) {
			// Accept
			client_fd[clientesActuales] = socket.Accept();

			if(client_fd[clientesActuales] == -1) {
				string mensError(strerror(errno));
				cerr << "Error: fallo en el Accept(): " + mensError + "\n";
				// Cerramos el socket
				socket.Close(socket_fd);
				exit(1);
			}

			cout << "Nuevo cliente en server3: " + to_string(clientesActuales) + "\n";
			// Thread para tratar al cliente
			cliente[clientesActuales]=thread(&servCliente, ref(socket), client_fd[clientesActuales], ref(ML6), ref(terminar));
			clientesActuales++;
			usleep(10000);
		}

		for (int i = 0; i < clientesActuales; i++) {
			cliente[i].join();
		}	
		cout << "Cerrando servidor Linda n3"<< endl;
		// Cerramos el socket del servidor
		error_code = socket.Close(socket_fd);
		if (error_code == -1) {
			string mensError(strerror(errno));
			cerr << "error: failed at close server socket: " + mensError + "\n";
		}
    }

    return error_code;
}

int largo(string cad) {
    stringstream sstream(cad);
    int tamanyo = 1;
    for(unsigned int i = 0; i < cad.size(); i++) {
        if(sstream.get() == ',') {
			tamanyo++;
		}
    }
    return tamanyo;
}

void servCliente(Socket& soc, int client_fd, MonitorLinda& ML6, bool& terminar) {
	char MENS_FIN[]	   = "END_OF_SERVICE";
	char MENS_CIERRE[] = "CLOSE_SERVER";
    char MENS_INFO[]   = "DATA_REQUIRED";
	// Buffer para recibir el mensaje
	int length = 100;
	char buffer[length];
	bool out = false; // Inicialmente no salir del bucle
	while(!out) {
		// Recibimos el mensaje del cliente
		int rcv_bytes = soc.Recv(client_fd,buffer,length);
		if (rcv_bytes <= 0) {
			string mensError(strerror(errno));
    		cerr << "Error: fallo al recibir mensaje en LindaServer1: " + mensError + "\n";
			// Cerramos los sockets
			out = true;
		}
		// Si recibimos "END OF SERVICE" --> Fin de la comunicación
		if (0 == strcmp(buffer, MENS_FIN)) {
			out = true; // Salir del bucle
		} else if(0 == strcmp(buffer, MENS_CIERRE)) { // Si recibimos "CLOSE_SERVER"
			terminar=true;
    		cout << "Linda3 ya no acepta nuevos clientes\n";
			out = true; // Salir del bucle
		} else {
			string message;
			if (0 == strcmp(buffer, MENS_INFO)) {
				ML6.INFO(message);
				message = message +":";
			} else {
				// Recibe el mensaje como operacion:tupla
				char* operation = strtok (buffer,":");
				char* tupla = strtok (NULL, ":");
				int size = largo(tupla);
				if (size != 6) {
					cerr << "Error: tamaño de tupla erroneo en server3\n";
					soc.Close(client_fd);
					exit(1);
				}
				Tupla t (size);
				t.from_string(tupla);

				// Determinación de la operación
				if (strcmp(operation,"PN" ) == 0) {
					// La operacion es PostNote
					// Ejecución de la operación del monitor
					ML6.PN(t);
					message = "OK";
				} else if (strcmp(operation,"RDN" ) == 0) {
					// La operacion es ReadNote
					Tupla r (6);
					// Ejecución de la operación del monitor
					ML6.RDN(t,r);
					message = r.to_string();
				} else if (strcmp(operation,"RN") == 0) {
					// La operación es RemoveNote
					Tupla r (6);
					// Ejecución de la operación del monitor
					ML6.RN(t,r);
					message = r.to_string();
				} else if (strcmp(operation,"RDN_2" ) == 0) {
					// La operacion es ReadNote_2
					tupla = strtok (NULL, ":");
					// Separa en <<tupla>> la segunda tupla con la que trabajar
					Tupla t2 (6);
					t2.from_string(tupla);
					// Tenemos la segunda tupla en <<t2>>
					Tupla r1(6), r2 (6);
					// Ejecución de la operación del monitor
					ML6.RDN_2(t,t2,r1,r2);
					message = r1.to_string() + ":" + r2.to_string() + ":";
				} else if (strcmp(operation,"RN_2") == 0) {
					// La operación es RemoveNote_2
					tupla = strtok (NULL, ":");
					// Separa en <<tupla>> la segunda tupla con la que trabajar
					Tupla t2 (6);
					t2.from_string(tupla);
					// Tenemos la segunda tupla en <<t2>>
					Tupla r1(6), r2 (6);
					// Ejecución de la operación del monitor
					ML6.RN_2(t,t2,r1,r2);
					message = r1.to_string() + ":" + r2.to_string() + ":";
				} else {
					message = "UNKNOWN_PETITION";
				}
			}
			if (!out) {
				// Si es Postnote se envia OK
				// Si es ReadNote o RemoveNote normal o múltiple se envia(n) la(s) tupla(s) pedida(s)
				// Si es INFO se envian los datos sobre el servidor
				int send_bytes = soc.Send(client_fd, message);
				if (send_bytes == -1) {
					string mensError(strerror(errno));
					cerr << "Error: fallo al enviar mensaje en LindaServer1: " + mensError + "\n";
					// Cerramos los sockets
					out = true;
				}
			}
		}
	}
	// Cerramos cliente
	cout << "Cliente desconectado del server3\n";
	soc.Close(client_fd);
}
//-------------------------------------------------------------
