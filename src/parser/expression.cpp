#include "expression.hpp"
#include "parser.hpp"

#include <unordered_map>

/*
parse_expression()
    return parse_expression_1(parse_primary(), 0)

parse_expression_1(lhs, min_precedence)
    lookahead := peek next token
    while lookahead is a binary operator whose precedence is >= min_precedence
        op := lookahead
        advance to next token
        rhs := parse_primary ()
        lookahead := peek next token
        while lookahead is a binary operator whose precedence is greater
                 than op's, or a right-associative operator
                 whose precedence is equal to op's
            rhs := parse_expression_1 (rhs, precedence of op + (1 if lookahead precedence is greater, else 0))
            lookahead := peek next token
        lhs := the result of applying op with operands lhs and rhs
    return lhs
*/

std::unordered_map <std::string, int> g_operator_precedence_map = {
    { "*" , 10 }, { "%" , 10 }, { "/" , 10 },
    { "+" , 9  }, { "-" , 9  },
    { "<<", 8  }, { ">>", 8  },
    { "<" , 7  }, { "<=", 7  }, { ">" , 7  }, { ">=", 7 },
    { "==", 6  }, { "!=", 6  },
    { "&" , 5  },
    { "^" , 4  },
    { "|" , 3  },
    { "&&", 2  },
    { "^^", 1  },
    { "||", 0  }
};

koala::expression* koala::parser::parse_primary() {
    expression* expr;

    switch (m_current.type) {
        case TK_OPENING_PARENT: {
            m_current = m_lexer->pop();

            expr = parse_expression();

            if (m_current.type != TK_CLOSING_PARENT) {
                printf("Expected \')\'\n");

                std::exit(1);
            }

            m_current = m_lexer->pop();
        } break;

        case TK_UNARY_OPERATOR: {
            unary_op ue;

            ue.op = m_current.text;

            m_current = m_lexer->pop();

            ue.expr = parse_primary();

            expr = new unary_op(ue);
        } break;

        case TK_BINARY_OPERATOR: {
            if (m_current.text != "-") {
                printf("Expected unary operator, got \'%s\'\n", m_current.text.c_str());

                std::exit(1);
            }

            unary_op ue;

            ue.post = false;
            ue.op = m_current.text;

            m_current = m_lexer->pop();

            ue.expr = parse_primary();

            expr = new unary_op(ue);
        } break;

        case TK_INTEGER: {
            integer_constant ie;

            ie.value = m_current.text;

            m_current = m_lexer->pop();

            expr = new integer_constant(ie);
        } break;

        case TK_IDENT: {
            if (m_ts.get_type(m_current.text)) {
                printf("Name reference names a type\n");

                std::exit(1);
            }

            name_ref nr;

            nr.name = m_current.text;
            
            m_current = m_lexer->pop();

            expr = new name_ref(nr);
        }
    }

    if (m_current.type == TK_UNARY_OPERATOR) {
        unary_op ue;

        ue.post = true;
        ue.op = m_current.text;

        m_current = m_lexer->pop();

        ue.expr = expr;

        expr = new unary_op(ue);
    }

    // To-do: Check for function call ('(')
    //        Check for array access ('[')

    return expr;
}

koala::expression* koala::parser::parse_expression() {
    return parse_expression_impl(parse_primary(), 0);
}

koala::expression* koala::parser::parse_expression_impl(expression* lhs, int mp) {
    int p = g_operator_precedence_map[m_current.text];

    while ((m_current.type == TK_BINARY_OPERATOR) && (p >= mp)) {
        std::string op = m_current.text;

        m_current = m_lexer->pop();

        expression* rhs = parse_primary();

        p = g_operator_precedence_map[m_current.text];

        int q = g_operator_precedence_map[op];

        while ((m_current.type == TK_BINARY_OPERATOR) && (p >= q)) {
            rhs = parse_expression_impl(rhs, p + (q > p));

            p = g_operator_precedence_map[m_current.text];
        }

        binary_op be;

        be.left = lhs;
        be.right = rhs;
        be.op = op;

        lhs = new binary_op(be);
    }

    return lhs;
}