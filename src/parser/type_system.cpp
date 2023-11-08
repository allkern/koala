#include "type_system.hpp"
#include "parser.hpp"

#include <sstream>

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
    type* t;

    switch (m_current.type) {
        case TK_KEYWORD_TYPEOF: {
            m_current = m_lexer->pop();

            if (m_current.type != TK_OPENING_PARENT) {
                printf("Expected \'(\'\n");

                std::exit(1);
            }

            m_current = m_lexer->pop();

            typeof_type* tt = new typeof_type(parse_expression());

            if (m_current.type != TK_CLOSING_PARENT) {
                printf("Expected \')\'\n");

                std::exit(1);
            }

            m_current = m_lexer->pop();

            t = tt;
        } break;

        case TK_IDENT: {
            t = m_ts.get_type(m_current.text);

            if (!t) {
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

            t = ft;
        } break;

        case TK_OPENING_BRACE: {
            struct_type* st = new struct_type();

            m_current = m_lexer->pop();

            while (m_current.type != TK_CLOSING_BRACE) {
                struct_member m;

                if (m_current.type != TK_IDENT) {
                    printf("Expected member name\n");

                    std::exit(1);
                }

                m.name = m_current.text;

                m_current = m_lexer->pop();

                if (m_current.type != TK_COLON) {
                    printf("Expected \':\'\n");

                    std::exit(1);
                }

                m_current = m_lexer->pop();

                m.t = parse_type();

                st->members.push_back(m);

                if (m_current.type == TK_COMMA)
                    m_current = m_lexer->pop();
            }

            m_current = m_lexer->pop();

            t = st;
        } break;

        default: {
            printf("Expected type\n");

            std::exit(1);
        } break;
    }

    while (m_current.text == "*") {
        t = new pointer_type(t);

        m_current = m_lexer->pop();
    }

    return (t->get_class() != TP_TYPEOF) ? m_ts.add_type(t) : t;
}

// To-do: Implement full type parser here
//        So we can do stuff like add_type("((char*, int) -> int)")
// koala::type* koala::type_system::add_type(std::string sig) {
//     lexer* m_lexer;
// }

void koala::type_system::init() {
    struct_type* st = new struct_type();
    pointer_type* pt = new pointer_type(get_type("char"));

    st->members.push_back({ get_type("u32"), "size" });
    st->members.push_back({ add_type(pt)   , "ptr"  });

    m_type_map["koala_string"] = st;
}