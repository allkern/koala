#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "../lexer/lexer.hpp"
#include "type_system.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "statements/function_def.hpp"
#include "statements/variable_def.hpp"
#include "common.hpp"

namespace koala {
    class parser {
        lexer*                   m_lexer;
        std::vector <statement*> m_ast;
        type_system              m_ts;
        lexer_token              m_current;

        bool expect(int token);
        type_signature parse_type();
        expression* parse_expression();
        statement* parse_function_def();
        statement* parse_variable_def();
    
    public:
        parser(lexer& lexer) :
            m_lexer(&lexer) {};
        
        void parse();
    };
}