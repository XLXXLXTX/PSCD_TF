#include "monitorLinda.hpp"


#include <iostream>

MonitorLinda::MonitorLinda() {};

MonitorLinda::~MonitorLinda() {};

void MonitorLinda::PN(Tupla t) {
    unique_lock<mutex> lck(mtxML);
    espacioTup.push_front(t);
    for (list<Tupla>::iterator it = espacioTup.begin(), end = espacioTup.end(); it != end; ++it) {
        cout << it->to_string() << " lista tuplas \n";
    }
    hayTupla.notify_all();
};

void MonitorLinda::RN(Tupla p, Tupla& t) {
    unique_lock<mutex> lck(mtxML);
    list<Tupla> l;
    bool esta = false;
    do {
        for (typename list<Tupla>::iterator it = espacioTup.begin(), end = espacioTup.end(); it != end && !esta; it++) {
            if ((*it).match(p)) {
                l.push_back(*it);
            }
        }
        if (l.empty()) hayTupla.wait(lck);
    }while(l.empty());

    int aleat = rand() % l.size();
    typename list<Tupla>::iterator it = l.begin();
    advance(it, aleat);
    for (int i = 0; i < t.size(); i++) {
        t.set(i, (*it).get(i));
    }
    bool borrado=false;
    for (it = espacioTup.begin(); it != espacioTup.end() && !borrado; it++) {
        if ((*it).match(t)) {
            it = espacioTup.erase(it);
            borrado=true;
        }
    }
};

void MonitorLinda::RDN(Tupla p, Tupla& t) {
    unique_lock<mutex> lck(mtxML);
    list<Tupla> l;
    bool esta = false;
    do {
        for (typename list<Tupla>::iterator it = espacioTup.begin(), end = espacioTup.end(); it != end && !esta; it++) {
            if ((*it).match(p)) {
                l.push_back(*it);
            }
        }
        if (l.empty()) hayTupla.wait(lck);
    }while(l.empty());

    int aleat = rand() % l.size();
    typename list<Tupla>::iterator it = l.begin();
    advance(it, aleat);
    for (int i = 0; i < t.size(); i++) {
        t.set(i, (*it).get(i));
    }
};

void MonitorLinda::RN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2) {
    unique_lock<mutex> lck(mtxML);
    string var = ""; // Para guardar la posible variable común
    int cont = 0; // Contador de variables comunes
    int vars1[t1.size()] = {0}; // Para guardar las posiciones de las variables comunes de p1
    int vars2[t2.size()] = {0}; // Para guardar las posiciones de las variables comunes de p2
    Tupla temp1(t1.size());
    Tupla temp2(t2.size());
    list<Tupla> l1, l2;
    bool esta = false;
    bool doblematch = false;
    srand(time(nullptr));
    
    // Localiza las variables comunes
    for (int i = 0; i < t1.size(); i++) {
        var = p1.get(i);
        if (var[0] == '?' && isalpha(var[1]) && isupper(var[1])) { // Busca en p1 una variable
            for (int j = 0; j < t2.size(); j++) {
                if (var == p2.get(j) && vars2[j] == 0) {// Busca en p2 todas las variables iguales a la encontrada en p1
                    // Marca las posiciones de la variable común
                    cont++;
                    vars1[i] = cont;
                    vars2[j] = cont;
                }
            }
        }
    } 

    do {
        // Busca todos los match para p1
        do {
            // Buscar match para p1 y si hay, guardar todos en la lista l1 y esta := true
            for (typename list<Tupla>::iterator it = espacioTup.begin(), end = espacioTup.end(); it != end; it++) {
                if ((*it).match(p1)) {
                    l1.push_back(*it); // Añade la tupla a l1
                }
            }
            esta = !l1.empty(); // Comprueba si hay alguna tupla en l1
            if (!esta) hayTupla.wait(lck);
        }while(!esta);
        
        // Prueba todas las combinaciones posibles de doblematch hasta encontrar uno o se acaben las combinaciones
        do {
            // Escoge una tupla aleatoria de l1 y la guarda en temp1
            int aleat = rand() % l1.size();
            typename list<Tupla>::iterator it = l1.begin();
            advance(it, aleat); // Avanza el iterador random posiciones
            for (int i = 0; i < temp1.size(); i++) {
                temp1.set(i, (*it).get(i));
            }
            l1.erase(it); // Borra la tupla de l1
            // Modificar temp2 con p2 y los nuevos valores de las variables comunes
            for (int i = 0; i < t1.size(); i++) {
                temp2.set(i, p2.get(i));
            }
            for (int i = 0; i < t1.size(); i++) {
                if (vars1[i] != 0) {
                    var = temp1.get(i); // Valor de la variable común
                    for (int j = 0; j < t2.size(); j++) {
                        if (vars1[i] == vars2[j]) {
                            temp2.set(j, var); // Pone el valor de la variable común correspondiente
                        }
                    }
                }
            }
            // Buscar match para temp2 y si hay, guardar todos en la lista l2 y doblematch := true
            bool encontradoIgualAt1=false;
            for (typename list<Tupla>::iterator it = espacioTup.begin(), end = espacioTup.end(); it != end; it++) {
                if ((*it).match(temp2)) {
                    if(!encontradoIgualAt1 && temp1.match(*it)) {
                        encontradoIgualAt1= true;
                    }
                    else {
                        l2.push_back(*it); // Añade la tupla a l2
                    }
                }
            }
            doblematch = !l2.empty(); // Comprueba si hay alguna tupla en l2
        }while (!l1.empty() && !doblematch);
        if (!doblematch) hayTupla.wait(lck);
    }while(!doblematch);
    // Asignar tuplas que hacen doblematch a t1 y t2
    for (int i = 0; i < t2.size(); i++) {
        t1.set(i, temp1.get(i));
    }
    int aleat = rand() % l2.size();
    typename list<Tupla>::iterator it = l2.begin();
    advance(it, aleat);
    for (int i = 0; i < t2.size(); i++) {
        t2.set(i, (*it).get(i));
    }

    // Borrar ambas del espacio
    bool buscat1 = true;
    bool buscat2 = true;
    for (typename list<Tupla>::iterator it = espacioTup.begin(); it != espacioTup.end() && (buscat1 || buscat2); it++) {
        if (buscat1 && (*it).match(t1)) {
            it = espacioTup.erase(it);
            it--;
            buscat1 = false;
        }
        else if (buscat2 && (*it).match(t2)) {
            espacioTup.erase(it);
            it = espacioTup.erase(it);
            it--;
            buscat2 = false;
        }
    }
};

void MonitorLinda::RDN_2(Tupla p1, Tupla p2, Tupla& t1, Tupla& t2) {
    unique_lock<mutex> lck(mtxML);
    string var = ""; // Para guardar la posible variable común
    int cont = 0; // Contador de variables comunes
    int vars1[t1.size()] = {0}; // Para guardar las posiciones de las variables comunes de p1
    int vars2[t2.size()] = {0}; // Para guardar las posiciones de las variables comunes de p2
    Tupla temp1(t1.size());
    Tupla temp2(t2.size());
    list<Tupla> l1, l2;
    bool esta = false;
    bool doblematch = false;
    srand(time(nullptr));
    
    // Localiza las variables comunes
    for (int i = 0; i < t1.size(); i++) {
        var = p1.get(i);
        if (var[0] == '?' && isalpha(var[1]) && isupper(var[1])) { // Busca en p1 una variable
            for (int j = 0; j < t2.size(); j++) {
                if (var == p2.get(j) && vars2[j] == 0) {// Busca en p2 todas las variables iguales a la encontrada en p1
                    // Marca las posiciones de la variable común
                    cont++;
                    vars1[i] = cont;
                    vars2[j] = cont;
                }
            }
        }
    } 

    do {
        // Busca todos los match para p1
        do {
            // Buscar match para p1 y si hay, guardar todos en la lista l1 y esta := true
            for (typename list<Tupla>::iterator it = espacioTup.begin(), end = espacioTup.end(); it != end; it++) {
                if ((*it).match(p1)) {
                    l1.push_back(*it); // Añade la tupla a l1
                }
            }
            esta = !l1.empty(); // Comprueba si hay alguna tupla en l1
            if (!esta) hayTupla.wait(lck);
        }while(!esta);
        
        // Prueba todas las combinaciones posibles de doblematch hasta encontrar uno o se acaben las combinaciones
        do {
            // Escoge una tupla aleatoria de l1 y la guarda en temp1
            int aleat = rand() % l1.size();
            typename list<Tupla>::iterator it = l1.begin();
            advance(it, aleat); // Avanza el iterador random posiciones
            for (int i = 0; i < temp1.size(); i++) {
                temp1.set(i, (*it).get(i));
            }
            l1.erase(it); // Borra la tupla de l1
            // Modificar temp2 con p2 y los nuevos valores de las variables comunes
            for (int i = 0; i < t1.size(); i++) {
                temp2.set(i, p2.get(i));
            }
            for (int i = 0; i < t1.size(); i++) {
                if (vars1[i] != 0) {
                    var = temp1.get(i); // Valor de la variable común
                    for (int j = 0; j < t2.size(); j++) {
                        if (vars1[i] == vars2[j]) {
                            temp2.set(j, var); // Pone el valor de la variable común correspondiente
                        }
                    }
                }
            }
            // Buscar match para temp2 y si hay, guardar todos en la lista l2 y doblematch := true
            bool encontradoIgualAt1=false;
            for (typename list<Tupla>::iterator it = espacioTup.begin(), end = espacioTup.end(); it != end; it++) {
                if ((*it).match(temp2)) {
                    if(!encontradoIgualAt1 && temp1.match(*it)) {
                        encontradoIgualAt1= true;
                    }
                    else {
                        l2.push_back(*it); // Añade la tupla a l2
                    }
                }
            }
            doblematch = !l2.empty(); // Comprueba si hay alguna tupla en l2
        }while (!l1.empty() && !doblematch);
        if (!doblematch) hayTupla.wait(lck);
    }while(!doblematch);
    // Asignar tuplas que hacen doblematch a t1 y t2
    for (int i = 0; i < t2.size(); i++) {
        t1.set(i, temp1.get(i));
    }
    int aleat = rand() % l2.size();
    typename list<Tupla>::iterator it = l2.begin();
    advance(it, aleat);
    for (int i = 0; i < t2.size(); i++) {
        t2.set(i, (*it).get(i));
    }
};