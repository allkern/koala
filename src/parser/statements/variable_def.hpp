#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    enum : int {
        SD_STATIC,
        SD_DYNAMIC
    };

    class variable_def : public statement {
    public:
        bool mut = false;
        int storage_duration = 0;

        std::string name;
        type* type = nullptr;
        std::string assignment_op;
        expression* init = nullptr;

        int get_tag() override {
            return ST_VARIABLE_DEF;
        }

        std::string print(int hierarchy) override {
            std::string str;

            str += std::string(hierarchy * 4, ' ') + "let ";
            str += storage_duration ? "static " : "";
            str += mut ? "mut " : "";

            str += name + ": " + (type ? type->str() : "<auto>");

            if (init) {
                str += " " + assignment_op + " " + init->print(0);
            }

            str += ";\n";

            return str;
        }
    };
}