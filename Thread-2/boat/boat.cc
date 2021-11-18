#include <thread>
#include <vector>
#include <unistd.h>
#include <chrono>


#include "boat.h"

namespace proj2{
	
Boat::Boat(){
}

void Boat::adult_thread(BoatGrader *bg){
    bg->initializeAdult();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::unique_lock<std::mutex> lck(permission);
    waiting_adult.wait(lck);
while (bg->childrenLeft() >= 2 || !boat_at_Oahu || passengers == 1){
        waiting_adult.wait(lck);
    }
    bg->AdultRowToMolokai();
    boat_at_Oahu = false; //then wake up someone to row back
    lck.unlock();
    waiting_back.notify_all();
    return;
}

void Boat::child_thread(BoatGrader *bg){
    bg->initializeChild();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    bool on_Oahu = true, need_to_wake = false;
    while (!finished){
        std::unique_lock<std::mutex> lck(permission);
        need_to_wake = false;
        if (on_Oahu){
            while(!boat_at_Oahu){
                waiting_child.wait(lck);
            }
            if (boat_at_Oahu && passengers == 1){
                if (bg->childrenLeft() < 2 && bg->adultsLeft() == 0){//last pair of children
                    bg->ChildRideToMolokai();
                    boat_at_Oahu = false;
                    on_Oahu = false;
                    finished = true;
                    need_to_wake = true;
                }
                else{
                    bg->ChildRideToMolokai();
                    boat_at_Oahu = false;
                    on_Oahu = false;
                    need_to_wake = true;
                }
            }
            else if (bg->childrenLeft() < 2){
                if (bg->adultsLeft() == 0){//last child
                    bg->ChildRowToMolokai();
                    boat_at_Oahu = false;
                    on_Oahu = false;
                    finished = true;
                }
            }
            else{
                bg->ChildRowToMolokai();
                on_Oahu = false;
                passengers = 1;
            }
        }
        else{
            while (boat_at_Oahu){
                waiting_back.wait(lck);
            }
            if (finished){
                continue;
            }
            bg->ChildRowToOahu();
            boat_at_Oahu = true;
            on_Oahu = true;
            passengers = 0;
            need_to_wake = true;
        }
        if (need_to_wake){
            if (!boat_at_Oahu){
                lck.unlock();
                waiting_back.notify_all();
            }
            else{
                if (bg->childrenLeft() >= 2){
                    lck.unlock();
                    waiting_child.notify_all();
                }
                else{
                    lck.unlock();
                    waiting_adult.notify_all();
                }
            }
        }
    }
}

void Boat:: begin(int a, int b, BoatGrader *bg){
	std::thread threads[a + b];
	for (int i = 0; i < a; i++)
	{
		threads[i] = std::thread(&Boat::adult_thread, this, bg);
	}
	for (int i = 0; i < b; ++i)
	{
		threads[a + i] = std::thread(&Boat::child_thread, this, bg);
	}

	for (int i = 0; i < a + b; ++i)
	{
		threads[i].join();
	}

	return;
} 
}