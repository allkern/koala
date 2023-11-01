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

    while (!m_lexer->empty()) {
        statement* stmt = nullptr;

        switch (m_current.type) {
            case TK_KEYWORD_FN: stmt = parse_function_def(); break;
            case TK_KEYWORD_CONST: case TK_KEYWORD_STATIC: case TK_KEYWORD_MUT: {
                stmt = parse_variable_def();
            } break;

            case TK_IDENT: {
                if (m_ts.is_type(m_current.text)) {
                    stmt = parse_variable_def();
                } else {
                    printf("Call, assignment, etc.");

                    std::exit(1);
                }
            } break;
        }
    }
}