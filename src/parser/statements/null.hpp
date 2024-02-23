#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class null : public statement {
    public:
        int get_tag() override {
            return ST_NULL;
        }

        std::string print(int hierarchy) override {
            return ";";
        }
    };
}