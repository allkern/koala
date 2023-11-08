#include "lexer.hpp"

bool iseof(char c) {
    return c == -1;
}

std::unordered_map <std::string, int> g_ident_keyword_map = {
    { "fn"      , koala::TK_KEYWORD_FN },
    { "const"   , koala::TK_KEYWORD_CONST },
    { "static"  , koala::TK_KEYWORD_STATIC },
    { "mut"     , koala::TK_KEYWORD_MUT },
    { "let"     , koala::TK_KEYWORD_LET },
    { "typeof"  , koala::TK_KEYWORD_TYPEOF },
    { "return"  , koala::TK_KEYWORD_RETURN }
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
    { "&=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "|=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "^=" , koala::TK_ASSIGNMENT_OPERATOR },
    { "<<=", koala::TK_ASSIGNMENT_OPERATOR },
    { ">>=", koala::TK_ASSIGNMENT_OPERATOR },
    { "&&=", koala::TK_ASSIGNMENT_OPERATOR },
    { "||=", koala::TK_ASSIGNMENT_OPERATOR },
    { "^^=", koala::TK_ASSIGNMENT_OPERATOR },
    { "%=" , koala::TK_ASSIGNMENT_OPERATOR },
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
    { "~"  , koala::TK_UNARY_OPERATOR },
    { "."  , koala::TK_UNARY_OPERATOR }
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

int koala::lexer::lex_literal() {
    // To-do: Handle char literals, hexadecimal, octal, binary
    if (!isdigit(m_c))
        return TK_NONE;
    
    lexer_token& tk = push_token(TK_INTEGER);

    while (isdigit(m_c))
        tk.text.push_back(next());
    
    return TK_INTEGER;
}

int koala::lexer::lex_string() {
    if (m_c != '\"')
        return TK_NONE;
    
    next();

    lexer_token& tk = push_token(TK_STRING);

    while (m_c != '\"')
        tk.text.push_back(next());
    
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