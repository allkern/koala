#include "assignment.hpp"

koala::statement* koala::parser::parse_assignment(koala::expression* expr) {
    assignment a;

    a.dst = expr;
    a.op = m_current.text;

    m_current = m_lexer->pop();

    a.src = parse_expression();

    return new assignment(a);
}