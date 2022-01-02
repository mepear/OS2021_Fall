#include "mma_client.h"

namespace proj4{
    int MmaClient::ReadPage(int array_id, int virtual_page_id, int offset){
        MemoryRequest request;
        MemoryReply reply;
        request.set_array_id(array_id);
        request.set_op_typ(MemoryRequest_Operation::MemoryRequest_Operation_READ);
        request.set_virtual_page_id(virtual_page_id);
        request.set_offset(offset);

        ClientContext context;

        Status status = stub_->Serve_request(&context, request, &reply);

        if (status.ok()) {
            int value = reply.value();
            return value;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }
    void MmaClient::WritePage(int array_id, int virtual_page_id, int offset, int value){
        MemoryRequest request;
        MemoryReply reply;
        request.set_array_id(array_id);
        request.set_op_typ(MemoryRequest_Operation::MemoryRequest_Operation_WRITE);
        request.set_virtual_page_id(virtual_page_id);
        request.set_offset(offset);
        request.set_value(value);

        ClientContext context;

        Status status = stub_->Serve_request(&context, request, &reply);

        if (status.ok()) {
            return;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return;
        }
    }
    ArrayList* MmaClient::Allocate(size_t size){
        while(true){
            MemoryRequest request;
            MemoryReply reply;
            request.set_op_typ(MemoryRequest_Operation::MemoryRequest_Operation_ALLOCATE);
            request.set_value(size);

            ClientContext context;

            Status status = stub_->Serve_request(&context, request, &reply);

            if (status.ok()) {
                int array_id = reply.value();
                return new ArrayList(size,this,array_id);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    void MmaClient::Free(ArrayList* array){
        MemoryRequest request;
        MemoryReply reply;
        request.set_op_typ(MemoryRequest_Operation::MemoryRequest_Operation_FREE);
        int array_id = array->Get_array_id();
        request.set_array_id(array_id);

        ClientContext context;

        Status status = stub_->Serve_request(&context, request, &reply);

        if (status.ok()) {
            delete array;
            return;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return;
        }
    }
}