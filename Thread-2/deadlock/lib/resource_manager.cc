#include <mutex>
#include <vector>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include "resource_manager.h"

namespace proj2 {

int ResourceManager::request(RESOURCE r, int amount) {
    if (amount <= 0)  return 1;
    auto this_id = std::this_thread::get_id();
    std::unique_lock<std::mutex> lk(banker_lock);
    int flag = 0;
    while (!flag) {
        this->banker_cv.wait_for(
            lk, std::chrono::milliseconds(100),
            [this, r, amount] { return this->resource_amount[r] >= amount; });
        
        this->data_lock.lock();
        auto resource_allocation_tmp = this->resource_allocation;
        auto resource_need_tmp = this->resource_need;
        auto resource_amount_tmp = this->resource_amount;
        auto current_thread_tmp = this->current_thread;
        this->data_lock.unlock();
        
        resource_amount_tmp[r] -= amount;
        resource_need_tmp[this_id][r] -= amount;
        resource_allocation_tmp[this_id][r] += amount;
        flag = ResourceManager::banker(resource_amount_tmp, resource_allocation_tmp, resource_need_tmp, current_thread);
        //std::cout << this_id << '\n';
        //std::cout << "GPU:" <<this->resource_amount[GPU] << "\n" << "MEMORY: " << this->resource_amount[MEMORY] << "\n";
        if (flag == 0){
            this->banker_lock.unlock();
            banker_cv.notify_all();
        }
    }
    this->data_lock.lock();
    this->resource_allocation[this_id][r] += amount;
    this->resource_need[this_id][r] -= amount;
    this->resource_amount[r] -= amount;
    this->data_lock.unlock();
    this->banker_lock.unlock();
    banker_cv.notify_all();
    return 0;
}

void ResourceManager::release(RESOURCE r, int amount) {
    if (amount <= 0)  return;
    auto this_id = std::this_thread::get_id();
    std::unique_lock<std::mutex> lk(this->data_lock);
    this->resource_amount[r] += amount;
    this->resource_need[this_id][r] += amount;
    this->resource_allocation[this_id][r] -= amount;
    this->data_lock.unlock();
    this->banker_cv.notify_all();
}

void ResourceManager::budget_claim(std::map<RESOURCE, int> budget) {
    // This function is called when some workload starts.
    // The workload will eventually consume all resources it claims
    auto this_id = std::this_thread::get_id();
    this->data_lock.lock();
    for (RESOURCE i = GPU; i <= NETWORK;i = RESOURCE(i+1)){
        this->resource_max[this_id][i] = budget[i];
        this->resource_need[this_id][i] = budget[i];
        this->resource_allocation[this_id][i] = 0;
    }
    this->current_thread.push_back(this_id);
    this->data_lock.unlock();
    return;
}

int ResourceManager::banker(std::map<RESOURCE, int> resource_amount_now, 
               std::map<std::thread::id, std::map<RESOURCE, int>> resource_allocation_now, 
               std::map<std::thread::id, std::map<RESOURCE, int>> resource_need_now,
               std::vector<std::thread::id> thread_now){
    // if (thread_now.size() > 1){
    // std::cout << "Amount\n";
    // std::cout << "GPU: " << resource_amount_now[GPU] << "MEMORY: " << resource_amount_now[MEMORY] << '\n';
    // std::cout << "Allocate Now\n";
    // std::cout << "GPU: " << resource_allocation_now[thread_now.at(0)][GPU] << "MEMORY: " << resource_allocation_now[thread_now.at(0)][MEMORY] << '\n';
    // std::cout << "GPU: " << resource_allocation_now[thread_now.at(1)][GPU] << "MEMORY: " << resource_allocation_now[thread_now.at(1)][MEMORY] << '\n';
    // std::cout << "Need Now\n";
    // std::cout << "GPU: " << resource_need_now[thread_now.at(0)][GPU] << "MEMORY: " << resource_need_now[thread_now.at(0)][MEMORY] << '\n';
    // std::cout << "GPU: " << resource_need_now[thread_now.at(1)][GPU] << "MEMORY: " << resource_need_now[thread_now.at(1)][MEMORY] << '\n';
    // }
    int length = thread_now.size();
    std::vector<int> finish(length, 0);
    int prev = -1, curr = 0;
    while (prev < curr){
        prev = curr;
        for(int i = 0; i < length;i++){
            if (finish.at(i) == 0){
                auto id = thread_now.at(i);
                int flag = 1;
                for(RESOURCE r = GPU; r <= NETWORK;r = RESOURCE(r+1)){
                    if (resource_amount_now[r] < resource_need_now[id][r]){
                        flag = 0;
                    }
                }
                if (flag == 1){
                    curr ++;
                    for(RESOURCE r = GPU; r <= NETWORK;r = RESOURCE(r+1)){
                        resource_amount_now[r] += resource_allocation_now[id][r];
                    }
                    finish.at(i) = 1;
                }
            }
        }
    }
    if (curr == length){
        return 1;
    }
    else{
        return 0;
    }
}

void ResourceManager::print_resource(){
    std::cout << "GPU: " << this->resource_amount[GPU] << "MEOMRY: " << this->resource_amount[MEMORY] << '\n';
}

} // namespace: proj2
