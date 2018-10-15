// Implementation of the Predictor

#include "a3c/cc/predictor/predictor.h"
#include "a3c/cc/parser/simple_ini.h"
#include "zmq.hpp"
#include <exception>
#include <iostream>

Predictor::Predictor(const u_int32_t id, const char * config_dir_path):
    _id(id),
    _config_dir_path(config_dir_path)
{
    // Constructor
    //
    // Args:
    //     id: Identifier of the predictor.
    //     config_file_path: Path to the config files directory.
    //
    char path_to_file[200]; // Hopefully sufficient length for path
    strcpy(path_to_file, config_dir_path);
    strcat(path_to_file, "/predictor.ini");
    
    SI_Error rc = Predictor::get_config()->LoadFile(path_to_file);
    if (rc < 0) {
        throw std::runtime_error("Failed to load predictor.ini file");
    }

}

Predictor::~Predictor() {
    delete Predictor::_config;
}
