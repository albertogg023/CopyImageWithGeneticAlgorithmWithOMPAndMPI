# CopyImageWithGeneticAlgorithmWithOMP-MPI

Este proyecto consiste en un algoritmo genético paralelo implementado en C++ utilizando OpenMP y MPI. El objetivo del programa es tomar una imagen de entrada y generar una nueva imagen que sea una copia de la imagen de entrada. El algoritmo genético utiliza paralelismo para optimizar la tarea.

## Contenido del Repositorio

- **Código Fuente:** El código fuente del programa se encuentra en la carpeta `code`. El algoritmo genético paralelo se implementa en varios archivos, incluyendo `main.c`, `imagen.c` y `ga.c`. Se utiliza OpenMP y MPI para aprovechar el paralelismo.

- **Makefile:** El archivo `Makefile` en la carpeta `code/src` permite compilar y ejecutar el programa de manera sencilla. Puedes utilizar este archivo para compilar el código fuente y ejecutar el programa.

- **Documentación:** En este repositorio, encontrarás tres documentos PDF que explican en detalle el proceso de paralelización y optimización del algoritmo genético:
  - `Memoria-OMP.pdf`: Documento que describe las paralelizaciones y optimizaciones realizadas con OpenMP.
  - `Memoria-MPI.pdf`: Documento que explica las paralelizaciones y optimizaciones realizadas con MPI.
  - `Memoria-MPI-&-OMP-&-Optimized.pdf`: Documento que aborda un estudio sobre cómo combinar ambos enfoques para obtener el máximo rendimiento y analiza los mejores valores de entrada para resolver el problema. Esto corresponde 

## Uso

### Aclaraciones
Cabe destacar que, la compilación y ejecución de este comando, tan solo funcionará en las máquinas para las que fue desarrollado el programa. Esto, debido a las dependencias necesarias y las características de los equipos en los que se ejecuta. Recordemos que, este programa ha sido optimizado para equipos, con un cierto número de procesadores, cores y distintas características más.


### Compilación del Programa
```bash
make mpi
```

### Ejecución del Programa

Una vez que hayas compilado el programa, puedes ejecutarlo con diferentes parámetros según tus necesidades. El comando general para ejecutar el programa con MPI es el siguiente:

```bash
make test_mpi
```
Este comando ejecutará el programa con MPI y utilizará los valores de entrada y otros parámetros que se definen en el mismo Makefile. 

### Salida del Programa
La salida del programa se guardará en la carpeta output con el nombre de archivo especificado en el comando de ejecución. La imagen de salida será una copia de la imagen de entrada, generada mediante el algoritmo genético implementado en el programa.
