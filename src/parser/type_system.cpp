#include "type_system.hpp"
#include "parser.hpp"

koala::type* koala::type_system::add_type(type* t) {
    std::string sig = t->str();

    type* u = get_type(sig);

    if (u) {
        // This happens only when the type is a plain type
        // (e.g. int), this causes get_type to return
        // a pointer that's already on the type map.
        // Which is then checked again for addition later
        // and deleted because it already exists.
        if (t != u)
            delete t;

        return u;
    }

    m_type_map[sig] = t;

    return t;
}

koala::type* koala::type_system::get_type(std::string sig) {
    if (m_type_map.contains(sig))
        return m_type_map[sig];

    return nullptr;
}

bool koala::type_system::is_equal(type* t, type* u) {
    return false;
}

bool koala::type_system::is_comparable(type* t, type* u) {
    return false;
}

koala::type* koala::parser::parse_type() {
    type* type;

    switch (m_current.type) {
        case TK_IDENT: {
            type = m_ts.get_type(m_current.text);

            if (!type) {
                printf("\'%s\' does not name a type", m_current.text.c_str());

                std::exit(0);
            }

            m_current = m_lexer->pop();
        } break;

        case TK_OPENING_PARENT: {
            function_type* ft = new function_type();

            m_current = m_lexer->pop();

            if (m_current.type != TK_OPENING_PARENT) {
                std::printf("Expected \'(\'\n");

                std::exit(1);
            }

            m_current = m_lexer->pop();

            while (m_current.type != TK_CLOSING_PARENT) {
                ft->arg_types.push_back(parse_type());

                if (m_current.type == TK_COMMA)
                    m_current = m_lexer->pop();
            }

            m_current = m_lexer->pop();

            if (m_current.type != TK_ARROW) {
                printf("Expected \'->\' after argument list\n");

                std::exit(1);
            }

            m_current = m_lexer->pop();

            ft->return_type = parse_type();

            if (m_current.type != TK_CLOSING_PARENT) {
                printf("Expected \')\' after function type\n");

                std::exit(1);
            }

            m_current = m_lexer->pop();

            type = ft;
        } break;

        case TK_OPENING_BRACE: {
            struct_type* st = new struct_type();

            m_current = m_lexer->pop();

            while (m_current.type != TK_CLOSING_BRACE) {
                st->member_types.push_back(parse_type());

                if (m_current.type == TK_COMMA)
                    m_current = m_lexer->pop();
            }

            m_current = m_lexer->pop();

            type = st;
        } break;
    }

    while (m_current.text == "*") {
        type = new pointer_type(type);

        m_current = m_lexer->pop();
    }

    return m_ts.add_type(type);
}