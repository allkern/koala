#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class name_ref : public expression {
    public:
        std::string name;

        std::string print(int hierarchy) override {
            return name;
        }

        int get_tag() override {
            return EX_NAME_REF;
        }
    };
}