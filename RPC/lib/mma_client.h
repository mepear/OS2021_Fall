#ifndef MMA_CLIENT_H
#define MMA_CLIENT_H

#include <memory>
#include <cstdlib>
#include <thread>

#include <grpc++/grpc++.h>

#ifdef BAZEL_BUILD
#include "proto/mma.grpc.pb.h"
#else
#include "mma.grpc.pb.h"
#endif

#include "array_list.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using mma::Memory_Manage;
using mma::MemoryReply;
using mma::MemoryRequest;
using mma::MemoryRequest_Operation;

#define PageSize 1024

namespace proj4 {

class ArrayList;

class MmaClient {
public:
    MmaClient(std::shared_ptr<Channel> channel)
        : stub_(Memory_Manage::NewStub(channel)) {}
    int ReadPage(int array_id, int virtual_page_id, int offset);
    void WritePage(int array_id, int virtual_page_id, int offset, int value);
    ArrayList* Allocate(size_t size);
    void Free(ArrayList *);
private:
    std::unique_ptr<Memory_Manage::Stub> stub_;
};

} //namespace proj4

#endif