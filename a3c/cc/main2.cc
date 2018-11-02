#include "zmq.hpp"
#include <unistd.h>
#include <iostream>
#include "broker.h"


int main() {

        
    Broker* broker = new Broker("/Users/sebastianlettner/GoogleDrive/MachineLearning/a3c-framework/configurations");
    broker->run();
    return 0;

}