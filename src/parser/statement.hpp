#pragma once

#include "../common.hpp"

namespace koala {
    enum : int {
        ST_FUNCTION_DEF = 0,
        ST_VARIABLE_DEF,
        ST_STRUCT_DEF
    };

    class statement {
        location_info m_loc;

    public:
        location_info& get_loc() {
            return m_loc;
        }

        virtual int get_type() = 0;
    };
}