// Implementation of the Predictor

#include "a3c/cc/predictor/predictor.h"


Predictor::Predictor(const u_int32_t id, const char * config_dir_path):
    _id(id),
    _context(1),
    _broker(_context, ZMQ_REP),
    _controller(_context, ZMQ_SUBSCRIBE),
    _config_predictor(new CSimpleIni(0, 1, 1)),
    _config_server(new CSimpleIni(0, 1, 1)),
    _graph_accessor(new ClusterNode(spec))
{
    // Constructor
    //
    // Args:
    //     id: Identifier of the predictor.
    //     config_file_path: Path to the config files directory.
    //

    char path_to_file_predictor[200]; // Hopefully sufficient length for path
    strcpy(path_to_file_predictor, config_dir_path);
    strcat(path_to_file_predictor, "/predictor.ini");
    SI_Error rc = _config_predictor->LoadFile(path_to_file_predictor);
    if (rc < 0) {
        throw std::runtime_error("Failed to load predictor.ini file");
    }
    char path_to_file_server[200]; // Hopefully sufficient length for path
    strcpy(path_to_file_server, config_dir_path);
    strcat(path_to_file_server, "/server.ini");
    rc = _config_server->LoadFile(path_to_file_server);
    if (rc < 0) {
        throw std::runtime_error("Failed to load predictor.ini file");
    }

    // Get the addresses from the ini files
    const char * server_publish = _config_server->GetValue("ADDRESSES", "SERVER_INTERFACE_SUB");
    const char * broker_interface = _config_predictor->GetValue("ADDRESSES", "BROKER_INTERFACE");

    // Connect to the sockets
    Predictor::_broker.connect(broker_interface);
    Predictor::_controller.connect(server_publish);
    Predictor::_controller.setsockopt(ZMQ_SUBSCRIBE, "", 0);

}

Predictor::~Predictor() {

}

void Predictor::run() {

    // The main function of the predictor

    int status;

    zmq::pollitem_t items [] = {
        { Predictor::_broker, 0, ZMQ_POLLIN, 0 },
        { Predictor::_controller, 0, ZMQ_POLLIN, 0}
    };

    zmq::message_t message;

    while (not _should_stop) {

        status = zmq::poll (&items [0], 2, -1);
        if (status == -1) {
            throw std::runtime_error("Poll Error in Predictor");           
        }
        // Request from the broker
        if (items[0].revents & ZMQ_POLLIN) {
            Predictor::_broker.recv(&message);
            auto observation = obs::GetObservable(message.data());
        }
        // Activity from the server
        if (items[1].revents & ZMQ_POLLIN) {
            
            // Check for KILL Signal

            auto msg = zhelper::recv_msg(Predictor::_controller);
            std::string cmd(static_cast<char *>(msg.data()), msg.size());
            if (std::strcmp(cmd.c_str(), "KILL")) {
                Predictor::stop();
            } // END IF
        } // END IF
    } // WHILE
} // RUN

