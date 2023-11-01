#pragma once

#include "type_system.hpp"

#include <string>

namespace koala {
    struct variable_info {
        std::string name;

        type* type;
    };
}