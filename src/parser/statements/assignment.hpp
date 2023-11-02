#pragma once

#include "../type_signature.hpp"
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
        std::string dest;
        expression* src;

        int get_tag() override {
            return ST_ASSIGNMENT;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(hierarchy * 4, ' ') + dest + " " + op + " " + src->print(0) + ";\n";

            return str;
        }
    };
}