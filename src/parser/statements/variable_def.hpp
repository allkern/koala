#pragma once

#include "../type_signature.hpp"
#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class variable_def : public statement {
    public:
        std::string name;
        type* type;
        std::string assignment_op;
        expression* init;

        int get_tag() override {
            return ST_VARIABLE_DEF;
        }

        std::string print(int hierarchy) override {
            std::string str;

            if (init) {
                str += std::string(hierarchy * 4, ' ') + get_signature_string(type->sig) + " "
                    + name + " " + assignment_op + " " + init->print(0) + ";\n";
            } else {
                str += std::string(hierarchy * 4, ' ') + get_signature_string(type->sig) + " "
                    + name + ";\n";
            }

            return str;
        }
    };
}