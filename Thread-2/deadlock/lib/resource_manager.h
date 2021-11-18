#ifndef DEADLOCK_LIB_RESOURCE_MANAGER_H_
#define DEADLOCK_LIB_RESOURCE_MANAGER_H_

#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include "thread_manager.h"

namespace proj2 {

enum RESOURCE {
    GPU = 0,
    MEMORY,
    DISK,
    NETWORK
};

class ResourceManager {
public:
    ResourceManager(ThreadManager *t, std::map<RESOURCE, int> init_count): \
        resource_amount(init_count), tmgr(t) {}
    void budget_claim(std::map<RESOURCE, int> budget);
    int request(RESOURCE, int amount);
    void release(RESOURCE, int amount);
    int banker(std::map<RESOURCE, int> resource_amount_now, 
               std::map<std::thread::id, std::map<RESOURCE, int>> resource_allocation_now, 
               std::map<std::thread::id, std::map<RESOURCE, int>> resource_need_now,
               std::vector<std::thread::id> thread_now);
    void print_resource();
private:
    std::vector<std::thread::id> current_thread;
    std::mutex data_lock;
    std::map<std::thread::id, std::map<RESOURCE, int>> resource_max;
    std::map<std::thread::id, std::map<RESOURCE, int>> resource_allocation;
    std::map<std::thread::id, std::map<RESOURCE, int>> resource_need;
    std::map<RESOURCE, int> resource_amount;
    std::mutex banker_lock;
    std::condition_variable banker_cv;
    ThreadManager *tmgr;
};

}  // namespce: proj2

#endif