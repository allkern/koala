#include "statement.hpp"
#include "parser.hpp"

#include "statements/function_call.hpp"
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

        case TK_IDENT: {
            std::string name = m_current.text;

            m_current = m_lexer->pop();

            switch (m_current.type) {
                case TK_ASSIGNMENT_OPERATOR: {
                    stmt = parse_assignment(name);
                } break;

                case TK_OPENING_PARENT: {
                    stmt = parse_function_call(name);
                } break;

                default: {
                    printf("Expected an assignment or call\n");

                    std::exit(1);
                } break;
            }
        } break;
    }

    return stmt;
}