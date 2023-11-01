#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class function_def : public statement {
    public:
        std::string name;
        type* return_type;
        std::vector <variable_info> args;
        expression* body;

        int get_type() override {
            return ST_FUNCTION_DEF;
        }
    };
}