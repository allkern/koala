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
        function_arg fa;

        while (m_current.type != TK_IDENT) {
            switch (m_current.type) {
                case TK_KEYWORD_LET: break;
                case TK_KEYWORD_MUT: fa.mut = true; break;
                case TK_KEYWORD_CONST: fa.mut = false; break;

                default: {
                    printf("Unexpected token \'%s\'\n", m_current.text.c_str());

                    std::exit(1);
                } break;
            }

            m_current = m_lexer->pop();
        }

        fa.name = m_current.text;

        m_current = m_lexer->pop();

        if (m_current.type != TK_COLON) {
            printf("Expected \':\' before type\n");

            std::exit(1);
        }

        m_current = m_lexer->pop();

        fa.t = parse_type();

        fd.args.push_back(fa);

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

    if (m_current.type == TK_ARROW) {
        m_current = m_lexer->pop();

        fd.return_type = parse_type();
    }

    fd.body = parse_statement();

    return new function_def(fd);
}