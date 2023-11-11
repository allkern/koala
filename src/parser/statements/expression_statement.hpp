#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class expression_statement : public statement {
    public:
        expression* expr = nullptr;

        int get_tag() override {
            return ST_EXPRESSION;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(hierarchy * 4, ' ') + expr->print(0) + ";\n";

            return str;
        }

        expression_statement(expression* expr) :
            expr(expr) {};
    };
}