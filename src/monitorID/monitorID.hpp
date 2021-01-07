#ifndef MONITOR_ID_HPP
#define MONITOR_ID_HPP

#include <mutex>

using namespace std;

class monitorID{
private:
	int idP;	//dirección ip    
	mutex mtx;
public:
	// constructores -----------------------------
	monitorID();
	// destructor -----------------------------
	~monitorID();
	// operadores -----------------------------

    // Pre:  True
	// Post: Devuelve el idP actual y lo actualiza.
	int monitorID::getId(const int caso);
};

#endif // MONITOR_ID_HPP