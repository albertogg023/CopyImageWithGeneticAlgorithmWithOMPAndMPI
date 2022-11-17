#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include "../include/imagen.h"
#include "../include/ga.h"
#include "./derivados_mpi.c"

static double mseconds() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1000 + t.tv_usec/1000;
}

int main(int argc, char **argv)
{	
	// ******************************** INICIO TODOS ********************************************

	// Inicializamos entorno MPI (se crean un cierto numero de procesos)
	MPI_Init(&argc,&argv);
	int numProcesos, idProceso;
	// Obtenemos el numero de procesos creados
	MPI_Comm_size(MPI_COMM_WORLD,&numProcesos);
	// Obtenemos el id del proceso actual
	MPI_Comm_rank(MPI_COMM_WORLD,&idProceso);

	int ancho, alto, max;
	ancho = alto = max = 0;
	
	RGB *imagen_objetivo;
	RGB *mejor_imagen;

	double ti;	// para medir tiempos en el padre

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FIN TODOS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



	// ******************************** INICIO SOLO EL PADRE ********************************************

	if(idProceso == 0){
		// Comprobamos los datos de entrada del programa
		if(argc < 9) {
			printf("Ayuda:\n"); 
			printf("  ./programa entrada salida num_generaciones tam_poblacion\n");
			return(-1);
		}
		
		if (atoi(argv[4]) % 4 != 0) {
			printf("El tamaño de la población debe ser divisible por 4\n");
			return(-1);
		}

		if (atof(argv[5]) > 1 || atof(argv[5]) < 0) {
			printf("La probabilidad de mutación debe ser un número entre 0 y 1\n");
			return(-1);
		}

		if (atoi(argv[6]) <= 0) {
			printf("El número de cores no puede ser 0 o negativo\n");
			return(-1);
		}

		if (atoi(argv[6]) <= 0) {
			printf("El número de cores no puede ser 0 o negativo\n");
			return(-1);
		}

		if (atoi(argv[7]) <= 0 || atoi(argv[7]) >= atoi(argv[3]) || atoi(argv[3]) % atoi(argv[7]) != 0) {
			printf("NGM debe ser menor que el número de generaciones, mayor que 0 y que sea un divididendo del numero de generaciones\n");
			return(-1);
		}

		if (atoi(argv[8]) <= 0) {
			printf("NEM debe ser mayor que cero y menor que el tamaño de población \n");
			return(-1);
		}

		if (atoi(argv[9]) <= 0) {
			printf("NPM debe ser mayor que cero y menor que el tamaño de población \n");
			return(-1);
		}

		// Leemos la imagen de entrada
		imagen_objetivo = leer_ppm(argv[1], &ancho, &alto, &max);
		// Reservamos memoria para la imagen de salida
		mejor_imagen = (RGB *) malloc(ancho*alto*sizeof(RGB));

		// Cogemos el instante de inicio
		#ifdef TIME
			ti = mseconds();
		#endif
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FIN SOLO EL PADRE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	

	
	// ******************************** INICIO TODOS ********************************************

	// Compartimos los datos de la imagen leida a los hijos
	MPI_Bcast(&ancho,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&alto,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&max,1,MPI_INT,0,MPI_COMM_WORLD);

	// ******************************** INICIO HIJOS ********************************************
	// Reservamos espacio para el array de la imagen objetivo que tendrá cada hijo
	if(idProceso != 0) {
		imagen_objetivo = (RGB *) malloc(ancho*alto*sizeof(RGB));
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FIN HIJOS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Creamos el tipo de dato Individuo en MPI
	MPI_Datatype individuo_type;
	MPI_Datatype rgb_type;
	crear_tipo_datos(ancho*alto, &rgb_type, &individuo_type);

	// Compartimos los datos de la imagen leida a los hijos
	MPI_Bcast(imagen_objetivo,ancho*alto,rgb_type,0,MPI_COMM_WORLD);

	// Reservamos memoria para la imagen de salida
	mejor_imagen = (RGB *) malloc(ancho*alto*sizeof(RGB));

	// Llamamos al algortimo genetico
	crear_imagen(imagen_objetivo, ancho, alto, max, \
				 atoi(argv[3]), atoi(argv[4]), atof(argv[5]), mejor_imagen, argv[2], idProceso, numProcesos, atoi(argv[7]), atoi(argv[8]), atoi(argv[9]), individuo_type, rgb_type);	
	
	free(imagen_objetivo);

	// Compartimos mejor_imagen a los hijos ya que sera necesario para el suavizado de la imagen.
	MPI_Bcast(mejor_imagen, ancho*alto, rgb_type, 0, MPI_COMM_WORLD);

	// Suavizamos la imagen de salida
	suavizar(ancho, alto, mejor_imagen, idProceso, numProcesos, rgb_type);
	
	if(idProceso == 0) {
		#ifdef DEBUG
			// Print Result
			escribir_ppm(argv[2], ancho, alto, max, mejor_imagen);
		#endif
		// Obtenemos el instante de fin del algortimo y calculamos y mostramos el tiempo de ejecucion
		#ifdef TIME
			double tf = mseconds();
			printf("Execution Time = %.6lf seconds\n", (tf - ti));
		#endif
	}

	free(mejor_imagen); 
	
	// Finalizamos entorno MPI
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FIN TODOS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return(0);
}
