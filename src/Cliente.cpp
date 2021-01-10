#include <iostream>
#include <thread>
#include <string>
#include "../libs/Tupla/Tupla.hpp"
#include "../libs/Linda_Driver/LindaDriver.hpp"

using namespace std;

const int PRECIO_MAX     = 10;  // Precio máximo para la creación del precio
const int PRECIO_MIN     = 5;   // Precio minimo para la creación del precio

const int TAM_T_1  = 1;   // Tamaño maximo de una tupla tipo id:            [ID]
const int TAM_T_2  = 2;   // Tamaño maximo de una tupla tipo permiso:       [CAMP_1,CAMP_2]
const int TAM_T_3  = 3;   // Tamaño maximo de una tupla tipo fin servicio:  [END,NUM_FINALIZADOS,SERVICE]
const int TAM_T_4  = 4;   // Tamaño maximo de una tupla tipo oferta:        [ID_Oferta,C_Origen,C_Destino,Precio]

const int MAX_PERMISOS_BUSQUEDA   = 8;   // Máximo número de procesos buscadores que pueden estar buscando a la vez
const int ITER_BUSCADOR_SIMPL     = 10;  // Número de veces que se ejecuta un proceso Buscador Simple antes de acabar
const int ITER_BUSCADOR_COMBINADO = 5;   // Número de veces que se ejecuta un proceso Buscador Combinado antes de acabar

const int N_PUBLICADORES = 5;   // Número de procesos publicadores
const int N_BUSCADORES_S = 10;  // Número de procesos buscadores simples
const int N_BUSCADORES_C = 5;   // Número de procesos buscadores combinados

const int MAX_BS  = 10;         // Número de veces que se repite el proceso buscador simple
const int MAX_BC  = 5;          // Número de veces que se repite el proceso buscador simple

const int END_OF_SERVIE = 15;   // Número de repeticiones totales entre BuscSimple y BuscCombinado

// Ciudades empleadas en el ejercicio
const string ciudades[] = {
    "HUESCA","ZARAGOZA","TERUEL","LA RIOJA","NAVARRA","BILBAO","LLEIDA","BARCELONA","MADRID","CANTABRIA"
};

// Pre:  True
// Post: Inicializa la sincronización
//          -> ID's
//          -> Permiso de busqueda
//          -> End Of Service
void init(const string IP, const int PORT);

// Pre:  Recibe una tupla de tipo oferta
// Post: Escribe por pantalla la información de esta con el formato:
// Form: ID: i, ORIGEN->DESTINO, X$
void infoTupla(const Tupla oferta1);

// Pre:  Recibe dos tupla de tipo oferta
// Post: Escribe por pantalla la información de estas con el formato:
// Form: ID: i, ORIGEN->DESTINO, X$
void infoTupla(const Tupla oferta1, const Tupla oferta2);

// Pre:  Recibe el ID de una tupla
// Post: Devuelve una tupla de tipo oferta mediante el ID dado,
//       una ciudad origen y destino distintas y a su vez aleatorias
//       entre todas las disponibles y un precio aleatorio entre
//       PRECIO_MIN y PRECIO_MAX
Tupla generarOferta(const string ID);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación
// Post: Lanza <N_PUBLICADORES> procesos publicadores, 
//             <N_BUSCADORES_S> procesos buscadores simples, 
//             <N_BUSCADORES_C> procesos buscadores combinados.
void controlProcesos(const string IP, const int PORT);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación,
//       monitor para llevar la cuenta correcta del id de tupla publicada.
// Post: Publican ofertas hasta que todos los buscadores hayan podido completar
//       sus busquedas
// Coms: Lanza ofertas con...
//          Origen:  ciudades[rand(0-9)]
//          Destino: ciudades[rand(0-9)]
// Nota: Las ciudades origen y destino nunca serán iguales
void publicadores(const string IP, const int PORT)

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación.
// Post: Escriben por pantalla la informacion de <rand(1-3)> tuplas consecutivas,
//       se realiza 10 veces.
void buscarOferta(const string IP, const int PORT);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación.
// Post: Busca una oferta de viajes con una parada intermedia entre las publicadas,
//       imprime por pantalla la información de ambos viajes.
// Ejem: LA RIOJA  -> NAVARRA   |   Ejemplo = 1 oferta con parada entre ciudades
//       NAVARRA   -> BILBAO    |
void buscarOfertaCombinada(const string IP, const int PORT);

int main(int numArg, char* args[]) {
    int error_code;

    if (numArg < 3) {           // Falta de parametros
        error_code = -2;
        cerr << "ERROR: Falta de parametros.\n" <<
                "       ./Cliente <IP> <PORT>\n";
    } else if (numArg > 3) {    // Exceso de parametros
        error_code = 2;
        cerr << "ERROR: Exceso de parametros.\n" <<
                "       ./Cliente <IP> <PORT>\n";
    } else {
        // Datos servidor despliegue
        const string DESPLIEGUE_ADDRESS = args[1];            // IP   servidor despliegue
        cosnt int    DESPLIEGUE_PORT    = atoi(args[2]);      // PORT servidor despliegue

        init(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);
        controlProcesos(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);
    }

    return error_code;
}

void init(const string IP, const int PORT) {
    LindaDriver ld(IP, PORT);
    // Tupla control de ID's
    const Tupla ID("1");
    ld.PN(ID);
    
    // Tupla control de Permisos
    Tupla permiso("permiso","permiso");
    for (int i = 1; i <= MAX_PERMISOS_BUSQUEDA; i++) ld.PN(permiso);    // Publicamos 8 permisos

    // Tupla control de cuantos han acabado
    Tupla end_service("0","0","0");
    ld.PN(end_service);
}

void infoTupla(const Tupla oferta1) {
    cout << "ID: " + oferta1.get(0) + ", " + oferta1.get(1) + "->"+ oferta1.get(2) + ", "+  oferta1.get(3) + "$\n";
}

void infoTupla(const Tupla oferta1, const Tupla oferta2) {
    cout << "ID: " + oferta1.get(0) + ", " + oferta1.get(1) + "->"+ oferta1.get(2) + ", "+  oferta1.get(3) + "$\n";
    cout << "ID: " + oferta2.get(0) + ", " + oferta2.get(1) + "->"+ oferta2.get(2) + ", "+  oferta2.get(3) + "$\n";
}

Tupla generarOferta(const string ID) {
    // Selección de las ciudades de forma que ambas ciudades (ORIGEN, DESTINO) sean distintas
    int x = rand() % 10;    // Genera un numero entre 0 - 9
    int y;
    do {
        y = rand() % 10;
    } while(x == y);

    // Creación del precio de la oferta [PRECIO_MIN, PRECIO_MAX]
    int PRECIO = PRECIO_MIN + rand() % (PRECIO_MAX + 1 - PRECIO_MIN);

    // Creamos la tupla y la devolvemos
    Tupla oferta(ID, ciudades[x], ciudades[y], PRECIO);
    return oferta;
}

void controlProcesos(const string IP, const int PORT) {
    // Creacion de threads <publicadores>
    thread Publicadores[N_PUBLICADORES];
    for (int id = 0; id < N_PUBLICADORES; id++) {
        Publicadores[id] = thread(&publicadores, IP, PORT, monitor);
    }
    // Creacion de threads <Buscador simple>
    thread BuscadorSimple[N_PUBLICADORES];
    for (int id = 0; id < N_PUBLICADORES; id++) {
        BuscadorSimple[id] = thread();
    }
    // Creacion de threads <buscador combinado>
    thread BuscadorCombinado[N_PUBLICADORES];
    for (int id = 0; id < N_PUBLICADORES; id++) {
        BuscadorCombinado[id] = thread();
    }

    // Stop threads <publicadores>
    for (int id = 0; id < N_PUBLICADORES; id++) {
        Publicadores[id].join();
    }
    // Stop threads <Buscador simple>
    for (int id = 0; id < N_PUBLICADORES; id++) {
        BuscadorSimple[id].join();
    }
    // Stop threads <buscador combinado>
    for (int id = 0; id < N_PUBLICADORES; id++) {
        BuscadorCombinado[id].join();
    }
}

void publicadores(const string IP, const int PORT) {
    LindaDriver ld(IP, PORT);
    const Tupla id("?X");           // Tupla patrón que solicita ID actual
    Tupla t(TAM_T_1);               // Tupla que contiene el ID actual y posterior mente lo actualizará
    Tupla oferta(MAX_TAMANIO_O);    // Tupla oferta

    int ID; // ID empiezan en 1...N

    // Tuplas para comprobar el "end of service"
    Tupla end_service_VO("?X","?X","?X");   // Patrón para coger cuantos prcesos se han acabado hasta el momento
    Tupla T_end(TAM_T_3);                   // Tupla donde guardamos el número de procesos finalizados

    while (true) {
        RDN(end_service_VO,T_end);
        if (stoi(T_end.get(0)) != END_OF_SERVIE) {
            ld.RN(id,t);                // Solicitud del ID actual
            ID = stoi(t.get(0));        // Guardamos el ID actual en la var_local: "ID"
            oferta = generarOferta(ID); // Devuelve una tupla con el formato: [ID,ORIGEN,DESTINO,PRECIO]
            ld.PN(oferta);              // Publica la Tupla oferta
            t(to_string(ID+1));         // Actualiza el ID para la siguiente publicación
            ld.PN(t);                   // Postea el nuevo ID
            this_thread::sleep_for(chrono::seconds(1));
        } else {
            cout << "END OF SERVICE\n";
        }
    }
}

void buscarOferta(const string IP, const int PORT) {
    LindaDriver ld(IP, PORT);

    // Tuplas para pedir y devolver el permiso de busqueda
    const Tupla permE("?X","?Y");
    Tupla permR(TAM_T_2);
    // Tuplas para pedir y mostrar las ofertas
    const Tupla patron("?W","?X","?Y","?Z");
    Tupla ofertaE(TAM_T_4);
    Tupla ofertaR(TAM_T_4);
    // Tuplas para obtener el ID MAX actual
    const Tupla idE("?X");
    Tupla idR(TAM_T_1);

    const int MAX_ID;   // Máximo número de ofertas que hay en el espacio de tuplas
    int busquedas;      // Número de busquedas que realizara el sistema en una vuelta
    int ID;             // ID  de una tupla aleatoria que permite escribir consecutivamente
    string ofer;        // Formato para buscar tupla con los ID consecutivos

    for (int i = 1; i <= ITER_BUSCADOR_SIMPLE; i++) {
        busquedas = 1 + rand() % 3; // Número aleatorio entre 1 y 3
        ld.RN(permE, permR);        // Comprueba si se puede acceder al servidor linda para realizar una busqueda
        ld.RDN(patron, ofertaR);    // Busca una oferta cualquiera dentro del espacio Tuplas

        ld.RDN(idE, idR);           // Solicitud de MAX_ID
        MAX_ID = idR.get(0);        // ID MAX del espacio tuplas. Indica el dominio de tuplas: [1..MAX_ID]

        // ID es el ID de una tupla cualquiera del espacio Tuplas
        if (MAX_ID <= busquedas) ID = 1;
        else ID = 1 + rand() % (MAX_ID - busquedas);    // ID de una tupla cualquiera del espacio Tuplas

        for (int i = 0; i < busquedas; i++) {
            ofer = '['+ to_string(ID) + ',' + "?X" + ',' + "?Y" + ',' + "?Z" +']';
            ofertaE.from_string(ofer);
            ld.RDN(ofertaE, ofertaR);   // integer id = t.get(0) | coge el id de la tupla leida
            infoTupla(ofertaR);         // Muestra la información de la Tupla con ID = id
        }
        ld.PN(permR);                   // Devuelve el permiso que permite el acceso al espacio de Tuplas
    }

    // Aviso de que el proceso ha acabado para llevar la cuenta de cuando apagar el publicador
    Tupla end_service_VO("?X","?X","?X");   // Patrón para coger cuantos prcesos se han acabado hasta el momento
    Tupla T_end(TAM_T_3);                   // Tupla donde guardamos el número de procesos finalizados
    Tupla end_service_VN(TAM_T_3);          // Actualiza los procesos finalizados

    ld.RN(end_service,T_end);   // Recojo el numero de procesos acabados en end
    int end = T_end.get(0);     // Obtengo el valor numérico del número de procesos acabados mediante t.get(x)

    // Actualizamos los procesos acabados
    string actualizacion_end = '['+ to_string(end+1) + ',' + to_string(end+1) + ',' + to_string(end+1) +']';
    end_service_mas_1.from_string(actualizacion_end);

    // Subimos la actualización
    ld.PN(end_service_mas_1);
}

void buscarOfertaCombinada(const string IP, const int PORT) {
    LindaDriver ld(IP, PORT);

    // Tuplas para pedir y devolver el permiso de busqueda
    const Tupla permE("?X","?Y");
    Tupla permR(TAM_T_2);
    // Tuplas para pedir y mostrar las ofertas
    Tupla patron1(TAM_T_4);
    Tupla patron2(TAM_T_4);
    Tupla ofer1(TAM_T_4);
    Tupla ofer2(TAM_T_4);

    int x;          // indice para escoger la ciudad de origen
    int y;          // Indice para escoger la ciudad de destino
    string ptrn1;   // Formato para el patrón con ciudad origen 
    string ptrn2;   // Formato para el patrón con ciudad destino 

    for (int i = 1; i <= ITER_BUSCADOR_COMBINADO; i++) {
        ld.RN(permE, permR);    // Comprueba si se puede acceder al servidor linda para realizar una busqueda

        // Selección de ciudades
        x = rand() % 10;
        do {
            y = rand() % 10;
        } while (x == y);
        // Creación de Tuplas
        ptrn1 = '['+ "?A" + ',' + ciudades[x] + ',' + "?B" + ',' + "?C" +']';
        ptrn2 = '['+ "?X" + ',' + "?B" + ',' + ciudades[y] + ',' + "?Z" +']';
        patron1.from_string(ptrn1);
        patron2.from_string(ptrn2);
        // Petición de las ofertas que cumplen la transitividad de origen->parada->destino
        ld.RdN2(patron1, patron2, ofer1, ofer2);

        infoTupla(ofer1, ofer2);    // Muestra la informacion de 2 ofertas
        ld.PN(permR);               // Devuelve el permiso que garantiza el acceso al espacio de Tuplas
    }

    // Aviso de que el proceso ha acabado para llevar la cuenta de cuando apagar el publicador
    Tupla end_service_VO("?X","?X","?X");   // Patrón para coger cuantos prcesos se han acabado hasta el momento
    Tupla T_end(TAM_T_3);                   // Tupla donde guardamos el número de procesos finalizados
    Tupla end_service_VN(TAM_T_3);          // Actualiza los procesos finalizados

    ld.RN(end_service,T_end);   // Recojo el número de procesos acabados en end
    int end = T_end.get(0);     // Obtengo el valor numérico del número de procesos acabados mediante t.get(x)

    // Actualizamos los procesos acabados
    string actualizacion_end = '['+ to_string(end+1) + ',' + to_string(end+1) + ',' + to_string(end+1) +']';
    end_service_mas_1.from_string(actualizacion_end);
    
    // Subimos la actualización
    ld.PN(end_service_mas_1);
}