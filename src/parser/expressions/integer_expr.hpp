#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class integer_expr : public expression {
    public:
        std::string value;

        std::string print(int hierarchy) {
            return "(" + value + ")";
        }

        int get_type() override {
            return EX_INTEGER;
        }
    };
}