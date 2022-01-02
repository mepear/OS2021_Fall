#ifndef MMA_SERVER_H
#define MMA_SERVER_H

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <math.h>

#include <grpc++/grpc++.h>
#include <grpc++/ext/proto_server_reflection_plugin.h>
#include <grpc++/health_check_service_interface.h>

#ifdef BAZEL_BUILD
#include "proto/mma.grpc.pb.h"
#else
#include "mma.grpc.pb.h"
#endif

#include "memory_manager.h"
#include "array_list.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using mma::Memory_Manage;
using mma::MemoryReply;
using mma::MemoryRequest;
using mma::MemoryRequest_Operation;

// Logic and data behind the server's behavior.

namespace proj4 {

class ServiceImpl final : public Memory_Manage::Service {
public:
    void Init(size_t phy_page_num);
    void Init_res(size_t phy_page_num, size_t max_vir_page_num);
    Status Serve_request(ServerContext* context, const MemoryRequest* request, MemoryReply* reply);
private:
    MemoryManager *mma;
    int flag; // Whether choose to restrict virtual memory
    size_t max_vir;
    size_t current_vir;
    std::map<int, int> size_map; 
};

// setup a server with UnLimited virtual memory space
void RunServerUL(size_t phy_page_num);

// setup a server with Limited virtual memory space
void RunServerL(size_t phy_page_num, size_t max_vir_page_num);

// shutdown the server setup by RunServerUL or RunServerL
void ShutdownServer();

} //namespace proj4

#endif