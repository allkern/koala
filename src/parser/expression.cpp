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
    { "*" , 10 }, { "%" , 10 },
    { "+" , 9  }, { "-" , 9  },
    { "<<", 8  }, { ">>", 8  },
    { "<" , 7  }, { "<=", 7  }, { ">" , 7 }, { ">=", 7 },
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

        case TK_OPERATOR: {
            unary_expr ue;

            ue.op = m_current.text;

            m_current = m_lexer->pop();

            ue.expr = parse_primary();

            expr = new unary_expr(ue);
        } break;

        case TK_INTEGER: {
            integer_expr ie;

            ie.value = m_current.text;

            m_current = m_lexer->pop();

            expr = new integer_expr(ie);
        } break;
    }

    return expr;
}

koala::expression* koala::parser::parse_expression() {
    return parse_expression_impl(parse_primary(), 0);
}

koala::expression* koala::parser::parse_expression_impl(expression* lhs, int mp) {
    int p = g_operator_precedence_map[m_current.text];

    while ((m_current.type == TK_OPERATOR) && (p >= mp)) {
        std::string op = m_current.text;

        m_current = m_lexer->pop();

        expression* rhs = parse_primary();

        p = g_operator_precedence_map[m_current.text];

        int q = g_operator_precedence_map[op];

        while ((m_current.type == TK_OPERATOR) && (p >= q)) {
            rhs = parse_expression_impl(rhs, p + (q > p));

            p = g_operator_precedence_map[m_current.text];
        }

        binary_expr be;

        be.left = lhs;
        be.right = rhs;
        be.op = op;

        lhs = new binary_expr(be);
    }

    return lhs;
}

// koala::expression* koala::parser::parse_expression() {
//     if (m_current.type == TK_OPENING_PARENT) {
//         m_current = m_lexer->pop();

//         // printf("Found opening parent, recursing...\n");
//         expression* expr = parse_expression();

//         if (m_current.type != TK_CLOSING_PARENT) {
//             printf("Expected \')\'");

//             std::exit(1);
//         }

//         // printf("Found closing parent\n");

//         m_current = m_lexer->pop();

//         // Handle binary operator
//         if (m_current.type == TK_OPERATOR) {
//             binary_expr be;

//             be.op = m_current.text;
//             be.left = expr;

//             m_current = m_lexer->pop();

//             // printf("Found post operator after closing parent, recursing...\n");
//             be.right = parse_expression();

//             return new binary_expr(be);
//         }

//         return expr;
//     }

//     // Handle pre operator
//     if (m_current.type == TK_OPERATOR) {
//         unary_expr ue;

//         ue.op = m_current.text;

//         m_current = m_lexer->pop();

//         // printf("Found pre operator, recursing...\n");
//         ue.expr = parse_expression();

//         expression* expr = new unary_expr(ue);

//         // Handle post operator
//         if (m_current.type == TK_OPERATOR) {
//             binary_expr be;

//             be.op = m_current.text;
//             be.left = expr;

//             m_current = m_lexer->pop();

//             // printf("Found post operator after unary op expression, recursing...\n");
//             be.right = parse_expression();

//             return new binary_expr(be);
//         }

//         return expr;
//     }

//     // printf("Not found pre operator or opening parent, parsing leaf\n");

//     expression* expr = nullptr;

//     switch (m_current.type) {
//         case TK_INTEGER: {
//             // printf("Found integer\n");

//             expr = parse_integer_expr();
//         } break;
//     }

//     // Handle post operator
//     if (m_current.type == TK_OPERATOR) {
//         binary_expr be;

//         be.op = m_current.text;
//         be.left = expr;

//         m_current = m_lexer->pop();

//         // printf("Found post operator, recursing...\n");
//         be.right = parse_expression();

//         return new binary_expr(be);
//     }

//     // printf("Done parsing leaf\n");

//     return expr;
// }