#include "mma_server.h"

namespace proj4{
    void ServiceImpl::Init(size_t phy_page_num){
        mma = new MemoryManager(phy_page_num);
        flag = 0;
        current_vir = 0;
        max_vir = 0;
        return;
    }
    void ServiceImpl::Init_res(size_t phy_page_num, size_t max_vir_page_num){
        mma = new MemoryManager(phy_page_num);
        flag = 1;
        current_vir = 0;
        max_vir = max_vir_page_num;
        return;
    }
    Status ServiceImpl::Serve_request(ServerContext* context, const MemoryRequest* request, MemoryReply* reply){
        if (request->op_typ() == MemoryRequest_Operation::MemoryRequest_Operation_ALLOCATE){
            size_t size = request->value();
            int page_num = ceil((float)size / PageSize);
            if ((flag == 1) && (page_num + current_vir > max_vir)){
                return Status::CANCELLED;
            }
            current_vir += page_num;
            int array_id = mma->Allocate(size);
            size_map.insert(std::pair<int,int>(array_id,page_num));
            reply->set_value(array_id);
            return Status::OK;
        } else if (request->op_typ() == MemoryRequest_Operation::MemoryRequest_Operation_READ){
            int array_id = request->array_id();
            int virtual_page_id = request->virtual_page_id();
            int offset = request->offset();
            int value = mma->ReadPage(array_id,virtual_page_id,offset);
            reply->set_value(value);
            return Status::OK;
        } else if (request->op_typ() == MemoryRequest_Operation::MemoryRequest_Operation_WRITE){
            int array_id = request->array_id();
            int virtual_page_id = request->virtual_page_id();
            int offset = request->offset();
            int value = request->value();
            mma->WritePage(array_id,virtual_page_id,offset,value);
            return Status::OK;
        } else if (request->op_typ() == MemoryRequest_Operation::MemoryRequest_Operation_FREE){
            int array_id = request->array_id();
            auto page_num = size_map[array_id];
            current_vir = current_vir - page_num;
            mma->Release(array_id);
            return Status::OK;
        }
    }
    void RunServerUL(size_t phy_page_num){
        std::string server_address("0.0.0.0:50051");
        ServiceImpl service;
        service.Init(phy_page_num);

        grpc::EnableDefaultHealthCheckService(true);
        grpc::reflection::InitProtoReflectionServerBuilderPlugin();
        ServerBuilder builder;
        // Listen on the given address without any authentication mechanism.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Register "service" as the instance through which we'll communicate with
        // clients. In this case it corresponds to an *synchronous* service.
        builder.RegisterService(&service);
        // Finally assemble the server.
        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "Server listening on " << server_address << std::endl;

        // Wait for the server to shutdown. Note that some other thread must be
        // responsible for shutting down the server for this call to ever return.
        server->Wait();
    }
    void RunServerL(size_t phy_page_num, size_t max_vir_page_num){
        std::string server_address("0.0.0.0:50051");
        ServiceImpl service;
        service.Init_res(phy_page_num,max_vir_page_num);

        grpc::EnableDefaultHealthCheckService(true);
        grpc::reflection::InitProtoReflectionServerBuilderPlugin();
        ServerBuilder builder;
        // Listen on the given address without any authentication mechanism.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Register "service" as the instance through which we'll communicate with
        // clients. In this case it corresponds to an *synchronous* service.
        builder.RegisterService(&service);
        // Finally assemble the server.
        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout << "Server listening on " << server_address << std::endl;

        // Wait for the server to shutdown. Note that some other thread must be
        // responsible for shutting down the server for this call to ever return.
        server->Wait();
    }
    void ShutdownServer(){
        return;
    }
}