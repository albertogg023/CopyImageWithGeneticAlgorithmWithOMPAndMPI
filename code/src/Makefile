T_POB = 200
N_GEN = 4000
NGM = 200
NEM = 4
NPM = 2

NP = 6
H1 = "marte" 
H2 = "venus"

CC = mpicc
CFLAGS = -O3 -Wall -std=c99 -g -lm

EXEC = mpi
OUTFILE = ../output/out.txt
C_FILES = main.c imagen.c ga.c

mpi: $(C_FILES)
	$(CC) $(CFLAGS) $(C_FILES) -lm -o $(EXEC) -fopenmp -DTIME -DDEBUG

test_mpi:
	mpirun -np $(NP) -host $(H1) $(EXEC) ../input/pitufo.ppm ../output/pitufo-out_$(N_GEN)_$(T_POB)_$(NGM)_$(NEM)_$(NPM).ppm $(N_GEN) $(T_POB) $(NGM) $(NEM) $(NPM) : -np $(NP) -host $(H2) $(EXEC) ../input/pitufo.ppm ../output/pitufo-out_$(N_GEN)_$(T_POB)_$(NGM)_$(NEM)_$(NPM).ppm $(N_GEN) $(T_POB) $(NGM) $(NEM) $(NPM) > $(OUTFILE)

clean:
	rm -f $(EXEC)