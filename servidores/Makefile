CC=g++

MONITOR_DIR=../librerias/MonitorLinda
MONITOR=${MONITOR_DIR}/MonitorLinda

DESPLIEGUE=despliegue
ANALISIS=monitorizacion
KILLER=finalizador

SERVER1=serverLinda1
SERVER2=serverLinda2
SERVER3=serverLinda3

TUPLAS_DIR=../librerias/Tupla
TUPLAS=${TUPLAS_DIR}/Tupla
SOCKET_DIR=../librerias/Socket
SOCKET=${SOCKET_DIR}/Socket

CPPFLAGS=-I. -O2 -std=c++11 -lsockets -Wall# Flags compilacion
LDFLAGS=-pthread # Flags linkado threads

all: ${SERVER1} ${SERVER2} ${SERVER3} ${DESPLIEGUE} ${KILLER}
#----------------------------------------------------------------------------
#Para gestionar opciones de compilación según la máquina: hendrix tiene sus manías
#Descomentar la siguiente línea para compilar en hendrix
SOCKETSFLAGS=-lsocket -lnsl
#-----------------------------------------------------------
# MONITOR
# Compilacion
${MONITOR}.o: ${MONITOR}.hpp ${MONITOR}.cpp
	${CC} -c ${CPPFLAGS} ${MONITOR}.cpp -o ${MONITOR}.o
${MONITOR}: ${TUPLAS}.o ${SOCKET}.o ${MONITOR}.o  
	${CC} ${LDFLAGS} ${TUPLAS}.o ${SOCKET}.o ${MONITOR}.o -o ${MONITOR} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# DESPLIEGUE
# Compilacion
${DESPLIEGUE}.o: ${DESPLIEGUE}.cpp
	${CC} -c ${CPPFLAGS} ${DESPLIEGUE}.cpp

# Linkado
${DESPLIEGUE}: ${SOCKET}.o ${DESPLIEGUE}.o  
	${CC} ${LDFLAGS} ${SOCKET}.o ${DESPLIEGUE}.o -o ${DESPLIEGUE} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# KILLER: 
# Compilacion
${KILLER}.o: ${KILLER}.cpp
	${CC} -c ${CPPFLAGS} ${KILLER}.cpp

# Linkado
${KILLER}: ${SOCKET}.o ${KILLER}.o  
	${CC} ${LDFLAGS} ${SOCKET}.o ${KILLER}.o -o ${KILLER} ${SOCKETSFLAGS}

#-----------------------------------------------------------

#-----------------------------------------------------------
# SERVER1
# Compilacion
${SERVER1}.o: ${SERVER1}.cpp 
	${CC} -c ${CPPFLAGS} ${SERVER1}.cpp

# Linkado
${SERVER1}: ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER1}.o
	${CC} ${LDFLAGS} ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER1}.o -o ${SERVER1} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# SERVER2
# Compilacion
${SERVER2}.o: ${SERVER2}.cpp 
	${CC} -c ${CPPFLAGS} ${SERVER2}.cpp

# Linkado
${SERVER2}: ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER2}.o
	${CC} ${LDFLAGS} ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER2}.o -o ${SERVER2} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# SERVER3
# Compilacion
${SERVER3}.o: ${SERVER3}.cpp 
	${CC} -c ${CPPFLAGS} ${SERVER3}.cpp

# Linkado
${SERVER3}: ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER3}.o
	${CC} ${LDFLAGS} ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER3}.o -o ${SERVER3} ${SOCKETSFLAGS}
#-----------------------------------------------------------


#-----------------------------------------------------------
# SOCKETS
# Compilacion libreria de Sockets
${SOCKET}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS} ${SOCKET}.cpp -o ${SOCKET}.o
#-----------------------------------------------------------
# TUPLAS
# Compilacion
${TUPLAS}.o: ${TUPLAS}.hpp  ${TUPLAS}.cpp 
	${CC} -c ${CPPFLAGS} ${TUPLAS}.cpp -o ${TUPLAS}.o
#-----------------------------------------------------------

#-----------------------------------------------------------	
# LIMPIEZA
clean:
	$(RM) ${SOCKET}.o
	$(RM) ${TUPLAS}.o
	$(RM) ${KILLER} ${KILLER}.o
	$(RM) ${SERVER1} ${SERVER1}.o
	$(RM) ${SERVER2} ${SERVER2}.o
	$(RM) ${SERVER3} ${SERVER3}.o
	$(RM) ${MONITOR} ${MONITOR}.o
	$(RM) ${DESPLIEGUE} ${DESPLIEGUE}.o
