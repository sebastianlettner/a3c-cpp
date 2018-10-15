//
// Created by Sebastian Lettner on 01.10.18.
//

#ifndef A3C_FRAMEWORK_PREDICTOR_H
#define A3C_FRAMEWORK_PREDICTOR_H

#include "a3c/cc/parser/simple_ini.h"


class Predictor {
    // The Predictor
public:

    Predictor(const u_int32_t id, const char * config_dir_path);

    ~Predictor();

    CSimpleIni * get_config() {return Predictor::_config;}

private:

    const u_int32_t _id;
    const char * _config_dir_path;
    CSimpleIni * _config = new CSimpleIni(1, 0, 0);


};

#endif //A3C_FRAMEWORK_PREDICTOR_H
