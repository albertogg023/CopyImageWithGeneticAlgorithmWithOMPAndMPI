#ifndef _IMAGEN
#define _IMAGEN

#include <mpi.h>

typedef struct {
	unsigned char r, g, b;
} RGB;

RGB *leer_ppm(const char *, int *, int *, int *);
void escribir_ppm(const char *, int, int, int, const RGB *);
void suavizar(int ancho, int alto, RGB *imagen, int idProceso, int numProcesos, MPI_Datatype typeRGB);

#endif