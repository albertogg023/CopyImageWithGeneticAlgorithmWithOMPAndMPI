/**
 *  Crea un nuevo tipo de datos derivado en MPI
 *  Necesario para el envio/recepcion de mensajes con datos de tipo Individuo
 **/
#include <mpi.h>
#include "../include/ga.h"

void crear_tipo_datos(int num_pixels, MPI_Datatype *rgb_type, MPI_Datatype *individuo_type)
{
	RGB ejemplo;	
	int blocklen1[3] = { 1, 1, 1 };
	int blocklen2[2] = { num_pixels, 1 };
	
	MPI_Datatype dtype[3] = { MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR };
	MPI_Aint addresses[3];
	MPI_Aint disp1[3];
	MPI_Aint disp2[2];
	
	MPI_Get_address(&ejemplo, &addresses[0]);
	MPI_Get_address(&(ejemplo.g), &addresses[1]);
	MPI_Get_address(&(ejemplo.b), &addresses[2]);
	
	disp1[0] = 0;
	disp1[1] = addresses[1] - addresses[0];
	disp1[2] = addresses[2] - addresses[0];
	
	MPI_Type_create_struct(3, blocklen1, disp1, dtype, rgb_type);
	MPI_Type_commit(rgb_type);
	
	MPI_Datatype dtype2[2] = { *rgb_type, MPI_DOUBLE };
	
	disp2[0] = offsetof(Individuo, imagen);
	disp2[1] = offsetof(Individuo, fitness);
	
	MPI_Type_create_struct(2, blocklen2, disp2, dtype2, individuo_type);
	MPI_Type_commit(individuo_type);
}
