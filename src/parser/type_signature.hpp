#pragma once

#include <string>

namespace koala {
    struct type_signature {
        bool is_mut;
        bool is_static;
        std::string base;
        int pointer_rank;

        type_signature() :
            is_mut(false), is_static(false), base(""), pointer_rank(0) {};

        type_signature(std::string base) :
            is_mut(false), is_static(false), base(base), pointer_rank(0) {};
    };

    std::string get_signature_string(type_signature sig);
}