#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class string_literal : public expression {
    public:
        std::string value;

        std::string print(int hierarchy) override {
            return "\"" + value + "\"";
        }

        int get_tag() override {
            return EX_STRING_LITERAL;
        }
    };
}