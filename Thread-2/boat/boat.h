#ifndef BOAT_H_
#define BOAT_H_

#include<stdio.h>
#include <unistd.h>

#include <string>   // string
#include <mutex> // lock
#include <condition_variable> // support for monitor
#include <thread> // multi-threading
#include <iostream>

#include "boatGrader.h"

namespace proj2{
class Boat{
public:
	Boat();
    ~Boat(){};
	void begin(int, int, BoatGrader*);
	std::mutex permission;
	std::condition_variable waiting_adult, waiting_child, waiting_back;
	int passengers = 0, count = 0;
	bool boat_at_Oahu = true, finished = false;
	void adult_thread(BoatGrader*);
	void child_thread(BoatGrader*);
};

}

#endif // BOAT_H_
