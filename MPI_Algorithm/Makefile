LD=mpicxx 
CXX=mpicxx 
LDFLAGS=-O3 -std=c++11 
CXXFLAGS=-O3 -std=c++11 

heat: heat.o
	$(LD) heat.o -o heat

clean:
	- rm -rf a.out heat core.* *.o
