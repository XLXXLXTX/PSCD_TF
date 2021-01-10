CC = g++

DRIVER_DIR = librerias/LindaDriver
DRIVER	   = ${DRIVER_DIR}/LindaDriver

ANALISIS   = monitorizacion
CLIENT	   = mainLinda
CARGA	   = clienteCarga

TUPLAS_DIR = librerias/Tupla
TUPLAS	   = ${TUPLAS_DIR}/Tupla
SOCKET_DIR = librerias/Socket
SOCKET	   = ${SOCKET_DIR}/Socket

CPPFLAGS   = -I. -O2 -std=c++11 -lsockets -Wall# Flags compilacion
LDFLAGS	   = -pthread # Flags linkado threads

all: ${ANALISIS} ${CLIENT} ${CARGA}
#----------------------------------------------------------------------------
#Para gestionar opciones de compilación según la máquina: hendrix tiene sus manías
#Descomentar la siguiente línea para compilar en hendrix
SOCKETSFLAGS = -lsocket -lnsl
#-----------------------------------------------------------
# DRIVER
# Compilacion
${DRIVER}.o: ${DRIVER}.hpp ${DRIVER}.cpp
	${CC} -c ${CPPFLAGS} ${DRIVER}.cpp -o ${DRIVER}.o

# Linkado
${DRIVER}: ${TUPLAS}.o ${SOCKET}.o ${DRIVER}.o  
	${CC} ${LDFLAGS} ${TUPLAS}.o ${SOCKET}.o ${DRIVER}.o -o ${DRIVER} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# MONITORIZACION: ANALISIS
# Compilacion
${ANALISIS}.o: ${ANALISIS}.cpp
	${CC} -c ${CPPFLAGS} ${ANALISIS}.cpp

# Linkado
${ANALISIS}: ${SOCKET}.o ${ANALISIS}.o  
	${CC} ${LDFLAGS} ${SOCKET}.o ${ANALISIS}.o -o ${ANALISIS} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# CLIENTE
# Compilacion
${CLIENT}.o: ${CLIENT}.cpp
	${CC} -c ${CPPFLAGS} ${CLIENT}.cpp

# Linkado
${CLIENT}: ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${CLIENT}.o  
	${CC} ${LDFLAGS} ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${CLIENT}.o -o ${CLIENT} ${SOCKETSFLAGS}
#-----------------------------------------------------------
# CARGA
# Compilacion
${CARGA}.o: ${CARGA}.cpp
	${CC} -c ${CPPFLAGS} ${CARGA}.cpp

# Linkado
${CARGA}: ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${CARGA}.o  
	${CC} ${LDFLAGS} ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${CARGA}.o -o ${CARGA} ${SOCKETSFLAGS}
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
	$(RM) ${CLIENT} ${CLIENT}.o
	$(RM) ${CARGA} ${CARGA}.o
	$(RM) ${DRIVER} ${DRIVER}.o
	$(RM) ${ANALISIS} ${ANALISIS}.o
