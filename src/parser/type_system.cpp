#include "type_system.hpp"
#include "parser.hpp"

bool koala::type_system::is_type(std::string ident) {
    return m_type_map.contains(get_signature_string(type_signature(ident)));
}

koala::type* koala::type_system::get_type(type_signature sig) {
    std::string sig_str = get_signature_string(sig);

    if (m_type_map.contains(sig_str))
        return m_type_map[sig_str];
    
    if (!is_type(sig.base)) {
        printf("Base type does not name a type\n");

        std::exit(1);
    }

    type* nty = nullptr;
    type* bty = get_type(type_signature(sig.base));

    switch (bty->get_type()) {
        case TP_INTEGRAL: {
            integral_type* it = (integral_type*)bty;

            nty = new integral_type(*it);
        } break;

        case TP_POINTER: {
            pointer_type* pt = (pointer_type*)bty;

            nty = new pointer_type(*pt);
        } break;
    }

    nty->is_mut = sig.is_mut;
    nty->is_static = sig.is_static;

    int pointer_rank = sig.pointer_rank;

    while (pointer_rank) {
        nty = new pointer_type(sig, nty);

        pointer_rank--;
    }

    nty->sig = sig;

    m_type_map[sig_str] = nty;

    return nty;
}

bool koala::type_system::is_equal(type* t, type* u) {
    return false;
}

bool koala::type_system::is_comparable(type* t, type* u) {
    return false;
}

koala::type_signature koala::parser::parse_type() {
    type_signature sig;

    sig.is_mut = false;
    sig.is_static = false;

    while (true) {
        switch (m_current.type) {
            case TK_IDENT: {
                if (!m_ts.is_type(m_current.text)) {
                    printf("Base type does not name a type\n");

                    std::exit(1);
                }

                sig.base = m_current.text;

                m_current = m_lexer->pop();

                while (m_current.text == "*") {
                    ++sig.pointer_rank;

                    m_current = m_lexer->pop();
                }

                return sig;
            } break;

            case TK_KEYWORD_MUT: {
                sig.is_mut = true;

                m_current = m_lexer->pop();
            } break;

            case TK_KEYWORD_CONST: {
                sig.is_mut = false;

                m_current = m_lexer->pop();
            } break;

            case TK_KEYWORD_STATIC: {
                sig.is_static = true;

                m_current = m_lexer->pop();
            } break;

            default: {
                printf("Expected type\n");

                std::exit(1);
            } break;
        }
    }
}