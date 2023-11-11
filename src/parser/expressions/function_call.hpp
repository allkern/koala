#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class function_call : public expression {
    public:
        expression* expr;
        std::vector <expression*> args;

        int get_tag() override {
            return EX_FUNCTION_CALL;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(hierarchy * 4, ' ') + expr->print(0) + "(";

            if (!args.size()) {
                return str + ")";
            }

            for (int i = 0; i < args.size() - 1; i++) {
                str += args[i]->print(0) + ", ";
            }

            str += args.back()->print(0) + ")";

            return str;
        }
    };
}