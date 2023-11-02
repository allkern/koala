#include "../lexer/lexer.hpp"
#include "parser.hpp"

const char* g_readable_token_names[] = {
    "none",
    "identifier",
    "integer-literal",
    "string-literal",
    "(",
    "{",
    "[",
    ")",
    "}",
    "]",
    ",",
    ";",
    ",",
    "->",
    "operator",
    "fn",
    "const",
    "static",
    "mut"
};

bool koala::parser::expect(int token) {
    m_current = m_lexer->pop();

    if (m_current.type != token) {
        printf("Expected \'%s\', but got \'%s\' instead\n",
            g_readable_token_names[token],
            g_readable_token_names[m_current.type]
        );

        std::exit(1);
    }

    return true;
}

void koala::parser::parse() {
    m_current = m_lexer->pop();

    while (m_current.type != TK_EOF) {
        std::cout << parse_expression()->print(0) << std::endl;

        printf("-----------------------------------\n");

        // m_ast.push_back(parse_statement());

        // if (!m_ast.front())
        //     std::exit(1);
    }
}