#-------------------------------------------------------------------------------------------
# File:    run.bash
# Authors: Pablo Lacueva(799134) & Hector Toral(798095)
# Date:    dic 2020
# Coms:    Trabajo final PSCD
#-------------------------------------------------------------------------------------------
#!/bin/bash
make -f Makefile_TEST clean
make -f Makefile_TEST
./test
#clear