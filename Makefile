compile:
	g++ motionTracking.cpp -o tracking `pkg-config --cflags --libs opencv`
