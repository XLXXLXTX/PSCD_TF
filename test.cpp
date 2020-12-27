// Deberá usarse fuera de la carpeta recursos o modificar el makefile(opcion no recomendable)

#include <iostream>
#include <string>
#include <vector>
#include "libs/Tupla/Tupla.hpp"
#include <map>

using namespace std;

int main() {
    
    //Pruebas de pasar un String a Tupla sabiendo el tamanio.
    //----------------------------------------------------------------------
    string test = "[aumento,de,sueldo,carlos,carlos,carlos]";
    Tupla tuple(6);
    tuple.from_string(test);

    for (int i = 0; i < tuple.size(); i++){
        cout << tuple.get(i) << endl;
    }
    
    cout  << endl;
    cout  << tuple.to_string() << endl;
    //----------------------------------------------------------------------
    
    Tupla tuplaPatron("aumento","de","sueldo","?C","?C","?C");

    if(tuple.match(tuplaPatron)){
        cout  << "MATCH OKAY BOOMER" << endl;   
    } else {
        cout  << "MATCH NOT OKAY BOOMER" << endl;
    }

    //----------------------------------------------------------------------
    
    Tupla copia(tuplaPatron);
    cout  << "AAAA " << copia.to_string() << endl;

    return 0;
}