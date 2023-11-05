#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class function_call : public statement {
    public:
        std::string callee;
        std::vector <expression*> args;

        int get_tag() override {
            return ST_FUNCTION_CALL;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(hierarchy * 4, ' ') + callee + "(";

            if (!args.size()) {
                return str + ");\n";
            }

            for (int i = 0; i < args.size() - 1; i++) {
                str += args[i]->print(0) + ", ";
            }

            str += args.back()->print(0) + ");\n";

            return str;
        }
    };
}