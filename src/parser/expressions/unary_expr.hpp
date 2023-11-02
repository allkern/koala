#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class unary_expr : public expression {
    public:
        std::string op;
        expression* expr;

        std::string print(int hierarchy) {
            return "(" + op + " " + expr->print(0) + ")";
        }

        int get_type() override {
            return EX_UNARY_OP;
        }
    };
}