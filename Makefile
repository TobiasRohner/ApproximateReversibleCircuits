
.PHONY: all
all: optimization_plots plots_vs_noise


.PHONY: clean
clean:
	rm -f optim.out 2of5_??.txt 4mod5_??.txt 5mod5_??.txt 6sym_??.txt 9sym_??.txt NthPrime?_??.txt xor5_??.txt
	rm -f optim.out .2of5.txt.dummy .4mod5.txt.dummy .5mod5.txt.dummy .6sym.txt.dummy .9sym.txt.dummy .NthPrime?.txt.dummy .xor5.txt.dummy
	rm -f optim.out 2of5.dat 4mod5.dat 5mod5.dat 6sym.dat 9sym.dat NthPrime?.dat xor5.dat
	rm -f optim.out 2of5.pdf 4mod5.pdf 5mod5.pdf 6sym.pdf 9sym.pdf NthPrime?.pdf xor5.pdf


.PHONY: optimization_plots
optimization_plots: 2of5.pdf 4mod5.pdf 5mod5.pdf 6sym.pdf NthPrime3.pdf NthPrime4.pdf Xor5.pdf


.PHONY: plots_vs_noise
plots_vs_noise: 2of5_vs_noise.pdf 4mod5_vs_noise.pdf 5mod5_vs_noise.pdf 6sym_vs_noise.pdf Xor5_vs_noise.pdf


optim.out: classical_circuit_optimizer.cc circuit.hh functions.hh instruction.hh mutation_strategy.hh optimizer.hh
	g++ $^ -o $@ -std=c++2a -O3 -march=native -fopenmp -lboost_program_options -g


.2of5.txt.dummy: optim.out
	seq -f "%02g" 1 16 | parallel ./$< -o 2of5_{}.txt -f 2of5 -l 6 -d 1 -D 20 -i 1 -S 100 -F 100 -b 32 -n 1 -s {}
	touch $@


.4mod5.txt.dummy: optim.out
	seq -f "%02g" 1 16 | parallel ./$< -o 4mod5_{}.txt -f 4mod5 -l 5 -d 1 -D 15 -i 1 -S 30 -F 50 -b 16 -n 1 -s {}
	touch $@


.5mod5.txt.dummy: optim.out
	seq -f "%02g" 1 16 | parallel ./$< -o 5mod5_{}.txt -f 5mod5 -l 6 -d 1 -D 30 -i 1 -S 60 -F 100 -b 32 -n 1 -s {}
	touch $@


.6sym.txt.dummy: optim.out
	seq -f "%02g" 1 16 | parallel ./$< -o 6sym_{}.txt -f 6sym -l 7 -d 1 -D 30 -i 1 -S 60 -F 100 -b 64 -n 1 -s {}
	touch $@


.9sym.txt.dummy: optim.out
	seq -f "%02g" 1 16 | parallel ./$< -o 9sym_{}.txt -f 9sym -l 10 -d 1 -D 40 -i 1 -S 60 -F 100 -b 180 -n 1 -s {}
	touch $@


.NthPrime3.txt.dummy: optim.out
	seq -f "%02g" 1 16 | parallel ./$< -o NthPrime3_{}.txt -f NthPrime3 -l 5 -d 1 -D 20 -i 1 -S 30 -F 50 -b 8 -n 1 -s {}
	touch $@


.NthPrime4.txt.dummy: optim.out
	seq -f "%02g" 1 16 | parallel ./$< -o NthPrime4_{}.txt -f NthPrime4 -l 6 -d 1 -D 30 -i 1 -S 30 -F 50 -b 16 -n 1 -s {}
	touch $@


.Xor5.txt.dummy: optim.out
	seq -f "%02g" 1 16 | parallel ./$< -o Xor5_{}.txt -f Xor5 -l 5 -d 1 -D 20 -i 1 -S 60 -F 100 -b 32 -n 1 -s {}
	touch $@


2of5.dat: post_processing.py .2of5.txt.dummy
	./$< 2of5


4mod5.dat: post_processing.py .4mod5.txt.dummy
	./$< 4mod5


5mod5.dat: post_processing.py .5mod5.txt.dummy
	./$< 5mod5


6sym.dat: post_processing.py .6sym.txt.dummy
	./$< 6sym


9sym.dat: post_processing.py .9sym.txt.dummy
	./$< 9sym


NthPrime3.dat: post_processing.py .NthPrime3.txt.dummy
	./$< NthPrime3


NthPrime4.dat: post_processing.py .NthPrime4.txt.dummy
	./$< NthPrime4


Xor5.dat: post_processing.py .Xor5.txt.dummy
	./$< Xor5


2of5.pdf: plot.py 2of5.dat
	./$< 2of5


4mod5.pdf: plot.py 4mod5.dat
	./$< 4mod5


5mod5.pdf: plot.py 5mod5.dat
	./$< 5mod5


6sym.pdf: plot.py 6sym.dat
	./$< 6sym


9sym.pdf: plot.py 9sym.dat
	./$< 9sym


NthPrime3.pdf: plot.py NthPrime3.dat
	./$< NthPrime3


NthPrime4.pdf: plot.py NthPrime4.dat
	./$< NthPrime4


Xor5.pdf: plot.py Xor5.dat
	./$< Xor5


2of5_vs_noise.pdf: plot_vs_error.py plot_vs_error/2of5/known.tfc plot_vs_error/2of5/optimized1.txt plot_vs_error/2of5/optimized2.txt
	./$< plot_vs_error/2of5/known.tfc plot_vs_error/2of5/optimized1.txt 2of5 20 0.01 1
	mv 2of5_vs_noise.pdf 2of5_vs_noise_alternative.pdf
	./$< plot_vs_error/2of5/known.tfc plot_vs_error/2of5/optimized2.txt 2of5 20 0.01 1


4mod5_vs_noise.pdf: plot_vs_error.py plot_vs_error/4mod5/known.tfc plot_vs_error/4mod5/optimized.txt
	./$^ 4mod5 20 0.01 1


5mod5_vs_noise.pdf: plot_vs_error.py plot_vs_error/5mod5/known.tfc plot_vs_error/5mod5/optimized.txt
	./$^ 5mod5 20 0.01 1


6sym_vs_noise.pdf: plot_vs_error.py plot_vs_error/6sym/known.tfc plot_vs_error/6sym/optimized.txt
	./$^ 6sym 20 0.01 1


Xor5_vs_noise.pdf: plot_vs_error.py plot_vs_error/Xor5/known.tfc plot_vs_error/Xor5/optimized.txt
	./$^ Xor5 20 0.01 1
