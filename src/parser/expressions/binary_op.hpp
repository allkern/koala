#pragma once

#include "../type_system.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class binary_op : public expression {
    public:
        std::string op;
        expression* left;
        expression* right;

        std::string print(int hierarchy) override {
            return "(" + op + " " + left->print(0) + " " + right->print(0) + ")";
        }

        int get_tag() override {
            return EX_BINARY_OP;
        }
    };
}