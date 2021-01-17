#--------------------------------------------------------------------------------------------
# Authors: Pablo Bueno,  Santiago Illa, Luis Palazón,
#          Carlos Paesa, Javier Pardos, Héctor Toral
# Date:    Enero 2021
# Coms:    Compila y crea todos los ejecutables del trabajo final
# File:    Makefile
# Encode:  UTF-8
# EOF:     LF
# Commands:
#	 - make:  make 
#	 - clean: make clean
#--------------------------------------------------------------------------------------------

CC       	 = g++
CPPFLAGS 	 = -I. -O2 -std=c++11 -lsockets -Wall # Flags compilacion
LDFLAGS  	 = -pthread 						  # Flags linkado threads
# SOCKETSFLAGS = -lsocket -lnsl					  # Para compilar en Hendrix

# Direcciones
LIBS_DIR 	   = ../../libs
SRC_DIR  	   = ../../src
TEST_DIR  	   = $(SRC_DIR)/testing

# Librerias
DRIVER_DIR	= $(LIBS_DIR)/LindaDriver
SOCKET_DIR	= $(LIBS_DIR)/Socket
TUPLAS_DIR	= $(LIBS_DIR)/Tupla

DRIVER  = ${DRIVER_DIR}/LindaDriver
SOCKET	= ${SOCKET_DIR}/Socket
TUPLAS	= ${TUPLAS_DIR}/Tupla

# Testing
CARGA   = ${TEST_DIR}/clienteCarga
LINDA   = ${TEST_DIR}/mainLinda
PELOSIG = ${TEST_DIR}/PeloSig

all: ${CARGA} ${LINDA} ${PELOSIG}
#===========================================================
# LINKADO...
# LIBRERIAS...
# LINDA DRIVER
${DRIVER}:             ${TUPLAS}.o ${SOCKET}.o ${DRIVER}.o  
	${CC}  ${LDFLAGS}  ${TUPLAS}.o ${SOCKET}.o ${DRIVER}.o -o ${DRIVER} ${SOCKETSFLAGS}
#-------------------------------------------------------
# SOURCE...
# CARGA
${CARGA}:       	 ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${CARGA}.o
	${CC} ${LDFLAGS} ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${CARGA}.o -o ${CARGA} ${SOCKETSFLAGS}
#-------------------------------------------------------
# LINDA
${LINDA}:       	 ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${LINDA}.o
	${CC} ${LDFLAGS} ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${LINDA}.o -o ${LINDA} ${SOCKETSFLAGS}
#-------------------------------------------------------
# PELOSIG
${PELOSIG}:       	 ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${PELOSIG}.o
	${CC} ${LDFLAGS} ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${PELOSIG}.o -o ${PELOSIG} ${SOCKETSFLAGS}
#===========================================================
# COMPILACION...
# LIBRERIAS...
# LINDA DRIVER
${DRIVER}.o: ${DRIVER}.hpp ${DRIVER}.cpp
	${CC} -c ${CPPFLAGS}   ${DRIVER}.cpp -o ${DRIVER}.o
#-------------------------------------------------------
# SOCKETS
${SOCKET}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS}   ${SOCKET}.cpp -o ${SOCKET}.o
#-------------------------------------------------------
# TUPLAS
${TUPLAS}.o: ${TUPLAS}.hpp  ${TUPLAS}.cpp 
	${CC} -c ${CPPFLAGS}	${TUPLAS}.cpp -o ${TUPLAS}.o
#-------------------------------------------------------
# SOURCE...
# CARGA
${CARGA}.o:				 ${CARGA}.cpp 
	${CC} -c ${CPPFLAGS} ${CARGA}.cpp -o ${CARGA}.o
#-------------------------------------------------------
# LINDA
${LINDA}.o:				 ${LINDA}.cpp 
	${CC} -c ${CPPFLAGS} ${LINDA}.cpp -o ${LINDA}.o
#-------------------------------------------------------
# PELOSIG
${PELOSIG}.o:			 ${PELOSIG}.cpp 
	${CC} -c ${CPPFLAGS} ${PELOSIG}.cpp -o ${PELOSIG}.o
#===========================================================
# LIMPIEZA
clean:
	# Librerias
	$(RM) ${DRIVER} ${DRIVER}.o
	$(RM) ${TUPLAS}.o
	$(RM) ${SOCKET}.o
	# Source
		# Testing
		$(RM) ${CARGA} 	 ${CARGA}.o
		$(RM) ${LINDA} 	 ${LINDA}.o
		$(RM) ${PELOSIG} ${PELOSIG}.o
