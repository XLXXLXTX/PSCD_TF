//--------------------------------------------------------------------------------------------
// Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
//          Carlos Paesa, Javier Pardos, Héctor Toral
// Date:    Enero 2021
// Coms:    Fichero de implementación de las operaciones con la clase Tupla
// File:    Tupla.cpp
// Encode:  UTF-8
// EOF:     LF
//--------------------------------------------------------------------------------------------

#include "Tupla.hpp"

Tupla::Tupla(int n) {
    data = new vector<string>(n,"");
}

Tupla::Tupla(string s1) {
    data = new vector<string>(1,s1);
}

Tupla::Tupla(string s1, string s2) {
    data = new vector<string>(2,"");
    data->at(0) = s1;
    data->at(1) = s2;
}

Tupla::Tupla(string s1, string s2, string s3) {
    data = new vector<string>(3,"");
    data->at(0) = s1;
    data->at(1) = s2;
    data->at(2) = s3;
}

Tupla::Tupla(string s1, string s2, string s3, string s4) {
    data = new vector<string>(4,"");
    data->at(0) = s1;
    data->at(1) = s2;
    data->at(2) = s3;
    data->at(3) = s4;
}

Tupla::Tupla(string s1, string s2, string s3, string s4, string s5) {
    data = new vector<string>(5,"");
    data->at(0) = s1;
    data->at(1) = s2;
    data->at(2) = s3;
    data->at(3) = s4;
    data->at(4) = s5;
}

Tupla::Tupla(string s1, string s2, string s3, string s4, string s5, string s6) {
    data = new vector<string>(6,"");
    data->at(0) = s1;
    data->at(1) = s2;
    data->at(2) = s3;
    data->at(3) = s4;
    data->at(4) = s5;
    data->at(5) = s6;
}

Tupla::Tupla(const Tupla& t) {
    data = new vector<string>(t.data->size(),"");
    
    for (unsigned int pos = 0; pos < t.data->size(); pos++) {
        data->at(pos) = t.data->at(pos);
    }
}

Tupla::~Tupla(){
    delete data;
}

int Tupla::size() {
    return data->size();
}

string Tupla::to_string() {
    string res = "[";
    for (unsigned int i = 0; i < data->size() - 1; i++) {
        res = res + data->at(i) + ",";
    }
    res = res + data->at(data->size() - 1) + "]";
    return res;
}

void Tupla::from_string(string s) {
    // Se supone que desde el main llamamos al construstor sabiendo el tamaño del string
    assert(s.length() > 2 && s[0] == '[' && s[s.size() - 1] == ']');
    int pos = 0;
    string value = "";

    for (string::iterator it = s.begin() + 1; it != s.end() && pos < size(); ++it) {
        if (*it != LIM_SEPARATOR && *it != LIM_END) { 
            value += *it;
        } else {
            data->at(pos) = value;
            value = "";
            pos++;
        }
    }
}

string Tupla::get(int pos) {
    return data->at(pos);
}

void Tupla::set(int pos, string value) {
    data->at(pos) = value;
}

bool Tupla::match(Tupla& p) {
    bool iguales = p.size() == size();
    
    if (iguales) {
        map<char,string> myDicc;
        map<char,string>::iterator it;

        // Recorre las componentes de las tuplas y mira si hacen match
        for (int pos = 0; pos < p.size() && iguales; pos++) {
            // Mira si Tupla[i] == ?[A-Z]
            // Convierte la variable de la tupla a char para usar el caracter de [A-Z] en el diccionario
            char clave = p.get(pos)[1]; // Guardamos la clave: [A-Z]
            
            if (p.get(pos)[0] == '?' && isalpha(clave) && isupper(clave)) {    
                it = myDicc.find(clave);    // Inicia iterador
                
                if (it != myDicc.end()) {   // Si find([A-Z]):
                    iguales = it->second == get(pos);
                } else {
                    myDicc.insert(pair<char,string>(clave, get(pos)));    // Si la clave NO existe la guarda en myDicc
                }

            } else {
                iguales = p.data->at(pos) == get(pos);
            }
        }
    } 
    return iguales;
}