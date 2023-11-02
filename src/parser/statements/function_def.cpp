#include "function_def.hpp"

koala::statement* koala::parser::parse_function_def() {
    if (m_current.type != TK_KEYWORD_FN)
        return nullptr;

    if (!expect(TK_IDENT))
        return nullptr;

    function_def fd;
    
    fd.name = m_current.text;

    if (!expect(TK_OPENING_PARENT))
        return nullptr;
    
    m_current = m_lexer->pop();

    while (m_current.type != TK_CLOSING_PARENT) {
        type_signature type = parse_type();
        
        if (m_current.type != TK_IDENT) {
            printf("Expected name after type\n");

            std::exit(1);
        }

        variable_info arg;
        
        arg.name = m_current.text;
        arg.type = m_ts.get_type(type);

        fd.args.push_back(arg);

        m_current = m_lexer->pop();

        if (m_current.type == TK_COMMA) {
            m_current = m_lexer->pop();
        } else if (m_current.type == TK_CLOSING_PARENT) {
            break;
        } else {
            printf("Expected \',\' or \')\'\n");

            std::exit(1);
        }
    }

    if (!expect(TK_ARROW))
        return nullptr;

    m_current = m_lexer->pop();

    fd.return_type = m_ts.get_type(parse_type());

    if (m_current.type != TK_OPENING_BRACE)
        return nullptr;

    m_current = m_lexer->pop();

    while (m_current.type != TK_CLOSING_BRACE) {
        fd.body.push_back(parse_statement());

        if (m_current.type != TK_SEMICOLON) {
            printf("Expected \';\' after statement");

            std::exit(1);
        }

        m_current = m_lexer->pop();
    }

    m_current = m_lexer->pop();

    return new function_def(fd);
}