#include <gtest/gtest.h>
#include <chrono>
#include "recommend_system.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

std::vector<mutex_wrapper> item_lock;
double embA[20] = {-0.9166718150866153,0.43829227278389205,-0.6129205231425989,0.7738693110376684,-0.04456069156634124,0.8807831166061564,-0.8206278700342509,0.5013513447020947,0.8570578529268591,0.3906799520892723,0.6558968505921785,0.0011654760044317314,0.3008835791217257,-0.15983430198414705,-0.0693333837225647,-0.6526558237637923};
double embB[20] = {-0.9166718150866153,0.43829227278389205,-0.6129205231425989,0.7738693110376684,-0.04456069156634124,0.8807831166061564,-0.8206278700342509,0.5013513447020947,0.8570578529268591,0.3906799520892723,0.6558968505921785,0.0011654760044317314,0.3008835791217257,-0.15983430198414705,-0.0693333837225647,-0.6526558237637923};
double embC[20] = {-0.9166718150866153,0.43829227278389205,-0.6129205231425989,0.7738693110376684,-0.04456069156634124,0.8807831166061564,-0.8206278700342509,0.5013513447020947,0.8570578529268591,0.3906799520892723,0.6558968505921785,0.0011654760044317314,0.3008835791217257,-0.15983430198414705,-0.0693333837225647,-0.6526558237637923};
double result[20] = {0.000000,0.100000,0.200000,0.300000,0.400000,0.500000,0.600000,0.700000,0.800000,0.900000,1.000000,1.100000,1.200000,1.300000,1.400000,1.500000,1.600000,1.700000,1.800000,1.900000};

namespace proj1 {
namespace testing{
class RecommendSystemTest : public ::testing::Test {
    public:
        void SetUp() override {
        emb_testA = new Embedding(20, embA);
        emb_testB = new Embedding(20, embB);
        emb_testC = new Embedding(20, embC);
        result_emb = new Embedding(20, result);
        for (int i = 0; i < 20; i++){
            embhA.push_back(emb_testA);
        }
        for (int i = 0; i < 20; i++){
            embhB.push_back(emb_testB);
        }
        for (int i = 0; i < 20; i++){
            embhC.push_back(emb_testA);
        }
        embhC.push_back(result_emb);
        embh_testA = new EmbeddingHolder(embhA);
        embh_testB = new EmbeddingHolder(embhB);
        embh_testC = new EmbeddingHolder(embhC);
        std::vector<mutex_wrapper> user_lock;
        std::condition_variable cond;
        std::mutex size_lock;
        std::mutex init_lock;
        std::unique_lock<std::mutex> lck(size_lock);
        inst_testA = new Instruction("0"); // for initial test
        inst_testB = new Instruction("1 3 3 1 0"); // for update test
        }
    EmbeddingHolder* embh_testA;
    EmbeddingHolder* embh_testB;
    EmbeddingHolder* embh_testC;
    std::vector<mutex_wrapper> user_lock;
    std::condition_variable cond;
    std::mutex size_lock;
    std::mutex init_lock;
    std::unique_lock<std::mutex> lck;
    Instructions instructions;
    Embedding* emb_testA;
    Embedding* emb_testB;
    Embedding* emb_testC;
    Embedding* result_emb;
    std::vector<Embedding*>embhA;
    std::vector<Embedding*>embhB;
    std::vector<Embedding*>embhC;
    Instruction* inst_testA;
    Instruction* inst_testB;
};

TEST_F(RecommendSystemTest, initial){
    for (unsigned i = 0; i < embh_testA->get_n_embeddings(); i++){
        user_lock.push_back(mutex_wrapper());
    }
    for (unsigned i = 0; i < embh_testB->get_n_embeddings(); i++){
        item_lock.push_back(mutex_wrapper());
    }
	initial(*inst_testA, embh_testA, embh_testB, size_lock, init_lock, cond, user_lock, lck);
    EXPECT_EQ((*embh_testA == *embh_testC), true);
}

TEST_F(RecommendSystemTest, update){
     for (unsigned i = 0; i < embh_testA->get_n_embeddings(); i++){
         user_lock.push_back(mutex_wrapper());
     }
     for (unsigned i = 0; i < embh_testB->get_n_embeddings(); i++){
         item_lock.push_back(mutex_wrapper());
     }
 	update(*inst_testB, embh_testA, embh_testB, size_lock, init_lock, cond, user_lock, lck);
    double result_user_data[20] = {-0.912088,0.436101,-0.609856,0.770000,-0.044338,0.876379,-0.816525,0.498845,0.852773,0.388727,0.652617,0.001160,0.299379,-0.159035,-0.068987,-0.649393,0.000000,0.000000,0.000000,0.000000};
    double result_item_data[20] = {-0.916216,0.438074,-0.612616,0.773484,-0.044539,0.880345,-0.820220,0.501102,0.856631,0.390486,0.655571,0.001165,0.300734,-0.159755,-0.069299,-0.652331,0.000000,0.000000,0.000000,0.000000};
    Embedding* result_user = new Embedding(20, result_user_data);
    Embedding* result_item = new Embedding(20, result_item_data);
    for (int i = 0; i < 20; i++){
        //printf("%f\n", embh_testA->get_embedding(3)->get_data()[i]);
        EXPECT_EQ((*result_user == *embh_testA->get_embedding(3)), true);
        EXPECT_EQ((*result_item == *embh_testB->get_embedding(3)), true);
    }
}

} // namespace testing
} // namespace proj1

int main(int argc,char **argv){
  testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}