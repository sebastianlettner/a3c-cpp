#include "gtest/gtest.h"
#include "a3c/cc/broker/broker.h"
#include <pthread.h>
#include <unistd.h>

// ====== HELPER ====== //

// void* simulate_server(void* arg) {
//     zmq::context_t c(1);
//     zmq::socket_t s(c, ZMQ_PUB);
//     zmq_bind(s, "tcp://127.0.0.1:3333");
//     sleep(1);
//     zmq::message_t msg_1;
//     std::string some_command = "SOME";
//     memcpy((void*)msg_1.data(), some_command.c_str(), some_command.size());
//     std::cout << "Sending some.." << std::endl;
//     s.send(msg_1);
//     sleep(1);
//     zmq::message_t msg_2;
//     std::string kill_command = "KILL";
//     memcpy((void*)msg_2.data(), kill_command.c_str(), kill_command.size());
//     int i = 0;
//     while (i < 3) {
//        std::cout << "Sending kill.." << std::endl;
//        s.send(msg_2);
//        sleep(1);  
//     }
//     s.close();
//     c.close();
// }

// void* simulate_broker(void* arg) {
//     zmq::context_t c(1);
//     zmq::socket_t s(c, ZMQ_SUB);
//     zmq_connect(s, "tcp://127.0.0.1:3333");
//     s.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    
//     zmq::pollitem_t item [] = {
//         { s, 0, ZMQ_POLLIN, 0 }
//     };
//     zmq::poll(&item[0], 1, -1);
//     if (item [0].revents & ZMQ_POLLIN) {
//         zmq::message_t msg;
//         s.recv(&msg);
//         std::string content(static_cast<const char *>(msg.data()));
//         std::cout << "Received: " << content << std::endl;
//     }
//     s.close();
//     c.close();
    
// }

// ====== TESTS ====== //


TEST(TestBroker, CheckInit) {
    //
    // Test the initialization. 
    //
    Broker* broker = new Broker("/nas/lfe/home/ga63fiy/projects/a3c-framework/configurations");
    free(broker);
}

// TEST(TestBroker, CheckRun) {
//     //
//     // Test the run function.
//     //
    
//     Broker* broker = new Broker("/Users/sebastianlettner/GoogleDrive/MachineLearning/a3c-framework/configurations");
//     broker->run();

// }