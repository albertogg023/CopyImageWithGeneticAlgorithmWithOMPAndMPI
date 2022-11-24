T_POB = 200
N_GEN = 4000
NGM = 200
NEM = 4
NPM = 2

H1 = "marte" 
NPH1 = 2
H2 = "venus"
NPH2 = 4

CC = mpicc
CFLAGS = -O3 -Wall -std=c99 -g -lm -fopenmp

EXEC = mpi
OUTFILE = ../output/out.txt
C_FILES = main.c imagen.c ga.c

mpi: $(C_FILES)
	$(CC) $(CFLAGS) $(C_FILES) -o $(EXEC) -DTIME -DDEBUG

test_mpi:
	mpirun -np $(NPH1) -host $(H1) $(EXEC) ../input/pitufo.ppm ../output/pitufo-out_$(N_GEN)_$(T_POB)_$(NGM)_$(NEM)_$(NPM).ppm $(N_GEN) $(T_POB) $(NGM) $(NEM) $(NPM) : -np $(NPH2) -host $(H2) $(EXEC) ../input/pitufo.ppm ../output/pitufo-out_$(N_GEN)_$(T_POB)_$(NGM)_$(NEM)_$(NPM).ppm $(N_GEN) $(T_POB) $(NGM) $(NEM) $(NPM) > $(OUTFILE)

clean:
	rm -f $(EXEC)