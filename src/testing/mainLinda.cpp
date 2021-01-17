//*****************************************************************
// File:   mainLinda.cpp
// Author: PSCD-Unizar
// Date:   septiembre 2020
// Coms:   Esqueleto para prueba del TP-6
//         invocar mediante
//            mainLindaDriver IP_LindaRegisterServer Port_LindaRegisterServer ...
//*****************************************************************

#include "../../libs/LindaDriver/LindaDriver.hpp"
#include "../../libs/Tupla/Tupla.hpp"

using namespace std;

//-----------------------------------------------------
int main(int argc, char* argv[]) {

    //faltan argumentos en la invocación?
    if (argc < 4 ) {
        cerr << "Invocar como:" << endl
             << "   mainLinda <IP_LS> <Port_LS> ..." << endl
             << "      <IP_LS>: IP del servidor Linda" << endl
             << "      <Port_LS>: puerto del servidor Linda" << endl;
        return 1;
    }

    // un driver con los datos pasados en la invocación
    LindaDriver LD(argv[1], argv[2]);
    // Ya ha contactado con el registro, obtenido los datos necesarios de
    // los tres servidores y establecido las conexiones requeridas

    // Varias formas de construir tuplas
    Tupla t1("1","mi casa","árbol");  // 3 elementos
    Tupla t2("1000");
    Tupla t3("aprieta","el","pan","45","34","88");
    Tupla t4("aprieta","fuerte","pan","tt","34","pan");
    // insertar las tuplas en linda: varios PostNote
    LD.PN(t1);
    LD.PN(t2);
    LD.PN(t3);
    LD.PN(t3);
    LD.PN(t3);
    LD.PN(t4);

    //muestra "mi casa" por stdout
    cout << t1.get(1) << endl;
    // t3.to_string() devuelve el string "[aprieta,el,pan,45,34,88]"
    string serial = t3.to_string();
    cout << serial << endl;
    // las componentes de t3 tomarán, respectivamente,
    // los valores "a","b","c","45","34","pan"
    t3.from_string("[a,b,c,45,34,pan]");
    LD.PN(t3);
    // mostrará [a,b,c,45,34,pan] por stdout
    cout << t3.to_string() << endl;

    // Crea una tupla de 3 elementos "".
    // Equivalente a Tupla t5("","","")
    Tupla t5(3);
    t5.set(1, "hola");
    t5.set(2, "Mundo");
    LD.PN(t5);
    // mostrará [,hola,Mundo] por stdout
    cout << t5.to_string() << endl;
    // informará de que tiene 3 elementos
    cout << "t5 tiene " << t5.size() << " elementos" << endl;

    // Un patrón no es otra cosa que una tupla con
    // la posibilidad de contener el comodín "?" en una o más posiciones
    // "?" no puede ser un valor de componente de tupla. Para ello se
    // tendría que representar mediante "??". Creamos dos patrones
    Tupla p1("?X");
    Tupla p2("a","?X","c","?Y","34","?Z");
    // Dos nuevas tuplas, de tamaño 1 y 6, respectivamente
    Tupla res1(1),
          res2(p2.size());
    // ejemplos de RemoveNote
    LD.RN(p1, res1); // res1 tomará él valor que tenía t2
    LD.RN(p2, res2); // res2 tomará el valor que tenía t3
    cout << res1.to_string() << endl; //mostrará [1000]
    cout << res2.to_string() << endl; //mostrará [a,b,c,45,34,pan]

    // ¿Si necesitamos un array de tuplas?
    // Tupla v[2]; // NO permitido: no hay constructor por defecto
    Tupla* v[2];
    v[0] = new Tupla("Juan", "1000");
    v[1] = new Tupla("Luisa", "1000", "enero");
    delete v[0];
    delete v[1];

    Tupla t6("EINA","AOC","DIIS");
    Tupla t7("EINA","PSCD","DIIS");
    LD.PN(t6);
    LD.PN(t7);
    Tupla p3("EINA","?Y","?X");
    Tupla p4("EINA","?Z","?X");

    Tupla t8(3), t9(3);
    LD.RN_2(p3,p4,t8,t9);
    // Podrá haber cargado t8 con ["EINA","AOC","DIIS"] y 
    // t9 con ["EINA","PSCD","DIIS"], o viceversa
    cout << t8.to_string() << endl;
    cout << t9.to_string() << endl;
    LD.PN(t1);
    return 0;
}
