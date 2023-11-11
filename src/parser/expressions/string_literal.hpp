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
            std::string escaped;

            for (char c : value) {
                if (isprint(c)) {
                    escaped.push_back(c);

                    continue;
                }

                switch (c) {
                    case '\0': escaped.append("\\0"); break;
                    case '\a': escaped.append("\\a"); break;
                    case '\b': escaped.append("\\b"); break;
                    case '\e': escaped.append("\\e"); break;
                    case '\f': escaped.append("\\f"); break;
                    case '\n': escaped.append("\\n"); break;
                    case '\r': escaped.append("\\r"); break;
                    case '\t': escaped.append("\\t"); break;
                    case '\v': escaped.append("\\v"); break;
                    default: {
                        char buf[256];

                        sprintf(buf, "%x", c);

                        escaped.append("\\x" + std::string(buf));
                    } break;
                }
            }

            return "\"" + escaped + "\"";
        }

        int get_tag() override {
            return EX_STRING_LITERAL;
        }
    };
}