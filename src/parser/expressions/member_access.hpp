#pragma once

#include "../expression.hpp"
#include "../parser.hpp"
#include "../common.hpp"

#include <string>
#include <vector>

namespace koala {
    class member_access : public expression {
    public:
        expression* expr;
        std::string member;

        std::string print(int hierarchy) override {
            return "(" + expr->print(0) + ")." + member;
        }

        int get_tag() override {
            return EX_MEMBER_ACCESS;
        }
    };
}