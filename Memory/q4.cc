#include<vector>
#include<thread>

#include "lib/array_list.h"
#include "lib/memory_manager.h"
#include "lib/utils.h"
const int thread_num = 10;

const size_t workload_sz = 2000;

void workload(proj3::MemoryManager * my_mma){
    proj3::ArrayList* arr = my_mma->Allocate(workload_sz);
    for(unsigned long j = 0; j < workload_sz; j++)arr->Write(j, j);
    for(unsigned long j = 0; j < workload_sz; j++)assert(j == arr->Read(j));
    my_mma->Release(arr);
}

int main() {
    for (int i = 10; i <= 20; i ++){
        proj3::AutoTimer timer("Q4 thread nuumber " + std::to_string(i));
        auto mma = new proj3::MemoryManager(i);
        mma->replacement_policy = CLOCK;
        std::vector<std::thread*> pool;
        for(int i = 0; i<thread_num; i++) {
            pool.push_back(new std::thread(&workload, mma));
        }

        for (auto t: pool) {
            t->join();
        }
    }   
}