// zhelper.cc

#include "a3c/cc/helper/zhelper.h"

zmq::message_t zhelper::recv_msg(zmq::socket_t& sock) {
    // Receive message type from socket
    zmq::message_t message;
    sock.recv(&message);
    return message;
}