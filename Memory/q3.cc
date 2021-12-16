#include<vector>
#include<thread>

#include "lib/array_list.h"
#include "lib/memory_manager.h"
#include "lib/utils.h"

const int workload_sz_1 = 4000, workload_sz_2 = 2000, workload_sz_3 = 100; 
const int loop_times = 100;

int main() {
    for (int i = 1; i <= 10; i ++){
        proj3::AutoTimer timer("Q3 FIFO test2 " + std::to_string(i));
        auto mma = new proj3::MemoryManager(i);
        mma->replacement_policy = FIFO;
        std::vector<proj3::ArrayList*>arr;
        for(int i = 0; i<loop_times; i++){
            arr.push_back(mma->Allocate(workload_sz_2));
            for(unsigned long j = 0; j < workload_sz_2; j++)arr[i]->Write(j, i);
        }
        for(int i = 0; i<loop_times; i++){
            if(i %2)mma->Release(arr[i]);
            else for(unsigned long j = 0; j < workload_sz_2; j++) assert(i == arr[i]->Read(j));
        }
        for(int i = 0; i<loop_times; i++){
            if(i %2 == 0)mma->Release(arr[i]);
        }
    }    
    for (int i = 1; i <= 10; i ++){
        proj3::AutoTimer timer("Q3 CLOCK test2 " + std::to_string(i));
        auto mma = new proj3::MemoryManager(i);
        mma->replacement_policy = CLOCK;
        std::vector<proj3::ArrayList*>arr;
        for(int i = 0; i<loop_times; i++){
            arr.push_back(mma->Allocate(workload_sz_2));
            for(unsigned long j = 0; j < workload_sz_2; j++)arr[i]->Write(j, i);
        }
        for(int i = 0; i<loop_times; i++){
            if(i %2)mma->Release(arr[i]);
            else for(unsigned long j = 0; j < workload_sz_2; j++) assert(i == arr[i]->Read(j));
        }
        for(int i = 0; i<loop_times; i++){
            if(i %2 == 0)mma->Release(arr[i]);
        }
    }    
}