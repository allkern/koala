#include "assignment.hpp"

koala::statement* koala::parser::parse_assignment(std::string name) {
    assignment a;

    a.dest = name;
    a.op = m_current.text;

    m_current = m_lexer->pop();

    a.src = parse_expression();

    return new assignment(a);
}