all:
	g++ main.cpp mqtt_in.cpp mqtt_out.cpp -std=c++0x -pthread -lmosquitto -lmosquittopp -lcurl -ljsoncpp -o go
