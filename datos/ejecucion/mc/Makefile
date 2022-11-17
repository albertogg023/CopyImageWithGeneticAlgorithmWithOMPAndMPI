CC = gcc
CFLAGS = -O3 -Wall -std=c99 -g

N_GEN = 
T_POB = 
N_HILOS_INI =
N_HILOS_FIT =

EXEC = omp
OUTFILE = ../output/out.txt
C_FILES = main.c imagen.c ga.c

omp: $(C_FILES)
	$(CC) $(CFLAGS) $(C_FILES) -o $(EXEC) -lm -fopenmp -DTIMEGenPobIni

test_omp:
	./$(EXEC) ../input/pitufo.ppm ../output/pitufo-out.ppm $(N_GEN) $(T_POB) $(N_HILOS_INI) $(N_HILOS_FIT) >> $(OUTFILE)

clean:
	rm -f $(EXEC)
