CXXFLAGS=-std=c++11 -O3 -fopenmp -march=native -mtune=native
LDFLAGS=-fopenmp 
CUDAFLAGS=-O3 -arch=sm_52 -std=c++11

polynomial:

polynomial_gpu:
	nvcc $(CUDAFLAGS) polynomial_gpu.cu -o polynomial_gpu

bench_cpu:
	./queue_cpu.sh

bench_gpu:
	./queue_gpu.sh

clean:
	-rm polynomial polunomial_gpu

assignment-cuda-polynomial.pdf: assignment-cuda-polynomial.tex
	pdflatex assignment-cuda-polynomial.tex

assignment-cuda-polynomial.tgz: assignment-cuda-polynomial.pdf \
                                Makefile \
                                polynomial_gpu.cu polynomial.cpp \
                                bench_cpu.sh bench_gpu.sh queue_cpu.sh queue_gpu.sh
	tar zcvf assignment-cuda-polynomial.tgz \
                 Makefile \
                 assignment-cuda-polynomial.pdf \
                 polynomial_gpu.cu polynomial.cpp \
                 bench_cpu.sh bench_gpu.sh queue_cpu.sh queue_gpu.sh
