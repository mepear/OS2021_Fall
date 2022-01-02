#include "array_list.h"

#include "mma_client.h"

namespace proj4 {
    ArrayList::ArrayList(size_t sz, MmaClient* cur_mmc, int id){
        size = sz;
        mmc = cur_mmc;
        array_id = id;
    }
    int ArrayList::Read (unsigned long idx){
        //read the value in the virtual index of 'idx' from mma's memory space
        return mmc->ReadPage(array_id, idx / PageSize, idx % PageSize);
    }
    void ArrayList::Write (unsigned long idx, int value){
        //write 'value' in the virtual index of 'idx' into mma's memory space
        mmc->WritePage(array_id, idx / PageSize, idx % PageSize, value);
        return;
    }
    int ArrayList::Get_array_id(){
        return array_id;
    }
    ArrayList::~ArrayList(){
    }
} // namespce: proj4