#pragma once

#include "../common.hpp"

namespace koala {
    enum : int {
        EX_INTEGER,
        EX_UNARY_OP,
        EX_BINARY_OP
    };

    class expression {
        location_info m_loc;

    public:
        location_info& get_loc() {
            return m_loc;
        }

        virtual std::string print(int hierarchy) = 0;
        virtual int get_type() = 0;
    };
}