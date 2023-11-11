#include "lexer.hpp"

bool iseof(char c) {
    return c == -1;
}

std::unordered_map <std::string, int> g_ident_keyword_map = {
    { "fn"      , koala::TK_KEYWORD_FN     },
    { "const"   , koala::TK_KEYWORD_CONST  },
    { "static"  , koala::TK_KEYWORD_STATIC },
    { "mut"     , koala::TK_KEYWORD_MUT    },
    { "let"     , koala::TK_KEYWORD_LET    },
    { "typeof"  , koala::TK_KEYWORD_TYPEOF },
    { "return"  , koala::TK_KEYWORD_RETURN },
    { "while"   , koala::TK_KEYWORD_WHILE  },
    { "for"     , koala::TK_KEYWORD_FOR    },
    { "do"      , koala::TK_KEYWORD_DO     },
    { "if"      , koala::TK_KEYWORD_IF     },
    { "else"    , koala::TK_KEYWORD_ELSE   }
};

const char* g_token_names[] = {
    "TK_NONE",
    "TK_IDENT",
    "TK_INTEGER",
    "TK_STRING",
    "TK_OPENING_PARENT",
    "TK_OPENING_BRACE",
    "TK_OPENING_BRACKET",
    "TK_CLOSING_PARENT",
    "TK_CLOSING_BRACE",
    "TK_CLOSING_BRACKET",
    "TK_COLON",
    "TK_SEMICOLON",
    "TK_COMMA",
    "TK_ARROW",
    "TK_DOT",
    "TK_ASSIGNMENT_OPERATOR",
    "TK_BINARY_OPERATOR",
    "TK_UNARY_OPERATOR",
    "TK_KEYWORD_FN",
    "TK_KEYWORD_CONST",
    "TK_KEYWORD_STATIC",
    "TK_KEYWORD_MUT",
    "TK_KEYWORD_LET",
    "TK_KEYWORD_TYPEOF",
    "TK_KEYWORD_RETURN"
};

char koala::lexer::next() {
    char last_c = m_c;

    m_c = m_input->get();

    if (m_c == '\n') {
        ++m_loc.row;

        m_loc.column = 0;
    } else {
        m_loc.column++;
    }

    return last_c;
}

void koala::lexer::ignore_whitespace() {
    while (isspace(m_c))
        next();
}

koala::lexer_token& koala::lexer::push_token(int type) {
    m_output.push(lexer_token { m_loc, type, "" });

    return m_output.back();
}

int koala::lexer::lex_ident() {
    if (!(isalpha(m_c) || (m_c == '_')))
        return TK_NONE;

    lexer_token& tk = push_token(TK_IDENT);

    while (isalnum(m_c) || (m_c == '_'))
        tk.text.push_back(next());
    
    if (g_ident_keyword_map.contains(tk.text))
        tk.type = g_ident_keyword_map[tk.text];
    
    return tk.type;
}

int koala::lexer::lex_separator() {
    switch (m_c) {
        case '(': push_token(TK_OPENING_PARENT ).text = "("; next(); return TK_OPENING_PARENT;
        case '{': push_token(TK_OPENING_BRACE  ).text = "{"; next(); return TK_OPENING_BRACE;
        case '[': push_token(TK_OPENING_BRACKET).text = "["; next(); return TK_OPENING_BRACKET;
        case ')': push_token(TK_CLOSING_PARENT ).text = ")"; next(); return TK_CLOSING_PARENT;
        case '}': push_token(TK_CLOSING_BRACE  ).text = "}"; next(); return TK_CLOSING_BRACE;
        case ']': push_token(TK_CLOSING_BRACKET).text = "]"; next(); return TK_CLOSING_BRACKET;
        case ':': push_token(TK_COLON          ).text = ":"; next(); return TK_COLON;
        case ';': push_token(TK_SEMICOLON      ).text = ";"; next(); return TK_SEMICOLON;
        case ',': push_token(TK_COMMA          ).text = ","; next(); return TK_COMMA;
        case '.': push_token(TK_DOT            ).text = "."; next(); return TK_DOT;
        case '-': {
            if (m_input->peek() == '>') {
                next();
                next();

                push_token(TK_ARROW).text = "->";

                return TK_ARROW;
            }
        } break;
    }

    return TK_NONE;
}

bool isop(char c) {
    return (c == '+') || (c == '-') || (c == '*') || (c == '/') ||
           (c == '%') || (c == '&') || (c == '|') || (c == '^') ||
           (c == '!') || (c == '<') || (c == '>') || (c == '~') ||
           (c == '=') || (c == '.');
}

std::unordered_map <std::string, int> g_operator_type_map = {
    { "="  , koala::TK_ASSIGNMENT_OPERATOR },
    { "+=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "-=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "*=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "/=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "%=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "&=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "!&=", koala::TK_ASSIGNMENT_OPERATOR },
    { "|=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "!|=", koala::TK_ASSIGNMENT_OPERATOR },
    { "^=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "!^=", koala::TK_ASSIGNMENT_OPERATOR },
    { "<<=", koala::TK_ASSIGNMENT_OPERATOR },
    { ">>=", koala::TK_ASSIGNMENT_OPERATOR },
    { "&&=", koala::TK_ASSIGNMENT_OPERATOR },
    { "||=", koala::TK_ASSIGNMENT_OPERATOR },
    { "^^=", koala::TK_ASSIGNMENT_OPERATOR },
    { "==" , koala::TK_BINARY_OPERATOR },
    { "!=" , koala::TK_BINARY_OPERATOR },
    { "<=" , koala::TK_BINARY_OPERATOR },
    { ">=" , koala::TK_BINARY_OPERATOR },
    { "<"  , koala::TK_BINARY_OPERATOR },
    { ">"  , koala::TK_BINARY_OPERATOR },
    { "&&" , koala::TK_BINARY_OPERATOR },
    { "||" , koala::TK_BINARY_OPERATOR },
    { "^^" , koala::TK_BINARY_OPERATOR },
    { "+"  , koala::TK_BINARY_OPERATOR },
    { "-"  , koala::TK_BINARY_OPERATOR },
    { "*"  , koala::TK_BINARY_OPERATOR },
    { "/"  , koala::TK_BINARY_OPERATOR },
    { "%"  , koala::TK_BINARY_OPERATOR },
    { "&"  , koala::TK_BINARY_OPERATOR },
    { "!&" , koala::TK_BINARY_OPERATOR },
    { "|"  , koala::TK_BINARY_OPERATOR },
    { "!|" , koala::TK_BINARY_OPERATOR },
    { "^"  , koala::TK_BINARY_OPERATOR },
    { "!^" , koala::TK_BINARY_OPERATOR },
    { "<<" , koala::TK_BINARY_OPERATOR },
    { ">>" , koala::TK_BINARY_OPERATOR },
    { "++" , koala::TK_UNARY_OPERATOR },
    { "--" , koala::TK_UNARY_OPERATOR },
    { "!"  , koala::TK_UNARY_OPERATOR },
    { "~"  , koala::TK_UNARY_OPERATOR }
};

int koala::lexer::lex_operator() {
    if (!isop(m_c))
        return TK_NONE;

    std::string op;

    while (isop(m_c))
        op.push_back(next());
    
    if (!g_operator_type_map.contains(op)) {
        int type = 0;

        for (char c : op) {
            std::string sop = std::string(1, c);

            type = g_operator_type_map[sop];

            push_token(type).text = sop;
        }

        return type;
    }

    int type = g_operator_type_map[op];
    
    push_token(type).text = op;

    return type;
}

bool isliteral(char c) {
    return isdigit(c) || (c == '\'');
}

int koala::lexer::lex_char() {
    int c = 0;

    if (m_c == '\\') {
        next();

        switch (m_c) {
            case '0' : c = 0x00; next(); break;
            case 'a' : c = 0x07; next(); break;
            case 'b' : c = 0x08; next(); break;
            case 'e' : c = 0x1b; next(); break;
            case 'f' : c = 0x0c; next(); break;
            case 'n' : c = 0x0a; next(); break;
            case 'r' : c = 0x0d; next(); break;
            case 't' : c = 0x09; next(); break;
            case 'v' : c = 0x0b; next(); break;
            case '?' : c = 0x3f; next(); break;
            case '\\': c = 0x5c; next(); break;
            case '\'': c = 0x27; next(); break;
            case '\"': c = 0x22; next(); break;
            case 'x' : {
                std::string code;

                next();

                while (isxdigit(m_c))
                    code.push_back(next());

                c = std::stoul(code, nullptr, 16);
            } break;
        }
    } else {
        c = next();
    }

    return c;
}

int koala::lexer::lex_literal() {
    // To-do: Handle char literals, hexadecimal, octal, binary
    if (!isliteral(m_c))
        return TK_NONE;
    
    lexer_token& tk = push_token(TK_INTEGER);

    if (m_c == '\'') {
        next();

        int c = lex_char();

        if (m_c != '\'')
            return TK_ERROR;

        next();

        tk.text = std::to_string(c);

        return TK_INTEGER;
    }

    switch (m_input->peek()) {
        case 'x': {
            tk.text = "0x";

            next();
            next();

            while (isxdigit(m_c) || (m_c == '\'') || (m_c == '_')) {
                if ((m_c == '\'') || (m_c == '_')) {
                    next();

                    continue;
                }

                tk.text.push_back(next());
            }

            tk.text = std::to_string(std::stoull(tk.text, nullptr, 0));
        } break;

        case 'o': {
            tk.text = "0";

            next();
            next();

            while (isdigit(m_c) || (m_c == '\'') || (m_c == '_')) {
                if ((m_c == '\'') || (m_c == '_')) {
                    next();

                    continue;
                }

                tk.text.push_back(next());
            }

            tk.text = std::to_string(std::stoull(tk.text, nullptr, 0));
        } break;

        case 'b': {
            next();
            next();

            while ((m_c == '0') || (m_c == '1') || (m_c == '\'') || (m_c == '_')) {
                if ((m_c == '\'') || (m_c == '_')) {
                    next();

                    continue;
                }

                tk.text.push_back(next());
            }

            tk.text = std::to_string(std::stoull(tk.text, nullptr, 2));
        } break;

        default: {
            while (isdigit(m_c))
                tk.text.push_back(next());
        } break;
    }

    return TK_INTEGER;
}

int koala::lexer::lex_string() {
    if (m_c != '\"')
        return TK_NONE;
    
    next();

    lexer_token& tk = push_token(TK_STRING);

    while (m_c != '\"')
        tk.text.push_back(lex_char());
    
    next();

    return TK_STRING;
}

void koala::lexer::lex() {
    while (!iseof(m_c)) {
        ignore_whitespace();

        if (lex_ident()) goto lexed;
        if (lex_separator()) goto lexed;
        if (lex_operator()) goto lexed;
        if (lex_literal()) goto lexed;
        if (lex_string()) goto lexed;

        printf("Unknown token at line %u col %u\n",
            m_output.back().loc.row + 1,
            m_output.back().loc.column + 1
        );

        std::exit(1);

        lexed:

        continue;

        // std::cout << g_token_names[m_output.back().type] << " (" << m_output.back().text << ")" << std::endl;
    }
}

koala::lexer_token koala::lexer::pop() {
    if (m_output.empty())
        return { m_loc, TK_EOF, "<eof>" };

    lexer_token tk = m_output.front();

    m_output.pop();

    return tk;
}

koala::lexer_token& koala::lexer::peek() {
    return m_output.front();
}

bool koala::lexer::empty() {
    return m_output.empty();
}