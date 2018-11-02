//
// Created by Sebastian Lettner on 01.10.18.
//
// ZeroMQ Helper functions
//

#ifndef A3C_FRAMEWORK_ZHELPER_H
#define A3C_FRAMEWORK_ZHELPER_H

#include "zmq.hpp"

namespace  zhelper {

    zmq::message_t recv_msg(zmq::socket_t& sock);

}


#endif