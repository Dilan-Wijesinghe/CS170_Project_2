# Executable for ml
ml: ml.o
	g++ -std=c++17 ml.o -o ml

ml.o: ml.cpp
	g++ -std=c++17  -c  ml.cpp

clean:
	rm  *.o ml
