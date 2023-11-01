#pragma once

#include <string>

namespace koala {
    struct location_info {
        int row, column;

        std::string path;
    };
}