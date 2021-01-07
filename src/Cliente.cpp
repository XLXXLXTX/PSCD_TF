#include <iostream>
#include <thread>
#include <string>
#include "../src/monitorID/monitorID.hpp"
#include "../libs/Tupla/Tupla.hpp"
#include "../libs/Linda_Driver/LindaDriver.hpp"

using namespace std;

const int PRECIO_MAX     = 10;  // Precio máximo para la creación del precio
const int PRECIO_MIN     = 5;   // Precio minimo para la creación del precio
const int MAX_TAMANIO_T  = 4;   // Tamaño maximo de una tupla tipo oferta: [ID_Oferta,C_Origen,C_Destino,Precio]
const int N_PUBLICADORES = 5;   // Número de procesos publicadores
const int N_BUSCADORES_S = 10;  // Número de procesos buscadores simples
const int N_BUSCADORES_C = 5;   // Número de procesos buscadores combinados
const int MAX_BUSQUEDAS  = 10;  // Número de veces que se repite el proceso buscador simple

const string ciudades[] = {
    "HUESCA","ZARAGOZA","TERUEL","LA RIOJA","NAVARRA","BILBAO","LLEIDA","BARCELONA","MADRID","CANTABRIA"
};

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación
// Post: Lanza "N_PUBLICADORES" procesos publicadores, 
//             "N_BUSCADORES_S" procesos buscadores simples, 
//             "N_BUSCADORES_C" procesos buscadores combinados.
void controlThreads(const string IP, const int PORT);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación,
//       monitor para llevar la cuenta correcta del id de tupla publicada.
// Post: Publican 1 oferta
// Coms: Lanza ofertas con...
//          Origen:  ciudades[rand(0-9)]
//          Destino: ciudades[(rand(0-9)+1) % 9] -> justo la ciudad siguiente
void publicadores(const string IP, const int PORT, monitorID& monitor);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación.
// Post: Escriben por pantalla la informacion de rand(1-3) tuplas consecutivas,
//       se realiza 10 veces.
void buscarOferta(const string IP, const int PORT);

// Pre:  IP y PORT del servidor despliegue para la creación de comunicación.
// Post: Busca una oferta de viajes con destino intermedio entre las publicadas,
//       imprime por pantalla la información de ambos viajes.
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
        string DESPLIEGUE_ADDRESS = args[1];            // IP   servidor despliegue
        int    DESPLIEGUE_PORT    = atoi(args[2]);      // PORT servidor despliegue

        controlThreads(DESPLIEGUE_ADDRESS, DESPLIEGUE_PORT);
    }

    return error_code;
}

void controlThreads(const string IP, const int PORT) {
    // Creacion de monitor para controlar los IDs
    monitorID monitor();

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

void publicadores(const string IP, const int PORT, monitorID& monitor) {
    int indice_ciudad = rand() % 9;
    int precio = PRECIO_MIN + rand() % (PRECIO_MAX + 1 - PRECIO_MIN);

    string origen  = ciudades[indice_ciudad];           // ciudad origen
    string destino = ciudades[indice_ciudad + 1 % 9];   // ciudad destino

    // Informacion de la oferta
    string mensaje = '[' + to_string(monitor.getId() + ',' + origen + ',' + destino + ',' + to_string(precio) + ']';

    Tupla t(MAX_TAMANIO_T);
    t.from_string(mensaje);

    LindaDriver ld(IP, PORT);
    ld.PN(t);
}

void buscarOferta(const string IP, const int PORT) {
    int busquedas;      // Rand(1-3) ofertas consecutivas a buscar
    int id_tupla;       // tupla.get(0)
    string mensaje;     // Datos de una oferta cualquiera
    string mAux;        // Datos de una oferta con "id+i", i: 0..(busquedas-1)

    LindaDriver ld(IP, PORT);   // Crea la comunicación con el linda server adecuado
    Tupla t(MAX_TAMANIO_T);     // Tupla a recibir
    Tupla p(MAX_TAMANIO_T);     // Tupla a enviar
    Tupla pAux(MAX_TAMANIO_T);  // Tupla a enviar

    for (int iteracion = 1; iteracion <= MAX_BUSQUEDAS; iteracion++) {
        busquedas = 1 + rand() % 3;
        mensaje = '[' + "?O" + ',' + "?X" + ',' + "?Y" + ',' + "?Z" + ']';

        p.from_string(mensaje);
        ld.RDN(p,t);  // [1,zgz,bcn,10]
        id_tupla = stoi(t.get(0));

        for (int i = 0; i < busquedas; i++) {
            mAux = '[' + to_string(id_tupla+i) + ',' + "?X" + ',' + "?Y" + ',' + "?Z" + ']';
            pAux.from_string(mAux);
            ld.RDN(pAux,t);
            // 1: zaragoza->barcelona; 15$
            cout << id_tupla + ":" + t.get(1) + "->" + t.get(2) + ';' + t.get(3) + "$ " + "\n";
        }
    }
}

void buscarOfertaCombinada(const string IP, const int PORT) {
    string mensaje1 = '[' + "?O" + ',' + "?X" + ',' + "?Y" + ',' + "?P" + ']';
    string mensaje2 = '[' + "?K" + ',' + "?Y" + ',' + "?Z" + ',' + "?Q" + ']';

    Tupla p1(MAX_TAMANIO_T); // Tupla a enviar
    Tupla p2(MAX_TAMANIO_T); // Tupla a enviar
    p1.from_string(mensaje1);
    p2.from_string(mensaje2);

    Tupla t1(MAX_TAMANIO_T); // Tupla a recibir
    Tupla t2(MAX_TAMANIO_T); // Tupla a recibir

    LindaDriver ld(IP, PORT);
    ld.RDN_2(p1,p2,t1,t2);  // [1,zgz,bcn,10]

    // Ej: 1: zaragoza->teruel; 15$
    //     4: teruel->barcelona; 20$
    cout << t1.get(0) + ":" + t1.get(1) + "->" + t1.get(2) + ';' + t1.get(3) + "$ " + "\n";
    cout << t2.get(0) + ":" + t2.get(1) + "->" + t2.get(2) + ';' + t2.get(3) + "$ " + "\n\n";
}