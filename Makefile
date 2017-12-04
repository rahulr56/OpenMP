heat: newHeat.o
	mpicxx newHeat.cpp -o heat

clean:
	- rm -rf a.out heat core.*
