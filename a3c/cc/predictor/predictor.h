//
// Created by Sebastian Lettner on 01.10.18.
//

#ifndef A3C_FRAMEWORK_PREDICTOR_H
#define A3C_FRAMEWORK_PREDICTOR_H

#include <exception>
#include <iostream>
#include "SimpleIni.h"
#include "a3c/cc/serialization/graphout_generated.h"
#include "a3c/cc/helper/zhelper.h"
#include "a3c/cc/serialization/serializer.h"
#include "a3c/cc/predictor/cluster_node.h"


class Predictor {


public:

    Predictor(const u_int32_t id, const char * config_dir_path);

    ~Predictor();

    void stop() { Predictor::_should_stop = true; }

    void run();

private:

    const u_int32_t _id;

    zmq::context_t _context;
    zmq::socket_t _broker;
    zmq::socket_t _controller;

    std::unique_ptr<CSimpleIni> _config_predictor;
    std::unique_ptr<CSimpleIni> _config_server;

    bool _should_stop = false;

    Serializer<g_out::GraphOutBuilder> serializer;
    std::unique_ptr<ClusterNode> _graph_accessor;


};

#endif //A3C_FRAMEWORK_PREDICTOR_H
