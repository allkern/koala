#include "variable_def.hpp"

koala::statement* koala::parser::parse_variable_def() {
    variable_def vd;

    type_signature sig = parse_type();

    if (m_current.type != TK_IDENT) {
        printf("Expected variable name after type in variable-def\n");

        std::exit(1);
    }

    vd.name = m_current.text;
    vd.type = m_ts.get_type(sig);

    m_current = m_lexer->pop();

    if (m_current.type == TK_ASSIGNMENT_OPERATOR) {
        vd.assignment_op = m_current.text;

        m_current = m_lexer->pop();

        vd.init = parse_expression();
    }

    return new variable_def(vd);
}