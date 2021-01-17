### TRABAJO FINAL PSCD
El trabajo consiste en la realización de un sistema de publicación y lectura de ofertas
mediate un sistema linda.

#### Clientes
------------
- **Publicadores:** Publican las ofertas que los lectores tendrán que leer cada X tiempo, las ofertas, tendrán el siguiente formato:
                [ID,ORIGEN,DESTINO,PRECIO]

- **Buscadores:** Dentro de estos distinguimos entre dos tipos:
	- Simples: Buscan una oferta tupla mediante el siguiente patrón:

			[?I,?O,?D,?P]	-- Id, Origen, Destino, Precio

	En el ejercicio, hay 10 procesos buscadores simples que realizan su tarea 10 veces, pudiendo realizar en cada una de estas iteraciones hasta 3 busquedas consecutivas.

	- Combinados:   Buscan dos oferta tupla en las que la ciudad de destino de la primera tupla sea la ciudad de origen de la segunda tal cual muestra el siguiente patron:

			[?I,?O,?P,?X]	Oferta 1:	 Id, Origen, Parada,  Precio
			[?J,?P,?d,?Y]	Oferta 2:	 Id, Parada, Destino, Precio

	Ejemplo: Zaragoza -> Pamplona -> Bilbao

#### Servidores
------------

- **Servidor Despliegue:** Almacena toda la información necesaria para que un proceso publicador o buscador pueda conectarse al Linda Server que necesite mediante Linda Driver .

- **Servidores Linda:** Servidores con un espacio de tuplas, en el ejercicio hay 3.
		[Linda server1]	-- Tuplas de tamaño [1-3]
		[Linda server2]	-- Tuplas de tamaño [4-5]
		[Linda server3]	-- Tuplas de tamaño [6]

#### Apoyo
------------

- **Monitorización:**   Servidor encargado de realizar una imagen de los servidores linda cada X tiempo.

- **Finalizador:** Finaliza la ejecución del servidor _Despliegue_ y de los Servidores _Linda_ impidiendo que se conecte alguien más a estos.

## SETUP / USAGE / HOW TO
El ejercicio se descompone en diferentes partes: clientes, servidores y apoyo.
Una posible despligue del sistema sería:
- **PILGOR:**

		ADDRESS:	[pilgor.cps.unizar.es, 155.210.152.12]
		PORTS:		 [32080] [32064] [32048]

	Servicios lanzados: Servidor Linda1, Servidor Linda2, Servidor Linda3.

		Auto:		./runServers
		Manual:	./ServidorLinda1 32080 & ServidorLinda2 32064 &
							  ServidorLinda2 32048
- **HENDRIX:**

		ADDRESS:	[hendrix01.cps.unizar.es, 155.210.152.183]
		PORT:		  [2160]

	Servicios lanzados: Servidor Despliegue y Monitorizacion.

		Auto:		./runDespliegue & ./runMonitorizacion
		Manual:	./Despliegue 2160 & ./Monitorizacion 2160


- **PC_1:**

		ADDRESS:	[XXX.XXX.XXX.XXX]
		PORT:		   [XXXX]

	Servicios lanzados: Publicadores y Buscadores
	Coms: Se lanzarán con la ip y puerto del servidor despligue

		Auto:		./runPublicadores & ./runBuscadores
		Manual:	./Publicadores 155.210.152.183 2160 & 
						 ./Buscadores 155.210.152.183 2160

- **PC_2:**

		ADDRESS:	[XXX.XXX.XXX.XXX]
		PORT:		   [XXXX]

	Servicios lanzados: Cliente Carga
	Coms: Se lanzará con un parámetro de carga inicial de 1000 tuplas el cual podrá modificarse

		Auto:		./runClienteCarga
		Manual:	./ClienteCarga 155.210.152.183 2160

- **PC_3:**

		ADDRESS:	[XXX.XXX.XXX.XXX]
		PORT:		   [XXXX]

	Servicios lanzados: Finalizador
	Coms: Finalizará la ejecución de los servidores linda

		Auto:		./runFinalizador
		Manual:	./Finalizador 155.210.152.183 2160

**NOTA 1:** En la raiz del repositorio se encuentra un archivo `Makefile` que compila todo el ejercicio en el sistema local, es decir, si se quiere realizar la ejecución de todo el sistema en distribuido tendra que tener cuidado en los flags que usa el make, en el caso de nuestra prueba, hendrix y pilgor no comparten un flag y necesitan una compilación distinta.

**NOTA 2:** En la raiz del repositorio se encuentra un archivo `BASH`"menu.bash" que despliega un menu desde el cual se podrá controlar casi la totalidad de los lanzamientos del ejercicio.

## SPECS

- **TUPLAS:** Representan los permisos y ofertas del sistema.
	- Patrones: Se considera patron a la siguiente expresión regular: `?[A-Z]`
	- No patrón: Se considera no patrón todo aquello que no cumpla la expresión regular anterior.
- **LINDA DRIVER:** Libreria con la que los clientes se comunican con los servidores linda.

## FAQ / CONTACT / TROUBLESHOOT

1. **¿Cual es la estrategia de finalización del sistema?**
	La estrategia de finalización del sistema consiste en que los publicadores publican tuplas tipo oferta hasta detectar que todos los procesos buscadores han acabado.
	Los servidores linda y depliegue se apagaran con la ejecución del finalizador.

1. **¿Estructura del espacio de tuplas?**
	Hay un espacio de tuplas para cada tamaño de estas.

## DEVELOPERS

| Pablo Bueno |  Carlos Paesa
| Santiago Illa | Javier Pardos
|  Luis Palazón |  Héctor Toral
