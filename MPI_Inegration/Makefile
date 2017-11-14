CXX=mpicxx
LD=mpicxx
CXXFLAGS=-O3 -std=c++11
CFLAGS=-O3
ARCHIVES=libfunctions.a


static_sched: static_sched.o
	$(LD) $(LDFLAGS) static_sched.o $(ARCHIVES) -o static_sched

dynamic_sched: dynamic_sched.o
	$(LD) $(LDFLAGS) dynamic_sched.o $(ARCHIVES) -o dynamic_sched


libfunctions.a: functions.o
	ar rcs libfunctions.a functions.o

libintegrate.a: sequential_lib.o
	ar rcs libintegrate.a sequential_lib.o

sequential: sequential.o 
	$(LD) $(LDFLAGS) sequential.o  libintegrate.a $(ARCHIVES) -o sequential


plots.pdf:
	./plots.sh

assignment-mpi-integration.tgz: \
	params.sh  plot_dynamic.sh  plot_static.sh  queue_dynamic.sh  queue_sequential.sh  queue_static.sh  run_dynamic.sh  run_sequential.sh  run_static.sh \
	dynamic_sched.cpp  sequential.cpp static_sched.cpp \
	libfunctions.a libintegrate.a \
	Makefile assignment-mpi-integration.pdf
	tar zcvf assignment-mpi-integration.tgz \
		params.sh  plot_dynamic.sh  plot_static.sh  queue_dynamic.sh  queue_sequential.sh  queue_static.sh  run_dynamic.sh  run_sequential.sh  run_static.sh \
		dynamic_sched.cpp  sequential.cpp static_sched.cpp \
		libfunctions.a libintegrate.a \
		Makefile assignment-mpi-integration.pdf
