#include "zmq.hpp"
#include "include/flatbuffers/flatbuffers.h"
#include "obs_generated.h"
#include <iostream>


int main() {

    zmq::context_t context(1);
    zmq::socket_t sock(context, ZMQ_REQ);
    sock.connect("tcp://127.0.0.1:3333");

    flatbuffers::FlatBufferBuilder builder;
    std::vector<flatbuffers::Offset<fbs::Entry>> entries;
    std::vector<float> vec1 = {1.0f, 2.0f};
    entries.push_back(fbs::CreateEntryDirect(builder, "state1", &vec1));
    auto vec = builder.CreateVectorOfSortedTables(&entries);
    auto dict = fbs::CreateDict(builder, vec, 1);
    builder.Finish(dict); 
    //ffb.Finish(respone);
    //int buffersize = ffb.GetSize();
    //zmq::message_t request(buffersize);
    //memcpy((void *)request.data(), ffb.GetBufferPointer(), buffersize);
    //sock.send(request);
    //std::cout << "Send request" << std::endl;
    uint8_t* buf = (uint8_t*) malloc(builder.GetSize());
    memcpy(buf, builder.GetBufferPointer(), builder.GetSize());
    
    auto aaa = fbs::GetDict(buf);
    const fbs::Entry * val = aaa->entries()->LookupByKey("state1");
    std::vector<float> a(val->value()->begin(), val->value()->end());
    
    std::cout << a[0] << std::endl;
    sock.close();
    context.close();
    return 0;
}