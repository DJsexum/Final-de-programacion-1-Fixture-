#include <stdio.h>
#include <stdlib.h>
#include "Funciones.c"

int main ()
{
    struct grupos listaDeGrupos[CantidadMaximaDeGrupos];
    int CantEquipos = CantidadMaximaDeEquipos;

        menu1(listaDeGrupos, CantEquipos);

    getchar ();
    return 0;
}