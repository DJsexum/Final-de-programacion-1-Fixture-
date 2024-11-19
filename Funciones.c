#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define CantidadMaximaDeEquipos 4
#define CantidadMaximaDeGrupos 4
#define LimiteDelNombreDelEquipo 30
#define CantidadDeEnfrentamientos 6

FILE *MiArchivo;

struct Equipos
{
    char equipos[LimiteDelNombreDelEquipo];
    int partidosJugados;
    int puntos;
    int partidosGanados;
    int partidosEmpatados;
    int partidosPerdidos;
    int Goles;
    int GolesEnContra;
    int DiferenciaGoles;
};

struct grupos
{
    char nombreGrupo;
    struct Equipos seleccion[CantidadMaximaDeEquipos];
    int enfrentamientos[CantidadDeEnfrentamientos][2]; // Para almacenar los índices de los equipos que juegan entre sí
    int enfrentamientosDefinidos[CantidadDeEnfrentamientos]; // Indica si los enfrentamientos han sido definidos
    int goles[CantidadDeEnfrentamientos][2]; // Goles de cada equipo en los enfrentamientos
};


void limpiarBufferEntrada();
void verSoloUnGrupo(struct grupos listaDeGrupos[], int CantEquipos);
int todosLosEnfrentamientosEstanDefinidos(struct grupos grupo);
void mostrarEnfrentamientos(struct grupos grupo);
void menu1(struct grupos listaDeGrupos[], int CantEquipos); 
void menu2(struct grupos listaDeGrupos[], int CantEquipos);
void CrearFixture(struct grupos listaDeGrupos[], int CantEquipos);
void cargarEquipos(struct grupos listaG[], int CantEquipos, int numGrupo);
void FuncionMayuscula(char caracteres[]);
void CrearArchivo(struct grupos listaDeGrupos[]);
void LeerArchivo(struct grupos listaDeGrupos[]);
void verFixtureCompleto(struct grupos listaDeGrupos[]);
void cargarResultados(struct grupos listaDeGrupos[]);
void modificarUnDato(struct grupos listaDeGrupos[]);
void acomodar(struct grupos listaG[], int n);

void limpiarBufferEntrada()
{
    int c;
    while (((c = getchar()) != '\n') && (c != EOF));
}

void menu1(struct grupos listaDeGrupos[], int CantEquipos)
{
    int respuestaMenuPrinci = 0;

    do
    {
        printf("===============MENU PRINCIPAL===============\n");
        printf("|             1-CREAR FIXTURE              |\n");
        printf("|             2-VER FIXTURE                |\n");
        printf("|             3-SALIR                      |\n");
        printf("============================================\n");
        printf("\nINGRESE UNA RESPUESTA: ");
        scanf("%d", &respuestaMenuPrinci);
        limpiarBufferEntrada();
        printf("\n");
    }
    while (respuestaMenuPrinci < 1 || respuestaMenuPrinci > 3);

    switch (respuestaMenuPrinci)
    {
        case 1:
            CrearFixture(listaDeGrupos, CantEquipos);
        break;

        case 2:
            MiArchivo = fopen("Fixture.txt", "r");

            if (MiArchivo == NULL)
            {
                printf("\nNO HAY UN FIXTURE CARGADO. DEBERA CREAR UNO PRIMERO\n");
                menu1(listaDeGrupos, CantEquipos);
            }
                else
                {
                    LeerArchivo(listaDeGrupos);
                    fclose(MiArchivo);
                    menu2(listaDeGrupos, CantEquipos);
                }
        break;

        case 3:
            exit(0);
        break;
    }
}

void menu2(struct grupos listaDeGrupos[], int CantEquipos)
{
    int respuestaMenu2;

    do
    {
        printf("\n===============MENU FIXTURE===============\n");
        printf("|             1-VER FIXTURE COMPLETO     |\n");
        printf("|             2-VER UN GRUPO             |\n");
        printf("|             3-CARGAR RESULTADOS        |\n");
        printf("|             4-MODIFICAR UN DATO        |\n");
        printf("|             5-VOLVER AL MENU PRINCIPAL |\n");
        printf("==========================================\n");
        printf("\nINGRESE UNA RESPUESTA: ");
        scanf("%d", &respuestaMenu2);
        limpiarBufferEntrada();
        printf("\n\n");
    }
    while (respuestaMenu2 < 1 || respuestaMenu2 > 5);

    switch (respuestaMenu2)
    {
        case 1:
            acomodar(listaDeGrupos, CantidadMaximaDeGrupos);
            menu2(listaDeGrupos, CantEquipos);
        break;

        case 2:
            verSoloUnGrupo(listaDeGrupos, CantEquipos);
        break;

        case 3:
            cargarResultados(listaDeGrupos);
        break;

        case 4:
            modificarUnDato(listaDeGrupos);
        break;

        case 5:
            menu1(listaDeGrupos, CantEquipos);
        break;
    }
}

void CrearFixture(struct grupos listaDeGrupos[], int CantEquipos)
{
    MiArchivo = fopen("Fixture.txt", "r");

    if (MiArchivo != NULL)
    {
        fclose(MiArchivo);
        char respuesta;
        printf("YA EXISTE UN FIXTURE CREADO. DESEAS SOBRESCRIBIR LOS DATOS? SI (S) NO (N): ");
        scanf(" %c", &respuesta);
        limpiarBufferEntrada();

        if (respuesta != 's' && respuesta != 'S')
        {
            printf("\nNO SE CREARA UN NUEVO FIXTURE, REGRESANDO AL MENU...\n");
            menu2(listaDeGrupos, CantEquipos);
            return;
        }
    }

    for (int i = 0; i < CantidadMaximaDeGrupos; i++)
    {
        listaDeGrupos[i].nombreGrupo = 'A' + i; 
        printf("\n|INGRESE LOS EQUIPOS DEL GRUPO %c|\n", listaDeGrupos[i].nombreGrupo);
        cargarEquipos(listaDeGrupos, CantEquipos, i); 
        
        // Inicializar los enfrentamientos a "a definir"
        for (int j = 0; j < 6; j++)
        {
            listaDeGrupos[i].enfrentamientosDefinidos[j] = 0; // 0 indica que el enfrentamiento no está definido
        }
    }

    CrearArchivo(listaDeGrupos);
    acomodar(listaDeGrupos, CantidadMaximaDeGrupos);
    menu2(listaDeGrupos, CantEquipos);
}

void cargarEquipos(struct grupos listaG[], int CantEquipos, int numGrupo)
{
    for (int i = 0; i < CantEquipos; i++)
    {
        printf("\nINGRESE EL NOMBRE DEL EQUIPO %d: ", i + 1);
        fgets(listaG[numGrupo].seleccion[i].equipos, sizeof(listaG[numGrupo].seleccion[i].equipos), stdin);
        
        // Eliminar el salto de línea al final si está presente
        listaG[numGrupo].seleccion[i].equipos[strcspn(listaG[numGrupo].seleccion[i].equipos, "\n")] = '\0';

        // Verificar que el nombre del equipo no esté vacío
        while (strlen(listaG[numGrupo].seleccion[i].equipos) == 0) // Asegura que el nombre no esté vacío
        {
            printf("EL NOMBRE DEL EQUIPO NO PUEDE ESTAR VACIO. POR FAVOR, INGRESE UN NOMBRE VALIDO.\n");
            printf("INGRESE EL NOMBRE DEL EQUIPO %d: ", i + 1);
            fgets(listaG[numGrupo].seleccion[i].equipos, sizeof(listaG[numGrupo].seleccion[i].equipos), stdin);
            listaG[numGrupo].seleccion[i].equipos[strcspn(listaG[numGrupo].seleccion[i].equipos, "\n")] = '\0'; // Eliminar salto de línea
        }

        // Reemplazar espacios por guiones bajos
        for (int j = 0; listaG[numGrupo].seleccion[i].equipos[j] != '\0'; j++)
        {
            if (listaG[numGrupo].seleccion[i].equipos[j] == ' ')
            {
                listaG[numGrupo].seleccion[i].equipos[j] = '_';
            }
        }

        // Convertir a mayúsculas
        FuncionMayuscula(listaG[numGrupo].seleccion[i].equipos);
        
        // Inicializar las estadísticas del equipo
        listaG[numGrupo].seleccion[i].puntos = 0;
        listaG[numGrupo].seleccion[i].partidosJugados = 0;
        listaG[numGrupo].seleccion[i].partidosGanados = 0;
        listaG[numGrupo].seleccion[i].partidosPerdidos = 0;
        listaG[numGrupo].seleccion[i].Goles = 0;
        listaG[numGrupo].seleccion[i].GolesEnContra = 0;
        listaG[numGrupo].seleccion[i].DiferenciaGoles = 0;
        listaG[numGrupo].seleccion[i].partidosEmpatados = 0;
    }
}

void FuncionMayuscula(char caracteres[])
{
    for (int i = 0; caracteres[i] != '\0'; i++)
    {
        caracteres[i] = toupper(caracteres[i]);
    }
}

void CrearArchivo(struct grupos listaDeGrupos[])
{
    MiArchivo = fopen("Fixture.txt", "w");

        if (MiArchivo == NULL)
        {
            printf("ERROR AL CREAR EL ARCHIVO.\n");
            return;
        }

        for (int g = 0; g < CantidadMaximaDeGrupos; g++)
        {
            fprintf(MiArchivo, "GRUPO %c:\n", listaDeGrupos[g].nombreGrupo);
            
            // Guardar información de los equipos
            for (int t = 0; t < CantidadMaximaDeEquipos; t++)
            {
                fprintf(MiArchivo, "%s %d %d %d %d %d %d %d %d\n",
                listaDeGrupos[g].seleccion[t].equipos,
                listaDeGrupos[g].seleccion[t].puntos,
                listaDeGrupos[g].seleccion[t].partidosJugados,
                listaDeGrupos[g].seleccion[t].partidosGanados,
                listaDeGrupos[g].seleccion[t].partidosEmpatados,
                listaDeGrupos[g].seleccion[t].partidosPerdidos,
                listaDeGrupos[g].seleccion[t].Goles,
                listaDeGrupos[g].seleccion[t].GolesEnContra,
                listaDeGrupos[g].seleccion[t].DiferenciaGoles);
            }

            // Guardar enfrentamientos definidos y sus goles
            for (int e = 0; e < CantidadDeEnfrentamientos; e++)
            {
                fprintf(MiArchivo, "%d %d %d %d %d\n",
                listaDeGrupos[g].enfrentamientosDefinidos[e],
                listaDeGrupos[g].enfrentamientos[e][0],
                listaDeGrupos[g].enfrentamientos[e][1],
                listaDeGrupos[g].goles[e][0],
                listaDeGrupos[g].goles[e][1]);
            }
        }

    fclose(MiArchivo);
}

void LeerArchivo(struct grupos listaDeGrupos[])
{
    MiArchivo = fopen("Fixture.txt", "r");

        if (MiArchivo == NULL)
        {
            printf("ERROR AL ABRIR EL ARCHIVO. ASEGURESE DE QUE EL ARCHIVO EXISTE Y NO ESTA CORRUPTO.\n");
            return;
        }

        for (int g = 0; g < CantidadMaximaDeGrupos; g++)
        {
            fscanf(MiArchivo, " GRUPO %c:\n", &listaDeGrupos[g].nombreGrupo);
            
            for (int t = 0; t < CantidadMaximaDeEquipos; t++)// Leer información de los equipos
            {
                if (fscanf(MiArchivo, "%s %d %d %d %d %d %d %d %d\n",
                    listaDeGrupos[g].seleccion[t].equipos,
                    &listaDeGrupos[g].seleccion[t].puntos,
                    &listaDeGrupos[g].seleccion[t].partidosJugados,
                    &listaDeGrupos[g].seleccion[t].partidosGanados,
                    &listaDeGrupos[g].seleccion[t].partidosEmpatados,
                    &listaDeGrupos[g].seleccion[t].partidosPerdidos,
                    &listaDeGrupos[g].seleccion[t].Goles,
                    &listaDeGrupos[g].seleccion[t].GolesEnContra,
                    &listaDeGrupos[g].seleccion[t].DiferenciaGoles) != 9)
                {
                    printf("ERROR AL LEER DATOS DEL EQUIPO EN GRUPO %c. FORMATO INCORRECTO.\n", listaDeGrupos[g].nombreGrupo);
                    fclose(MiArchivo);
                    return;
                }
            }
            
            for (int e = 0; e < CantidadDeEnfrentamientos; e++)// Leer enfrentamientos definidos y sus goles
            {
                if (fscanf(MiArchivo, "%d %d %d %d %d\n",
                    &listaDeGrupos[g].enfrentamientosDefinidos[e],
                    &listaDeGrupos[g].enfrentamientos[e][0],
                    &listaDeGrupos[g].enfrentamientos[e][1],
                    &listaDeGrupos[g].goles[e][0],
                    &listaDeGrupos[g].goles[e][1]) != 5)
                {
                    printf("ERROR AL LEER DATOS DE ENFRENTAMIENTOS EN GRUPO %c. FORMATO INCORRECTO.\n", listaDeGrupos[g].nombreGrupo);
                    fclose(MiArchivo);
                    return;
                }
            }
        }

    fclose(MiArchivo);
}

void cargarResultados(struct grupos listaDeGrupos[])
{
    char grupoBuscado;
    int equipo1, equipo2;
    int golesEquipo1, golesEquipo2;

    // Preguntar repetidamente qué grupo quiere hasta ingresar una opción válida
    do
    {
        printf("\nINGRESE EL GRUPO DEL PARTIDO QUE DESEA CARGAR RESULTADOS (A-B-C-D): ");
        scanf(" %c", &grupoBuscado);
        limpiarBufferEntrada();
        grupoBuscado = toupper(grupoBuscado);
    }
    while (grupoBuscado < 'A' || grupoBuscado > 'D');

    // Buscar el grupo en la lista de grupos
    int grupoEncontrado = -1;
    for (int i = 0; i < CantidadMaximaDeGrupos; i++)
    {
        if (listaDeGrupos[i].nombreGrupo == grupoBuscado)
        {
            grupoEncontrado = i;
            break;
        }
    }

    if (grupoEncontrado == -1)
    {
        printf("GRUPO NO ENCONTRADO. INTENTE NUEVAMENTE.\n");
        return;
    }

    // Mostrar los equipos del grupo seleccionado
    printf("\nEQUIPOS DEL GRUPO %c:\n", listaDeGrupos[grupoEncontrado].nombreGrupo);
    for (int j = 0; j < CantidadMaximaDeEquipos; j++)
    {
        printf("%d. %s\n", j + 1, listaDeGrupos[grupoEncontrado].seleccion[j].equipos);
    }

    // Cargar los resultados
    for (int j = 0; j < CantidadDeEnfrentamientos; j++)
    {
        if (listaDeGrupos[grupoEncontrado].enfrentamientosDefinidos[j] == 1)
        {
            // Si ya hay un enfrentamiento definido, lo salta para evitar sobreescribir
            continue;
        }
        printf("\nPARTIDO %d:\n", j + 1);

        // Solicitar el número del primer equipo
        do
        {
            printf("INGRESE EL NUMERO DEL PRIMER EQUIPO: ");
            scanf("%d", &equipo1);
            limpiarBufferEntrada();
        }
        while (equipo1 < 1 || equipo1 > CantidadMaximaDeEquipos);

        // Solicitar el número del segundo equipo
        do
        {
            printf("INGRESE EL NUMERO DEL SEGUNDO EQUIPO: ");
            scanf("%d", &equipo2);
            limpiarBufferEntrada();
        }
        while (equipo2 < 1 || equipo2 > CantidadMaximaDeEquipos || equipo2 == equipo1);

        equipo1--; // Ajustar a índice de arreglo
        equipo2--; // Ajustar a índice de arreglo

        // Mostrar y solicitar los resultados del partido
        printf("\n%s VS %s\n", listaDeGrupos[grupoEncontrado].seleccion[equipo1].equipos, listaDeGrupos[grupoEncontrado].seleccion[equipo2].equipos);
        printf("CANTIDAD DE GOLES DE %s: ", listaDeGrupos[grupoEncontrado].seleccion[equipo1].equipos);

        do
        {
            scanf("%d", &golesEquipo1);
            limpiarBufferEntrada();
        }
        while (golesEquipo1 < 0);

        printf("CANTIDAD DE GOLES DE %s: ", listaDeGrupos[grupoEncontrado].seleccion[equipo2].equipos);

        do
        {
            scanf("%d", &golesEquipo2);
            limpiarBufferEntrada();
        }
        while (golesEquipo2 < 0);

        // Guardar enfrentamiento y marcar como definido
        listaDeGrupos[grupoEncontrado].enfrentamientos[j][0] = equipo1;
        listaDeGrupos[grupoEncontrado].enfrentamientos[j][1] = equipo2;
        listaDeGrupos[grupoEncontrado].goles[j][0] = golesEquipo1;
        listaDeGrupos[grupoEncontrado].goles[j][1] = golesEquipo2;
        listaDeGrupos[grupoEncontrado].enfrentamientosDefinidos[j] = 1;

        // Actualizar los datos de ambos equipos
        listaDeGrupos[grupoEncontrado].seleccion[equipo1].partidosJugados++;
        listaDeGrupos[grupoEncontrado].seleccion[equipo2].partidosJugados++;
        listaDeGrupos[grupoEncontrado].seleccion[equipo1].Goles += golesEquipo1;
        listaDeGrupos[grupoEncontrado].seleccion[equipo2].Goles += golesEquipo2;
        listaDeGrupos[grupoEncontrado].seleccion[equipo1].GolesEnContra += golesEquipo2;
        listaDeGrupos[grupoEncontrado].seleccion[equipo2].GolesEnContra += golesEquipo1;
        listaDeGrupos[grupoEncontrado].seleccion[equipo1].DiferenciaGoles += (golesEquipo1 - golesEquipo2);
        listaDeGrupos[grupoEncontrado].seleccion[equipo2].DiferenciaGoles += (golesEquipo2 - golesEquipo1);

        // Actualizar estadísticas del partido
        if (golesEquipo1 > golesEquipo2)
        {
            listaDeGrupos[grupoEncontrado].seleccion[equipo1].partidosGanados++;
            listaDeGrupos[grupoEncontrado].seleccion[equipo2].partidosPerdidos++;
            listaDeGrupos[grupoEncontrado].seleccion[equipo1].puntos += 3;
        }
            else if (golesEquipo1 < golesEquipo2)
            {
                listaDeGrupos[grupoEncontrado].seleccion[equipo2].partidosGanados++;
                listaDeGrupos[grupoEncontrado].seleccion[equipo1].partidosPerdidos++;
                listaDeGrupos[grupoEncontrado].seleccion[equipo2].puntos += 3;
            }
                else
                {
                    listaDeGrupos[grupoEncontrado].seleccion[equipo1].partidosEmpatados++;
                    listaDeGrupos[grupoEncontrado].seleccion[equipo2].partidosEmpatados++;
                    listaDeGrupos[grupoEncontrado].seleccion[equipo1].puntos++;
                    listaDeGrupos[grupoEncontrado].seleccion[equipo2].puntos++;
                }
    }
    // Guardar los cambios en el archivo
    CrearArchivo(listaDeGrupos);
    printf("\nRESULTADOS CARGADOS EXITOSAMENTE.\n");
    menu2(listaDeGrupos, CantidadMaximaDeGrupos);
}

void verSoloUnGrupo(struct grupos listaDeGrupos[], int CantEquipos)
{
    char grupoBuscado;

    printf("\nINGRESE EL GRUPO QUE DESEA VER (A-B-C-D): ");
    scanf(" %c", &grupoBuscado);
    limpiarBufferEntrada();
    grupoBuscado = toupper(grupoBuscado);

    while (grupoBuscado < 'A' || grupoBuscado > 'D')
    {
        printf("OPCION INCORRECTA. INTENTE NUEVAMENTE.\n");
        printf("\nINGRESE EL GRUPO QUE DESEA VER (A-B-C-D): ");
        scanf(" %c", &grupoBuscado);
        limpiarBufferEntrada();
        grupoBuscado = toupper(grupoBuscado);
    }
    printf("\n\n");

    for (int i = 0; i < CantidadMaximaDeGrupos; i++)
    {
        if (listaDeGrupos[i].nombreGrupo == grupoBuscado)
        {
            printf("===================================== GRUPO: %c =====================================\n", listaDeGrupos[i].nombreGrupo);
            printf("||  EQUIPO               |  PJ  |  PG  |  PE  |  PP  |  GF  |  GC  |  DG  |  PTS  ||\n");

            for (int j = 0; j < CantidadMaximaDeEquipos; j++)
            {
                printf("||  %-18s   |%4d  |%4d  |%4d  |%4d  |%4d  |%4d  |%4d  |%4d   ||\n",
                listaDeGrupos[i].seleccion[j].equipos,
                listaDeGrupos[i].seleccion[j].partidosJugados,
                listaDeGrupos[i].seleccion[j].partidosGanados,
                listaDeGrupos[i].seleccion[j].partidosEmpatados,
                listaDeGrupos[i].seleccion[j].partidosPerdidos,
                listaDeGrupos[i].seleccion[j].Goles,
                listaDeGrupos[i].seleccion[j].GolesEnContra,
                listaDeGrupos[i].seleccion[j].DiferenciaGoles,
                listaDeGrupos[i].seleccion[j].puntos);
            }

            printf("||================================================================================||\n");
            printf("||                                       V S                                      ||\n");

            mostrarEnfrentamientos(listaDeGrupos[i]);

            printf("====================================================================================\n");
            menu2(listaDeGrupos, CantEquipos);
            return;
        }
    }
}

int todosLosEnfrentamientosEstanDefinidos(struct grupos grupo)
{
    for (int i = 0; i < CantidadDeEnfrentamientos; i++)
    {
        if (grupo.enfrentamientosDefinidos[i] == 0)
        {
            return 0; // Algun enfrentamiento no está definido
        }
    }
    return 1; // Todos los enfrentamientos están definidos
}

void mostrarEnfrentamientos(struct grupos grupo)
{
    for (int i = 0; i < CantidadDeEnfrentamientos; i++)
    {
        if (grupo.enfrentamientosDefinidos[i])
        {
            int equipo1 = grupo.enfrentamientos[i][0]; // Índice del primer equipo
            int equipo2 = grupo.enfrentamientos[i][1]; // Índice del segundo equipo

            // Verifica si los índices de los equipos son válidos
            if (equipo1 >= 0 && equipo1 < CantidadMaximaDeEquipos && equipo2 >= 0 && equipo2 < CantidadMaximaDeEquipos)
            {
                // Mostrar enfrentamientos con marcador definido
                printf("||                 %20s %d - %d %-20s                ||\n",
                grupo.seleccion[equipo1].equipos, grupo.goles[i][0],
                grupo.goles[i][1], grupo.seleccion[equipo2].equipos);
            }
        }
            else
            {
                // Mostrar enfrentamientos no definidos
                printf("||                                        -                                       ||\n");
            }
    }
}

void acomodar(struct grupos listaG[], int n)
{
    int i = 0, controlando = 0;
    printf("========================================== COPA AMERICA USA 2024 ==========================================");

    for (int j = 0; j < n; j++) 
    {
        
        printf("\n|  GRUPO %c:                                                                                               |\n", listaG[j].nombreGrupo);
        printf("|  -----------------------------------------------------------------------------------------------------  |\n");
        printf("|  ||EQUIPO                 PTS      PJ        PG        PP        PE         GF         GC         DG||  |\n");

        for (i = 0; i < CantidadMaximaDeEquipos; i++)// Mostrar toda la información del equipo
        {
            printf("|  ||%-20s %4d     %4d      %4d      %4d      %4d       %4d       %4d       %4d||  |\n", 
            listaG[j].seleccion[i].equipos, 
            listaG[j].seleccion[i].puntos, 
            listaG[j].seleccion[i].partidosJugados, 
            listaG[j].seleccion[i].partidosGanados, 
            listaG[j].seleccion[i].partidosPerdidos, 
            listaG[j].seleccion[i].partidosEmpatados, 
            listaG[j].seleccion[i].Goles, 
            listaG[j].seleccion[i].GolesEnContra, 
            listaG[j].seleccion[i].DiferenciaGoles);
        }

        printf("|  -----------------------------------------------------------------------------------------------------  |\n");

        if (controlando < 3)
        {
            printf("|                                                                                                         |");
            controlando = controlando + 1;
        }
        if (i < CantidadMaximaDeEquipos)
        {
            printf("|                                                                                                         |");
        }
    }
    printf("===========================================================================================================\n\n");
}

void modificarUnDato(struct grupos listaDeGrupos[])
{
    char grupoBuscado;
    int equipoSeleccionado;
    int datoSeleccionado;

    // Preguntar repetidamente qué grupo quiere modificar hasta ingresar una opción válida
    do
    {
        printf("\nINGRESE EL GRUPO QUE DESEA MODIFICAR (A-B-C-D): ");
        scanf(" %c", &grupoBuscado);  // El espacio antes de %c ayuda a evitar el problema del buffer
        limpiarBufferEntrada();
        grupoBuscado = toupper(grupoBuscado);
    }
    while (grupoBuscado < 'A' || grupoBuscado > 'D');

    // Buscar el grupo en la lista de grupos
    for (int i = 0; i < CantidadMaximaDeGrupos; i++)
    {
        if (listaDeGrupos[i].nombreGrupo == grupoBuscado)
        {
            // Mostrar los equipos del grupo que se eligió
            printf("\nGRUPO %c:\n", listaDeGrupos[i].nombreGrupo);

            for (int j = 0; j < CantidadMaximaDeEquipos; j++)
            {
                printf("%d. %-20s | PTS: %2d | PJ: %2d | PG: %2d | PE: %2d | PP: %2d | GF: %2d | GC: %2d | DG: %2d\n",
                j + 1, listaDeGrupos[i].seleccion[j].equipos, listaDeGrupos[i].seleccion[j].puntos,
                listaDeGrupos[i].seleccion[j].partidosJugados, listaDeGrupos[i].seleccion[j].partidosGanados,
                listaDeGrupos[i].seleccion[j].partidosEmpatados, listaDeGrupos[i].seleccion[j].partidosPerdidos,
                listaDeGrupos[i].seleccion[j].Goles, listaDeGrupos[i].seleccion[j].GolesEnContra,
                listaDeGrupos[i].seleccion[j].DiferenciaGoles);
            }

            // Preguntar qué equipo se quiere modificar
            do
            {
                printf("SELECCIONE EL NUMERO CORRESPONDIENTE DEL EQUIPO QUE DESEA MODIFICAR: ");

                while (scanf("%d", &equipoSeleccionado) != 1) // Verificar que se haya ingresado un número
                {
                    limpiarBufferEntrada();
                    printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                }
                limpiarBufferEntrada();
            }
            while (equipoSeleccionado < 1 || equipoSeleccionado > CantidadMaximaDeEquipos);

            // Menú para seleccionar el dato a modificar
            do
            {
                printf("\nQUE DATO DESEA MODIFICAR?\n");
                printf("1-NOMBRE DEL EQUIPO\n");
                printf("2-PUNTOS\n");
                printf("3-PARTIDOS JUGADOS\n");
                printf("4-PARTIDOS GANADOS\n");
                printf("5-PARTIDOS EMPATADOS\n");
                printf("6-PARTIDOS PERDIDOS\n");
                printf("7-GOLES A FAVOR\n");
                printf("8-GOLES EN CONTRA\n");
                printf("9-DIFERENCIA DE GOLES\n");
                printf("SELECCIONE LA OPCION DESEADA: ");
                
                while (scanf("%d", &datoSeleccionado) != 1) // Verificar que se haya ingresado un número
                {
                    printf("\n");
                    limpiarBufferEntrada();
                    printf("ERROR, INGRESE UNA OPCION VALIDA: ");
                }
                limpiarBufferEntrada();
            }
            while (datoSeleccionado < 1 || datoSeleccionado > 9);

            // Modificar el dato seleccionado
            switch (datoSeleccionado)
            {
                case 1:// Se está modificando el nombre del equipo
                    printf("INGRESE EL NUEVO NOMBRE DEL EQUIPO: ");
                    fgets(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos, sizeof(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos), stdin);
                    
                    // Eliminar el salto de línea al final si está presente
                    listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos[strcspn(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos, "\n")] = '\0';

                    // Verificar que el nuevo nombre del equipo no esté vacío
                    while (strlen(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos) == 0) // Asegura que el nombre no esté vacío
                    {
                        printf("EL NOMBRE DEL EQUIPO NO PUEDE ESTAR VACIO. POR FAVOR, INGRESE UN NOMBRE VALIDO.\n");
                        printf("INGRESE EL NUEVO NOMBRE DEL EQUIPO: ");
                        fgets(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos, sizeof(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos), stdin);
                        listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos[strcspn(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos, "\n")] = '\0'; // Eliminar salto de línea
                    }

                    for (int k = 0; k < strlen(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos); k++)// Buscar el espacio entre palabra para agregarle el guion 
                    {
                        if (listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos[k] == ' ')
                        {
                            listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos[k] = '_';
                        }
                    }
                    FuncionMayuscula(listaDeGrupos[i].seleccion[equipoSeleccionado - 1].equipos);
                break;

                case 2:// Se está modificando los puntos del equipo
                    do
                    {
                        printf("INGRESE LOS NUEVOS PUNTOS: ");

                        while (scanf("%d", &listaDeGrupos[i].seleccion[equipoSeleccionado - 1].puntos) != 1)
                        {
                            limpiarBufferEntrada();
                            printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                        }
                        limpiarBufferEntrada();
                    }
                    while (listaDeGrupos[i].seleccion[equipoSeleccionado - 1].puntos < 0); // Controla que los puntos no sean negativos
                break;

                case 3:// Se está modificando la cantidad de partidos jugados
                    do
                    {
                        printf("INGRESE LA NUEVA CANTIDAD DE PARTIDOS JUGADOS: ");

                        while (scanf("%d", &listaDeGrupos[i].seleccion[equipoSeleccionado - 1].partidosJugados) != 1)
                        {
                            limpiarBufferEntrada();
                            printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                        }
                        limpiarBufferEntrada();
                    }
                    while (listaDeGrupos[i].seleccion[equipoSeleccionado - 1].partidosJugados < 0); // Controla que los partidos jugados no sean negativos
                break;

                case 4:// Se está modificando la cantidad de partidos ganados
                    do
                    {
                        printf("INGRESE LA NUEVA CANTIDAD DE PARTIDOS GANADOS: ");

                        while (scanf("%d", &listaDeGrupos[i].seleccion[equipoSeleccionado - 1].partidosGanados) != 1)
                        {
                            limpiarBufferEntrada();
                            printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                        }
                        limpiarBufferEntrada();
                    }
                    while (listaDeGrupos[i].seleccion[equipoSeleccionado - 1].partidosGanados < 0); // Controla que los partidos ganados no sean negativos
                break;

                case 5:// Se está modificando la cantidad de partidos empatados
                    do
                    {
                        printf("INGRESE LA NUEVA CANTIDAD DE PARTIDOS EMPATADOS: ");

                        while (scanf("%d", &listaDeGrupos[i].seleccion[equipoSeleccionado - 1].partidosEmpatados) != 1)
                        {
                            limpiarBufferEntrada();
                            printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                        }
                        limpiarBufferEntrada();
                    }
                    while (listaDeGrupos[i].seleccion[equipoSeleccionado - 1].partidosEmpatados < 0); // Controla que los partidos empatados no sean negativos
                break;

                case 6:// Se está modificando la cantidad de partidos perdidos
                    do
                    {
                        printf("INGRESE LA NUEVA CANTIDAD DE PARTIDOS PERDIDOS: ");

                        while (scanf("%d", &listaDeGrupos[i].seleccion[equipoSeleccionado - 1].partidosPerdidos) != 1)
                        {
                            limpiarBufferEntrada();
                            printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                        }
                        limpiarBufferEntrada();
                    }
                    while (listaDeGrupos[i].seleccion[equipoSeleccionado - 1].partidosPerdidos < 0); // Controla que los partidos perdidos no sean negativos
                break;

                case 7:// Se está modificando la cantidad de goles a favor
                    do
                    {
                        printf("INGRESE LA NUEVA CANTIDAD DE GOLES A FAVOR: ");

                        while (scanf("%d", &listaDeGrupos[i].seleccion[equipoSeleccionado - 1].Goles) != 1)
                        {
                            limpiarBufferEntrada();
                            printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                        }
                        limpiarBufferEntrada();
                    }
                    while (listaDeGrupos[i].seleccion[equipoSeleccionado - 1].Goles < 0); // Controla que los goles a favor no sean negativos
                break;

                case 8:// Se está modificando la cantidad de goles en contra
                    do
                    {
                        printf("INGRESE LA NUEVA CANTIDAD DE GOLES EN CONTRA: ");

                        while (scanf("%d", &listaDeGrupos[i].seleccion[equipoSeleccionado - 1].GolesEnContra) != 1)
                        {
                            limpiarBufferEntrada();
                            printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                        }
                        limpiarBufferEntrada();
                    }
                    while (listaDeGrupos[i].seleccion[equipoSeleccionado - 1].GolesEnContra < 0); // Controla que los goles en contra no sean negativos
                break;

                case 9:// Se está modificando la diferencia de goles (puede ser negativo, cero o positivo)
                    do
                    {
                        printf("INGRESE LA NUEVA DIFERENCIA DE GOLES (PUEDE SER NEGATIVO): ");

                        while (scanf("%d", &listaDeGrupos[i].seleccion[equipoSeleccionado - 1].DiferenciaGoles) != 1)
                        {
                            limpiarBufferEntrada();
                            printf("ENTRADA INVALIDA. POR FAVOR, INGRESE UN NUMERO VALIDO: ");
                        }
                        limpiarBufferEntrada();
                    }
                    while (getchar() != '\n'); // Asegura que el valor ingresado sea válido y no deje espacios en blanco
                break;

                default:
                    printf("OPCION INVALIDA.\n");
                return;
            }
            printf("DATO MODIFICADO\n");// Sale del bucle si se logró modificar el dato seleccionado

            // Guardar los cambios en el archivo
            CrearArchivo(listaDeGrupos);
            menu2(listaDeGrupos, CantidadMaximaDeGrupos);
        }
    }
    printf("GRUPO NO ENCONTRADO. INTENTA NUEVAMENTE.\n");// Si no se encuentra el grupo que se haya elegido
    modificarUnDato(listaDeGrupos);
}