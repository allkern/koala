#include "type_checker.hpp"

koala::type* koala::type_checker::complete_type(koala::type* t) {
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

    return nullptr;
}

koala::symbol* koala::type_checker::push_symbol(std::string name, koala::type* t) {
    if (m_scope) {
        m_local_symbols.push_back(symbol{ t, name });

        return &m_local_symbols.back();
    } else {
        m_global_symbols.push_back(symbol{ t, name });

        return &m_global_symbols.back();
    }
}

koala::symbol& koala::type_checker::lookup_symbol(std::string name) {
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

koala::struct_member& koala::type_checker::lookup_member(std::string name, koala::struct_type* st) {
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
koala::type* koala::type_checker::get_type(koala::integer_constant* ic) {
    if (ic->t)
        return ic->t;

    ic->t = m_ts->get_type("int");

    return ic->t;
}

koala::type* koala::type_checker::get_type(koala::string_literal* sl) {
    if (sl->t)
        return sl->t;

    sl->t = m_ts->get_type("koala_string");

    return sl->t;
}

koala::type* koala::type_checker::get_type(koala::binary_op* bo) {
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

koala::type* koala::type_checker::get_type(koala::unary_op* uo) {
    if (uo->t)
        return uo->t;

    uo->t = get_type(uo->expr);

    return uo->t;
}

koala::type* koala::type_checker::get_type(koala::name_ref* nr) {
    if (nr->t)
        return nr->t;

    symbol& s = lookup_symbol(nr->name);

    nr->t = s.t;

    return nr->t;
}

koala::type* koala::type_checker::get_type(koala::member_access* ma) {
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

koala::type* koala::type_checker::get_type(koala::array_access* aa) {
    type* expr_type = get_type(aa->expr);
    type* index_type = get_type(aa->index);

    if (expr_type->get_class() != TP_POINTER) {
        printf("Attempting to use array access operator on non-pointer type \'%s\'\n",
            expr_type->str().c_str()
        );

        std::exit(1);
    }

    if (index_type->get_class() != TP_INTEGRAL) {
        printf("Using non-integral type \'%s\' as array access index\n",
            index_type->str().c_str()
        );

        std::exit(1);
    }

    pointer_type* ptr_type = (pointer_type*)expr_type;

    return ptr_type->target;
}

koala::type* koala::type_checker::get_type(koala::function_call* fc) {
    type* callee_type = get_type(fc->expr);

    if (callee_type->get_class() != TP_FUNCTION) {
        printf("Attempting to call a non-function object of type \'%s\'\n",
            callee_type->str().c_str()
        );

        std::exit(1);
    }

    function_type* ft = (function_type*)callee_type;

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

    return ft->return_type;
}

koala::type* koala::type_checker::get_type(koala::expression* e) {
    switch (e->get_tag()) {
        case EX_FUNCTION_CALL: {
            return get_type((function_call*)e);
        } break;

        case EX_ARRAY_ACCESS: {
            return get_type((array_access*)e);
        } break;

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

void koala::type_checker::check_statement(koala::variable_def* vd) {
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

void koala::type_checker::check_statement(koala::assignment* a) {
    type* dst_type = get_type(a->dst);
    type* src_type = get_type(a->src);

    if (dst_type->get_class() != src_type->get_class()) {
        printf("Assigning value of type \'%s\' to object of type \'%s\'\n",
            src_type->str().c_str(),
            dst_type->str().c_str()
        );

        std::exit(1);
    }
}

void koala::type_checker::check_statement(koala::expression_statement* es) {
    type* discard = get_type(es->expr);
}

void koala::type_checker::check_statement(koala::return_expr* re) {
    if (!m_current_ft) {
        printf("Return statement used outside of a function definition\n");

        std::exit(1);
    }

    type* expr_type = get_type(re->expr);

    if (!m_current_ft->return_type) {
        m_current_ft->return_type = expr_type;
        ((function_type*)m_current_symbol->t)->return_type = expr_type;

        // Clear signature
        m_current_ft->sig = "";
    } else {
        if (m_current_ft->return_type->get_class() != expr_type->get_class()) {
            printf("Trying to return a value of type \'%s\' on a function with return type \'%s\'",
                expr_type->str().c_str(),
                m_current_ft->return_type->str().c_str()
            );

            std::exit(1);
        }
    }
}

void koala::type_checker::check_statement(koala::while_loop* wl) {
    if (get_type(wl->cond)->get_class() != TP_INTEGRAL) {
        printf("Condition of while loop is not an object of integral type\n");

        std::exit(1);
    }

    for (statement* s : wl->body)
        check_statement(s);
}

void koala::type_checker::check_statement(koala::function_def* fd) {
    function_type* ft = new function_type();

    m_local_symbols.clear();
    m_local_symbols.push_back(symbol{ m_ts->get_type("koala_string"), "__func__" });

    if (fd->return_type)
        ft->return_type = complete_type(fd->return_type);
    
    for (function_arg& fa : fd->args) {
        fa.t = complete_type(fa.t);

        push_symbol(fa.name, fa.t);

        ft->arg_types.push_back(fa.t);
    }

    type* t = nullptr;

    m_current_ft = (function_type*)m_ts->add_type(ft);
    m_current_symbol = push_symbol(fd->name, m_current_ft);

    m_scope = 1;

    for (statement* s : fd->body)
        check_statement(s);

    m_scope = 0;

    m_current_ft = nullptr;
    m_current_symbol = nullptr;
}

void koala::type_checker::check_statement(koala::statement* s) {
    switch (s->get_tag()) {
        case ST_VARIABLE_DEF: {
            check_statement((variable_def*)s);
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

        case ST_EXPRESSION: {
            check_statement((expression_statement*)s);
        } break;

        case ST_WHILE_LOOP: {
            check_statement((while_loop*)s);
        } break;
    }
}

void koala::type_checker::check() {
    function_type* ft = new function_type();

    ft->arg_types.push_back(m_ts->get_type("char"));
    ft->return_type = m_ts->get_type("char");

    ft = (function_type*)m_ts->add_type(ft);

    m_global_symbols.push_back(symbol {ft, "putchar"});

    for (statement* s : *m_ast)
        check_statement(s);
}