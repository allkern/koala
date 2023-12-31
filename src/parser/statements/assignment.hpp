#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class assignment : public statement {
    public:
        std::string op;
        expression* dst;
        expression* src;

        int get_tag() override {
            return ST_ASSIGNMENT;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(hierarchy * 4, ' ') + dst->print(0) + " " + op + " " + src->print(0) + ";\n";

            return str;
        }
    };
}