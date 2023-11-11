#include "statement.hpp"
#include "parser.hpp"

#include "statements/expression_statement.hpp"
#include "statements/function_def.hpp"
#include "statements/variable_def.hpp"
#include "statements/return_expr.hpp"
#include "statements/assignment.hpp"

koala::statement* koala::parser::parse_statement() {
    statement* stmt = nullptr;

    switch (m_current.type) {
        case TK_KEYWORD_FN: {
            stmt = parse_function_def();
        } break;

        case TK_KEYWORD_RETURN: {
            stmt = parse_return_expr();
        } break;

        case TK_KEYWORD_CONST: case TK_KEYWORD_STATIC:
        case TK_KEYWORD_MUT: case TK_KEYWORD_LET: {
            stmt = parse_variable_def();
        } break;

        case TK_IDENT: case TK_OPENING_PARENT: case TK_INTEGER: {
            expression* expr = parse_expression();

            if (m_current.type == TK_ASSIGNMENT_OPERATOR) {
                stmt = parse_assignment(expr);
            } else {
                stmt = new expression_statement(expr);
            }
        } break;

        case TK_SEMICOLON: {
            m_current = m_lexer->pop();
        } break;
    }

    return stmt;
}