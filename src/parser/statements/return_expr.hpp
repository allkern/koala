#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class return_expr : public statement {
    public:
        expression* expr = nullptr;

        int get_tag() override {
            return ST_RETURN_EXPR;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(hierarchy * 4, ' ') + "return ";
            str += expr->print(0);
            str += ";\n";

            return str;
        }
    };
}