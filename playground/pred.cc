#include "zmq.hpp"
#include "include/flatbuffers/flatbuffers.h"
#include "obs_generated.h"
#include "tensorflow/core/public/session.h"
#include <iostream>

using namespace tensorflow;
using namespace std;


int main() {

    
    flatbuffers::FlatBufferBuilder ffb;
    Serializer::InputBuilder builder(ffb);
    zmq::context_t context(1);
    zmq::socket_t sock(context, ZMQ_REP);
    sock.bind("tcp://127.0.0.1:3333");

    zmq::message_t msg;
    sock.recv(&msg);
    auto input = Serializer::GetInput(msg.data());
    std::cout << "Receive Message " << std::endl;

    Session* session;
    std::string graph_definition = "/Users/sebastianlettner/GoogleDrive/MachineLearning/a3c-framework/playground/graph.pb";
    GraphDef graph_def;
    SessionOptions opts;
    std::vector<Tensor> outputs; // Store outputs
    TF_CHECK_OK(ReadBinaryProto(Env::Default(), graph_definition, &graph_def));

    // create a new session
    TF_CHECK_OK(NewSession(opts, &session));

    // Load graph into session
    TF_CHECK_OK(session->Create(graph_def));

    // Initialize our variables
    TF_CHECK_OK(session->Run({}, {}, {"init_all_vars_op"}, nullptr));
    Tensor b(DT_FLOAT, TensorShape({4}));


    std::vector<std::pair<string, tensorflow::Tensor>> inputs = {
        { "input", b }
    };
    TF_CHECK_OK(session->Run(inputs, {"multiply_op"}, {}, &outputs));

    auto output = outputs[0].scalar<float>();
    std::cout << "Output: " << output << std::endl;
    TF_CHECK_OK(session->Close());
    delete session;
    sock.close();
    context.close();
    return 0;
}