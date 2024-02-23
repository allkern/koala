#include "compound.hpp"

koala::statement* koala::parser::parse_compound() {
    if (m_current.type != TK_OPENING_BRACE)
        return nullptr;

    compound cs;

    m_current = m_lexer->pop();

    while (m_current.type != TK_CLOSING_BRACE) {
        statement* stmt = parse_statement();

        if (stmt)
            cs.body.push_back(stmt);
    }

    m_current = m_lexer->pop();

    return new compound(cs);
}