motion:
	g++ motionTracking.cpp -o motion `pkg-config --cflags --libs opencv`

speed:
	g++ speedTracking.cpp -o speed `pkg-config --cflags --libs opencv`

