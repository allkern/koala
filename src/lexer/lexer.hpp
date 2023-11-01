#pragma once

#include "../common.hpp"

#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>

namespace koala {
    struct lexer_token {
        location_info loc;

        int type;

        std::string text;
    };

    class lexer {
        location_info            m_loc;
        std::istream*            m_input;
        std::queue <lexer_token> m_output;
        char                     m_c;

        char next();
        void ignore_whitespace();
        lexer_token& push_token(int type);
        int lex_ident();
        int lex_separator();
        int lex_operator();
        int lex_literal();
        int lex_string();

    public:
        lexer(std::istream& input, const std::string& path) {
            m_input = &input;
            m_loc = { 0, 0, path };
            m_c = m_input->get();
        };

        void lex();
        lexer_token pop();
        lexer_token& peek();
        bool empty();
    };

    enum : int {
        TK_ERROR = -1,
        TK_NONE,
        TK_IDENT,
        TK_INTEGER,
        TK_STRING,
        TK_OPENING_PARENT,
        TK_OPENING_BRACE,
        TK_OPENING_BRACKET,
        TK_CLOSING_PARENT,
        TK_CLOSING_BRACE,
        TK_CLOSING_BRACKET,
        TK_COLON,
        TK_SEMICOLON,
        TK_COMMA,
        TK_ARROW,
        TK_OPERATOR,
        TK_KEYWORD_FN,
        TK_KEYWORD_CONST,
        TK_KEYWORD_STATIC,
        TK_KEYWORD_MUT
    };
}