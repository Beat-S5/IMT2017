main: extendedbinomialtree.o main.o
	g++ -o main extendedbinomialtree.o main.o -lQuantLib
extendedbinomialtree.o: extendedbinomialtree.cpp extendedbinomialtree.hpp
	g++ extendedbinomialtree.cpp -c -lQuantLib 
main.o: main.cpp extendedbinomialtree.hpp 
	g++ main.cpp -c -lQuantLib 


