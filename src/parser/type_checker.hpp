#pragma once

#include "../common.hpp"
#include "type_system.hpp"
#include "parser.hpp"

namespace koala {
    class type_checker {
        std::vector <symbol> m_local_symbols;
        std::vector <symbol> m_global_symbols;

        parser* m_parser;

        void type_check(assignment* a) {

        }

        void type_check(statement* s) {
            switch (s->get_tag()) {
                case ST_ASSIGNMENT: {
                    
                } break;
            }
        }

    public:
        type_checker(parser& parser) :
            m_parser(&parser) {};

    };
}