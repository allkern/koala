#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    struct function_arg {
        bool mut = false;

        std::string name;
        type* t = nullptr;
    };

    class function_def : public statement {
    public:
        std::string name;
        type* return_type = nullptr;
        std::vector <function_arg> args;
        std::vector <statement*> body;

        int get_tag() override {
            return ST_FUNCTION_DEF;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(4 * hierarchy, ' ') + "fn " + name + "(";
            
            if (args.size()) {
                for (int i = 0; i < args.size() - 1; i++) {
                    str += (args[i].mut ? "mut " : "");
                    str += args[i].name + ": " + args[i].t->str() + ", ";
                }

                str += (args.back().mut ? "mut " : "");
                str += args.back().name + ": " + args.back().t->str();
            }

            str += ") -> " + return_type->str() + " {\n";

            for (statement* s : body)
                str += s->print(hierarchy + 1);
            
            str += std::string(4 * hierarchy, ' ') + "}\n";

            return str;
        }
    };
}