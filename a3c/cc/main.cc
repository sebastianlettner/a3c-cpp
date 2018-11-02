#include "zmq.hpp"
#include <unistd.h>
#include <iostream>

int main() {
    zmq::context_t c(1);
    zmq::socket_t s(c, ZMQ_PUB);
    zmq_bind(s, "tcp://127.0.0.1:3333");
    sleep(1);
    zmq::message_t msg_1;
    std::string some_command = "SOME";
    memcpy((void*)msg_1.data(), some_command.c_str(), some_command.size());
    std::cout << "Sending some.." << std::endl;
    s.send(msg_1);
    sleep(1);
    zmq::message_t msg_2;
    std::string kill_command = "KILL";
    memcpy((void*)msg_2.data(), kill_command.c_str(), kill_command.size());
    int i = 0;
    while (i < 3) {
       std::cout << "Sending kill.." << std::endl;
       s.send(msg_2);
       sleep(1);  
    }
    s.close();
    c.close();
    return 0;
}