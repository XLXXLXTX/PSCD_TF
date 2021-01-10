//********************************************************************************************
// File:   despliegue.cpp
// Author: Pablo Bueno, Santiago Illa, Luis Palazón, Carlos Paesa, Javier Pardos y Héctor Toral
// Date:   enero 2021
// Coms:   Fichero de implementación de un proceso que bloquea el acceso a nuevos clientes en 
//         el servidor de despliegue y los servidores que integran el sistema Linda
//********************************************************************************************
#include <iostream>
#include <string>  // string
#include "../../librerias/Socket/Socket.hpp"

using namespace std;

const int LENGTH = 500;             // Longitud de los mensajes
const int MAX_ATTEMPS    = 10;      // Máximo número de intento de conexiones

// Pre:  Tiene creado un socket con la direccion del servidor despliegue
// Post: buffer guarda las direcciones y puerto de los servidores Linda
//       Además, bloquea nuevos clientes en el servidor de despliegue
void cierreDespliegue(Socket& chan, char* buffer, int& error_code);

// Pre:  La ip y puerto corresponden a un servidor Linda encendido
// Post: Bloquea nuevos clientes en el servidor Linda
void cierreServidor(char* IP_ADDRESS, const int PORT_ADDRESS, int& error_code);

int main(int numArg, char* args[]) {
    if (numArg == 3) {
        int error_code;
        // Datos para comunicación con Despliegue
        string DESPLIEGUE_ADDRESS = args[1];            // IP   servidor despliegue
        int    DESPLIEGUE_PORT    = atoi(args[2]);      // PORT servidor despliegue
        Socket chan(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);   // Creación del Socket que lleva la comunicación con el servidor.
        char buffer[LENGTH];
        cierreDespliegue(chan, buffer,error_code);

        if (error_code == -1) {
            cerr << "1- Error al obtener las [IP][PORT] del servidor despliegue.\n";
            cerr << "2- Error cerrando el socket del servidor despliegue.\n";
            return -1;
        } else {
            char* servidor = strtok (buffer,":");
            int puerto = atoi(strtok (NULL,":"));
            cierreServidor(servidor, puerto, error_code);
            for (int i = 1; i < 3; i++) {
                servidor = strtok (NULL,":");
                puerto = atoi(strtok (NULL,":"));
                cierreServidor(servidor, puerto, error_code);
            }
            cout << "Finalizador ha terminado su ejecución\n";
            return error_code;
        }
    } else {
        cerr << "ERROR -> Fallo en parametros en la ejecución\n";
        cerr << "\t-- Prueba a ejecutarlo asi: ./finalizador <server_IP> <server_PORT>\n";
        return -2;
    }
}

void cierreDespliegue(Socket& chan, char* buffer, int& error_code) {
    int socket_fd;
    int count = 0;
	char MENS_CIERRE[]="CLOSE_SERVER";
    do {
        // Conexión con el servidor despliegue
        socket_fd = chan.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd == -1) {
            sleep(1);
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if (socket_fd != -1) {
        // Recibir direcciones de los servidores linda
        int rcv_bytes = chan.Recv(socket_fd, buffer, LENGTH);    // Recibimos el mensaje del cliente
        if (rcv_bytes == -1) {
            cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
            chan.Close(socket_fd);   // Cerramos los sockets.
            exit(1);
        }

        // Fin de la comunicación con servidor despliegue
        int send_bytes = chan.Send(socket_fd, MENS_CIERRE);	// Envíamos el string
        if(send_bytes == -1){
            cerr << "Error cerrar socket_fd despliegue: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        error_code = chan.Close(socket_fd); // Cerramos el socket del servidor
    }
}

void cierreServidor(char* IP_ADDRESS, const int PORT_ADDRESS, int& error_code) {
    Socket soc(IP_ADDRESS, PORT_ADDRESS);
    int socket_fd;
    int count = 0;
	char MENS_CIERRE[]="CLOSE_SERVER";
    do {
        // Conexión con el servidor despliegue
        socket_fd = soc.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd == -1) {
            sleep(1);
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if (socket_fd != -1) {
        // Fin de la comunicación con servidor Linda
        int send_bytes = soc.Send(socket_fd, MENS_CIERRE);	// Envíamos el string
        if(send_bytes == -1){
            cerr << "Error cerrar socket_fd despliegue: " << strerror(errno) << endl;
            // Cerramos el socket
            soc.Close(socket_fd);
            exit(1);
        }
        error_code = soc.Close(socket_fd); // Cerramos el socket del servidor
    }
}
