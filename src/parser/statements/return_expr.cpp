#include "return_expr.hpp"

koala::statement* koala::parser::parse_return_expr() {
    return_expr re;

    m_current = m_lexer->pop();

    if (m_current.type == TK_SEMICOLON)
        return new return_expr(re);

    re.expr = parse_expression();

    return new return_expr(re);
}