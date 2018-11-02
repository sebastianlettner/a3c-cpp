// Implementation of the Broker

#include "a3c/cc/broker/broker.h"


Broker::Broker(const char * config_dir_path):
    _context(1),
    _frontend(_context, ZMQ_ROUTER),
    _backend(_context, ZMQ_DEALER),
    _controller(_context, ZMQ_SUB),
    _config_server(new CSimpleIni(0, 1, 1)),
    _config_broker(new CSimpleIni(0, 1, 1))

    {

    // Constructor
    // Args:
    //     config_dir_path: Path to the directory containing all the config files.
    //

    char path_to_file_server[200]; // Hopefully sufficient length for path
    strcpy(path_to_file_server, config_dir_path);
    strcat(path_to_file_server, "/server.ini");

    char path_to_file_broker[200]; // Hopefully sufficient length for path
    strcpy(path_to_file_broker, config_dir_path);
    strcat(path_to_file_broker, "/broker.ini");
    SI_Error rc = _config_server->LoadFile(path_to_file_server);
    if (rc < 0) {
        throw std::runtime_error("Failed to load server.ini file");
    }
    rc = _config_broker->LoadFile(path_to_file_broker);
    if (rc < 0) {
        throw std::runtime_error("Failed to load broker.ini file");
    }
    // Retrieve the value for the socket communicating with the agent
    const char * frontend_bind = _config_broker->GetValue("ADDRESSES", "PREDICTOR_INTERFACE", NULL);
    // Retrieve the value for the socket communicating with the predictor
    const char * backend_bind = _config_broker->GetValue("ADDRESSES", "AGENT_INTERFACE", NULL);
    // Retrieve the value for the socket communicating with the server
    // Mainly for receiveing the KILL signal so far.
    const char * server_publish = _config_server->GetValue("ADDRESSES", "INTERFACE_PUB", NULL);
    // Bind the addresses to the sockets
    zmq_bind(Broker::_backend, backend_bind);
    zmq_bind(Broker::_frontend,frontend_bind);
    zmq_connect(Broker::_controller, server_publish);
    Broker::_controller.setsockopt(ZMQ_SUBSCRIBE, "", 0);
}

Broker::~Broker() {

}

void Broker::run() {
    // The main function of the Broker.

    int status; // for receiving error messages

    zmq::pollitem_t items [] = {
        // Always poll for worker activity on backend
        { Broker::_backend,  0, ZMQ_POLLIN, 0 },
        { Broker::_controller, 0, ZMQ_POLLIN, 0},
        //  Poll front-end only if we have available workers
        { Broker::_frontend, 0, ZMQ_POLLIN, 0 },
    };

    std::queue<std::string> worker_queue;
    // Initialize the workers
    unsigned int num_predictors = static_cast<unsigned int>(*Broker::_config_broker->GetValue("SETUP", "NUM_PREDICTORS", NULL));
    for (unsigned int i = 0; i < num_predictors; i++) {
        std::string id = std::to_string(i);
        worker_queue.push(id);
    }

    while (not Broker::_should_stop) {
        if (worker_queue.size()) {
            status = zmq::poll(&items[0], 3, -1);
        }
        else {
            //only poll backend and controller
            status = zmq::poll(&items[0], 2, -1);
        }
        if (status == -1) {
            throw std::runtime_error("Poll Error in Predictor");           
        }

        // Handle worker activity on backend
        if (items [0].revents & ZMQ_POLLIN) {
            
            // First frame is worker id
            auto msg_1 = zhelper::recv_msg(this->_backend);
            std::string worker_id(static_cast<char *>(msg_1.data()), msg_1.size());
            worker_queue.push(worker_id);

            //  Second frame is empty
            auto msg_2 = zhelper::recv_msg(Broker::_backend);
            assert(msg_2.size() == 0);

            //  Third frame is Client ID forwarded to the agent
            auto msg_3 = zhelper::recv_msg(Broker::_backend);
            Broker::_frontend.send(msg_3);

            // Fourth frame is empty
            auto msg_4 = zhelper::recv_msg(Broker::_backend);
            assert(msg_4.size() == 0);
            // Send the frame to the agent
            Broker::_frontend.send(msg_4);

            // Fifth frame contains the graph output, also send to the agent
            auto msg_5 = zhelper::recv_msg(Broker::_backend);
            Broker::_frontend.send(msg_5);
        
        }
        // Handle activity from the server
        if (items [1].revents & ZMQ_POLLIN) {

            // first frame specifies message
            auto msg = zhelper::recv_msg(Broker::_controller);
            std::string cmd(static_cast<char *>(msg.data()), msg.size());
            if (std::strcmp(cmd.c_str(), "KILL")) {
                Broker::stop();
            }
        } // END IF

        // handle activity from the frontend
        if (items [2].revents & ZMQ_POLLIN) {
            
            // first frame is client id
            auto msg_1 = zhelper::recv_msg(Broker::_frontend);
            Broker::_backend.send(msg_1);

            //second frame is empty
            auto msg_2 = zhelper::recv_msg(Broker::_frontend);
            assert(msg.size() == 0);
            Broker::_backend.send(msg_2);

            // Third frame is the serialized observation.
            auto msg_3 = zhelper::recv_msg(Broker::_frontend);
            Broker::_backend.send(msg_3);

            // send empty frame
            zmq::message_t empty_frame(0);
            memcpy((void *)empty_frame.data(), "", 0);
            Broker::_backend.send(empty_frame);

            // Fifth frame is the worker id
            zmq::message_t worker_id;
            std::string free_worker = worker_queue.front();
            worker_queue.pop();
            memcpy((void *)worker_id.data(), free_worker.c_str(), free_worker.size());
            Broker::_backend.send(worker_id);

        } // END IF

    } // END WHILE
    Broker::_backend.close();
    Broker::_frontend.close();
    Broker::_context.close();

}  // END FUNC
