#include "variable_def.hpp"

koala::statement* koala::parser::parse_variable_def() {
    variable_def vd;

    while (m_current.type != TK_IDENT) {
        switch (m_current.type) {
            case TK_KEYWORD_LET: break;
            case TK_KEYWORD_MUT: vd.mut = true; break;
            case TK_KEYWORD_CONST: vd.mut = false; break;
            case TK_KEYWORD_STATIC: vd.storage_duration = SD_STATIC; break;

            default: {
                printf("Unexpected token\n");

                std::exit(1);
            } break;
        }

        m_current = m_lexer->pop();
    }

    vd.name = m_current.text;

    m_current = m_lexer->pop();

    if (m_current.type == TK_COLON) {
        m_current = m_lexer->pop();

        vd.type = parse_type();
    } 

    if (m_current.type == TK_ASSIGNMENT_OPERATOR) {
        vd.assignment_op = m_current.text;

        m_current = m_lexer->pop();

        vd.init = parse_expression();
    } else {
        if (!vd.type) {
            printf("Cannot define a variable with automatic typing without initialization\n");

            std::exit(1);
        }
    }

    return new variable_def(vd);
}