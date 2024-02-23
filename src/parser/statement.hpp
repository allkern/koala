#pragma once

#include "../common.hpp"

namespace koala {
    enum : int {
        ST_FUNCTION_DEF = 0,
        ST_VARIABLE_DEF,
        ST_STRUCT_DEF,
        ST_ASSIGNMENT,
        ST_COMPOUND,
        ST_EXPRESSION,
        ST_RETURN_EXPR,
        ST_WHILE_LOOP,
        ST_FOR_LOOP,
        ST_NULL,
        ST_IF_ELSE_BLOCK
    };

    class statement {
        location_info m_loc;

    public:
        location_info& get_loc() {
            return m_loc;
        }

        virtual int get_tag() = 0;
        virtual std::string print(int hierarchy) = 0;
    };
}