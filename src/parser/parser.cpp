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
    ":",
    ";",
    ",",
    "->",
    "assignment-operator",
    "binary-operator",
    "unary-operator",
    "fn",
    "const",
    "static",
    "mut",
    "let",
    "typeof",
    "return"
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
        m_ast.push_back(parse_statement());
    }

    for (statement* s : m_ast)
        std::cout << s->print(0) << std::endl;
}

std::vector <koala::statement*>* koala::parser::get_ast() {
    return &m_ast;
}

koala::type_system* koala::parser::get_type_system() {
    return &m_ts;
}

namespace koala {
    struct var {
        std::string name;
        uint32_t value;
    };

    var& lookup_var(std::string name, std::vector <var>& vars) {
        for (var& v : vars)
            if (v.name == name)
                return v;

        printf("Variable \'%s\' not found\n", name.c_str());

        std::exit(1);
    }

    uint32_t evaluate_expression(expression* expr, std::vector <var>& vars) {
        switch (expr->get_tag()) {
            case EX_INTEGER_CONSTANT: {
                integer_constant* ie = (integer_constant*)expr;

                return std::atoi(ie->value.c_str());
            } break;

            case EX_BINARY_OP: {
                binary_op* bo = (binary_op*)expr;

                uint32_t left = evaluate_expression(bo->left, vars);
                uint32_t right = evaluate_expression(bo->right, vars);

                uint32_t result;

                if (bo->op == "+") {
                    return left + right;
                } else if (bo->op == "-") {
                    return left - right;
                } else if (bo->op == "*") {
                    return left * right;
                } else if (bo->op == "/") {
                    return left / right;
                } else if (bo->op == "&") {
                    return left & right;
                } else if (bo->op == "|") {
                    return left | right;
                } else if (bo->op == "^") {
                    return left ^ right;
                } else if (bo->op == "<<") {
                    return left << right;
                } else if (bo->op == ">>") {
                    return left >> right;
                } else if (bo->op == "&&") {
                    return left && right;
                } else if (bo->op == "||") {
                    return left || right;
                } else if (bo->op == "^^") {
                    return (left && !right) || (right && !left);
                } else {
                    return 0;
                }
            } break;

            case EX_UNARY_OP: {
                unary_op* uo = (unary_op*)expr;

                uint32_t operand = evaluate_expression(uo->expr, vars);

                if (uo->op == "-") {
                    return -operand;
                } else if (uo->op == "!") {
                    return !operand;
                } else if (uo->op == "++") {
                    int et = uo->expr->get_tag();

                    if (et != EX_NAME_REF) {
                        printf("Can only apply ++/-- to variables\n");

                        std::exit(1);
                    }

                    name_ref* nr = (name_ref*)uo->expr;

                    var& v = lookup_var(nr->name, vars);

                    return v.value++;
                } else if (uo->op == "--") {
                    int et = uo->expr->get_tag();

                    if (et != EX_NAME_REF) {
                        printf("Can only apply ++/-- to variables\n");

                        std::exit(1);
                    }

                    name_ref* nr = (name_ref*)uo->expr;

                    var& v = lookup_var(nr->name, vars);

                    return v.value--;
                } else if (uo->op == "~") {
                    return ~operand;
                } else {
                    return 0;
                }
            } break;

            case EX_NAME_REF: {
                name_ref* nr = (name_ref*)expr;

                return lookup_var(nr->name, vars).value;
            } break;
        }
    }
}

void koala::parser::execute_function(std::string name) {
    function_def* fd;

    for (statement* s : m_ast) {
        if (s->get_tag() != ST_FUNCTION_DEF)
            continue;

        fd = (function_def*)s;

        if (fd->name == name)
            break;
    }

    std::vector <var> vars;

    for (statement* s : fd->body) {
        switch (s->get_tag()) {
            case ST_VARIABLE_DEF: {
                variable_def* vd = (variable_def*)s;

                var v;

                v.name = vd->name;

                if (vd->init) {
                    v.value = evaluate_expression(vd->init, vars);
                } else {
                    v.value = 0;
                }

                vars.push_back(v);
            } break;

            case ST_ASSIGNMENT: {
                assignment* a = (assignment*)s;

                uint32_t& v = lookup_var(a->dest, vars).value;
                uint32_t e = evaluate_expression(a->src, vars);

                if (a->op == "=") {
                    v = e;
                } else if (a->op == "+=") {
                    v += e;
                } else if (a->op == "-=") {
                    v -= e;
                } else if (a->op == "*=") {
                    v *= e;
                } else if (a->op == "/=") {
                    v /= e;
                } else if (a->op == "&=") {
                    v &= e;
                } else if (a->op == "|=") {
                    v |= e;
                } else if (a->op == "^=") {
                    v ^= e;
                } else if (a->op == "<<=") {
                    v <<= e;
                } else if (a->op == ">>=") {
                    v >>= e;
                }
            } break;

            case ST_FUNCTION_CALL: {
                function_call* fc = (function_call*)s;

                if (fc->callee == "print") {
                    printf("%s = %u\n",
                        fc->args[0]->print(0).c_str(),
                        evaluate_expression(fc->args[0], vars)
                    );
                }
            } break;
        }
    }
}


// koala::type* koala::parser::get_type(expression* e) {
//     if (e->type)
//         return e->type;

//     switch (e->get_tag()) {
//         case EX_INTEGER_CONSTANT: {
//             integer_constant* ic = (integer_constant*)e;

//             uint32_t value = std::atoi(ic->value.c_str());

//             if (value & 0xffff0000) {
//                 ic->type = m_ts.get_type(type_signature("u32"));
//             } else if (value & 0xff00) {
//                 ic->type = m_ts.get_type(type_signature("u16"));
//             } else {
//                 ic->type = m_ts.get_type(type_signature("u8"));
//             }
//         } break;

//         case EX_BINARY_OP: {
//             binary_op* bo = (binary_op*)e;

//             return bo->type;
//         } break;
//     }
// }