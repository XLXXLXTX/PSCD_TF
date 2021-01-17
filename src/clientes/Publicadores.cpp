//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de los clientes Publicadores del ejercicio, los cuales publican tuplas dentro 
//          dentro del sistema Linda.
// File:    Publicadores.cpp
// Encode:  UTF-8
// EOF:     LF
// Compile with make:
//      make:  make -f Makefile_Publicadores
//      clean: make -f Makefile_Publicadores clean
// Run with bash: ./runPublicadores.bash
// Run manually:  ./Publicadores <IP_DESPLIEGUE> <PORT_DESPLIEGUE>
//--------------------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <time.h>
#include <stdlib.h>
#include "../../libs/Tupla/Tupla.hpp"
#include "../../libs/LindaDriver/LindaDriver.hpp"

using namespace std;

const int PRECIO_MAX = 50;  // Precio máximo para la creación del precio
const int PRECIO_MIN = 20;  // Precio mínimo para la creación del precio
const int X_COM   = 10;     // Cada X_COM se publica una oferta con el id de la anterior para generar viajes con parada
const int TAM_T_1 = 1;      // Tamaño máximo de una tupla tipo id:            [ID]
const int TAM_T_2 = 2;      // Tamaño máximo de una tupla tipo permiso:       [CAMP_1,CAMP_2]
const int TAM_T_3 = 3;      // Tamaño máximo de una tupla tipo fin servicio:  [END,NUM_FINALIZADOS,SERVICE]
const int TAM_T_4 = 4;      // Tamaño máximo de una tupla tipo oferta:        [ID_Oferta,C_Origen,C_Destino,Precio]
const int MAX_PERMISOS_BUSQUEDA = 8;   // Máximo número de procesos buscadores que pueden estar buscando a la vez
const int N_PUBLICADORES        = 5;   // Número de procesos publicadores
const int END_OF_SERVICE        = 15;  // Número de repeticiones totales entre Buscadores simple y Buscadores combinado
const int TIEMPO_PUBLICACION    = 5;   // Segundos a esperar para volver a publicar

// Ciudades empleadas en el ejercicio
const string CIUDADES[] = {"HUESCA","ZARAGOZA","TERUEL","LOGROÑO","PAMPLONA","BILBAO","LLEIDA","BARCELONA","MADRID","SANTANDER"};

// Pre:  True
// Post: Inicializa la sincronización
//          -> ID's
//          -> Permiso de busqueda
//          -> End Of Service
void init(const string IP, const string PORT);

// Pre:  Recibe el ID de una tupla y un dato entero anterior
// Post: Devuelve una tupla de tipo oferta mediante el ID dado,
//       una ciudad origen y destino distintas y a su vez aleatorias
//       entre todas las disponibles y un precio aleatorio entre
//       PRECIO_MIN y PRECIO_MAX
void generarOferta(const string ID, Tupla& tup, int& anterior);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación
// Post: Lanza <N_PUBLICADORES> procesos publicadores
void controlProcesos(const string IP, const string PORT);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación,
//       monitor para llevar la cuenta correcta del id de tupla publicada.
// Post: Publican ofertas hasta que todos los buscadores hayan podido completar
//       sus busquedas
// Coms: Lanza ofertas con...
//          Origen:  CIUDADES[rand(0-9)]
//          Destino: CIUDADES[rand(0-9)]
// Nota: Las ciudades origen y destino nunca serán iguales
void publicadores(const string IP, const string PORT);

// Pre:  ---
// Post: Escribe por pantalla un mensaje de finalizacion de programa
void mensajeFinalizacion();

int main(int numArg, char* args[]) {
    int error_code = 0;

    if (numArg < 3) {               // Faltan parámetros
        cerr << "\033[1;31mERROR\033[0m: Faltan parámetros.\n       ./Cliente <IP> <PORT>\n";
        error_code = -2;
    } else if (numArg > 3) {        // Exceso de parámetros
        cerr << "\033[1;31mERROR\033[0m: Exceso de parámetros.\n       ./Cliente <IP> <PORT>\n";
        error_code = 2;
    } else {
        // Datos servidor despliegue
        string DESPLIEGUE_ADDRESS = args[1];    // IP   servidor despliegue
        string DESPLIEGUE_PORT = args[2];       // PORT servidor despliegue

        srand(time(NULL));
        init(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);
        controlProcesos(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);
        mensajeFinalizacion();
    }

    return error_code;
}

void init(const string IP, const string PORT) {
    LindaDriver LD(IP, PORT);
    // Tupla control de ID's
    const Tupla ID("1");
    LD.PN(ID);
    
    // Tupla control de Permisos
    Tupla permiso("permiso","permiso");
    for (int i = 1; i <= MAX_PERMISOS_BUSQUEDA; i++) LD.PN(permiso);    // Publicamos 8 permisos busqueda

    // Tupla control de cuantos han acabado
    Tupla end_service("0","0","0");
    LD.PN(end_service);
}

void generarOferta(const string ID, Tupla& tup, int& anterior) {
    // Selección de las ciudades de forma que ambas ciudades (ORIGEN, DESTINO) sean distintas
    int x = rand() % 10;        // Genera un numero entre 0 - 9
    int y;
    
    if (stoi(ID) % X_COM == 0) x = anterior;
    do {
        y = rand() % 10;
    } while(x == y);
    anterior = y;

    // Creación del precio de la oferta [PRECIO_MIN, PRECIO_MAX]
    int precio = PRECIO_MIN + rand() % (PRECIO_MAX + 1 - PRECIO_MIN);

    // Creación de la tupla
    tup.set(0,ID);
    tup.set(1, CIUDADES[x]);
    tup.set(2,CIUDADES[y]);
    tup.set(3,to_string(precio));
}

void controlProcesos(const string IP, const string PORT) {
    // Creación de procesos <publicadores>
    thread Publicadores[N_PUBLICADORES];
    for (int id = 0; id < N_PUBLICADORES; id++) {
        Publicadores[id] = thread(&publicadores, IP, PORT);
    }

    // Stop procesos <publicadores>
    for (int id = 0; id < N_PUBLICADORES; id++) {
        Publicadores[id].join();
    }
}

void publicadores(const string IP, const string PORT) {
    LindaDriver LD(IP, PORT);
    const Tupla id("?X");   // Tupla patrón que solicita ID actual
    Tupla t(TAM_T_1);       // Tupla que contiene el ID actual y posterior mente lo actualizará
    Tupla oferta(TAM_T_4);  // Tupla oferta

    int ID; // ID: 1...N
    int anterior = 0;
    // Tuplas para comprobar el "end of service"
    Tupla end_service_VO("?X","?X","?X");   // Patrón para coger cuantos prcesos se han acabado hasta el momento
    Tupla T_end(TAM_T_3);                   // Tupla donde guardamos el número de procesos finalizados

    // bucle publicadores + visual  
    LD.RDN(end_service_VO,T_end);
    while(stoi(T_end.get(0)) < END_OF_SERVICE) {
        LD.RN(id,t);                // Solicitud del ID actual
        ID = stoi(t.get(0));        // Guardamos el ID actual en la var_local: "ID"
        
        generarOferta(to_string(ID), oferta, anterior); // Devuelve una tupla con el formato: [ID,ORIGEN,DESTINO,PRECIO]
        LD.PN(oferta);              // Publica la Tupla oferta
        Tupla t(to_string(ID+1));   // Actualiza el ID para la siguiente publicación
        LD.PN(t);                   // Postea el nuevo ID
        this_thread::sleep_for(chrono::seconds(TIEMPO_PUBLICACION));
        LD.RDN(end_service_VO,T_end);
    }
}

void mensajeFinalizacion() {
    cout << "\t________ __    __ _______\n";
    cout << "\t|        \\  \\  |  \\       \\ \n";
    cout << "\t| ▓▓▓▓▓▓▓▓ ▓▓\\ | ▓▓ ▓▓▓▓▓▓▓\n";
    cout << "\t| ▓▓__   | ▓▓▓\\| ▓▓ ▓▓  | ▓▓\n";
    cout << "\t| ▓▓  \\  | ▓▓▓▓\\ ▓▓ ▓▓  | ▓▓\n";
    cout << "\t| ▓▓▓▓▓  | ▓▓\\▓▓ ▓▓ ▓▓  | ▓▓\n";
    cout << "\t| ▓▓_____| ▓▓ \\▓▓▓▓ ▓▓__/ ▓▓\n";
    cout << "\t| ▓▓     \\ ▓▓  \\▓▓▓ ▓▓    ▓▓\n";
    cout << "\t\\▓▓▓▓▓▓▓▓\\▓▓   \\▓▓\\▓▓▓▓▓▓▓ \n";
    cout << endl;
    cout << "copyright©\tGRUPO 16 Unizar©\t2020-2021\n";
    cout << "Desarrolladores:\n";
    cout << "\t+ Mat-Inf: Pablo Bueno, Santiago Illa, Luis Palazon, Carlos Paesa\n";
    cout << "\t+ Ing-Inf: Javier Pardos, Hector Toral\n";
}
