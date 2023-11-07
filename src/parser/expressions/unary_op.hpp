#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class unary_op : public expression {
    public:
        bool post = false;
        std::string op;
        expression* expr;

        std::string print(int hierarchy) override {
            return "(" + op + " " + expr->print(0) + ")";
        }

        int get_tag() override {
            return EX_UNARY_OP;
        }
    };
}