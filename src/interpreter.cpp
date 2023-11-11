#include "interpreter.hpp"

#include <algorithm>

std::unordered_map <std::string, int> g_binary_op_map = {
    { "==" , koala::BN_COMPARE_EQ },
    { "!=" , koala::BN_COMPARE_NE },
    { "<=" , koala::BN_COMPARE_LE },
    { ">=" , koala::BN_COMPARE_GE },
    { "<"  , koala::BN_COMPARE_LT },
    { ">"  , koala::BN_COMPARE_GT },
    { "&&" , koala::BN_LOGICAL_AND },
    { "||" , koala::BN_LOGICAL_OR },
    { "^^" , koala::BN_LOGICAL_XOR },
    { "+"  , koala::BN_ARITH_ADD },
    { "-"  , koala::BN_ARITH_SUB },
    { "*"  , koala::BN_ARITH_MUL },
    { "/"  , koala::BN_ARITH_DIV },
    { "%"  , koala::BN_ARITH_MOD },
    { "&"  , koala::BN_BITWISE_AND },
    { "!&" , koala::BN_BITWISE_NAND },
    { "|"  , koala::BN_BITWISE_OR },
    { "!|" , koala::BN_BITWISE_NOR },
    { "^"  , koala::BN_BITWISE_XOR },
    { "!^" , koala::BN_BITWISE_XNOR },
    { "<<" , koala::BN_BITWISE_SL },
    { ">>" , koala::BN_BITWISE_SR }
};

std::unordered_map <std::string, int> g_unary_op_map = {
    { "~"  , koala::UN_BITWISE_NOT },
    { "!"  , koala::UN_LOGICAL_NOT },
    { "++" , koala::UN_INCREMENT   },
    { "--" , koala::UN_DECREMENT   }
};

std::unordered_map <std::string, int> g_assignment_op_map = {
    { "="  , koala::AP_EQ },
    { "+=" , koala::AP_EQ_ADD },
    { "-=" , koala::AP_EQ_SUB },
    { "*=" , koala::AP_EQ_MUL },
    { "/=" , koala::AP_EQ_DIV },
    { "&=" , koala::AP_EQ_BITWISE_AND },
    { "!&=", koala::AP_EQ_BITWISE_NAND },
    { "|=" , koala::AP_EQ_BITWISE_OR },
    { "!|=", koala::AP_EQ_BITWISE_NOR },
    { "^=" , koala::AP_EQ_BITWISE_XOR },
    { "!^=", koala::AP_EQ_BITWISE_XNOR },
    { "<<=", koala::AP_EQ_BITWISE_SL },
    { ">>=", koala::AP_EQ_BITWISE_SR },
    { "&&=", koala::AP_EQ_LOGICAL_AND },
    { "||=", koala::AP_EQ_LOGICAL_OR },
    { "^^=", koala::AP_EQ_LOGICAL_XOR },
    { "%=" , koala::AP_EQ_MOD },
};

void binary_operation(int op, uint64_t& result, uint64_t lhs, uint64_t rhs) {
    switch (op) {
        case koala::BN_COMPARE_EQ: result = lhs == rhs; break;
        case koala::BN_COMPARE_NE: result = lhs != rhs; break;
        case koala::BN_COMPARE_LE: result = lhs <= rhs; break;
        case koala::BN_COMPARE_GE: result = lhs >= rhs; break;
        case koala::BN_COMPARE_LT: result = lhs < rhs; break;
        case koala::BN_COMPARE_GT: result = lhs > rhs; break;
        case koala::BN_LOGICAL_AND: result = lhs && rhs; break;
        case koala::BN_LOGICAL_OR: result = lhs || rhs; break;
        case koala::BN_LOGICAL_XOR: result = (lhs && !rhs) || (rhs && !lhs); break;
        case koala::BN_ARITH_ADD: result = lhs + rhs; break;
        case koala::BN_ARITH_SUB: result = lhs - rhs; break;
        case koala::BN_ARITH_MUL: result = lhs * rhs; break;
        case koala::BN_ARITH_DIV: result = lhs / rhs; break;
        case koala::BN_ARITH_MOD: result = lhs % rhs; break;
        case koala::BN_BITWISE_AND: result = lhs & rhs; break;
        case koala::BN_BITWISE_NAND: result = !(lhs & rhs); break;
        case koala::BN_BITWISE_OR: result = lhs | rhs; break;
        case koala::BN_BITWISE_NOR: result = !(lhs | rhs); break;
        case koala::BN_BITWISE_XOR: result = lhs ^ rhs; break;
        case koala::BN_BITWISE_XNOR: result = !(lhs ^ rhs); break;
        case koala::BN_BITWISE_SL: result = lhs << rhs; break;
        case koala::BN_BITWISE_SR: result = lhs >> rhs; break;
    }
}

void unary_operation(int op, uint64_t& result, uint64_t value) {
    switch (op) {
        case koala::UN_BITWISE_NOT: result = ~value; break;
        case koala::UN_LOGICAL_NOT: result = !value; break;
    }
}

uint64_t get_value(std::string v, koala::integral_type* it) {
    uint64_t value = 0;

    if (it->is_signed) {
        value = std::stoll(v);
    } else {
        *(int*)0 = 0;
        value = std::stoull(v);
    }

    int size = it->get_size();

    uint64_t mask = 0xffffffff >> ((4 - size) * 8);

    value &= mask;

    // Sign extend
    if (it->is_signed) {
        uint64_t sign_bit_mask = mask & ((~mask) >> 1);

        if (value & sign_bit_mask)
            value |= ~mask;
    }

    return value;
}

koala::interpreter::value* koala::interpreter::evaluate_expression(function_call* fc) {
    symbol* sym = search_symbol(fc->expr);

    if (sym->v->get_class() != TP_FUNCTION) {
        printf("Symbol \'%s\' does not refer to a function type object\n");

        std::exit(1);
    }

    function_value* fv = (function_value*)sym->v;

    std::vector <value*> args;

    for (expression* expr : fc->args)
        args.push_back(evaluate_expression(expr));

    return call_function(fv->def, args);
}

koala::interpreter::value* koala::interpreter::evaluate_expression(array_access* aa) {
    value* expr_eval = evaluate_expression(aa->expr);

    if (expr_eval->get_class() != TP_POINTER) {
        printf("Expression does not evaluate to a pointer type\n");

        std::exit(1);
    }

    pointer_value* pv = (pointer_value*)expr_eval;

    if (pv->t->target->get_class() != TP_INTEGRAL) {
        printf("Pointer target is not an integral type (interpreter forbidden)\n");

        std::exit(1);
    }

    value* index_eval = evaluate_expression(aa->index);

    if (index_eval->get_class() != TP_INTEGRAL) {
        printf("Index does not evaluate to an integral type\n");

        std::exit(1);
    }

    integral_value* index = (integral_value*)index_eval;
    integral_value iv;

    iv.t = (integral_type*)pv->t->target;

    switch (pv->t->target->size) {
        case 1: iv.value = *(((uint8_t*)pv->value) + index->value); break;
        case 2: iv.value = *(((uint16_t*)pv->value) + index->value); break;
        case 4: iv.value = *(((uint32_t*)pv->value) + index->value); break;
    }

    return new integral_value(iv);
}

koala::interpreter::value* koala::interpreter::evaluate_expression(integer_constant* ic) {
    integral_value iv;

    iv.t = (integral_type*)ic->t;
    iv.value = get_value(ic->value, iv.t);

    return new integral_value(iv);
}

koala::interpreter::value* koala::interpreter::evaluate_expression(string_literal* sl) {
    integral_value size_iv;
    pointer_value ptr_pv;
    struct_value sv;
    struct_type* st = (struct_type*)sl->t;

    symbol size, ptr;

    size_iv.t = (integral_type*)st->members[0].t;
    size_iv.value = sl->value.size();
    ptr_pv.t = (pointer_type*)st->members[1].t;
    ptr_pv.value = (uintptr_t)sl->value.c_str();

    size.mut = false;
    size.name = st->members[0].name;
    size.v = new integral_value(size_iv);

    ptr.mut = false;
    ptr.name = "ptr";
    ptr.v = new pointer_value(ptr_pv);

    sv.t = (struct_type*)sl->t;
    sv.members.push_back(size);
    sv.members.push_back(ptr);

    return new struct_value(sv);
}

koala::interpreter::value* koala::interpreter::evaluate_expression(binary_op* bo) {
    value* eval_lhs = evaluate_expression(bo->left);
    value* eval_rhs = evaluate_expression(bo->right);

    if ((eval_lhs->get_class() != TP_INTEGRAL) || (eval_rhs->get_class() != TP_INTEGRAL)) {
        printf("Cannot perform a binary operation between non-integral types\n");

        std::exit(1);
    }

    uint64_t lhs = ((integral_value*)eval_lhs)->value;
    uint64_t rhs = ((integral_value*)eval_rhs)->value;

    integral_value iv;

    iv.t = ((integral_value*)eval_lhs)->t;
    
    binary_operation(g_binary_op_map[bo->op], iv.value, lhs, rhs);

    return new integral_value(iv);
}

koala::interpreter::value* koala::interpreter::evaluate_expression(unary_op* uo) {
    // To-do: Add support for ++/--

    value* eval_expr = evaluate_expression(uo->expr);

    if (eval_expr->get_class() != TP_INTEGRAL) {
        printf("Cannot perform a unary operation on a non-integral type\n");

        std::exit(1);
    }

    int op = g_unary_op_map[uo->op];

    integral_value iv;

    switch (op) {
        case UN_INCREMENT: {
            symbol* sym = search_symbol(uo->expr);

            if (sym->v->get_class() != TP_INTEGRAL) {
                printf("Can not increment a non-integral object\n");

                std::exit(1);
            }

            integral_value* sym_iv = (integral_value*)sym->v;

            if (uo->post) {
                iv.t = sym_iv->t;
                iv.value = sym_iv->value;

                sym_iv->value += 1;
            } else {
                sym_iv->value += 1;

                iv.t = sym_iv->t;
                iv.value = sym_iv->value;
            }

            return new integral_value(iv);
        } break;

        case UN_DECREMENT: {
            symbol* sym = search_symbol(uo->expr);

            if (sym->v->get_class() != TP_INTEGRAL) {
                printf("Can not increment a non-integral object\n");

                std::exit(1);
            }

            integral_value* sym_iv = (integral_value*)sym->v;

            if (uo->post) {
                iv.t = sym_iv->t;
                iv.value = sym_iv->value;

                sym_iv->value -= 1;
            } else {
                sym_iv->value -= 1;

                iv.t = sym_iv->t;
                iv.value = sym_iv->value;
            }

            return new integral_value(iv);
        } break;
    }

    if (uo->post) {
        printf("Invalid post unary operator \'%s\'\n", uo->op.c_str());

        std::exit(1);
    }

    uint64_t value = ((integral_value*)eval_expr)->value;

    iv.t = ((integral_value*)eval_expr)->t;
    
    unary_operation(g_unary_op_map[uo->op], iv.value, value);

    return new integral_value(iv);
}

koala::interpreter::value* koala::interpreter::evaluate_expression(name_ref* nr) {
    return search_symbol(nr->name)->v;
}

koala::interpreter::value* koala::interpreter::evaluate_expression(member_access* ma) {
    value* expr_eval = evaluate_expression(ma->expr);

    if (expr_eval->get_class() != TP_STRUCT) {
        printf("Used member access operator on a non-struct type\n");

        std::exit(1);
    }

    struct_value* sv = (struct_value*)expr_eval;

    symbol* member = search_member(ma->member, sv);

    return member->v;
}

koala::interpreter::value* koala::interpreter::evaluate_expression(expression* expr) {
    switch (expr->get_tag()) {
        case EX_INTEGER_CONSTANT: {
            return evaluate_expression((integer_constant*)expr);
        } break;

        case EX_UNARY_OP: {
            return evaluate_expression((unary_op*)expr);
        } break;

        case EX_NAME_REF: {
            return evaluate_expression((name_ref*)expr);
        } break;

        case EX_BINARY_OP: {
            return evaluate_expression((binary_op*)expr);
        } break;

        case EX_STRING_LITERAL: {
            return evaluate_expression((string_literal*)expr);
        } break;

        case EX_MEMBER_ACCESS: {
            return evaluate_expression((member_access*)expr);
        } break;

        case EX_ARRAY_ACCESS: {
            return evaluate_expression((array_access*)expr);
        } break;

        case EX_FUNCTION_CALL: {
            return evaluate_expression((function_call*)expr);
        } break;
    }

    printf("Unknown expression\n");

    std::exit(1);

    return nullptr;
}

void koala::interpreter::execute_statement(variable_def* vd) {
    if (search_symbol(vd->name)) {
        printf("Attempting to define a variable with existing name \'%s\'\n",
            vd->name.c_str()
        );

        std::exit(1);
    }

    if (vd->init) {
        add_local_symbol(vd->name, evaluate_expression(vd->init), vd->mut);
    } else {
        // To-do: Default construct types

        add_local_symbol(vd->name, nullptr, vd->mut);
    }
}

void koala::interpreter::execute_statement(assignment* a) {
    symbol* sym = search_symbol(a->dst);

    if (!sym->mut) {
        printf("Cannot assign a value to an immutable object\n");

        std::exit(1);
    }

    if (a->op == "=") {
        sym->v = evaluate_expression(a->src);

        return;
    }

    value* eval_rhs = evaluate_expression(a->src);

    if ((sym->v->get_class() != TP_INTEGRAL) || (eval_rhs->get_class() != TP_INTEGRAL)) {
        printf("Cannot perform a binary operation between objects of non-integral types\n");

        std::exit(1);
    }

    uint64_t lhs = ((integral_value*)sym->v)->value;
    uint64_t rhs = ((integral_value*)eval_rhs)->value;

    // All assignment operators end with '='
    // Popping the last character results in the desired
    // binary operator

    std::string op = a->op;

    op.pop_back();

    binary_operation(g_binary_op_map[op], ((integral_value*)sym->v)->value, lhs, rhs);
}

void koala::interpreter::execute_statement(return_expr* re) {
    m_return_stack.top() = evaluate_expression(re->expr);
}

void koala::interpreter::execute_statement(expression_statement* es) {
    value* discard = evaluate_expression(es->expr);
}

void koala::interpreter::execute_statement(while_loop* wl) {
    value* cond_eval = evaluate_expression(wl->cond);

    if (cond_eval->get_class() != TP_INTEGRAL) {
        printf("While loop condition is not of integral type\n");

        std::exit(1);
    }

    integral_value* iv = (integral_value*)cond_eval;

    while (iv->value) {
        for (statement* s : wl->body)
            execute_statement(s);

        iv = (integral_value*)evaluate_expression(wl->cond);
    }
}

void koala::interpreter::execute_statement(function_def* fd) {
    std::printf("Nested functions are not allowed\n");

    std::exit(1);
}

bool koala::interpreter::execute_statement(statement* s) {
    switch (s->get_tag()) {
        case ST_VARIABLE_DEF: {
            execute_statement((variable_def*)s);
        } break;

        case ST_FUNCTION_DEF: {
            execute_statement((function_def*)s);
        } break;

        case ST_ASSIGNMENT: {
            execute_statement((assignment*)s);
        } break;

        case ST_WHILE_LOOP: {
            execute_statement((while_loop*)s);
        } break;

        case ST_RETURN_EXPR: {
            execute_statement((return_expr*)s);

            return false;
        } break;

        case ST_EXPRESSION: {
            execute_statement((expression_statement*)s);
        } break;
    }

    return true;
}

void koala::interpreter::init() {
    function_value* fv = new function_value();
    function_def* fd = new function_def();

    fd->args.push_back(function_arg { false, "c", m_ts->get_type("char") });
    fd->body.clear();
    fd->name = "putchar";
    fd->return_type = m_ts->get_type("char");

    fv->def = fd;

    m_globals.push_back(symbol { false, fv, "putchar" });

    for (statement* s : *m_ast) {
        if (s->get_tag() == ST_VARIABLE_DEF) {
            variable_def* vd = (variable_def*)s;

            symbol sym;

            sym.mut = vd->mut;
            sym.name = vd->name;
            
            if (vd->init) {
                sym.v = evaluate_expression(vd->init);
            }

            m_globals.push_back(sym);
        } else if (s->get_tag() == ST_FUNCTION_DEF) {
            function_def* fd = (function_def*)s;
            function_value fv;
            symbol sym;

            fv.def = fd;

            sym.mut = false;
            sym.name = fd->name;
            sym.v = new function_value(fv);

            m_globals.push_back(sym);
        } else {
            printf("Statements in global scope are not allowed\n");

            std::exit(1);
        }
    }
}

koala::interpreter::symbol* koala::interpreter::search_member(std::string name, struct_value* sv) {
    for (symbol& sym : sv->members)
        if (sym.name == name)
            return &sym;

    printf("Could not find member with name \'%s\'\n",
        name.c_str()
    );

    std::exit(1);
}

koala::interpreter::symbol* koala::interpreter::search_symbol(std::string name) {
    if (m_locals.size())
        for (symbol& sym : m_locals.top())
            if (sym.name == name)
                return &sym;
    
    for (symbol& sym : m_globals)
        if (sym.name == name)
            return &sym;
    
    return nullptr;
}

koala::interpreter::symbol* koala::interpreter::search_symbol(koala::expression* expr) {
    symbol* sym = nullptr;

    switch (expr->get_tag()) {
        case EX_NAME_REF: {
            name_ref* nr = (name_ref*)expr;

            sym = search_symbol(nr->name);
        } break;

        case EX_MEMBER_ACCESS: {
            member_access* ma = (member_access*)expr;

            symbol* struct_sym = search_symbol(ma->expr);

            if (struct_sym->v->get_class() != TP_STRUCT) {
                printf("Symbol \'%s\' does not refer to a struct type object\n",
                    struct_sym->name
                );

                std::exit(1);
            }

            struct_value* sv = (struct_value*)struct_sym->v;

            sym = search_member(ma->member, sv);
        } break;

        case EX_ARRAY_ACCESS: {
            printf("Getting a reference to an array object is forbidden in interpreter\n");

            std::exit(1);
        } break;

        default: {
            printf("Attempting to get a reference to a non-referenceable type\n");

            std::exit(1);
        } break;
    }

    if (!sym) {
        printf("Could not find a symbol in expression \'%s\'\n", expr->print(0));

        std::exit(1);
    }

    return sym;
}

void koala::interpreter::add_local_symbol(std::string name, koala::interpreter::value* v, bool mut) {
    symbol sym;

    sym.mut = mut;
    sym.name = name;
    sym.v = v;

    m_locals.top().push_back(sym);
}

koala::interpreter::value* koala::interpreter::call_function(koala::function_def* fd, std::vector <koala::interpreter::value*> args) {
    if (fd->name == "putchar") {
        if (args.size() != fd->args.size()) {
            printf("Calling function with incorrect number of arguments\n");

            std::exit(1);
        }

        if (args[0]->get_class() != TP_INTEGRAL) {
            printf("Argument %i is incompatible with function argument of type \"%s\"",
                1,
                fd->args[0].t->str().c_str()
            );

            std::exit(1);
        }

        integral_value* iv = (integral_value*)args[0];
        integral_value return_iv;

        return_iv.t = (integral_type*)m_ts->get_type("char");
        return_iv.value = std::putchar(iv->value);

        return new integral_value(return_iv);
    }

    m_locals.push(std::vector <symbol>());

    add_local_symbol("__func__", make_value(fd->name), false);

    if (args.size() != fd->args.size()) {
        printf("Calling function with incorrect number of arguments\n");

        std::exit(1);
    }

    if (args.size()) {
        for (int i = 0; i < args.size(); i++) {
            if (args[i]->get_class() != fd->args[i].t->get_class()) {
                printf("Argument %i is incompatible with function argument of type \"%s\"",
                    i + 1,
                    fd->args[i].t->str().c_str()
                );

                std::exit(1);
            }

            add_local_symbol(fd->args[i].name, args[i], fd->args[i].mut);
        }
    }

    m_return_stack.push(nullptr);

    for (statement* s : fd->body)
        if (!execute_statement(s))
            break;

    m_locals.pop();

    value* return_value = m_return_stack.top();

    m_return_stack.pop();

    return return_value;
}

koala::interpreter::value* koala::interpreter::call_function(std::string name, std::vector <koala::interpreter::value*> args) {
    symbol* sym = search_symbol(name);

    if (!sym) {
        printf("Could not find symbol \'%s\'\n", name.c_str());

        std::exit(1);
    }

    if (sym->v->get_class() != TP_FUNCTION) {
        printf("Symbol \'%s\' is not a function", name.c_str());

        std::exit(1);
    }

    function_value* fv = (function_value*)sym->v;

    return call_function(fv->def, args);
}