#pragma once

#include "../type_signature.hpp"
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
        std::vector <statement*> body;

        int get_tag() override {
            return ST_FUNCTION_DEF;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(4 * hierarchy, ' ') + "fn " + name + "(";
            
            if (args.size()) {
                for (int i = 0; i < args.size() - 1; i++) {
                    std::string sig = get_signature_string(args[i].type->sig);

                    str += sig + " " + args[i].name + ", ";
                }

                {
                    std::string sig = get_signature_string(args.back().type->sig);

                    str += sig + " " + args.back().name;
                }
            }

            str += ") -> " + get_signature_string(return_type->sig) + " {\n";

            for (statement* s : body)
                str += s->print(hierarchy + 1);
            
            str += std::string(4 * hierarchy, ' ') + "}\n";

            return str;
        }
    };
}