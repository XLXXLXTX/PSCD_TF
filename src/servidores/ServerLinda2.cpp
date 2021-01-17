//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de implementación del segundo de los servidores que integran el sistema Linda
// 		    Contiene en su espacio de tuplas las de tamaño 4 y 5
// File:    ServerLinda2.cpp
// Encode:  UTF-8
// EOF:     LF
// Compile with make:
//      make:  make -f Makefile_ServersLinda
//      clean: make -f Makefile_ServersLinda clean
// Run with bash: ./runServers.bash
// Run manually:  ./ServerLinda2 <PORT>
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

// Pre:
// Post
// Coms: Permite gestionar mediante threads a clientes que deseen usar espacios de tuplas de tamaños 4-5 del servicio Linda
void servCliente(Socket& soc, int client_fd, MonitorLinda& ML4, MonitorLinda& ML5, bool& terminar);

int main(int numArg, char* args[]) {
	int error_code = 0;

    if (numArg < 2) {           // Falta de parámetros
        cerr << "ERROR: Faltan parametros.\n       ./ServerLinda2 <PUERTO>\n";
        error_code = -2;
    } else if (numArg > 2) {    // Exceso de parámetros
        cerr << "ERROR: Exceso de parametros.\n       ./ServerLinda2 <PUERTO>\n";
        error_code = 2;
    } else {
		// Dirección del servidor y tamaño de las tuplas
		string SERVER_ADDRESS = "localhost";
		int SERVER_PORT 	  = atoi(args[1]);
		int client_fd[MAX_CONNECT];

		cout << "Iniciando servidor Linda nº2 en el puerto " + to_string(SERVER_PORT) + "\n";
		// Socket para comunicación
		Socket socket(SERVER_PORT);

		// Bind 
		int socket_fd =socket.Bind();
		if (socket_fd == -1) {
			string mensError(strerror(errno));
			cerr << "Error: fallo en el Bind(): " + mensError + "\n";
			exit(1);
		}

		// Listen
		int error_code = socket.Listen(MAX_CONNECT);
		if (error_code == -1) {
			string mensError(strerror(errno));
			cerr << "Error: fallo en el Listen(): " + mensError + "\n";
			// Cerramos el socket
			socket.Close(socket_fd);
			exit(1);
		}

		// Monitores para el manejo del espacio de tuplas
		MonitorLinda ML4;
		MonitorLinda ML5;

		thread cliente[MAX_CONNECT];
		int clientesActuales = 0;
		bool terminar = false;
		while (clientesActuales<MAX_CONNECT && !terminar) {
			// Accept
			client_fd[clientesActuales] = socket.Accept();

			if (client_fd[clientesActuales] == -1) {
				string mensError(strerror(errno));
				cerr << "Error: fallo en el Accept(): " + mensError + "\n";
				// Cerramos el socket
				socket.Close(socket_fd);
				exit(1);
			}

			cout << "Nuevo cliente en server2: " + to_string(clientesActuales) + "\n";
			// Thread para tratar al cliente
			cliente[clientesActuales] = thread(&servCliente, ref(socket), client_fd[clientesActuales], ref(ML4), ref(ML5), ref(terminar));
			clientesActuales++;
			usleep(10000);
		}

		for (int i = 0; i < clientesActuales; i++) {
			cliente[i].join();
		}
		cout << "Cerrando servidor Linda n2\n";
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
    for (unsigned int i = 0; i < cad.size(); i++) {
        if (sstream.get() == ',') {
			tamanyo++;
		}
    }
    return tamanyo;
}

void servCliente(Socket& soc, int client_fd, MonitorLinda& ML4, MonitorLinda& ML5, bool& terminar) {
	char MENS_FIN[]	   = "END_OF_SERVICE";
	char MENS_CIERRE[] = "CLOSE_SERVER";
    char MENS_INFO[]   = "DATA_REQUIRED";
	// Buffer para recibir el mensaje
	int length = 100;
	char buffer[length];
	bool out = false;	// Inicialmente no salir del bucle
	while (!out) {
		// Recibimos el mensaje del cliente
		int rcv_bytes = soc.Recv(client_fd,buffer,length);
		if (rcv_bytes <= 0) {
			string mensError(strerror(errno));
    		cerr << "Error: fallo al recibir mensaje en LindaServer2: " + mensError + "\n";
			// Cerramos los sockets
			out = true;
		}
		// Si recibimos "END OF SERVICE" --> Fin de la comunicación
		if (0 == strcmp(buffer, MENS_FIN)) {
			out = true; // Salir del bucle
		} else if(0 == strcmp(buffer, MENS_CIERRE)) { // Si recibimos "CLOSE_SERVER"
			terminar = true;
    		cout << "Linda2 ya no acepta nuevos clientes\n";
			out = true; // Salir del bucle
		} else {
			string message;
			if (0 == strcmp(buffer, MENS_INFO)) {
				string temp;
				ML4.INFO(temp);
				message = temp +":";
				ML5.INFO(temp);
				message = message + temp +":";
			} else {
				// Recibe el mensaje como operacion:tupla:tupla2 (tupla2 opcional)
				// Separa en <<operacion>> la operacion (PN, ReadN o RN) y 
				// en <<tupla>> la tupla con la que trabajar
				char* operation = strtok (buffer,":");
				char* tupla = strtok (NULL, ":");
				int size = largo(tupla);
				Tupla t (size);
				t.from_string(tupla);

				// Determinación de la operación
				if (strcmp(operation,"PN" ) == 0) {
					// La operacion es PostNote
					// Ejecución de la operación del monitor
					switch (size) {
						case 4: ML4.PN(t); break;
						case 5: ML5.PN(t); break;           
						default:
							cerr << "Error: tamaño de tupla erroneo en server2\n";
							soc.Close(client_fd);
							exit(1);
					}
					message = "OK";
				} else if (strcmp(operation,"RDN" ) == 0) {
					// La operacion es ReadNote
					Tupla r (size);
					// Ejecución de la operación del monitor
					switch (size) {
						case 4: ML4.RDN(t,r); break;
						case 5: ML5.RDN(t,r); break;           
						default:
							cerr << "Error: tamaño de tupla erroneo en server2\n";
							soc.Close(client_fd);
							exit(1);
					}
					message = r.to_string();
				} else if (strcmp(operation,"RN") == 0) {
					// La operación es RemoveNote
					Tupla r (size);
					// Ejecución de la operación del monitor
					switch (size) {
						case 4: ML4.RN(t,r); break;
						case 5: ML5.RN(t,r); break;           
						default:
							cerr << "Error: tamaño de tupla erroneo en server2\n";
							soc.Close(client_fd);
							exit(1);
					}
					message = r.to_string();
				} else if (strcmp(operation,"RDN_2" ) == 0) {
					// La operacion es ReadNote_2
					tupla = strtok (NULL, ":");
					// Separa en <<tupla>> la segunda tupla con la que trabajar
					Tupla t2 (size);
					t2.from_string(tupla);
					// Tenemos la segunda tupla en <<t2>>
					Tupla r1(size), r2 (size);
					// Ejecución de la operación del monitor
					switch (size) {
						case 4: ML4.RDN_2(t,t2,r1,r2); break;
						case 5: ML5.RDN_2(t,t2,r1,r2); break;           
						default:
							cerr << "Error: tamaño de tupla erroneo en server2\n";
							soc.Close(client_fd);
							exit(1);
					}
					message = r1.to_string() + ":" + r2.to_string() + ":";
				} else if (strcmp(operation,"RN_2") == 0) {
					// La operación es RemoveNote_2
					tupla = strtok (NULL, ":");
					// Separa en <<tupla>> la segunda tupla con la que trabajar
					Tupla t2 (size);
					t2.from_string(tupla);
					// Tenemos la segunda tupla en <<t2>>
					Tupla r1(size), r2 (size);
					// Ejecución de la operación del monitor
					switch (size) {
						case 4: ML4.RN_2(t,t2,r1,r2); break;
						case 5: ML5.RN_2(t,t2,r1,r2); break;           
						default:
							cerr << "Error: tamaño de tupla erroneo en server2\n";
							soc.Close(client_fd);
							exit(1);
					}
					message = r1.to_string() + ":" + r2.to_string() + ":";
				}else {
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
					cerr << "Error: fallo al enviar mensaje en LindaServer2: " + mensError + "\n";
					// Cerramos los sockets
					out = true;
				}
			}
		}
	}
	// Cerramos cliente
	cout << "Cliente desconectado del server2\n";
	soc.Close(client_fd);
}