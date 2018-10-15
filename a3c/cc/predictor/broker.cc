// Implementation of the Broker

#include "a3c/cc/predictor/broker.h"
#include <exception>


Broker::Broker(const char * config_dir_path) {

    // Constructor
    // Args:
    //     config_dir_path: Path to the directory containing all the config files.
    //
    
    // Get path to the predictor config file
    char path_to_file_pred[200]; // Hopefully sufficient length for path
    strcpy(path_to_file_pred, config_dir_path);
    strcat(path_to_file_pred, "/predictor.ini");

    // Get file to the agent congif.
    char path_to_file_agent[200]; // Hopefully sufficient length for path
    strcpy(path_to_file_agent, config_dir_path);
    strcat(path_to_file_agent, "/predictor.ini");

    char path_to_file_agent[200]; // Hopefully sufficient length for path
    strcpy(path_to_file_agent, config_dir_path);
    strcat(path_to_file_agent, "/server.ini");

    SI_Error rc = Broker::get_config_predictor()->LoadFile(path_to_file_pred);
    if (rc < 0) {
        throw std::runtime_error("Failed to load predictor.ini file");
    }
    SI_Error rc = Broker::get_config_agent()->LoadFile(path_to_file_agent);
    if (rc < 0) {
        throw std::runtime_error("Failed to load agent.ini file");
    }  
    SI_Error rc = Broker::get_config_agent()->LoadFile(path_to_file_agent);
    if (rc < 0) {
        throw std::runtime_error("Failed to load server.ini file");
    }
    // Retrieve the value for the socket communicating with the agent
    const char * frontend_bind = Broker::get_config_agent()->GetValue("ADDRESSES", "PREDICTOR_REQ", NULL);
    // Retrieve the value for the socket communicating with the predictor
    const char * backend_bind = Broker::get_config_predictor()->GetValue("ADDRESSES", "AGENT_REP", NULL);
    // Retrieve the value for the socket communicating with the server
    // Mainly for receiveing the KILL signal so far.
    const char * server_publish = Broker::get_config_predictor()->GetValue("ADDRESSES", "INTERFACE_PUB", NULL);

    // Bind the addresses to the sockets
    Broker::_frontend.bind(frontend_bind);
    Broker::_backend.bind(backend_bind);
    Broker::_controller.connect(server_publish);
    Broker::_controller.setsockopt(ZMQ_SUBSCRIBE, "", 0);
}

Broker::~Broker() {
    // Destructor
    delete Broker::_config_agent;
    delete Broker::_config_predictor;
    delete Broker::_config_server;
}

void Broker::run() {
    // The main function of the Broker.

    zmq::pollitem_t items [] = {
        { Broker::_frontend, 0, ZMQ_POLLIN, 0 },
        { Broker::_backend,  0, ZMQ_POLLIN, 0 },
        { Broker::_controller, 0, ZMQ_POLLIN, 0}
    };

    bool should_stop = 0;
    while (not should_stop) {
        
        // Loop runs until the server commands to stop.
        zmq::message_t message;
        int more;

        zmq::poll (&items [0], 2, -1);

        if (items [0].revents & ZMQ_POLLIN) {
            while (1) {
                //  Process all parts of the message
                frontend.recv(&message);
                size_t more_size = sizeof (more);
                frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                backend.send(message, more? ZMQ_SNDMORE: 0);
                
                if (!more)
                    break;      //  Last message part
            }
        }
        if (items [1].revents & ZMQ_POLLIN) {
            while (1) {
                //  Process all parts of the message
                backend.recv(&message);
                size_t more_size = sizeof (more);
                backend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                frontend.send(message, more? ZMQ_SNDMORE: 0);
                if (!more)
                    break;      //  Last message part
            }
        }

        if (items [2].revents & ZMQ_POLLIN) {
            
            Broker::_controller.recv(&message);
            std::string cmd(static_cast<char *>(message.data()), message.size());
            if (cmd == "KILL") {
                should_stop = true;
            } // END IF

        } // END IF

    } // END WHILE

}  // END FUNC
