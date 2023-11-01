#pragma once

#include "../common.hpp"

namespace koala {
    class expression {
        location_info m_loc;

    public:
        location_info& get_loc() {
            return m_loc;
        }

        virtual int get_type() = 0;
    };
}