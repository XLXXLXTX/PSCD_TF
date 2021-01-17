//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de prueba de concurrencia
// File:    PeloSig.cpp
// Encode:  UTF-8
// EOF:     LF
// Compile with make:
//      make:  make
//      clean: make clean
// Run manually:  ./PeloSig <IP_LS> <Port_LS> <N>
//--------------------------------------------------------------------------------------------

#include "../../libs/LindaDriver/LindaDriver.hpp"
#include "../../libs/Tupla/Tupla.hpp"

using namespace std;

//-----------------------------------------------------
int main(int argc, char* argv[]) {

    //faltan argumentos en la invocación?
    if (argc < 4 ) {
        cerr << "Invocar como:" << endl
             << "   PeloSig <IP_LS> <Port_LS> <N>" << endl
             << "      <IP_LS>: IP del servidor Linda" << endl
             << "      <Port_LS>: puerto del servidor Linda" << endl
             << "      <N>: Opcion a elegir [1-5]" << endl;
        return 1;
    }
    LindaDriver LD(argv[1], argv[2]);
    int valor = atoi(argv[3]);
    Tupla t1("","1");
    Tupla t2("","");
    Tupla p("2","?X");

    switch (valor) {
        case 1:
            t1.set(0, "1");
            LD.PN(t1);
            while(true) {
                LD.RN(p, t2);
                cout << "Tupla leída: " << t2.to_string() << endl;
                sleep(1);
                int i = stoi(t2.get(1));
                t1.set(1, to_string(i+1));
                LD.PN(t1);
            }
            break;
        case 2:
            t1.set(0, "2");
            p.set(0, "1");
            while(true) {
                LD.RN(p, t2);
                cout << "Tupla leída: " << t2.to_string() << endl;
                sleep(1);
                int i = stoi(t2.get(1));
                t1.set(1, to_string(i+1));
                LD.PN(t1);
            }
            break;
        case 3:
            p.set(0, "?Y");
            LD.RN(p, t2);
            cout << "Tupla borrada: " << t2.to_string() << endl;
            break;
        case 4:
            t1.set(0, "1");
            LD.PN(t1);
            break;
        case 5:
            t1.set(0, "2");
            LD.PN(t1);
            break;
    }

    return 0;
}

