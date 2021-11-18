#include <vector>
#include <tuple>

#include <string>   // string
#include <chrono>   // timer
#include <iostream> // cout, endl
#include <mutex> // lock
#include <algorithm>
#include <condition_variable> // support for monitor
#include <thread> // multi-threading

#include "lib/utils.h"
#include "lib/model.h" 
#include "lib/embedding.h" 
#include "lib/recommend_system.h"


std::vector<mutex_wrapper> user_lock;
std::vector<mutex_wrapper> item_lock;
std::condition_variable cond;
std::mutex size_lock;
std::mutex init_lock;
std::unique_lock<std::mutex> lck(size_lock);

namespace proj1 {


void run_one_instruction(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items) {
    switch(inst.order) {
        case INIT_EMB: {
            proj1::initial(inst, users, items, size_lock, init_lock, cond, user_lock, lck);
            break;
        }
        case UPDATE_EMB: {
            proj1::update(inst, users, items, size_lock, init_lock, cond, user_lock, lck);
            break;
        }
        case RECOMMEND: {
            proj1::recommend_system_outplace(inst, users, items);
            break;
        }
    }

}
} // namespace proj1


int main(int argc, char *argv[]) {
    std::vector<std::thread> thread_total;
    proj1::EmbeddingHolder* users = new proj1::EmbeddingHolder("data/q3.in");
    proj1::EmbeddingHolder* items = new proj1::EmbeddingHolder("data/q3.in");
    proj1::Instructions instructions = proj1::read_instructrions("data/test_2.tsv");
    for (unsigned i = 0; i < users->get_n_embeddings();i++){
        user_lock.push_back(mutex_wrapper());
    }
    for (unsigned i = 0; i < items->get_n_embeddings(); i++){
        item_lock.push_back(mutex_wrapper());
    }
    {
    proj1::AutoTimer timer("q5");   // using this to print out timing of the block
    proj1::Instructions recommends;
    // Some Pre-processing
    int max_recomd = -1;
    int max_epoch = 0;
    for (proj1::Instruction inst: instructions) {
        if (inst.order == proj1::INIT_EMB){
            thread_total.push_back(std::thread(proj1::run_one_instruction, inst, users, items));
        }
        if (inst.order == proj1::UPDATE_EMB){
            if (inst.payloads[3] > max_epoch){
                max_epoch = inst.payloads[3];
            }
        }
        if (inst.order == proj1::RECOMMEND){
            if (inst.payloads[1] > max_recomd){
                max_recomd = inst.payloads[1];
            }
        }
    }
    std::vector<proj1::Instructions> recommend_instructions(max_recomd + 2, std::vector<proj1::Instruction>());
    std::vector<proj1::Instructions> update_instructions(max_epoch + 1, std::vector<proj1::Instruction>());
    for (proj1::Instruction inst: instructions) {
        if (inst.order == proj1::RECOMMEND){
            recommend_instructions.at(inst.payloads[1] + 1).push_back(inst);
        }
        if (inst.order == proj1::UPDATE_EMB){
            update_instructions.at(inst.payloads[3]).push_back(inst);
        }
    }
    int itr = 1;
    if (max_recomd + 2 > max_epoch + 1){
        itr = max_recomd + 2;
    }
    else {
        itr = max_epoch + 1;
    }
    
    // Finish all INIT_EMB's
    for (int i = 0; i < thread_total.size(); i++){
        thread_total.at(i).join();
    }
    thread_total.clear();

    // Finish Recommend and Update in an alternative way
    for (int i = 0; i < itr; i++){
        //std::cout<<i<<std::endl;
        proj1::EmbeddingHolder* users_copy = users->deep_copy();
        proj1::EmbeddingHolder* items_copy = items->deep_copy();
        if(recommend_instructions.size() > i){
            if (recommend_instructions.at(i).size() > 0){
                for (proj1::Instruction inst: recommend_instructions.at(i)){
                    thread_total.push_back(std::thread(proj1::run_one_instruction, inst, users_copy, items_copy));
                }
                // for (int j = 0; j < thread_total.size(); j++){
                //     thread_total.at(j).join();
                // }
                // thread_total.clear();
            }
        }
        if (update_instructions.size() > i){
            if (update_instructions.at(i).size() > 0){
                for (proj1::Instruction inst: update_instructions.at(i)){                    
                    thread_total.push_back(std::thread(proj1::run_one_instruction, inst, users, items));
                }
            }
        }
        for (int j = 0; j < thread_total.size(); j++){
            thread_total.at(j).join();
        }
        thread_total.clear();
    }
    }

    // Write the result
    // users->write_to_stdout();
    // items->write_to_stdout();
    return 0;
}