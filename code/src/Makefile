CC = mpicc
CFLAGS = -O3 -Wall -std=c99 -g -lm

N_GEN = 4000
T_POB = 256
P_MUT = 0.0001
N_THREADS = 12
NGM = 200
NEM = 4
NPM = 2

EXEC = sec
OUTFILE = ../output/out.txt
C_FILES = main.c imagen.c ga.c

sec: $(C_FILES)
	$(CC) $(CFLAGS) $(C_FILES) -o $(EXEC) -DTIME -DDEBUG

test_sec:
	mpirun ./$(EXEC) ../input/pitufo.ppm ../output/pitufo-out_$(N_GEN)_$(T_POB)_$(P_MUT)_$(N_THREADS)_$(NGM)_$(NEM)_$(NPM).ppm $(N_GEN) $(T_POB) $(P_MUT) $(N_THREADS) $(NGM) $(NEM) $(NPM) >> $(OUTFILE)

clean:
	rm -f $(EXEC)
