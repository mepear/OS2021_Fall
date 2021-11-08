#ifndef THREAD_LIB_RECOMMEND_H_
#define THREAD_LIB_RECOMMEND_H_

#include <string>   // string
#include <chrono>   // timer
#include <iostream> // cout, endl
#include <mutex> // lock
#include <condition_variable> // support for monitor
#include <thread> // multi-threading

#include "utils.h"
#include "model.h" 
#include "embedding.h" 
#include "instruction.h"

extern std::vector<mutex_wrapper> item_lock;

namespace proj1 {

void new_start(int i, Instruction inst, EmbeddingGradient** gradients, Embedding* new_user, EmbeddingHolder* items, EmbeddingHolder* users);

void initial(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items, std::mutex &size_lock, std::mutex &init_lock, std::condition_variable &cond, std::vector<mutex_wrapper> &user_lock, std::unique_lock<std::mutex> &lck);

void update(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items, std::mutex &size_lock, std::mutex &init_lock, std::condition_variable &cond, std::vector<mutex_wrapper> &user_lock, std::unique_lock<std::mutex> &lck);

void recommend_system(Instruction inst, EmbeddingHolder* users, EmbeddingHolder* items);


} // namespace proj1

#endif // THREAD_LIB_RECOMMEND_H_