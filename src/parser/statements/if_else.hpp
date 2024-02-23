#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class if_else : public statement {
    public:
        expression* cond = nullptr;
        statement* if_path = nullptr;
        statement* else_path = nullptr;

        int get_tag() override {
            return ST_IF_ELSE_BLOCK;
        }

        std::string print(int hierarchy) override {
            std::string str;

            bool is_compound = if_path->get_tag() == ST_COMPOUND;

            str += std::string(hierarchy * 4, ' ') + "if " + cond->print(0) + " ";

            str += if_path->print(is_compound ? hierarchy : 0);

            if (else_path) {
                is_compound = else_path->get_tag() == ST_COMPOUND;

                str += " else ";

                str += else_path->print(is_compound ? hierarchy : 0);
            }

            return str;
        }
    };
}