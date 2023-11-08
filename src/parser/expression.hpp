#pragma once

//#include "type_system.hpp"
#include "../common.hpp"


namespace koala {
    class type;

    enum : int {
        EX_INTEGER_CONSTANT,
        EX_STRING_LITERAL,
        EX_UNARY_OP,
        EX_BINARY_OP,
        EX_NAME_REF,
        EX_MEMBER_ACCESS,
        EX_FUNCTION_CALL,
        EX_ARRAY_ACCESS
    };

    class expression {
        location_info m_loc;

    public:
        location_info& get_loc() {
            return m_loc;
        }

        type* t = nullptr;

        virtual std::string print(int hierarchy) = 0;
        virtual int get_tag() = 0;
    };
}