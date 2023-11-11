#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>
#include <cassert>

namespace koala {
    class array_access : public expression {
    public:
        expression* expr = nullptr;
        expression* index = nullptr;

        int get_tag() override {
            return EX_ARRAY_ACCESS;
        }

        std::string print(int hierarchy) override {
            std::string str;

            assert(index);
            assert(expr);

            str += std::string(hierarchy * 4, ' ') + expr->print(0) + "[";
            str += index->print(0) + "]";

            return str;
        }
    };
}