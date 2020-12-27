#include <iostream>
#include <thread>
#include <fstream>
#include <ctime>   // localtime
#include "../libs/Socket/Socket.hpp"

#include <chrono>  // chrono::system_clock

#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string

using namespace std;

const int N = 3;        // Número de servidores Linda
const int LENGTH = 100; // Longitud de los mensajes

// Pre:  Servidor cerrado
// Post: Abre servidor
// Coms: ---
void launchServer(Socket& chan, thread cliente[], int client_fd[], int& socket_fd, int& error_code);

// Pre:  Servidor abierto
// Post: Cierra el server
// Coms: ---
void closeServer(Socket& chan, thread cliente[], int& socket_fd, int& error_code);

// Pre:  True
// Post: mensaje = mensaje enviado por el servidor
// Coms: Envia un mensaje
void enviarMSG(Socket& soc, const int client_fd, const string mensaje);

// Pre:  True
// Post: Buffer = mensaje recibido por el cliente
// Coms: Recibe un mensaje
void recibirMSG(Socket& soc, const int client_fd, string& buffer);

// Pre:  True
// Post: Registra imagenes del estado en tiempo "real" del resto de servidores
// Coms: ---
void Analysis(Socket& soc, const int client_fd);


void escribirLog(string idFichero, int numTuplas, int lectPend, int lectTotal, int escPend, int escTotal);

int main(int numArg, char* args[]) {
    if (numArg == 3) {
        // IP Serv y puertoDespliegue

        string SERVER_ADDRESS = args[1]; 
        int    SERVER_PORT = atoi(args[2]);         // Puerto donde escucha el proceso servidor
        Socket chan(SERVER_ADDRESS, SERVER_PORT);   // Creación del Socket que lleva la comunicación con el servidor.

        thread cliente[N];
        int client_fd[N];
        int socket_fd = chan.Bind();
        int error_code;

        launchServer(chan, cliente, client_fd, socket_fd, error_code);
        closeServer(chan, cliente, socket_fd, error_code);

        return error_code;
    } else {
        cerr << "ERROR -> Falta de parametros en la ejecución\n";
        cerr << "\t-- Prueba a ejecutarlo asi: ./ServAnalisis <server_IP> <server_PORT>\n";
        return 0;   
    }

    return 0;
}

void launchServer(Socket& chan, thread cliente[], int client_fd[], int& socket_fd, int& error_code) {
    // Bind
    if (socket_fd == -1) {
        cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
        exit(1);
    }

    // Listen
    error_code = chan.Listen(N);
    if (error_code == -1) {
        cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
        chan.Close(socket_fd);  // Cerramos el socket
        exit(1);
    }

    for (int id = 0; id < N; id++) {
        // Accept
        client_fd[id] = chan.Accept();

        if(client_fd[id] == -1) {
            cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
            chan.Close(socket_fd);  // Cerramos el socket
            exit(1);
        }

        cliente[id] = thread(&Analysis);
        cout << "Nuevo cliente " + to_string(id) + " aceptado" + "\n";
    }
}

void closeServer(Socket& chan, thread cliente[], int& socket_fd, int& error_code) {
    for (int id = 0; id < N; id++) {
        cliente[id].join();
    }

    error_code = chan.Close(socket_fd); // Cerramos el socket del servidor
    if (error_code == -1) {
        cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
    }

    cout << "Bye bye" << endl;
}

void enviarMSG(Socket& soc, const int client_fd, const string mensaje) {
    int send_bytes;
    send_bytes = soc.Send(client_fd, mensaje);
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";    
        soc.Close(client_fd);   // Cerramos los sockets.
        exit(1);
    }
}

void recibirMSG(Socket& soc, const int client_fd, string& buffer) {
    int rcv_bytes;
    rcv_bytes = soc.Recv(client_fd, buffer, LENGTH);    // Recibimos el mensaje del cliente
    if (rcv_bytes == -1) {
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        soc.Close(client_fd);   // Cerramos los sockets.
    }
}

void Analysis() {
    // Recibir datos 
    // escribirLog(idFichero, numTuplas, lectPend, lectTotal, escPend, escTotal){}
    // Escribir en fichero: lectPend, lectTotal, escPend, escTotal
}

void escribirLog(string idFichero, int numTuplas, int lectPend, int lectTotal, int escPend, int escTotal) {
    string fichero   = "log-linda";
    string extension = ".log";
    string nombreFichero = fichero + "-" + idFichero + extension;
    
    ofstream fichLog(nombreFichero, ios::app);
    if (fichLog.is_open()) {
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);

        stringstream ss;
        ss << put_time(localtime(&in_time_t), "%d-%m-%Y at %T");
        
        fichLog << "Date: "<< ss.str()  << endl;
        fichLog <<  "\tNum tuplas: "    << numTuplas << endl;
        fichLog <<  "\tNum Lectura: "   << endl;
        fichLog <<  "\t\tTotal:      "  << lectTotal << endl;
        fichLog <<  "\t\tPendientes: "  << lectPend;
        fichLog <<  "\tNum Escritura: " << endl;
        fichLog <<  "\t\tTotal:      "  << escTotal << endl;
        fichLog <<  "\t\tPendientes: "  << escPend << endl;

        fichLog.close();
    } else {
        cerr << "ERROR > No se pudo abrir el fichero: \"" << nombreFichero << "\"" << endl;
    }
}