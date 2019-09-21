all:
	g++ -c ga/*.hpp
	g++ -c Simulador\ CANbus/*.hpp
	g++ ga/*.cpp Simulador\ CANbus/*.cpp -o ga.bin -lm
