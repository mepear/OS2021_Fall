#include<vector>
#include<thread>

#include "lib/array_list.h"
#include "lib/memory_manager.h"
#include "lib/utils.h"

const int workload_sz_1 = 4000, workload_sz_2 = 2000, workload_sz_3 = 100; 
const int loop_times = 100;
const int metrix_length = 10;

std::vector<std::vector<int>>metrix = {{ 2850, 2895, 2940, 2985, 3030, 3075, 3120, 3165, 3210, 3255 },
{ 7350, 7495, 7640, 7785, 7930, 8075, 8220, 8365, 8510, 8655 },
{ 11850, 12095, 12340, 12585, 12830, 13075, 13320, 13565, 13810, 14055 },
{ 16350, 16695, 17040, 17385, 17730, 18075, 18420, 18765, 19110, 19455 },
{ 20850, 21295, 21740, 22185, 22630, 23075, 23520, 23965, 24410, 24855 },
{ 25350, 25895, 26440, 26985, 27530, 28075, 28620, 29165, 29710, 30255 },
{ 29850, 30495, 31140, 31785, 32430, 33075, 33720, 34365, 35010, 35655 },
{ 34350, 35095, 35840, 36585, 37330, 38075, 38820, 39565, 40310, 41055 },
{ 38850, 39695, 40540, 41385, 42230, 43075, 43920, 44765, 45610, 46455 },
{ 43350, 44295, 45240, 46185, 47130, 48075, 49020, 49965, 50910, 51855 },};

int main() {
    {
    proj3::AutoTimer timer("Q2 FIFO test1");
    auto mma = new proj3::MemoryManager(10);
    mma->replacement_policy = FIFO;
	proj3::ArrayList* arr = mma->Allocate(workload_sz_1);
    for(unsigned long i = 0; i<workload_sz_1; i++){
        arr->Write(i, 1);
    }
    for(unsigned long i = 0; i<workload_sz_1; i++){
        assert(1 == arr->Read(i));
    }
    mma->Release(arr);
    }
    {
    proj3::AutoTimer timer("Q2 FIFO test2");
    auto mma = new proj3::MemoryManager(10);
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
    {
    proj3::AutoTimer timer("Q2 FIFO test3");
    auto mma = new proj3::MemoryManager(10);
    mma->replacement_policy = FIFO;
	std::vector<proj3::ArrayList*>metrixA, metrixB, metrixC;
    for(int i = 0; i<metrix_length; i++){
        metrixA.push_back(mma->Allocate(metrix_length));
        metrixB.push_back(mma->Allocate(metrix_length));
        metrixC.push_back(mma->Allocate(metrix_length));
        for(int j = 0; j < metrix_length; j++){
            metrixA[i]->Write(j, i*metrix_length+j);
            metrixB[i]->Write(j, i*metrix_length+j);
        }
    }
    
    for(int i = 0; i<metrix_length; i++){
        for(int j = 0; j<metrix_length; j++){
            for(int k = 0; k < metrix_length; k++){
                metrixC[i]->Write(j, metrixC[i]->Read(j)+metrixA[i]->Read(k)*metrixB[k]->Read(j));
            }
        }
    }

    for(int i = 0; i<metrix_length; i++){
        for(int j = 0; j<metrix_length; j++){
            assert(metrix[i][j] == metrixC[i]->Read(j));
        }
    }

    for(int i = 0; i<metrix_length; i++){
        mma->Release(metrixA[i]);
        mma->Release(metrixB[i]);
        mma->Release(metrixC[i]);
    }
    }
    {
    proj3::AutoTimer timer("Q2 CLOCK test1");
    auto mma = new proj3::MemoryManager(10);
    mma->replacement_policy = CLOCK;
	proj3::ArrayList* arr = mma->Allocate(workload_sz_1);
    for(unsigned long i = 0; i<workload_sz_1; i++){
        arr->Write(i, 1);
    }
    for(unsigned long i = 0; i<workload_sz_1; i++){
        assert(1 == arr->Read(i));
    }
    mma->Release(arr);
    }
    {
    proj3::AutoTimer timer("Q2 CLOCK test2");
    auto mma = new proj3::MemoryManager(10);
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
    {
    proj3::AutoTimer timer("Q2 CLOCK test3");
    auto mma = new proj3::MemoryManager(10);
    mma->replacement_policy = CLOCK;
	std::vector<proj3::ArrayList*>metrixA, metrixB, metrixC;
    for(int i = 0; i<metrix_length; i++){
        metrixA.push_back(mma->Allocate(metrix_length));
        metrixB.push_back(mma->Allocate(metrix_length));
        metrixC.push_back(mma->Allocate(metrix_length));
        for(int j = 0; j < metrix_length; j++){
            metrixA[i]->Write(j, i*metrix_length+j);
            metrixB[i]->Write(j, i*metrix_length+j);
        }
    }
    
    for(int i = 0; i<metrix_length; i++){
        for(int j = 0; j<metrix_length; j++){
            for(int k = 0; k < metrix_length; k++){
                metrixC[i]->Write(j, metrixC[i]->Read(j)+metrixA[i]->Read(k)*metrixB[k]->Read(j));
            }
        }
    }

    for(int i = 0; i<metrix_length; i++){
        for(int j = 0; j<metrix_length; j++){
            assert(metrix[i][j] == metrixC[i]->Read(j));
        }
    }

    for(int i = 0; i<metrix_length; i++){
        mma->Release(metrixA[i]);
        mma->Release(metrixB[i]);
        mma->Release(metrixC[i]);
    }
    }
}