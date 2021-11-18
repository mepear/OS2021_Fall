#include <thread>
#include "recommend_system.h"

namespace proj1 {

void new_start(int i, Instruction inst, EmbeddingGradient** gradients, Embedding* new_user, EmbeddingHolder* items, EmbeddingHolder* users){
    item_lock.at(inst.payloads[i]).lock();
    Embedding* item_emb = items->get_embedding(inst.payloads[i]);
    item_lock.at(inst.payloads[i]).unlock();
    gradients[i] = cold_start(new_user, item_emb);
    //delete item_emb;
    return;
}

void initial(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items, std::mutex &size_lock, std::mutex &init_lock, std::condition_variable &cond, std::vector<mutex_wrapper> &user_lock, std::unique_lock<std::mutex> &lck){
    init_lock.lock();
    // We need to init the embedding
    int length = users->get_emb_length();
    Embedding* new_user = new Embedding(length);
    int user_idx = users->append(new_user);
    init_lock.unlock();
    EmbeddingGradient** gradients = new EmbeddingGradient*[int(inst.payloads.size())];
    std::vector<std::thread> thread_cold;
    for (int i = 0; i < inst.payloads.size(); i++) {
        thread_cold.push_back(std::thread(proj1::new_start, i, inst, gradients, new_user, items, users));
    }
    for (int i = 0; i < thread_cold.size(); i++){
        thread_cold.at(i).join();
    }
    for (int i = 0; i < inst.payloads.size(); i++){
        users->update_embedding(user_idx, gradients[i], 0.01);
    }
    delete gradients;
    while(user_lock.size() < user_idx){
        cond.wait(lck);
    }
    user_lock.push_back(mutex_wrapper());
    size_lock.unlock();
    cond.notify_all();
    return;
}

void update(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items, std::mutex &size_lock, std::mutex &init_lock, std::condition_variable &cond, std::vector<mutex_wrapper> &user_lock, std::unique_lock<std::mutex> &lck){
    int user_idx = inst.payloads[0];
    int item_idx = inst.payloads[1];
    int label = inst.payloads[2];
    while (user_idx >= user_lock.size()){
        cond.wait(lck);
    }
    size_lock.unlock();
    user_lock.at(user_idx).lock();
    item_lock.at(item_idx).lock();
    Embedding* item = items->get_embedding(item_idx);
    Embedding* user = users->get_embedding(user_idx);
    EmbeddingGradient* gradient = calc_gradient(user, item, label);
    users->update_embedding(user_idx, gradient, 0.01);
    gradient = calc_gradient(item, user, label);
    items->update_embedding(item_idx, gradient, 0.001);
    delete gradient;
    item_lock.at(item_idx).unlock();
    user_lock.at(user_idx).unlock();
    return;
}

void recommend_system(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items){
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
    return;
}

void recommend_system_outplace(Instruction inst, EmbeddingHolder* users_copy, EmbeddingHolder* items_copy){
    int user_idx = inst.payloads[0];
    Embedding* user = users_copy->get_embedding(user_idx);
    std::vector<Embedding*> item_pool;
    int iter_idx = inst.payloads[1];
    for (unsigned int i = 2; i < inst.payloads.size(); ++i) {
        int item_idx = inst.payloads[i];
        item_pool.push_back(items_copy->get_embedding(item_idx));
    }
    Embedding* recommendation = recommend(user, item_pool);
    recommendation->write_to_stdout();
    return;
}

} // namespace proj1