#ifndef _GA
#define _GA

#include <mpi.h>
#include "imagen.h"


typedef struct {
	double fitness;
	RGB imagen[11904];	// tantas posiciones como el numero de pixeles totales de la imagen mas grande que se vaya a utilizar
} Individuo;

void crear_imagen(const RGB *, int, int, int,
	int, int,
	RGB *, const char *,
	int idProceso, int numProcesos,
	int NGM, int NEM, int NPM,
	MPI_Datatype individuo_type, MPI_Datatype rgb_type);
void cruzar(Individuo *, Individuo *, Individuo *, Individuo *, int);
void fitness(const RGB *, Individuo *, int);
void mutar(Individuo *, int, int);

#endif
