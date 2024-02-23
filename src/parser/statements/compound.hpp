#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class compound : public statement {
    public:
        std::vector <statement*> body;

        int get_tag() override {
            return ST_COMPOUND;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += "{\n";
            
            for (statement* s : body)
                str += s->print(hierarchy + 1);

            str += "}\n";

            return str;
        }
    };
}