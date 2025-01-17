#ifndef MEMORY_MANAGER_H_
#define MEMORY_MANAGER_H_

#include <assert.h>
#include <map>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#define FIFO 0
#define CLOCK 1

#define PageSize 1024

namespace proj3 {

class PageFrame {
public:
    PageFrame();
    int& operator[] (unsigned long);
    void WriteDisk(std::string);
    void ReadDisk(std::string);
    void ClearPage();
private:
    int mem[PageSize];
};

class PageInfo {
public:
    PageInfo();
    void SetInfo(int,int);
    void ClearInfo();
    int GetHolder();
    int GetVid();
    bool use = true;
private:
    int holder; //page holder id (array_id)
    int virtual_page_id; // page virtual #
    /*add your extra states here freely for implementation*/
    int array_id;
};

class ArrayList;

class MemoryManager {
public:
    // you should not modify the public interfaces used in tests
    MemoryManager(size_t);
    int ReadPage(int array_id, int virtual_page_id, int offset);
    void WritePage(int array_id, int virtual_page_id, int offset, int value);
    ArrayList* Allocate(size_t);
    void Release(ArrayList*);
    ~MemoryManager();
    int replacement_policy = CLOCK; 
private:
    std::map<int, std::map<int, int>> page_map; // // mapping from ArrayList's virtual page # to physical page #
    PageFrame* mem; // physical pages, using 'PageFrame* mem' is also acceptable 
    PageInfo* page_info; // physical page info
    unsigned int* free_list;  // use bitmap implementation to identify and search for free pages
    int next_array_id;
    size_t mma_sz;
    /*add your extra states here freely for implementation*/
    std::mutex lock;
    std::condition_variable cv;
    int * manipulating;
    int clock_pointer;

    void PageIn(int array_id, int virtual_page_id, int physical_page_id);
    void PageOut(int array_id, int virtual_page_id, int physical_page_id);
    int PageReplace(int array_id, int virtual_page_id);
};

}  // namespce: proj3

#endif

