#include <vector>
#include <tuple>

#include <string>   // string
#include <chrono>   // timer
#include <iostream> // cout, endl
#include <mutex> // lock
#include <condition_variable> // support for monitor
#include <thread> // multi-threading

#include "lib/utils.h"
#include "lib/model.h" 
#include "lib/embedding.h" 
#include "lib/instruction.h"
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
            proj1::recommend_system(inst, users, items);
            break;
        }
    }

}
} // namespace proj1

int main(int argc, char *argv[]) {
    std::vector<std::thread> thread_total;
    proj1::EmbeddingHolder* users = new proj1::EmbeddingHolder("data/q2.in");
    proj1::EmbeddingHolder* items = new proj1::EmbeddingHolder("data/q2.in");
    proj1::Instructions instructions = proj1::read_instructrions("data/q2_instruction.tsv");
    for (unsigned i = 0; i < users->get_n_embeddings();i++){
        user_lock.push_back(mutex_wrapper());
    }
    for (unsigned i = 0; i < items->get_n_embeddings(); i++){
        item_lock.push_back(mutex_wrapper());
    }    
    {
    proj1::AutoTimer timer("q2");   // using this to print out timing of the block
    // Run all the instructions
    for (proj1::Instruction inst: instructions) {
        thread_total.push_back(std::thread(proj1::run_one_instruction, inst, users, items));
    }
    for (int i = 0; i < thread_total.size(); i++){
        thread_total.at(i).join();
    }
    }
    // Write the result
    users->write_to_stdout();
    items->write_to_stdout();

    // We only need to delete the embedding holders, as the pointers are all
    // pointing at the emb_matx of the holders.
    delete users;
    delete items;

    return 0;
}