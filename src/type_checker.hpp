#pragma once

#include "parser/parser.hpp"

namespace koala {
    struct symbol {
        type* t;

        std::string name;
    };

    class type_checker {
        std::vector <statement*>* m_ast;
        type_system*              m_ts;
        std::vector <symbol>      m_local_symbols;
        std::vector <symbol>      m_global_symbols;
        int                       m_scope = 0;

        void push_symbol(std::string name, type* t) {
            if (m_scope) {
                m_local_symbols.push_back(symbol{ t, name });
            } else {
                m_global_symbols.push_back(symbol{ t, name });
            }
        }

        symbol& lookup_symbol(std::string name) {
            // Search local symbols first (locals shadow globals)
            for (symbol& s : m_local_symbols)
                if (s.name == name)
                    return s;

            for (symbol& s : m_global_symbols)
                if (s.name == name)
                    return s;

            printf("Symbol with name \'%s\' wasn't found", name.c_str());

            std::exit(1);
        }

        // To-do: Make this better
        type* get_type(integer_constant* ic) {
            if (ic->type)
                return ic->type;

            ic->type = m_ts->get_type("int");

            return ic->type;
        }

        type* get_type(binary_op* bo) {
            if (bo->type)
                return bo->type;

            // Return the bigger of the two types
            type* left = get_type(bo->left);
            type* right = get_type(bo->right);

            if (left->get_size() <= right->get_size()) {
                bo->type = right;
            } else {
                bo->type = left;
            }

            return bo->type;
        }

        type* get_type(unary_op* uo) {
            if (uo->type)
                return uo->type;

            uo->type = get_type(uo->expr);

            return uo->type;
        }

        type* get_type(name_ref* nr) {
            if (nr->type)
                return nr->type;

            symbol& s = lookup_symbol(nr->name);

            nr->type = s.t;

            return nr->type;
        }

        type* get_type(expression* e) {
            switch (e->get_tag()) {
                case EX_INTEGER_CONSTANT: {
                    return get_type((integer_constant*)e);
                } break;

                case EX_UNARY_OP: {
                    return get_type((unary_op*)e);
                } break;

                case EX_NAME_REF: {
                    return get_type((name_ref*)e);
                } break;

                case EX_BINARY_OP: {
                    return get_type((binary_op*)e);
                } break;
            }

            printf("Unknown expression\n");

            std::exit(1);

            return nullptr;
        }

        void check_statement(variable_def* vd) {
            if (!vd->init) {
                push_symbol(vd->name, vd->t);

                return;
            }

            type* init_type = get_type(vd->init);

            if (!vd->t)
                vd->t = init_type;

            if (init_type->get_class() != vd->t->get_class()) {
                printf("Initializing a variable of type \'%s\' with a value of type \'%s\'\n",
                    vd->t->str().c_str(),
                    init_type->str().c_str()
                );

                std::exit(1);
            }

            push_symbol(vd->name, vd->t);
        }

        void check_statement(assignment* a) {
            symbol& s = lookup_symbol(a->dest);

            type* src_type = get_type(a->src);

            if (s.t->get_class() != src_type->get_class()) {
                printf("Assigning value of type \'%s\' to variable of type \'%s\'\n",
                    src_type->str().c_str(),
                    s.t->str().c_str()
                );

                std::exit(1);
            }
        }

        void check_statement(function_call* fc) {
            symbol& s = lookup_symbol(fc->callee);

            if (s.t->get_class() != TP_FUNCTION) {
                printf("Attempting to call a non-function object of type \'%s\'\n",
                    s.t->str().c_str()
                );

                std::exit(1);
            }

            function_type* ft = (function_type*)s.t;

            if (ft->arg_types.size() < fc->args.size()) {
                printf("Too many arguments in function call\n");

                std::exit(1);
            } else if (ft->arg_types.size() < fc->args.size()) {
                printf("Too few arguments in function call\n");

                std::exit(1);
            }

            for (int i = 0; i < fc->args.size(); i++) {
                type* call_arg_type = get_type(fc->args[i]);
                type* def_arg_type = ft->arg_types[i];

                if (call_arg_type->get_class() != def_arg_type->get_class()) {
                    printf("Argument %i of type \'%s\' in function call doesn't match function argument of type \'%s\'\n",
                        i,
                        call_arg_type->str().c_str(),
                        def_arg_type->str().c_str()
                    );

                    std::exit(1);
                }
            }
        }

        void check_statement(return_expr* re) {
            return;
        }

        void check_statement(function_def* fd) {
            function_type* ft = new function_type();

            m_local_symbols.clear();

            ft->return_type = fd->return_type;
            
            for (function_arg& fa : fd->args) {
                push_symbol(fa.name, fa.t);

                ft->arg_types.push_back(fa.t);
            }

            type* t = m_ts->add_type(ft);

            push_symbol(fd->name, t);

            m_scope = 1;

            for (statement* s : fd->body)
                check_statement(s);

            m_scope = 0;
        }

        void check_statement(statement* s) {
            switch (s->get_tag()) {
                case ST_VARIABLE_DEF: {
                    check_statement((variable_def*)s);
                } break;

                case ST_FUNCTION_CALL: {
                    check_statement((function_call*)s);
                } break;

                case ST_FUNCTION_DEF: {
                    check_statement((function_def*)s);
                } break;

                case ST_ASSIGNMENT: {
                    check_statement((assignment*)s);
                } break;

                case ST_RETURN_EXPR: {
                    check_statement((return_expr*)s);
                } break;
            }
        }

    public:
        type_checker(parser& p) :
            m_ast(p.get_ast()), m_ts(p.get_type_system()), m_scope(0) {};

        void check() {
            for (statement* s : *m_ast)
                check_statement(s);
        }
    };
}