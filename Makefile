#----------------------------------------------------------------------------
CPPFLAGS = g++ -std=c++11
#-----------------------------------------------------------
# VARIABLES PARA LA COMPILACION
# Carpetas
LIBS_DIR  = libs
TUPLA_DIR = ${LIBS_DIR}/Tupla

# Archivos
TUPLA = ${TUPLA_DIR}/Tupla
MAIN  = test
EJECUTABLE = test

#-----------------------------------------------------------
# LINKADO
${MAIN}: ${MAIN}.o ${TUPLA}.o
	${CPPFLAGS} ${MAIN}.o ${TUPLA}.o -o ${EJECUTABLE}
#-----------------------------------------------------------
# COMPILACION
${MAIN}.o: ${MAIN}.cpp
	${CPPFLAGS} -c ${MAIN}.cpp
#-----------------------------------------------------------
${TUPLA}.o: ${TUPLA}.hpp ${TUPLA}.cpp
	${CPPFLAGS} -c ${TUPLA}.cpp -o ${TUPLA}.o
#-----------------------------------------------------------

# LIMPIEZA
clean:
	$(RM) *.o
	$(RM) ${TUPLA_DIR}/*.o
	$(RM) ${EJECUTABLE}