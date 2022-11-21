CC = mpicc
CFLAGS = -O3 -Wall -std=c99 -g -lm

T_POB = 200
P_MUT = 0.0001
N_THREADS = 4
NGM = 400
NEM = 4
NPM = 2

EXEC = mpi
OUTFILE = ../output/out.txt
C_FILES = main.c imagen.c ga.c

mpi: $(C_FILES)
	$(CC) $(CFLAGS) $(C_FILES) -lm -o $(EXEC) -fopenmp -DTIME -DDEBUG

test_mpi:
	mpirun -np $(NP) -host $(H1) $(EXEC) ../input/pitufo.ppm ../output/pitufo-out_$(N_GEN)_$(T_POB)_$(P_MUT)_$(N_THREADS)_$(NGM)_$(NEM)_$(NPM).ppm $(N_GEN) $(T_POB) $(P_MUT) $(N_THREADS) $(NGM) $(NEM) $(NPM) : -np $(NP) -host $(H2) $(EXEC) ../input/pitufo.ppm ../output/pitufo-out_$(N_GEN)_$(T_POB)_$(P_MUT)_$(N_THREADS)_$(NGM)_$(NEM)_$(NPM).ppm $(N_GEN) $(T_POB) $(P_MUT) $(N_THREADS) $(NGM) $(NEM) $(NPM) > $(OUTFILE)

clean:
	rm -f $(EXEC)
