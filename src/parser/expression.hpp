#pragma once

#include "type_system.hpp"
#include "../common.hpp"

namespace koala {
    enum : int {
        EX_INTEGER_CONSTANT,
        EX_UNARY_OP,
        EX_BINARY_OP,
        EX_NAME_REF,
    };

    class expression {
        location_info m_loc;

    public:
        location_info& get_loc() {
            return m_loc;
        }

        virtual std::string print(int hierarchy) = 0;
        virtual int get_tag() = 0;
    };
}