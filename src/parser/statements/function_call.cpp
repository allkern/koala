#include "function_call.hpp"

koala::statement* koala::parser::parse_function_call(std::string name) {
    function_call fc;

    fc.callee = name;
    
    m_current = m_lexer->pop();

    while (m_current.type != TK_CLOSING_PARENT) {
        fc.args.push_back(parse_expression());

        if (m_current.type == TK_COMMA) {
            m_current = m_lexer->pop();
        } else if (m_current.type == TK_CLOSING_PARENT) {
            break;
        } else {
            printf("Expected \',\' or \')\'\n");

            std::exit(1);
        }
    }

    m_current = m_lexer->pop();

    return new function_call(fc);
}