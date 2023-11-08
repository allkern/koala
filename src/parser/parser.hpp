#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <stack>

#include "../lexer/lexer.hpp"
#include "type_system.hpp"
#include "expression.hpp"
#include "expressions/integer_constant.hpp"
#include "expressions/binary_op.hpp"
#include "expressions/name_ref.hpp"
#include "expressions/unary_op.hpp"
#include "expressions/string_literal.hpp"
#include "expressions/member_access.hpp"
#include "expressions/function_call.hpp"
#include "expressions/array_access.hpp"
#include "statement.hpp"
#include "statements/assignment.hpp"
#include "statements/function_def.hpp"
#include "statements/function_call.hpp"
#include "statements/variable_def.hpp"
#include "statements/return_expr.hpp"

#include "common.hpp"

namespace koala {
    class parser {
        lexer*                   m_lexer;
        std::vector <statement*> m_ast;
        type_system              m_ts;
        lexer_token              m_current;

        bool expect(int token);
        type* parse_type();
        expression* parse_primary();
        expression* parse_expression();
        expression* parse_expression_impl(expression* lhs, int mp);
        statement* parse_statement();
        statement* parse_assignment(std::string name);
        statement* parse_function_call(std::string name);
        statement* parse_function_def();
        statement* parse_variable_def();
        statement* parse_return_expr();
    
    public:
        parser(lexer& lexer) :
            m_lexer(&lexer) {};
        
        void parse();
        std::vector <statement*>* get_ast();
        type_system* get_type_system();
        void execute_function(std::string name);
    };
}