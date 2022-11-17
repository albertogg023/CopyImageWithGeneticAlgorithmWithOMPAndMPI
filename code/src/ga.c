#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <mpi.h>
#include "../include/imagen.h"
#include "../include/ga.h"

#define PRINT 0

unsigned int randomSeed;

static int aleatorio(int max)
{
	return (rand() % (max + 1)); 
}

void mezclar(Individuo **poblacion, int izq, int med, int der)
{	
	int i, j, k;
	
	Individuo **pob = (Individuo **) malloc((der - izq)*sizeof(Individuo *));
	assert(pob);
	
	for(i = 0; i < (der - izq); i++) {
		pob[i] = (Individuo *) malloc(sizeof(Individuo));
	}
	
	k = 0;
	i = izq;
	j = med;
	while( (i < med) && (j < der) ) {
		if (poblacion[i]->fitness < poblacion[j]->fitness) {
			memmove(pob[k],poblacion[i],sizeof(Individuo));
			k++;
			i++;

		}
		else {
			memmove(pob[k],poblacion[j],sizeof(Individuo));
			k++;
			j++;

		}
	}
	
	for(; i < med; i++) {
		memmove(pob[k++], poblacion[i],sizeof(Individuo));
	}
	
	for(; j < der; j++) {
		memmove(pob[k++],poblacion[j],sizeof(Individuo));
	}
	
	i = 0;
	for(i = 0; i < (der - izq); i++) {
		memmove(poblacion[i+izq],pob[i],sizeof(Individuo));
		free(pob[i]);
	}
	free(pob);
}

void mergeSort(Individuo **poblacion, int izq, int der)
{
	int med = (izq + der) / 2;
	if ((der - izq) < 8)
	{
		return;
	}
	mergeSort(poblacion, izq, med);
	mergeSort(poblacion, med, der);
	mezclar(poblacion, izq, med, der);	
}

void init_imagen_aleatoria(RGB *imagen, int max, int total)
{
	for (int i = 0; i < total; i++)
	{
		imagen[i].r = aleatorio(max);
		imagen[i].g = aleatorio(max);
		imagen[i].b = aleatorio(max);
	}
}

static int comp_fitness(const void *a, const void *b)
{
	return ((Individuo *)a)->fitness - ((Individuo *)b)->fitness;
}

void crear_imagen(const RGB *imagen_objetivo, int ancho, int alto, int max, int num_generaciones, int tam_poblacion, float prob_mutacion,
	RGB *imagen_resultado, const char *output_file,
	int idProceso, int numProcesos, int NGM, int NEM, int NPM, MPI_Datatype typeIndividuo, MPI_Datatype typeRGB)
{	
	// ******************************** INICIO TODOS ********************************************
	
	// Inicializar srandr
	randomSeed = 47 * time(NULL);
	// Variables del algoritmo genetico
	int i, mutation_start;
	char output_file2[32];
	double diferencia_fitness, fitness_anterior, fitness_actual;
	// Numero total de pixeles de la imagen
	int num_pixels = ancho*alto;
	// Tamaño de isla, es decir, la porcion de la poblacion de cada proceso
	int chunkSize = tam_poblacion/numProcesos;
	// Población total
	Individuo *poblacion = NULL;
	// Isla en la que los procesos hijos recibirán su porción de la población
	Individuo *islaPoblacion = NULL;
	// Array en la que cada proceso vuelca sus NEM mejores individuos para recibirlos el padre
	Individuo *islaPoblacionNEM = NULL;
	// Array recibe los NEM mejores individos de cada hilo
	Individuo *poblacionNEM = NULL;
	// Array para pasar los NPM mejores individuos a todos los hilos
	Individuo *poblacionNPM = NULL;
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FIN TODOS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



	// ******************************** INICIO SOLO EL PADRE ********************************************
	
	// ---------------- A. Se inicializa la poblacion y se calcula su fitness inicial -------------------
	if(idProceso == 0){
		// Reservamos memoria para un array de arrays que representa la poblacion
		poblacion = (Individuo *) malloc(tam_poblacion*sizeof(Individuo));
		assert(poblacion);

		poblacionNEM = (Individuo *) malloc(numProcesos*NEM*sizeof(Individuo));
		assert(poblacionNEM);

		// TODO: preguntar por que el profesor lo ha separado en dos bucles, quizas mejora rendimiento...
        for(i = 0; i < tam_poblacion; i++) {
			init_imagen_aleatoria(poblacion[i].imagen, max, num_pixels);
			poblacion[i].fitness = 0;
		}
		for (i = 0; i < tam_poblacion; i++) {
			fitness(imagen_objetivo, &poblacion[i], num_pixels);
		}

        // Ordenar individuos según la función de bondad (menor "fitness" --> más aptos)
		qsort(poblacion, tam_poblacion, sizeof(Individuo), comp_fitness);
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FIN SOLO EL PADRE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



	// ******************************** INICIO TODOS ********************************************

	// Reservamos memoria para porcion de subpoblacion de cada proceso (OJO: tambien se crea en el padre)
    islaPoblacion = (Individuo *) malloc(chunkSize*sizeof(Individuo));
	assert(islaPoblacion);
	islaPoblacionNEM = (Individuo *) malloc(NEM*sizeof(Individuo));
	assert(islaPoblacionNEM);
	poblacionNPM = (Individuo *) malloc(NPM*sizeof(Individuo));
	assert(poblacionNPM);

	// El padre suministra subpoblaciones (de chunkSize) a los hijos
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scatter(poblacion, chunkSize, typeIndividuo, islaPoblacion, chunkSize, typeIndividuo, 0, MPI_COMM_WORLD);	

	// ---------------- B. Evolucionar la Población (durante un número de generaciones) -------------------
	for(int idNGM = 0; idNGM < num_generaciones/NGM; idNGM++) {
		for (int ng = 0; ng < NGM; ng++){
			fitness_anterior = islaPoblacion[0].fitness;

			// Promocionar a los descendientes de los individuos más aptos
			for (i = 0; i < (chunkSize / 2) - 1; i += 2){
				cruzar(&islaPoblacion[i], &islaPoblacion[i + 1], &islaPoblacion[chunkSize / 2 + i], &islaPoblacion[chunkSize / 2 + i + 1], num_pixels);
			}

			// Mutar una parte de la individuos de la población (se decide que muten tam_poblacion/4)
			mutation_start = chunkSize / 4;

			for (i = mutation_start; i < chunkSize; i++){
				mutar(&islaPoblacion[i], max, num_pixels, prob_mutacion);
			}

			// Recalcular Fitness
			for (i = 0; i < chunkSize; i++){
				fitness(imagen_objetivo, &islaPoblacion[i], num_pixels);
			}

			// Ordenar individuos según la función de bondad (menor "fitness" --> más aptos)
			qsort(islaPoblacion, chunkSize, sizeof(Individuo), comp_fitness);
			
			// La mejor solución está en la primera posición del array
			fitness_actual = islaPoblacion[0].fitness;

			diferencia_fitness = abs(fitness_actual - fitness_anterior);
		}
		
		for(int i=0;i<NEM;i++){
			islaPoblacionNEM[i]=islaPoblacion[i];
		}

		MPI_Gather(islaPoblacionNEM, NEM, typeIndividuo, poblacionNEM, NEM, typeIndividuo, 0, MPI_COMM_WORLD);

		if (idProceso == 0){
			// Sustituir los NEM*numprocesos peores individuos de poblacion por los mejores NEM individuos de cada proceso
			int index_poblacion_nem = 0;
			for (int i = tam_poblacion - 1; i > tam_poblacion - 1 - (numProcesos * NEM); i--)
			{
				poblacion[i] = poblacionNEM[index_poblacion_nem];
				index_poblacion_nem++;
			}

			// Ordenar individuos según la función de bondad (menor "fitness" --> más aptos)
			qsort(poblacion, tam_poblacion, sizeof(Individuo), comp_fitness);

			for(int i=0;i<NPM;i++){
				poblacionNPM[i]=poblacion[i];
			}
		}

		// Compartimos los datos de la imagen leida a los hijos
		MPI_Bcast(poblacionNPM,NPM,typeIndividuo,0,MPI_COMM_WORLD);

		qsort(islaPoblacion,chunkSize,sizeof(Individuo),comp_fitness);

		int index_poblacion_npm = 0;

		for (int i = chunkSize - 1; i > chunkSize - 1 - NPM; i--)
		{
			islaPoblacion[i] = poblacionNPM[index_poblacion_npm];
			index_poblacion_npm++;
		}
	}

	// Liberamos las porciones de la poblacion de cada proceso
	free(islaPoblacion);
	islaPoblacion = NULL;
	free(islaPoblacionNEM);
	islaPoblacionNEM = NULL;
	free(poblacionNPM);
	poblacionNPM = NULL;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FIN TODOS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



	// ******************************** INICIO SOLO EL PADRE ********************************************

	MPI_Barrier(MPI_COMM_WORLD);
	if(idProceso == 0){
		memmove(imagen_resultado, poblacion[0].imagen, num_pixels * sizeof(RGB));
		free(poblacionNEM);
		poblacionNEM= NULL;
		free(poblacion);	
		poblacion = NULL;
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FIN SOLO EL PADRE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

}

void cruzar(Individuo *padre1, Individuo *padre2, Individuo *hijo1, Individuo *hijo2, int num_pixels)
{

	//? Generamos un numero aleatorio de entre 0 y el numero total de pixeles para elegir
	//? el lugar del corte a partir del cual se distribuyen los
	//? genes de un padre o del otro

	int random_number = aleatorio(num_pixels);
			for (int i = 0; i < random_number; i++)
			{
				hijo1->imagen[i] = padre1->imagen[i];
				hijo2->imagen[i] = padre2->imagen[i];
			}
			for (int i = random_number; i < num_pixels; i++)
			{
				hijo1->imagen[i] = padre2->imagen[i];
				hijo2->imagen[i] = padre1->imagen[i];
			}
}

void fitness(const RGB *objetivo, Individuo *individuo, int num_pixels)
{
	// Determina la calidad del individuo (similitud con el objetivo)
	// calculando la suma de la distancia existente entre los pixeles

	double diff = 0.0;

	individuo->fitness = 0;

	for (int i = 0; i < num_pixels; i++)
	{
		diff +=abs(objetivo[i].r - individuo->imagen[i].r) + abs(objetivo[i].g - individuo->imagen[i].g) + abs(objetivo[i].b - individuo->imagen[i].b);
	}
	
	individuo->fitness=diff;
}

void mutar(Individuo *actual, int max, int num_pixels, float prob_mutacion)
{
	for (int i = 0; i < num_pixels; i++)
	{
		if (aleatorio(1500)<= 1)
		{
			actual->imagen[i].r = aleatorio(max);
			actual->imagen[i].g = aleatorio(max);
			actual->imagen[i].b = aleatorio(max);
		}
	}
}
