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
            init_lock.lock();
            // We need to init the embedding
            int length = users->get_emb_length();
            Embedding* new_user = new Embedding(length);
            int user_idx = users->append(new_user);
            init_lock.unlock();
            for (int item_index: inst.payloads) {
                item_lock.at(item_index).lock();
                Embedding* item_emb = items->get_embedding(item_index);
                // Call cold start for downstream applications, slow
                EmbeddingGradient* gradient = cold_start(new_user, item_emb);
                users->update_embedding(user_idx, gradient, 0.01);
                delete gradient;
                item_lock.at(item_index).unlock();
            }
            while(user_lock.size() < user_idx){
                cond.wait(lck);
            }
            user_lock.push_back(mutex_wrapper());
            size_lock.unlock();
            cond.notify_all();
            break;
        }
        case UPDATE_EMB: {
            int user_idx = inst.payloads[0];
            int item_idx = inst.payloads[1];
            int label = inst.payloads[2];
            while (user_idx >= user_lock.size()){
                cond.wait(lck);
            }
            size_lock.unlock();
            user_lock.at(user_idx).lock();
            item_lock.at(item_idx).lock();
            Embedding* user = users->get_embedding(user_idx);
            Embedding* item = items->get_embedding(item_idx);
            EmbeddingGradient* gradient = calc_gradient(user, item, label);
            users->update_embedding(user_idx, gradient, 0.01);
            gradient = calc_gradient(item, user, label);
            items->update_embedding(item_idx, gradient, 0.001);
            delete gradient;
            item_lock.at(item_idx).unlock();
            user_lock.at(user_idx).unlock();
            break;
        }
        case RECOMMEND: {
            int user_idx = inst.payloads[0];
            Embedding* user = users->get_embedding(user_idx);
            std::vector<Embedding*> item_pool;
            int iter_idx = inst.payloads[1];
            for (unsigned int i = 2; i < inst.payloads.size(); ++i) {
                int item_idx = inst.payloads[i];
                item_pool.push_back(items->get_embedding(item_idx));
            }
            Embedding* recommendation = recommend(user, item_pool);
            recommendation->write_to_stdout();
            break;
        }
    }

}
} // namespace proj1

int main(int argc, char *argv[]) {
    std::vector<std::thread> thread_total;
    proj1::EmbeddingHolder* users = new proj1::EmbeddingHolder("data/q1.in");
    proj1::EmbeddingHolder* items = new proj1::EmbeddingHolder("data/q1.in");
    proj1::Instructions instructions = proj1::read_instructrions("data/q1_instruction.tsv");
    for (unsigned i = 0; i < users->get_n_embeddings(); i++){
        user_lock.push_back(mutex_wrapper());
    }
    for (unsigned i = 0; i < items->get_n_embeddings(); i++){
        item_lock.push_back(mutex_wrapper());
    }
    {
    proj1::AutoTimer timer("q1");   // using this to print out timing of the block
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