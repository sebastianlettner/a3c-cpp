//
// Created by Sebastian Lettner on 01.10.18.
//
// Intermediary Entity. The Dealer manages the load balancing 
// of requests the agents send to the predictors.

#ifndef A3C_FRAMEWORK_BROKER_H
#define A3C_FRAMEWORK_BROKER_H

#include "zmq.hpp"
#include "a3c/cc/parser/simple_ini.h"

class Broker {

public:

    Broker(const char * config_file_path);

    ~Broker();

    CSimpleIni* get_config_predictor() {return Broker::_config_predictor;}
    CSimpleIni* get_config_agent() {return Broker::_config_agent;}

    void run();

private:

    zmq::context_t _context(1);
    zmq::socket_t _frontend (Broker::_context, ZMQ_ROUTER);
    zmq::socket_t _backend (Broker::_context, ZMQ_DEALER);
    zmq::socket_t _controller (Broker::_context, ZMQ_SUB);
    CSimpleIni* _config_predictor = new CSimpleIni(0, 1, 1);
    CSimpleIni* _config_agent = new CSimpleIni(0, 1, 1);
    CSimpleIni* _config_server = new CSimpleIni(0, 1, 1);


};


#endif //A3C_FRAMEWORK_BROKER_H