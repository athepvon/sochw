motion:
	g++ -std=c++11 motionTracking.cpp -o motion `pkg-config --cflags --libs opencv`
	
speed:
	g++ speedTracker.cpp -o speed `pkg-config --cflags --libs opencv`
	
clean:
	rm motion speed
