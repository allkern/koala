#pragma once

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
        expression* init;

        int get_type() override {
            return ST_VARIABLE_DEF;
        }
    };
}