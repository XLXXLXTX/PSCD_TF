//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de implementación de un programa de monitorización de los servidores de 
//          tuplas que integran el sistema Linda
// File:    Monitorizacion.cpp
// Encode:  UTF-8
// EOF:     LF
// Compile with make:
//      make:  make -f Makefile_Monitorizacion
//      clean: make -f Makefile_Monitorizacion clean
// Run manually:  ./Monitorizacion <IP_DESPLIEGUE> <PORT_DESPLIEGUE>
// Stop:            Escribir por entrada estandar el caracter "q"
//--------------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include "../libs/Socket/Socket.hpp"

using namespace std;

const int PERIODO = 10;
const int LENGTH = 500;             // Longitud de los mensajes
const int MAX_ATTEMPS = 10;      // Máximo número de intento de conexiones

// Pre:  Tiene creado un socket con la direccion del servidor despliegue
// Post: buffer guarda las direcciones de los servidores linda
void tryDespligue(Socket& chan, char* buffer, int& error_code);

// Pre:  La ip y puerto corresponden a un servidor Linda encendido
// Post: Se conecta al servidor linda correspondiente y crea un log nuevo cada PERIODO segundos
// Coms: Si error_code == -1 se cierran todos los canales de mensajes y se avisa por pantalla
//       de cual de todos los servidores ha fallado
void launchMonitor(char* IP_ADDRESS, int PORT_ADDRESS, int& error_code, const int ID, bool& fin);

// Pre:  True
// Post: Envía un mensaje mediante un socket([IP][POST])
// Coms: Envía un mensaje al servidor correspondiente
void enviarMSG(Socket& soc, const int client_fd, const char* mensaje, int& error_code);

// Pre:  True
// Post: Recibe un mensaje mediante un socket([IP][POST])
// Coms: Recibe un mensaje de un servidor 
void recibirMSG(Socket& soc, const int client_fd, char* buffer, int& error_code);

// Pre:  True
// Post: Pide y recibe los datos que usara escribirLog(...) para crear el log
void analysis(Socket& chan, const int monitor_fd, int& error_code, const int ID);

// Pre:  True
// Post: Añade o crea un log para el servidor linda correspondiente con los datos recogids en Analysis
void escribirLog(const int numTuplas, const int lectPend, const int lectTotal, const int borradas, const int escTotal, const int t);

int main(int numArg, char* args[]) {
    int error_code = 0;

    if (numArg < 3) {           // Falta de parámetros
        cerr << "ERROR: Faltan parametros.\n       ./Cliente <IP> <PORT>\n";
        error_code = -2;
    } else if (numArg > 3) {    // Exceso de parámetros
        cerr << "ERROR: Exceso de parametros.\n       ./Cliente <IP> <PORT>\n";
        error_code = 2;
    } else {
        // Datos para comunicación con Despliegue
        string DESPLIEGUE_ADDRESS = args[1];    // IP   servidor despliegue
        int DESPLIEGUE_PORT = atoi(args[2]);    // PORT servidor despliegue
        Socket chan(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);   // Creación del Socket que lleva la comunicaci�n con el servidor.
        char buffer[LENGTH];
        tryDespligue(chan, buffer,error_code);

        if (error_code == -1) {
            cerr << "1- Error al obtener las [IP][PORT] del servidor despliegue.\n";
            cerr << "2- Error cerrando el socket del servidor despliegue.\n";
            error_code = -1;
        } else {
            char* servidor = strtok (buffer,":");
            int puerto     = atoi(strtok (NULL,":"));
            bool fin = false;
            thread cliente[3];
            cliente[0] = thread(&launchMonitor, servidor, puerto, ref(error_code), 1, ref(fin));
            
            cout << "Nuevo monitor 1 creado\n";
            for (int i = 1; i < 3; i++) {
                servidor   = strtok (NULL,":");
                puerto     = atoi(strtok (NULL,":"));
                cliente[i] = thread(&launchMonitor, servidor, puerto, ref(error_code), i+1, ref(fin));
                cout << "Nuevo monitor " + to_string(i+1) + " creado.\n";
            }

            char entrada;
            cin >> entrada;
            while(entrada != 'q') {
                cin >> entrada;
            }
            fin = true;

            // Espera la finalización de los procesos
            for (int id = 0; id < 3; id++) {
                cliente[id].join();
            }
            cout << "Monitorización terminada\n";
        }
    }

    return error_code;
}

void tryDespligue(Socket& chan, char* buffer, int& error_code) {
    int socket_fd;
    int count  = 0;
    char END[] = "END_OF_SERVICE";
    do {
        // Conexión con el servidor despliegue
        socket_fd = chan.Connect();
        count++;

        // Si hay error --> esperamos 1 segundo para reconectar
        if(socket_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);
    error_code = socket_fd;
    
    // Chequeamos si se ha realizado la conexión
    if (socket_fd != -1) {
        // Recibir direcciones de los servidores linda
        recibirMSG(chan, socket_fd, buffer, error_code);

        // Fin de la comunicación con servidor despliegue
        int send_bytes = chan.Send(socket_fd, END);	// Envíamos el string
        
        if(send_bytes == -1){
            cerr << "Error cerrar socket_fd despliegue: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        error_code = chan.Close(socket_fd); // Cerramos el socket del servidor
    }
}

void launchMonitor(char* IP_ADDRESS, int PORT_ADDRESS, int& error_code, const int ID, bool& fin) {
    Socket soc(IP_ADDRESS, PORT_ADDRESS);
    int monitor_fd;
    int count  = 0;
    char END[] = "END_OF_SERVICE";
    do {
        cout << "Intento conectarme..." << endl;
        // Conexión con el servidor despliegue
        monitor_fd = soc.Connect();
        count++;
        // Si error --> esperamos 1 segundo para reconectar
        if (monitor_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(monitor_fd == -1 && count < MAX_ATTEMPS);

    this_thread::sleep_for(chrono::seconds(ID));
    // Chequeamos si se ha realizado la conexión
    if(monitor_fd != -1) {
        int i = 0;
        do {
            analysis(soc, monitor_fd, error_code, ID);
            this_thread::sleep_for(chrono::seconds(PERIODO));   // Hace un LOG cada X segundos
            i++;
        } while (error_code != -1 && !fin);

        enviarMSG(soc, monitor_fd, END, error_code);
        error_code = soc.Close(monitor_fd); // Cerramos el socket del servidor
        if (error_code == -1) {
            cerr << "Error cerrando el socket del servidor: " + to_string(ID) + " | Codigo de error: " + string(strerror(errno)) + "\n";
        }
    }
}

void enviarMSG(Socket& soc, const int client_fd, const char* mensaje, int& error_code) {
    int send_bytes = soc.Send(client_fd, mensaje);
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";    
        soc.Close(client_fd);   // Cerramos los sockets.
        error_code = -1;
    } else {
        error_code = 0;
    }
}

void recibirMSG(Socket& soc, const int client_fd, char* buffer, int& error_code) {
    int rcv_bytes = soc.Recv(client_fd, buffer, LENGTH);    // Recibimos el mensaje del cliente
    if (rcv_bytes == -1) {
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        soc.Close(client_fd);   // Cerramos los sockets.
        error_code = -1;
    } else {
        error_code = 0;
    }
}

void analysis(Socket& soc, const int monitor_fd, int& error_code, const int ID) {
    char data[LENGTH];
    char DATA[]   = "DATA_REQUIRED";
    int numTuplas = 0;
    int lectPend  = 0;
    int lectTotal = 0;
    int escTotal  = 0;
    int borradas  = 0;
    
    // Pedir datos
    enviarMSG(soc, monitor_fd, DATA, error_code);
    // Recibir datos
    recibirMSG(soc, monitor_fd, data, error_code);   // data = "numTuplas,lectPend,lectTotal,borradas,escTotal

    int t = 1;
    if (ID == 2)      t = 4;
    else if (ID == 3) t = 6;

    char* info;
    info = strtok(data,":");
    sscanf(info,"%d,%d,%d,%d,%d", &numTuplas, &lectPend, &lectTotal, &borradas, &escTotal);
    escribirLog(numTuplas, lectPend, lectTotal, borradas, escTotal, t);
    for (int i = ID + 1; i <= 3; i++) {
        t++;
        info = strtok(NULL,":");
        sscanf(info, "%d,%d,%d,%d,%d", &numTuplas, &lectPend, &lectTotal, &borradas, &escTotal);
        escribirLog(numTuplas, lectPend, lectTotal, borradas, escTotal, t);
    }
}

void escribirLog(const int numTuplas, const int lectPend, const int lectTotal, const int borradas, const int escTotal, const int t) {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%d-%m-%Y at %T");
    cout << "-------------------------------------\n"
         << "FECHA: "<< ss.str() << endl
         << "En el espacio de tumplas de tamaño " << t << ":" << endl
         << "\tNum tuplas: "     << setw(11) << numTuplas     << endl
         << "\tNum Lectura: "    << endl
         << "\t\t+ Total:"       << setw(10) << lectTotal     << endl
         << "\t\t+ Borradas: "   << setw(7)  << borradas      << endl
         << "\t\t+ Pendientes: " << setw(5)  << lectPend      << endl
         << "\tNum Escritura: "  << endl
         << "\t\t+ Total:"       << setw(10) << escTotal      << endl;
}