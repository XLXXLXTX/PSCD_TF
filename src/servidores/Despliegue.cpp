//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de implementación de un servidor de despliegue que comunica a otros 
//          procesos la localización de los servidores que integran el sistema Linda
// File:    Despliegue.cpp
// Encode:  UTF-8
// EOF:     LF
// Compile with make:
//      make:  make -f Makefile_Despliegue
//      clean: make -f Makefile_Despliegue clean
// Run with bash: ./runDespliegue.bash
// Run manually:  ./Despligue <PORT_DESPLIEGUE>
//--------------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <stdlib.h>
#include "../../libs/Socket/Socket.hpp"

using namespace std;

const string FICHERO  = "servidoresLinda.txt";
const int MAX_CONNECT = 100;

// Pre:  El socket tiene abierta ya la conexion mediante client_fd
//       message contiene la localización de los servidores Linda
// Post: Comunica por el socket la localización de los servidores Linda
//       Termina al recibir MENS_FIN o MENS_CIERRE (en el caso segundo bloquea nuevas 
//       conexiones al servidor de despliegue)
void servCliente(Socket& soc, int client_fd, string& message, bool& terminar);

int main(int numArg, char* argv[]) {
    // Puerto donde escucha el proceso servidor
    int error_code = 0;

    if (numArg < 2) {           // Falta de parámetros
            cerr << "ERROR: Faltan parámetros.\n       ./Despliegue <PORT>\n";
            error_code = -2;
        } else if (numArg > 2) {    // Exceso de parámetros
            cerr << "ERROR: Exceso de parámetros.\n       ./Despliegue <PORT>\n";
            error_code = 2;
        } else {
            // Datos servidor despliegue
            int SERVER_PORT = atoi(argv[1]);    // PORT servidor despliegue

            // Creación del socket con el que se llevará a cabo
            // la comunicación con el servidor.
            Socket chan(SERVER_PORT);
            cout << "Abriendo servidor de despliegue en el puerto " + to_string(SERVER_PORT) + "\n";
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
            int  client_fd[MAX_CONNECT];
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
            
                int clientesActuales = 0;
                bool terminar = false;
                while (clientesActuales<MAX_CONNECT && !terminar) {
                    // Accept
                    client_fd[clientesActuales] = chan.Accept();

                    if(client_fd[clientesActuales] == -1) {
                        cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
                        // Cerramos el socket
                        chan.Close(socket_fd);
                        exit(1);
                    }

                    cliente[clientesActuales] = thread(&servCliente, ref(chan), client_fd[clientesActuales], ref(message), ref(terminar));
                    cout << "Nuevo cliente en serverDespliegue: " + to_string(clientesActuales) + "\n";
                    usleep(10000);
                    clientesActuales++;
                }

                for (int i = 0; i < clientesActuales; i++) {
                    cliente[i].join();
                }
                cout << "Cerrando servidor de despliegue\n";
                // Cerramos el socket del servidor
                error_code = chan.Close(socket_fd);
                if (error_code == -1) {
                    cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
                }
            } // end if (f.is_open())
        } // end else

    return error_code;
}

void servCliente(Socket& soc, int client_fd, string& message, bool& terminar) {
	int length = 100;
	char buffer[length];
    char MENS_FIN[]    = "END_OF_SERVICE";
    char MENS_CIERRE[] = "CLOSE_SERVER";
    bool out = false;
    while (!out) {
        int send_bytes = soc.Send(client_fd, message);
        if (send_bytes == -1) {
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

        if (0 == strcmp(buffer, MENS_CIERRE)) { // Si recibimos "CLOSE_SERVER"
			terminar=true;
            out = true;
		} else if (0 == strcmp(buffer,MENS_FIN)) { // Si recibimos "END_OF_SERVICE"
            out = true;
        }
    }

	// Cerramos cliente
	cout << "Cliente desconectado del despliegue\n";
	soc.Close(client_fd);
}