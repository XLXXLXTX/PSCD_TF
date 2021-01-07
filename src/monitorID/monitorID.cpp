#include "miniMonitor.hpp"

monitorID::monitorID() {
    int idP  = 1;
}

monitorID::~monitorID() {}

int monitorID::getId(const int caso) {
    unique_lock<mutex> lck(mtx);
    int idAux = idP;
    idP++;
    return idAux;
}
