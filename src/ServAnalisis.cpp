#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <map>
#include "../libs/Socket/Socket.hpp"



using namespace std;

const int LENGTH = 500;             // Longitud de los mensajes
const int MAX_ATTEMPS    = 10;      // Máximo número de intento de conexines
const char LIM_SEPARATOR = ',';
const char LIM_END 		 = ']';
const string DATA = "DATA_REQUIRED";
const string END  = "END_OF_SERVICE";

bool CLOSE_SERVER;

// PRE:  Tiene creado un socket con la direccion del servidor despliegue
// POST: myDicc guarda las direcciones de los servidores linda
// COMS: 
void TryDespligue(Socket& chan, map<string,int> myDicc, int& N, int& socket_fd, int& error_code);

// PRE:  True
// POST: Se conecta al servidor linda correspondiente y crea un log nuevo cada X segundos
//       X es una constante que se encuntra dentro del sleep
// COMS: Si error_code == -1 se cierran todos los canales de mensajes y se avisa por pantalla
//       de cual de todos los servidores ha fallado
//       Si CLOSE_SERVER se cierran todos los canales de mensajes
void launchMonitor(const string IP_ADDRESS, const int PORT_ADDRESS, int& monitor_fd, int& error_code, const int ID);

// PRE:  True
// POST: Termina los threads
// COMS: 
void closeMonitor(thread cliente[],  const int N);

// PRE:  True
// POST: Envía un mensaje mediante un socket([IP][POST])
// COMS: Envía un mensaje al servidor correspondiente
void enviarMSG(Socket& soc, const int client_fd, const string mensaje, int& error_code);

// PRE:  True
// POST: Recibe un mensaje mediante un socket([IP][POST])
// COMS: Recibe un mensaje de un servidor 
void recibirMSG(Socket& soc, const int client_fd, string& buffer, int& error_code);

// PRE:  True
// POST: Pide y recibe los datos que usara escribirLog(...) para crear el log
// COMS: 
void Analysis(Socket& chan, const int monitor_fd, int& error_code);

// PRE:  True
// POST: Añade o crea un log para el servidor linda correspondiente con los datos recogids en Analysis
// COMS: 
void escribirLog(const string idFichero, const int numTuplas, const int lectPend, const int lectTotal, const int escPend, const int escTotal);

int main(int numArg, char* args[]) {
    if (numArg == 3) {
        CLOSE_SERVER = false;
        int socket_fd, error_code;
        // Datos para comunicación con Despliegue
        string DESPLIEGUE_ADDRESS = args[1];            // IP   servidor despliegue
        int    DESPLIEGUE_PORT    = atoi(args[2]);      // PORT servidor despliegue
        Socket chan(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);   // Creación del Socket que lleva la comunicación con el servidor.

        map<string,int> myDicc; // Almacena direcciones de los servidores linda [IP][PORT]
        int N = 0;              // Número de servidores Linda
        
        TryDespligue(chan, myDicc, N, socket_fd, error_code);
        if (socket_fd == -1 || error_code == -1) {
            cerr << "1- Error al obtener las [IP][PORT] del servidor despliegue.\n";
            cerr << "2- Error cerrando el socket del servidor despliegue.\n";
            return -1;
        } else {
            thread cliente[N];
            int monitor_fd[N];

            map<string,int>::iterator it = myDicc.begin();
            for (int id = 0; id < N && it != myDicc.end(); id++, it++) {
                cliente[id] = thread(&launchMonitor, it->first, it->second, ref(monitor_fd[id]), ref(error_code), id);
                cout << "Nuevo monitor " + to_string(id) + " creado" + "\n";
            }
            closeMonitor(cliente, N);
            return error_code;
        }
    } else {
        cerr << "ERROR -> Falta de parametros en la ejecución\n";
        cerr << "\t-- Prueba a ejecutarlo asi: ./ServAnalisis <server_IP> <server_PORT>\n";
        return -2;
    }
}

void TryDespligue(Socket& chan, map<string,int> myDicc, int& N, int& socket_fd, int& error_code) {
    int count = 0;
    do {
        // Conexión con el servidor despliegue
        socket_fd = chan.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if (socket_fd != -1) {
        // Recibir direcciones de los servidores linda
        string dirsLindaSrv;
        recibirMSG(chan, socket_fd, dirsLindaSrv, error_code);

        // Guardar [ip][port] -> myDicc
        string dirLinda;    // AUX: ip:port
        for (string::iterator it = dirsLindaSrv.begin() + 1; it != dirsLindaSrv.end(); it++) {
            if (*it != LIM_SEPARATOR && *it != LIM_END) {
                dirLinda += *it;
            } else {
                string ip   = dirLinda.substr(0, dirLinda.find(':'));
                int port = stoi(dirLinda.substr(ip.length()+1, dirLinda.length()-1));
                myDicc.insert(pair<string,int>(ip, port));
                dirLinda = "";
                N++;
            }
        }

        // Fin de la comunicación con servidor despliegue
        error_code = chan.Close(socket_fd); // Cerramos el socket del servidor
    }
}

void launchMonitor(const string IP_ADDRESS, const int PORT_ADDRESS, int& monitor_fd, int& error_code, const int ID) {
    Socket chan(IP_ADDRESS, PORT_ADDRESS);
    int count = 0;
    do {
        cout << "Intento conectarme..."<< endl;
        // Conexión con el servidor despliegue
        monitor_fd = chan.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(monitor_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(monitor_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(monitor_fd != -1) {
        do {
            Analysis(chan, monitor_fd, error_code);
            this_thread::sleep_for(chrono::seconds(10));    // Hace un LOG cada X segundos
        } while (error_code != -1 && !CLOSE_SERVER);

        if (CLOSE_SERVER) {
            error_code = chan.Close(monitor_fd); // Cerramos el socket del servidor
            if (error_code == -1) {
                cerr << "Error cerrando el socket del servidor: " + to_string(ID) + " | Codigo de error: " + string(strerror(errno)) + "\n";
            }
        }
    }
}

void closeMonitor(thread cliente[],  const int N) {
    for (int id = 0; id < N; id++) {
        cliente[id].join();
    }
    cout << "SERVER STATUS:     APAGADO\n";
}

void enviarMSG(Socket& soc, const int client_fd, const string mensaje, int& error_code) {
    int send_bytes = soc.Send(client_fd, mensaje);
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";    
        soc.Close(client_fd);   // Cerramos los sockets.
        error_code = -1;
    } else {
        error_code = 0;
    }
}

void recibirMSG(Socket& soc, const int client_fd, string& buffer, int& error_code) {
    int rcv_bytes = soc.Recv(client_fd, buffer, LENGTH);    // Recibimos el mensaje del cliente
    if (rcv_bytes == -1) {
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        soc.Close(client_fd);   // Cerramos los sockets.
        error_code = -1;
    } else {
        error_code = 0;
    }
}

void Analysis(Socket& chan, const int monitor_fd, int& error_code) {
    string data;
    string idFichero;
    int numTuplas = 0;
    int lectPend  = 0;
    int lectTotal = 0;
    int escPend   = 0;
    int escTotal  = 0;
    
    // Pedir datos
    enviarMSG(chan, monitor_fd, DATA, error_code);
    if (error_code == 0) {
        // Recibir datos
        recibirMSG(chan, monitor_fd, data, error_code);   // data = "idFichero,numTuplas,lectPend,lectTotal,escPend,escTotal"
        if (data == END) {
            CLOSE_SERVER = true;    // Fin del programa
        } else {
            sscanf(data.c_str(),"%s,%d,%d,%d,%d,%d", idFichero, numTuplas, lectPend, lectTotal, escPend, escTotal);
            escribirLog(idFichero, numTuplas, lectPend, lectTotal, escPend, escTotal);
        }
    }
}

void escribirLog(const string idFichero, const int numTuplas, const int lectPend, const int lectTotal, const int escPend, const int escTotal) {
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