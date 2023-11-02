#include "statement.hpp"
#include "parser.hpp"

#include "statements/function_def.hpp"
#include "statements/variable_def.hpp"

koala::statement* koala::parser::parse_statement() {
    statement* stmt = nullptr;

    switch (m_current.type) {
        case TK_KEYWORD_FN: {
            stmt = parse_function_def();
        } break;

        case TK_KEYWORD_CONST: case TK_KEYWORD_STATIC: case TK_KEYWORD_MUT: {
            stmt = parse_variable_def();
        } break;

        case TK_IDENT: {
            if (m_ts.is_type(m_current.text)) {
                stmt = parse_variable_def();
            } else {
                printf("Call, assignment, etc.");

                std::exit(1);
            }
        } break;
    }

    return stmt;
}