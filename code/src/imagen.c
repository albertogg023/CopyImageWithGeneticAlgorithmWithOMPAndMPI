#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>
#include "../include/imagen.h"

struct Pixel{

	int r,g,b;

};

int size;

RGB *leer_ppm(const char *file, int *ancho, int *alto, int *max)
{
	int i, n;
	FILE *fd;
	
	char c, b[100];
	int red, green, blue;
	
	fd = fopen(file, "r");
	
	n = fscanf(fd, "%[^\n] ", b);
	if(b[0] != 'P' || b[1] != '3') {
		printf("%s no es una imagen PPM\n", file);
		exit(0);
	}
	
	printf("Leyendo fichero PPM %s\n", file);
	n = fscanf(fd, "%c", &c);
	while(c == '#') {
		n = fscanf(fd, "%[^\n] ", b);
		printf("%s\n", b);
		n = fscanf(fd, "%c", &c);
	}
	
	ungetc(c, fd);
	n = fscanf(fd, "%d %d %d", ancho, alto, max);
	assert(n == 3);
	
	size = (*ancho)*(*alto);
	
	RGB *imagen = (RGB *) malloc(size*sizeof(RGB));
	assert(imagen);
	
	for(i=0; i<size; i++) {				//? recorremos solo hasta <size porque si no nos pasamos de iteraciones ( >255)
		n = fscanf(fd, "%d %d %d", &red, &green, &blue);
		assert(n == 3);
		
		imagen[i].r = (char)red;
		imagen[i].g = (char)green;
		imagen[i].b = (char)blue;
	}
	
	fclose(fd);
	return imagen;
}

void escribir_ppm(const char *fichero, int ancho, int alto, int max, const RGB *imagen)
{
	int i;
	FILE *fd;
	
	fd = fopen(fichero, "w");
	
	fprintf(fd, "P3\n");
	fprintf(fd, "%d %d\n%d\n", ancho, alto, max);
	
	int size = alto*ancho;
	for(i=0; i<size; i++) {			//? recorremos solo hasta <size
		fprintf(fd, "%d %d %d ", imagen[i].r, imagen[i].g, imagen[i].b);
		if((i+1) % 18 == 0) { fprintf(fd, "\n"); }
	}
	fclose(fd);
}

struct Pixel get_value_from_matrix(int x , int y,int ancho,int alto,RGB *imagen){

	//? Consigue la suma de valores rgb de la celda

	//* Por cada fila le sumammos el ancho

	int translation_number=y*ancho;

	//* Sumamos el desplazamiento horizontal al vertical

	int cell=translation_number+x;

	struct Pixel p;

	p.r=imagen[cell].r;
	p.g=imagen[cell].g;
	p.b=imagen[cell].b;

	return p;

}

int get_position_from_matrix(int x , int y,int ancho,int alto,RGB *imagen){

	//? Consigue la posicion en imagen de una coordenada x e y

	//* Por cada fila le sumammos el ancho

	int translation_number=y*ancho;

	//* Sumamos el desplazamiento horizontal al vertical

	return translation_number+x;

}


void suavizar_pixel(int x,int y ,int ancho , int alto,RGB *imagen_aux,RGB *imagen){

	int available_near_cells = 0;

	struct Pixel pixel;

	struct Pixel softened_pixel;

	softened_pixel.r=0;
	softened_pixel.g=0;
	softened_pixel.b=0;

	if (y - 1 >= 0 && x - 1 >= 0) // up left
	{

		pixel = get_value_from_matrix(y - 1, x - 1, ancho, alto, imagen_aux);
		available_near_cells += 1;
		softened_pixel.r += pixel.r;
		softened_pixel.g += pixel.g;
		softened_pixel.b += pixel.b;
	}

	if (y - 1 >= 0 && x + 1 < ancho) // up right
	{

		pixel = get_value_from_matrix(y - 1, x + 1, ancho, alto, imagen_aux);
		available_near_cells += 1;
		softened_pixel.r += pixel.r;
		softened_pixel.g += pixel.g;
		softened_pixel.b += pixel.b;
	}
	if (y + 1 < alto) // down left
	{

		pixel = get_value_from_matrix(y + 1, x, ancho, alto, imagen_aux);
		available_near_cells += 1;
		softened_pixel.r += pixel.r;
		softened_pixel.g += pixel.g;
		softened_pixel.b += pixel.b;
	}

	if (y + 1 < alto && x + 1 < ancho) // down right
	{
		pixel = get_value_from_matrix(y + 1, x + 1, ancho, alto, imagen_aux);
		available_near_cells += 1;
		softened_pixel.r += pixel.r;
		softened_pixel.g += pixel.g;
		softened_pixel.b += pixel.b;
	}

	if (y - 1 >= 0) // up
	{
		pixel = get_value_from_matrix(y - 1, x, ancho, alto, imagen_aux);
		available_near_cells += 1;
		softened_pixel.r += pixel.r;
		softened_pixel.g += pixel.g;
		softened_pixel.b += pixel.b;
	}

	if (x - 1 >= 0) // left
	{
		pixel = get_value_from_matrix(y, x - 1, ancho, alto, imagen_aux);
		available_near_cells += 1;
		softened_pixel.r += pixel.r;
		softened_pixel.g += pixel.g;
		softened_pixel.b += pixel.b;
	}

	if (x + 1 < ancho) // right
	{
		pixel = get_value_from_matrix(y, x + 1, ancho, alto, imagen_aux);
		available_near_cells += 1;
		softened_pixel.r += pixel.r;
		softened_pixel.g += pixel.g;
		softened_pixel.b += pixel.b;
	}

	if (y + 1 < alto) // down
	{
		pixel = get_value_from_matrix(y + 1, x, ancho, alto, imagen_aux);
		available_near_cells += 1;
		softened_pixel.r += pixel.r;
		softened_pixel.g += pixel.g;
		softened_pixel.b += pixel.b;
	}

	int pixel_position=get_position_from_matrix(y,x,ancho,alto,imagen_aux);

	imagen[pixel_position].r=softened_pixel.r/available_near_cells;
	imagen[pixel_position].g=softened_pixel.g/available_near_cells;
	imagen[pixel_position].b=softened_pixel.b/available_near_cells;

}

void suavizar(int ancho, int alto, RGB *imagen, int idProceso, int numProcesos, MPI_Datatype typeRGB)
{
	// imagen auxiliar
	RGB *imagen_auxiliar = (RGB *)malloc(alto * ancho * sizeof(RGB));

	// filas asignadas a cada proceso
	int processRows = floor(alto / numProcesos);

	// buffer en el que almacenaremos las secciones del array
	// el tamÃ±ao sera igual a las filas asignadas a cada proceso
	RGB *bufferRGB = (RGB *)malloc(processRows * ancho * sizeof(RGB));

	//el proceso master crea la copia y la pasa a los hijos
	if (idProceso == 0)
	{
		
		for (int i = 0; i < alto * ancho; i++)
		{
			imagen_auxiliar[i].r = imagen[i].r;
			imagen_auxiliar[i].g = imagen[i].g;
			imagen_auxiliar[i].b = imagen[i].b;
		}
		MPI_Bcast(imagen_auxiliar, ancho * alto, typeRGB, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Bcast(imagen_auxiliar, ancho * alto, typeRGB, 0, MPI_COMM_WORLD);
	}

	MPI_Status status;

	//Calculamos la ultima fila que le toca al proceso actual en funcion de su identificador
	int ultimaFIla = (idProceso * processRows) + processRows;

	//Cada uno suaviza su seccion concreta
	for (int i = idProceso * processRows; i < ultimaFIla; i++)
	{
		for (int j = 0; j < ancho; j++)
		{
			suavizar_pixel(i, j, ancho, alto, imagen_auxiliar, imagen);
		}
	}

	if (idProceso == 0)
	{
		// Si no se han suavizado todas las filas las suavizara el padre
		int ultimaFilaSuavizada = ((numProcesos - 1) * processRows) + processRows;
		if (ultimaFilaSuavizada != alto)
		{
			for (int i = ultimaFilaSuavizada; i < alto; i++)
			{
				for (int j = 0; j < ancho; j++)
				{
					suavizar_pixel(i, j, ancho, alto, imagen_auxiliar, imagen);
				}
			}
		}

		// Se Reciben las distintas secciones de los hijos y se transladan a la imagen del master
		for (int i = 1; i < numProcesos; i++)
		{

			MPI_Recv(bufferRGB, processRows * ancho, typeRGB, i, 0, MPI_COMM_WORLD, &status);

			for (int j = 0; j < processRows * ancho; j++)
			{
				imagen[i * processRows * ancho + j] = bufferRGB[j];
			}
		}
	}
	else
	{
		//enviamos la seccion que hayamos suavizado
		MPI_Ssend(&imagen[idProceso * processRows * ancho], processRows * ancho, typeRGB, 0, 0, MPI_COMM_WORLD);
	}
}
