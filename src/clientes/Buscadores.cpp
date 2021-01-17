//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de los clientes Buscadores del ejercicio, los cuales buscan tuplas "ofertas" 
//          dentro del sistema linda, las cuales pueden ser ofertas simples u ofertas combinadas.
// File:    Buscadores.cpp
// Encode:  UTF-8
// EOF:     LF
// Compile with make:
//      make:  make -f Makefile_Buscadores
//      clean: make -f Makefile_Buscadores clean
// Run with bash: ./runBuscadores.bash
// Run manually:  ./Buscadores <IP_DESPLIEGUE> <PORT_DESPLIEGUE>
//--------------------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <time.h>
#include <stdlib.h>
#include "../../libs/Tupla/Tupla.hpp"
#include "../../libs/LindaDriver/LindaDriver.hpp"

using namespace std;

const int TAM_T_1 = 1;   // Tamaño máximo de una tupla tipo id:            [ID]
const int TAM_T_2 = 2;   // Tamaño máximo de una tupla tipo permiso:       [CAMP_1,CAMP_2]
const int TAM_T_3 = 3;   // Tamaño máximo de una tupla tipo fin servicio:  [END,NUM_FINALIZADOS,SERVICE]
const int TAM_T_4 = 4;   // Tamaño máximo de una tupla tipo oferta:        [ID_Oferta,C_Origen,C_Destino,Precio]
const int MAX_PERMISOS_BUSQUEDA   = 8;   // Máximo número de procesos buscadores que pueden estar buscando a la vez
const int ITER_BUSCADOR_SIMPLE    = 10;  // Número de veces que se ejecuta un proceso Buscador Simple antes de acabar
const int ITER_BUSCADOR_COMBINADO = 5;   // Número de veces que se ejecuta un proceso Buscador Combinado antes de acabar
const int N_BUSCADORES_S = 10;  // Número de procesos buscadores simples
const int N_BUSCADORES_C = 5;   // Número de procesos buscadores combinados
const int MAX_BS = 10;          // Número de veces que se repite el proceso buscador simple
const int MAX_BC = 5;           // Número de veces que se repite el proceso buscador combinado

// Pre:  Recibe una tupla de tipo oferta
// Post: Escribe por pantalla la información de esta con el formato:
// Form: XX.Viaje: ORIGEN->DESTINO, X$
void infoTupla(Tupla oferta1, const int PID);

// Pre:  Recibe dos tupla de tipo oferta
// Post: Escribe por pantalla la información de estas con el formato:
// Form: XX.Viaje: ORIGEN->DESTINO, X$
//                 ORIGEN->DESTINO, X$
void infoTupla(Tupla oferta1, Tupla oferta2, const int PID);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación
// Post: Lanza <N_BUSCADORES_S> procesos buscadores simples, 
//             <N_BUSCADORES_C> procesos buscadores combinados.
void controlProcesos(const string IP, const string PORT);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación.
// Post: Escriben por pantalla la información de <rand(1-3)> tuplas consecutivas,
//       se realiza 10 veces.
void buscarOferta(const string IP, const string PORT, const int PID);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación.
// Post: Busca una oferta de viajes con una parada intermedia entre las publicadas,
//       imprime por pantalla la información de ambos viajes.
// Ejem: LA RIOJA  -> NAVARRA 5$   |   Ejemplo = 1 oferta con parada entre ciudades
//       NAVARRA   -> BILBAO  10$  |
void buscarOfertaCombinada(const string IP, const string PORT, const int PID);

int main(int numArg, char* args[]) {
    int error_code = 0;

    if (numArg < 3) {           // Falta de parámetros
        cerr << "\033[1;31mERROR\033[0m: Faltan parámetros.\n       ./Cliente <IP> <PORT>\n";
        error_code = -2;
    } else if (numArg > 3) {    // Exceso de parámetros
        cerr << "\033[1;31mERROR\033[0m: Exceso de parámetros.\n       ./Cliente <IP> <PORT>\n";
        error_code = 2;
    } else {
        // Datos servidor despliegue
        string DESPLIEGUE_ADDRESS = args[1];    // IP   servidor despliegue
        string DESPLIEGUE_PORT    = args[2];    // PORT servidor despliegue

        srand(time(NULL));
        controlProcesos(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);
    }

    return error_code;
}

// 0 normal, 1 negrita, 4 subrayado
// 35 violeta, 36 cyan, 37 blanco
void infoTupla(Tupla oferta1, const int PID) {
    string info = to_string(PID) + "." + "\033[1;37mViaje:\033[0m\t" + oferta1.get(1) + "->" + oferta1.get(2) + ", " + oferta1.get(3) + "$\n";
    cout << info;
}

void infoTupla(Tupla oferta1, Tupla oferta2, const int PID) {
    string info = to_string(PID) + "." + "\033[1;35mViaje:\033[0m\t" + oferta1.get(1) + "->" + oferta1.get(2) + ", " + oferta1.get(3) + "$\n" + "\t\t" + oferta2.get(1) + "->" + oferta2.get(2) + ", " +  oferta2.get(3) + "$\n" + "\t\t" + "\033[1;37mTOTAL:\033[0m\t" + to_string(stoi(oferta1.get(3)) + stoi(oferta2.get(3))) + "\n";
    cout << info;
}

void controlProcesos(const string IP, const string PORT) {
    // Creación de threads <Buscador simple>
    thread BuscadorSimple[N_BUSCADORES_S];
    for (int id = 0; id < N_BUSCADORES_S; id++) {
        BuscadorSimple[id] = thread(&buscarOferta, IP, PORT, id+1);
    }

    // Creación de threads <buscador combinado>
    thread BuscadorCombinado[N_BUSCADORES_C];
    for (int id = 0; id < N_BUSCADORES_C; id++) {
        BuscadorCombinado[id] = thread(&buscarOfertaCombinada, IP, PORT, id+1+N_BUSCADORES_S);
    }
    
    // Stop threads <Buscador simple>
    for (int id = 0; id < N_BUSCADORES_S; id++) {
        BuscadorSimple[id].join();
    }

    // Stop threads <buscador combinado>
    for (int id = 0; id < N_BUSCADORES_C; id++) {
        BuscadorCombinado[id].join();
    }
}

void buscarOferta(const string IP, const string PORT, const int PID) {
    LindaDriver LD(IP, PORT);

    // Tuplas para pedir y devolver el permiso de busqueda
    Tupla permE("?X","?Y");
    Tupla permR(TAM_T_2);
    // Tuplas para pedir y mostrar las ofertas
    Tupla patron("?W","?X","?Y","?Z");
    Tupla ofertaE(TAM_T_4);
    Tupla ofertaR(TAM_T_4);
    // Tuplas para obtener el ID MAX actual
    Tupla idE("?X");
    Tupla idR(TAM_T_1);

    int MAX_ID;     // Máximo número de ofertas que hay en el espacio de tuplas
    int busquedas;  // Número de busquedas que realizara el sistema en una vuelta
    int ID;         // ID  de una tupla aleatoria que permite escribir consecutivamente
    string ofer;    // Formato para buscar tupla con los ID consecutivos

    for (int i = 1; i <= ITER_BUSCADOR_SIMPLE; i++) {
        busquedas = 1 + rand() % 3; // Número aleatorio entre 1 y 3
        LD.RN(permE, permR);        // Comprueba si se puede acceder al servidor linda para realizar una busqueda
        LD.RDN(idE, idR);           // Solicitud de MAX_ID
        MAX_ID = stoi(idR.get(0));  // ID MAX del espacio tuplas. Indica el dominio de tuplas: [1..MAX_ID]

        // ID es el ID de una tupla cualquiera del espacio Tuplas
        if (MAX_ID <= busquedas) ID = 1;
        else ID = 1 + rand() % (MAX_ID - busquedas);    // ID de una tupla cualquiera del espacio Tuplas

        for (int i = 0; i < busquedas; i++) {
            ofer = "["+ to_string(ID+i) + ",?X,?Y,?Z]";
            ofertaE.from_string(ofer);
            LD.RDN(ofertaE, ofertaR);   // integer id = t.get(0) | coge el id de la tupla leida
            infoTupla(ofertaR, PID);  // Muestra la información de una Tupla
        }
        LD.PN(permR);                   // Devuelve el permiso que permite el acceso al espacio de Tuplas
    }

    // Aviso de que el proceso ha acabado
    Tupla end_service_VO("?X","?X","?X");   // Patrón para coger cuantos prcesos se han acabado hasta el momento
    Tupla T_end(TAM_T_3);                   // Tupla donde guardamos el número de procesos finalizados
    Tupla end_service_VN(TAM_T_3);          // Actualiza los procesos finalizados

    LD.RN(end_service_VO,T_end);    // Recoje el número de procesos acabados en end
    int end = stoi(T_end.get(0));   // Obtiene el valor numérico del número de procesos acabados mediante t.get(x)

    // Actualizamos los procesos acabados
    string actualizacion_end = "["+ to_string(end+1) + "," + to_string(end+1) + "," + to_string(end+1) +"]";
    end_service_VN.from_string(actualizacion_end);

    // Publicamos la actualización
    LD.PN(end_service_VN);
}

void buscarOfertaCombinada(const string IP, const string PORT, const int PID) {
    LindaDriver LD(IP, PORT);

    // Tuplas para pedir y devolver el permiso de busqueda
    Tupla permE("?X","?Y");
    Tupla permR(TAM_T_2);
    // Tuplas para pedir y mostrar las ofertas
    Tupla patron1("?A","?O","?P","?Y");
    Tupla patron2("?B","?P","?D","?Z");
    Tupla ofer1(TAM_T_4);
    Tupla ofer2(TAM_T_4);

    for (int i = 1; i <= ITER_BUSCADOR_COMBINADO; i++) {
        LD.RN(permE, permR);    // Comprueba si se puede acceder al servidor linda para realizar una busqueda

        // Petición de las ofertas que cumplen la transitividad de origen->parada->destino
        LD.RDN_2(patron1, patron2, ofer1, ofer2);

        infoTupla(ofer1, ofer2, PID);    // Muestra la información de 2 ofertas
        LD.PN(permR);               // Devuelve el permiso que garantiza el acceso al espacio de Tuplas
    }

    // Aviso de que el proceso ha acabado
    Tupla end_service_VO("?X","?X","?X");   // Patrón para coger cuantos prcesos se han acabado hasta el momento
    Tupla T_end(TAM_T_3);                   // Tupla donde guardamos el número de procesos finalizados
    Tupla end_service_VN(TAM_T_3);          // Actualiza los procesos finalizados

    LD.RN(end_service_VO,T_end);    // Recoje el número de procesos acabados en end
    int end = stoi(T_end.get(0));   // Obtiene el valor numérico del número de procesos acabados mediante t.get(x)

    // Actualizamos los procesos acabados
    string actualizacion_end = "[" + to_string(end + 1) + "," + to_string(end + 1) + "," + to_string(end + 1) + "]";
    end_service_VN.from_string(actualizacion_end);
    
    // Publicamos la actualización
    LD.PN(end_service_VN); 
}