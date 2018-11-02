//
// Created by Sebastian Lettner on 01.10.18.
//
// The Serializer. 
//

#ifndef A3C_FRAMEWORK_SERIALIZER_H
#define A3C_FRAMEWORK_SERIALIZER_H

#include "include/flatbuffers/flatbuffers.h"

template<typename B_TYPE>
class Serializer {

    //
    // The following serializations exist.
    //     agent(cpp) --> predictor(cpp) : Observable
    //     agent(cpp) <-- predictor(cpp) : GraphOut
    //     agent(cpp) --> server(python) : Batch
    // The user needs to implement the following functions:
    //     serialize_observation
    //     serialize_graph
    //     serialize_batch
    //     convert_to_graph_intput
    //

public:

    Serializer();
    ~Serializer(); 

    void reset_buffer() { Serializer::_ffb.Reset(); }
    flatbuffers::FlatBufferBuilder * get_buffer() { return &_ffb; }
    B_TYPE * get_builder() { return &_builder; }

    uint8_t* serialize();

private:

    flatbuffers::FlatBufferBuilder _ffb;
    B_TYPE _builder;

};

template<typename B_TYPE>
Serializer<B_TYPE>::Serializer():
    _builder(_ffb)
    {

    }

template<typename B_TYPE>
Serializer<B_TYPE>::~Serializer() {

}

template<typename B_TYPE>
uint8_t * Serializer<B_TYPE>::serialize() {

    //
    // This function serilizes the content of the builder.
    // Make sure that all the required fields are filled before you call this function.
    // Args:
    //
    // Return:
    //     buf: Pointer to the serialized data. This data can be sent over sockets now. 
    //

    auto serialized_structure = Serializer<B_TYPE>::_builder.Finish();
    Serializer<B_TYPE>::_ffb.Finish(serialized_structure);
    uint8_t* buf = (uint8_t*) malloc(Serializer<B_TYPE>::_ffb.GetSize());
    memcpy(buf, Serializer<B_TYPE>::_ffb.GetBufferPointer(), Serializer<B_TYPE>::_ffb.GetSize());
    Serializer<B_TYPE>::reset_buffer();
    return buf;

}

#endif //A3C_FRAMEWORK_SERIALIZER_H
