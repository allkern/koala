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
        function_def*             m_current_function;

        type* complete_type(type* t) {
            switch (t->get_class()) {
                case TP_TYPEOF: {
                    typeof_type* tt = (typeof_type*)t;

                    type* target = get_type(tt->target);

                    // Get rid of the typeof type
                    delete t;

                    return target;
                } break;

                case TP_FUNCTION: {
                    function_type* ft = (function_type*)t;

                    for (int i = 0; i < ft->arg_types.size(); i++) {
                        ft->arg_types[i] = complete_type(ft->arg_types[i]);
                    }

                    ft->return_type = complete_type(ft->return_type);

                    return t;
                } break;

                case TP_INTEGRAL: {
                    return t;
                } break;

                case TP_POINTER: {
                    pointer_type* pt = (pointer_type*)t;

                    pt->target = complete_type(pt->target);

                    return t;
                } break;

                case TP_STRUCT: {
                    struct_type* st = (struct_type*)t;

                    for (int i = 0; i < st->members.size(); i++)
                        st->members[i].t = complete_type(st->members[i].t);

                    return st;
                } break;
            }
        }

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

            printf("Symbol with name \'%s\' was not found\n", name.c_str());

            std::exit(1);
        }

        struct_member& lookup_member(std::string name, struct_type* st) {
            for (struct_member& s : st->members)
                if (s.name == name)
                    return s;

            printf("Member with name \'%s\' was not found in struct type \'%s\'\n",
                name.c_str(),
                st->str().c_str()
            );

            std::exit(1);
        }

        // To-do: Make this better
        type* get_type(integer_constant* ic) {
            if (ic->t)
                return ic->t;

            ic->t = m_ts->get_type("int");

            return ic->t;
        }

        type* get_type(string_literal* sl) {
            if (sl->t)
                return sl->t;

            sl->t = m_ts->get_type("koala_string");

            return sl->t;
        }

        type* get_type(binary_op* bo) {
            if (bo->t)
                return bo->t;

            // Return the bigger of the two types
            type* left = get_type(bo->left);
            type* right = get_type(bo->right);

            if (left->get_class() != right->get_class()) {
                printf("Attempting to perform a binary operation between \'%s\' and \'%s\'\n",
                    left->str().c_str(),
                    right->str().c_str()
                );

                std::exit(1);
            }

            if (left->get_size() <= right->get_size()) {
                bo->t = right;
            } else {
                bo->t = left;
            }

            return bo->t;
        }

        type* get_type(unary_op* uo) {
            if (uo->t)
                return uo->t;

            uo->t = get_type(uo->expr);

            return uo->t;
        }

        type* get_type(name_ref* nr) {
            if (nr->t)
                return nr->t;

            symbol& s = lookup_symbol(nr->name);

            nr->t = s.t;

            return nr->t;
        }

        type* get_type(member_access* ma) {
            if (ma->t)
                return ma->t;

            type* expr_type = get_type(ma->expr);

            if (expr_type->get_class() != TP_STRUCT) {
                printf("Trying to access member \'%s\' of non-struct type \'%s\'\n",
                    ma->member.c_str(),
                    expr_type->str().c_str()
                );

                std::exit(1);
            }

            struct_type* st = (struct_type*)expr_type;
            struct_member& m = lookup_member(ma->member, st);

            m.t = complete_type(m.t);

            return m.t;
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

                case EX_STRING_LITERAL: {
                    return get_type((string_literal*)e);
                } break;

                case EX_MEMBER_ACCESS: {
                    return get_type((member_access*)e);
                } break;
            }

            printf("Unknown expression\n");

            std::exit(1);

            return nullptr;
        }

        void check_statement(variable_def* vd) {
            if (!vd->init) {
                vd->t = complete_type(vd->t);

                push_symbol(vd->name, vd->t);

                return;
            }

            type* init_type = get_type(vd->init);

            if (!vd->t) {
                vd->t = init_type;
            } else {
                vd->t = complete_type(vd->t);
            }

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
            } else if (ft->arg_types.size() > fc->args.size()) {
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
            if (!m_current_function) {
                printf("Return statement used outside of a function definition\n");

                std::exit(1);
            }

            type* expr_type = get_type(re->expr);

            if (!m_current_function->return_type) {
                m_current_function->return_type = expr_type;
            } else {
                if (m_current_function->return_type->get_class() != expr_type->get_class()) {
                    printf("Trying to return a value of type \'%s\' on a function with return type \'%s\'",
                        expr_type->str().c_str(),
                        m_current_function->return_type->str().c_str()
                    );

                    std::exit(1);
                }
            }
        }

        void check_statement(function_def* fd) {
            m_current_function = fd;

            function_type* ft = new function_type();

            m_local_symbols.clear();

            if (ft->return_type)
                ft->return_type = complete_type(fd->return_type);
            
            for (function_arg& fa : fd->args) {
                fa.t = complete_type(fa.t);

                push_symbol(fa.name, fa.t);

                ft->arg_types.push_back(fa.t);
            }

            type* t = m_ts->add_type(ft);

            push_symbol(fd->name, t);

            m_scope = 1;

            for (statement* s : fd->body)
                check_statement(s);

            m_scope = 0;

            m_current_function = nullptr;
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