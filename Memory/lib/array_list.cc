#include "array_list.h"

#include "memory_manager.h"

namespace proj3 {
    ArrayList::ArrayList(size_t sz, MemoryManager* cur_mma, int id){
        size = sz;
        mma = cur_mma;
        array_id = id;
    }
    int ArrayList::Read (unsigned long idx){
        //read the value in the virtual index of 'idx' from mma's memory space
        return mma->ReadPage(array_id, idx / PageSize, idx % PageSize);
    }
    void ArrayList::Write (unsigned long idx, int value){
        //write 'value' in the virtual index of 'idx' into mma's memory space
        mma->WritePage(array_id, idx / PageSize, idx % PageSize, value);
        return;
    }
    int ArrayList::Get_array_id(){
        return array_id;
    }
    ArrayList::~ArrayList(){
    }
} // namespce: proj3