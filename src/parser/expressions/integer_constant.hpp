#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class integer_constant : public expression {
    public:
        std::string value;

        std::string print(int hierarchy) override {
            return value;
        }

        int get_tag() override {
            return EX_INTEGER_CONSTANT;
        }
    };
}