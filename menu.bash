#!/bin/bash
# Bash Menu Script Examplee

#https://misc.flogisoft.com/bash/tip_colors_and_formatting
#"Default \e[31mRed"
#"Default \e[32mGreen"
#"Default \e[35mMagenta"
#Negrita \e[1m

arrancarServDesp() {
    echo -e "\e[96m"    #Escribimos el output del serv despliegue en cyan
    #comando ejecutar servidor despliegue
    #./src/servidores/runDespliegue.bash
    ./src/servidores/Despliegue 2160
    echo -e "\e[0m"     #Dejamos de escribir en cyan
}

arrancarServLindas() {
    echo -e "\e[96m"    #Escribimos el output del serv despliegue en cyan
    #comando para ejecutar servidor Linda 
    #./src/servidores/runServers.bash 
    ./src/servidores/ServerLinda1 32080 & ./src/servidores/ServerLinda2 32064 & ./src/servidores/ServerLinda3 32048
    echo -e "\e[0m"     #Dejamos de escribir en cyan

}

arrancarPublicadores() {
    echo -e "\e[96m"    #Escribimos el output del serv despliegue en cyan
    #comando para ejecutar Publicadores 
    ./src/clientes/Publicadores 155.210.152.183 2160
    #./src/clientes/runPublicador.bash
    echo -e "\e[0m"     #Dejamos de escribir en cyan

}

arrancarBuscadores() {
    echo -e "\e[96m"    #Escribimos el output del serv despliegue en cyan
    #comando para ejecutar Buscadores
    ./src/clientes/Buscadores 155.210.152.183 2160
    #./src/clientes/runBuscadores.bash
    echo -e "\e[0m"     #Dejamos de escribir en cyan

}

arrancarMonitorizacion() {
    echo -e "\e[96m"    #Escribimos el output del serv despliegue en cyan
    #comando para ejecutar Monitorizacion 
    ./src/Monitorizacion 155.210.152.183 2160
    #./src/runMonitorizacion.bash
    echo -e "\e[0m"     #Dejamos de escribir en cyan

}

arrancarFinalizador() {
    echo -e "\e[96m"    #Escribimos el output del serv despliegue en cyan
    #comando para ejecutar Monitorizacion 
    ./src/Finalizador 155.210.152.183 2160
    #./src/runFinalizador.bash
    echo -e "\e[0m"     #Dejamos de escribir en cyan

}

echo -e "\e[1m\e[38;5;82m"
cat << "EOF"
  ____                  _     _              _     _           _        
 / ___|  ___ _ ____   _(_) __| | ___  _ __  | |   (_)_ __   __| | __ _  
 \___ \ / _ \ '__\ \ / / |/ _` |/ _ \| '__| | |   | | '_ \ / _` |/ _` | 
  ___) |  __/ |   \ V /| | (_| | (_) | |    | |___| | | | | (_| | (_| | 
 |____/ \___|_|    \_/ |_|\__,_|\___/|_|    |_____|_|_| |_|\__,_|\__,_|
EOF
echo -e "\033[0m"
echo -e "\e[1m\e[1;35mcopyright©\tGRUPO_16 Unizar©\t2020-2021\033[0m\n"
echo -e "\e[1m\e[34mDesarrolladores:\033[0m"
echo -e "\e[1m\e[37m\t+ Mat-Inf:\033[0m Pablo Bueno, Santiago Illa, Luis Palazon, Carlos Paesa"
echo -e "\e[1m\e[37m\t+ Ing-Inf:\033[0m Javier Pardos, Hector Toral\n"

PS3='Elige una opcion: '
options=("Lanzar Publicadores" "Lanzar Buscadores" "Lanzar Monitorizacion" "Lanzar Finalizador" "Lanzar clienteCarga" "Compilar proyecto entero" "EXIT")
select opt in "${options[@]}"
do
    case $opt in
        "Lanzar Publicadores")
            echo -e "\e[100m\e[1mIniciando Publicadores...\033[0m\n"
            arrancarPublicadores
            break
            ;;
        "Lanzar Buscadores")
            echo -e "\e[100m\e[1mIniciando Buscadores...\033[0m\n"
            arrancarBuscadores
            break
            ;;
        "Lanzar Monitorizacion")
            echo -e "\e[100m\e[1mIniciando Monitorizacion...\033[0m\n"
            arrancarMonitorizacion 
            break
            ;;
        "Lanzar Finalizador")
            echo -e "\e[100m\e[1mIniciando Finalizador...\033[0m\n"
            arrancarFinalizador 
            break
            ;;
        "Lanzar clienteCarga")
            echo -e "\e[100m\e[1mIniciando clienteCarga...\033[0m\n" 
            ./src/testing/clienteCarga 100 155.210.152.183 2160
            break
            ;;
        "Compilar proyecto entero")
            echo -e "\e[100m\e[1mIniciando compilacion...\033[0m\n"
            make clean
            make
            break
            ;;
        "EXIT")
            echo -e "\e[7mbye bye lindas\033[0m"
            break
            ;;
        *) echo "opcion invalida $REPLY";;
    esac
done
