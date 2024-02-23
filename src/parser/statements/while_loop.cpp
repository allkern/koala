#include "while_loop.hpp"

koala::statement* koala::parser::parse_while_loop() {
    while_loop wl;

    m_current = m_lexer->pop();

    if (m_current.type != TK_OPENING_PARENT) {
        printf("Expected \'(\'\n");

        std::exit(1);
    }

    m_current = m_lexer->pop();

    wl.cond = parse_expression();

    if (m_current.type != TK_CLOSING_PARENT) {
        printf("Expected \')\'\n");

        std::exit(1);
    }

    m_current = m_lexer->pop();

    wl.body = parse_statement();

    return new while_loop(wl);
}