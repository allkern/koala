#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class while_loop : public statement {
    public:
        expression* cond = nullptr;
        statement* body;

        int get_tag() override {
            return ST_WHILE_LOOP;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(hierarchy * 4, ' ') + "while (" + cond->print(0) + ") {\n";

            str += body->print(hierarchy + 1);

            str += std::string(4 * hierarchy, ' ') + "}\n";

            return str;
        }
    };
}