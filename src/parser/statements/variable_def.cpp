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

    if (m_current.type == TK_OPERATOR) {
        m_current = m_lexer->pop();

        printf("variable def with init\n");

        std::exit(1);

        vd.init = parse_expression();
    }

    printf("variable-def %s -> %s\n",
        vd.name.c_str(),
        get_signature_string(vd.type->sig).c_str()
    );

    return new variable_def(vd);
}