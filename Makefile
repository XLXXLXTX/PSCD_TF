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
SOCKETSFLAGS = -lsocket -lnsl					  # Para compilar en Hendrix

# Direcciones
LIBS_DIR 	   = libs
SRC_DIR  	   = src
CLIENTES_DIR   = $(SRC_DIR)/clientes
SERVIDORES_DIR = $(SRC_DIR)/servidores

# Librerias
DRIVER_DIR	= $(LIBS_DIR)/LindaDriver
MONITOR_DIR	= $(LIBS_DIR)/MonitorLinda
SOCKET_DIR	= $(LIBS_DIR)/Socket
TUPLAS_DIR	= $(LIBS_DIR)/Tupla

DRIVER  = ${DRIVER_DIR}/LindaDriver
MONITOR	= ${MONITOR_DIR}/MonitorLinda
SOCKET	= ${SOCKET_DIR}/Socket
TUPLAS	= ${TUPLAS_DIR}/Tupla

# Servidores
DESPLIEGUE = ${SERVIDORES_DIR}/Despliegue
SERVER1	   = ${SERVIDORES_DIR}/ServerLinda1
SERVER2	   = ${SERVIDORES_DIR}/ServerLinda2
SERVER3	   = ${SERVIDORES_DIR}/ServerLinda3

# Clientes
BUSCADORES   = ${CLIENTES_DIR}/Buscadores
PUBLICADORES = ${CLIENTES_DIR}/Publicadores

# Finalizacion y ANALIZER
ANALIZER = ${SRC_DIR}/Monitorizacion
KILLER   = ${SRC_DIR}/Finalizador

all: ${DESPLIEGUE} ${SERVER1} ${SERVER2} ${SERVER3} ${BUSCADORES} ${PUBLICADORES} ${ANALIZER} ${KILLER}
#===========================================================
# LINKADO...
# LIBRERIAS...
# LINDA DRIVER
${DRIVER}:             ${TUPLAS}.o ${SOCKET}.o ${DRIVER}.o  
	${CC}  ${LDFLAGS}  ${TUPLAS}.o ${SOCKET}.o ${DRIVER}.o -o ${DRIVER} ${SOCKETSFLAGS}
#-------------------------------------------------------
# MONITOR LINDA
${MONITOR}: 		 ${TUPLAS}.o ${SOCKET}.o ${MONITOR}.o  
	${CC} ${LDFLAGS} ${TUPLAS}.o ${SOCKET}.o ${MONITOR}.o -o ${MONITOR} ${SOCKETSFLAGS}
# SOURCE...
# PUBLICADORES
${PUBLICADORES}:     ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${PUBLICADORES}.o
	${CC} ${LDFLAGS} ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${PUBLICADORES}.o -o ${PUBLICADORES} ${SOCKETSFLAGS}
#---------------------------------------------------
# BUSCADORES
${BUSCADORES}:       ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${BUSCADORES}.o
	${CC} ${LDFLAGS} ${DRIVER}.o ${TUPLAS}.o ${SOCKET}.o ${BUSCADORES}.o -o ${BUSCADORES} ${SOCKETSFLAGS}
#---------------------------------------------------
# DESPLIEGUE	
${DESPLIEGUE}:       ${SOCKET}.o ${DESPLIEGUE}.o  
	${CC} ${LDFLAGS} ${SOCKET}.o ${DESPLIEGUE}.o -o ${DESPLIEGUE} ${SOCKETSFLAGS}
#---------------------------------------------------
# SERVIDORES LINDA
${SERVER1}:			 ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER1}.o
	${CC} ${LDFLAGS} ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER1}.o -o ${SERVER1} ${SOCKETSFLAGS}
#---------------------------------------------------
${SERVER2}: 		 ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER2}.o
	${CC} ${LDFLAGS} ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER2}.o -o ${SERVER2} ${SOCKETSFLAGS}
#---------------------------------------------------
${SERVER3}: 		 ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER3}.o
	${CC} ${LDFLAGS} ${TUPLAS}.o ${MONITOR}.o ${SOCKET}.o ${SERVER3}.o -o ${SERVER3} ${SOCKETSFLAGS}
#-------------------------------------------------------
# FINALIZADOR
${KILLER}:			   ${SOCKET}.o ${KILLER}.o
	${CC}  ${LDFLAGS}  ${SOCKET}.o ${KILLER}.o -o ${KILLER} ${SOCKETSFLAGS}
#-------------------------------------------------------
# ANALIZER
${ANALIZER}:		 ${SOCKET}.o ${ANALIZER}.o  
	${CC} ${LDFLAGS} ${SOCKET}.o ${ANALIZER}.o -o ${ANALIZER} ${SOCKETSFLAGS}
#===========================================================
# COMPILACION...
# LIBRERIAS...
# LINDA DRIVER
${DRIVER}.o: ${DRIVER}.hpp ${DRIVER}.cpp
	${CC} -c ${CPPFLAGS}   ${DRIVER}.cpp -o ${DRIVER}.o
#-------------------------------------------------------
# MONITOR LINDA
${MONITOR}.o: ${MONITOR}.hpp ${MONITOR}.cpp
	${CC} -c  ${CPPFLAGS} 	 ${MONITOR}.cpp -o ${MONITOR}.o
#-------------------------------------------------------
# SOCKETS
${SOCKET}.o: ${SOCKET}.hpp ${SOCKET}.cpp
	${CC} -c ${CPPFLAGS}   ${SOCKET}.cpp -o ${SOCKET}.o
#-------------------------------------------------------
# TUPLAS
${TUPLAS}.o: ${TUPLAS}.hpp  ${TUPLAS}.cpp 
	${CC} -c ${CPPFLAGS}	${TUPLAS}.cpp -o ${TUPLAS}.o
# SOURCE...
# PUBLICADORES
${PUBLICADORES}.o:	 	 ${PUBLICADORES}.cpp
	${CC} -c ${CPPFLAGS} ${PUBLICADORES}.cpp -o ${PUBLICADORES}.o
#---------------------------------------------------
# BUSCADORES
${BUSCADORES}.o:	 	 ${BUSCADORES}.cpp
	${CC} -c ${CPPFLAGS} ${BUSCADORES}.cpp -o ${BUSCADORES}.o
#---------------------------------------------------
# DESPLIEGUE
${DESPLIEGUE}.o:	 	 ${DESPLIEGUE}.cpp
	${CC} -c ${CPPFLAGS} ${DESPLIEGUE}.cpp -o ${DESPLIEGUE}.o
#---------------------------------------------------
# SERVIDORES LINDA
${SERVER1}.o:		 	 ${SERVER1}.cpp 
	${CC} -c ${CPPFLAGS} ${SERVER1}.cpp -o ${SERVER1}.o
#---------------------------------------------------
${SERVER2}.o:		 	 ${SERVER2}.cpp 
	${CC} -c ${CPPFLAGS} ${SERVER2}.cpp -o ${SERVER2}.o
#---------------------------------------------------
${SERVER3}.o:		 	 ${SERVER3}.cpp 
	${CC} -c ${CPPFLAGS} ${SERVER3}.cpp -o ${SERVER3}.o
#---------------------------------------------------
# FINALIZADOR
${KILLER}.o: 			 ${KILLER}.cpp
	${CC} -c ${CPPFLAGS} ${KILLER}.cpp -o ${KILLER}.o
#-------------------------------------------------------
# MONITORIZACION
${ANALIZER}.o:			 ${ANALIZER}.cpp
	${CC} -c ${CPPFLAGS} ${ANALIZER}.cpp -o ${ANALIZER}.o
#===========================================================
# LIMPIEZA
clean:
	# Librerias
	$(RM) ${DRIVER}  ${DRIVER}.o
	$(RM) ${MONITOR} ${MONITOR}.o
	$(RM) ${SOCKET}.o
	$(RM) ${TUPLAS}.o
	# Source
	$(RM) ${KILLER}   ${KILLER}.o
	$(RM) ${ANALIZER} ${ANALIZER}.o
		# Clientes
		$(RM) ${PUBLICADORES} ${PUBLICADORES}.o
		$(RM) ${BUSCADORES}   ${BUSCADORES}.o
		# Servidores
		$(RM) ${DESPLIEGUE} ${DESPLIEGUE}.o
		$(RM) ${SERVER1} 	${SERVER1}.o
		$(RM) ${SERVER2} 	${SERVER2}.o
		$(RM) ${SERVER3} 	${SERVER3}.o
