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

    if (!expect(TK_COLON))
        return nullptr;

    m_current = m_lexer->pop();

    fd.body = parse_expression();

    printf("function-def %s -> %s args:\n",
        fd.name.c_str(),
        get_signature_string(fd.return_type->sig).c_str()
    );

    for (variable_info& v : fd.args) {
        printf("    %s -> %s\n",
            v.name.c_str(),
            get_signature_string(v.type->sig).c_str()
        );
    }

    printf("body: %s",
        fd.body->print(0).c_str()
    );

    return new function_def(fd);
}