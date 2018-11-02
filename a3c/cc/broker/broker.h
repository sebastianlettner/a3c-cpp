//
// Created by Sebastian Lettner on 01.10.18.
//
// Intermediary Entity. The Dealer manages the load balancing 
// of requests the agents send to the predictors.

#ifndef A3C_FRAMEWORK_BROKER_H
#define A3C_FRAMEWORK_BROKER_H

#include "a3c/cc/helper/zhelper.h"
#include "SimpleIni.h"
#include <queue>
#include <exception>
#include <iostream>



class Broker {

public:

    Broker();
    explicit Broker(const char * config_file_path);

    ~Broker();


    void stop() { Broker::_should_stop = true; }

    void run();

private:

    zmq::context_t _context;
    zmq::socket_t _frontend;
    zmq::socket_t _backend;
    zmq::socket_t _controller;
    std::unique_ptr<CSimpleIni> _config_server;
    std::unique_ptr<CSimpleIni> _config_broker;
    bool _should_stop = false;

};


#endif //A3C_FRAMEWORK_BROKER_H