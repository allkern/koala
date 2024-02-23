#include "if_else.hpp"

koala::statement* koala::parser::parse_if_else() {
    if_else ie;

    m_current = m_lexer->pop();

    ie.cond = parse_expression();
    ie.if_path = parse_statement();

    if (m_current.type == TK_KEYWORD_ELSE) {
        m_current = m_lexer->pop();

        ie.else_path = parse_statement();
    }

    return new if_else(ie);
}