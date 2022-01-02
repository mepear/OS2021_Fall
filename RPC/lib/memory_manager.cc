#include "memory_manager.h"

namespace proj4 {
    PageFrame::PageFrame(){
    }
    int& PageFrame::operator[] (unsigned long idx){
        //each page should provide random access like an array
        return mem[idx];
    }
    void PageFrame::WriteDisk(std::string filename) {
        // write page content into disk files
        std::ofstream f;
        f.open(filename, std::ofstream::out | std::ofstream::trunc);
        f.write((char*)mem, PageSize * 4);
        f.close();
    }
    void PageFrame::ReadDisk(std::string filename) {
        // read page content from disk files
        std::ifstream f;
        f.open(filename, std::ifstream::in);
        f.read((char*)mem, PageSize * 4);
        f.close();
    }
    void PageFrame::ClearPage(){
        for (int i = 0; i < PageSize; i ++){
            mem[i] = 0;
        }
    }

    PageInfo::PageInfo(){
        ClearInfo();
    }
    void PageInfo::SetInfo(int cur_holder, int cur_vid){
        //modify the page states
        //you can add extra parameters if needed
        array_id = cur_holder;
        virtual_page_id = cur_vid;
    }
    void PageInfo::ClearInfo(){
        //clear the page states
        //you can add extra parameters if needed
        array_id = 0;
        virtual_page_id = 0;
    }

    int PageInfo::GetHolder(){
        return array_id;
    }
    int PageInfo::GetVid(){
        return virtual_page_id;
    }
    

    MemoryManager::MemoryManager(size_t sz){
        //mma should build its memory space with given space size
        //you should not allocate larger space than 'sz' (the number of physical pages) 
        mem = new PageFrame [sz];
        page_info = new PageInfo [sz];
        mma_sz = sz;
        clock_pointer = 0;
        next_array_id = 0;

        manipulating = new int [sz];
        for (int i = 0; i < sz; i ++){
            manipulating[i] = -1;
        }
    }
    MemoryManager::~MemoryManager(){
        delete mem;
        delete page_info;
        delete manipulating;
        page_map.clear();
    }
    void MemoryManager::PageOut(int array_id, int virtual_page_id, int physical_page_id){
        //swap out the physical page with the indx of 'physical_page_id out' into a disk file
        mem[physical_page_id].WriteDisk(
            std::to_string(array_id) + "|" + std::to_string(virtual_page_id)
        );
    }
    void MemoryManager::PageIn(int array_id, int virtual_page_id, int physical_page_id){
        //swap the target page from the disk file into a physical page with the index of 'physical_page_id out'
        mem[physical_page_id].ReadDisk(
            std::to_string(array_id) + "|" + std::to_string(virtual_page_id)
        );
    }
    int MemoryManager::PageReplace(int array_id, int virtual_page_id){
        //implement your page replacement policy here
        std::unique_lock<std::mutex> lck(lock);
        bool nw = false;
        int physical_page_id;
        if (page_map[array_id].find(virtual_page_id) != page_map[array_id].end()){
            physical_page_id = page_map[array_id][virtual_page_id];
            while (manipulating[physical_page_id] > -1){
                cv.wait(lck);
            }
            manipulating[physical_page_id] = array_id;
            if (page_info[physical_page_id].GetHolder() == array_id && page_info[physical_page_id].GetVid() == virtual_page_id){
                page_info[physical_page_id].use = true;
                return physical_page_id;
            } else {
                manipulating[physical_page_id] = -1;
                lck.unlock();
                cv.notify_all();
                lck.lock();
            }
        } else{nw = true;}

        if (replacement_policy == FIFO){
            physical_page_id = clock_pointer;
            clock_pointer = (clock_pointer + 1) % mma_sz;
        }
        else if(replacement_policy == CLOCK){
            while (page_info[clock_pointer].use){
                page_info[clock_pointer].use = false;
                clock_pointer = (clock_pointer + 1) % mma_sz;
            }
            
            while (manipulating[clock_pointer] > -1){
                cv.wait(lck);
            }
            physical_page_id = clock_pointer;
            clock_pointer = (clock_pointer + 1) % mma_sz;
        }

        page_info[physical_page_id].use = true;
        int old_array_id = page_info[physical_page_id].GetHolder(), old_virtual_page_id = page_info[physical_page_id].GetVid();
        page_info[physical_page_id].SetInfo(array_id, virtual_page_id);
        page_map[array_id][virtual_page_id] = physical_page_id;
        manipulating[physical_page_id] = array_id;
        lck.unlock();

        PageOut(old_array_id, old_virtual_page_id, physical_page_id);
        if (!nw)
            PageIn(array_id, virtual_page_id, physical_page_id);
        else 
            mem[physical_page_id].ClearPage();
        return physical_page_id;
    }
    int MemoryManager::ReadPage(int array_id, int virtual_page_id, int offset){
        // for arrayList of 'array_id', return the target value on its virtual space
        int physical_page_id = PageReplace(array_id, virtual_page_id);
        std::unique_lock<std::mutex>lck(lock);
        int result = mem[physical_page_id][offset];
        manipulating[physical_page_id] = -1;
        lck.unlock();
        cv.notify_all();
        return result;
    }
    void MemoryManager::WritePage(int array_id, int virtual_page_id, int offset, int value){
        // for arrayList of 'array_id', write 'value' into the target position on its virtual space
        int physical_page_id = PageReplace(array_id, virtual_page_id);
        std::unique_lock<std::mutex>lck(lock);
        mem[physical_page_id][offset] = value;
        manipulating[physical_page_id] = -1;
        lck.unlock();
        cv.notify_all();
    }
    int MemoryManager::Allocate(size_t sz){
        // when an application requires for memory, creat an
        std::unique_lock<std::mutex> lck(lock);
        next_array_id++;
        return next_array_id;
    }
    void MemoryManager::Release(int array_id){
        // an application will call release() function when destroying its arrayList
        // release the virtual space of the arrayList and erase the corresponding mappings
        std::unique_lock<std::mutex> lck(lock);
        page_map.erase(array_id);
    }
} // namespce: proj4