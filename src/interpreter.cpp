#include "interpreter.hpp"

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
    { "!"  , koala::UN_LOGICAL_NOT }
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

koala::interpreter::value* koala::interpreter::evaluate_expression(function_call_expr* fc) {
    if (fc->expr->get_tag() != EX_NAME_REF) {
        printf("Non-name-reference callee is not allowed in interpreter\n");

        std::exit(1);
    }

    std::string callee = ((name_ref*)fc->expr)->name;

    variable* var = search_variable(callee);

    std::vector <value*> args;

    if (!var) {
        constant* c = search_constant(callee);

        if (!c) {
            printf("Name \'%s\' was not found\n",
                callee.c_str()
            );

            std::exit(1);
        }

        for (expression* expr : fc->args)
            args.push_back(evaluate_expression(expr));

        return call_function(callee, args);
    } else {
        for (expression* expr : fc->args)
            args.push_back(evaluate_expression(expr));

        return call_function(callee, args);
    }
}

// a[b] a: char*
// char
// 
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

    variable size, ptr;

    size_iv.t = (integral_type*)st->members[0].t;
    size_iv.value = sl->value.size();
    ptr_pv.t = (pointer_type*)st->members[1].t;
    ptr_pv.value = (uintptr_t)sl->value.c_str();

    size.mut = false;
    size.name = st->members[0].name;
    size.t = st->members[0].t;
    size.v = new integral_value(size_iv);

    ptr.mut = false;
    ptr.name = "ptr";
    ptr.t = st->members[1].t;
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

    uint64_t value = ((integral_value*)eval_expr)->value;

    integral_value iv;

    iv.t = ((integral_value*)eval_expr)->t;
    
    unary_operation(g_unary_op_map[uo->op], iv.value, value);

    return new integral_value(iv);
}

koala::interpreter::value* koala::interpreter::evaluate_expression(name_ref* nr) {
    variable* var = search_variable(nr->name);

    if (!var) {
        constant* c = search_constant(nr->name);

        if (!c) {
            printf("Name \'%s\' was not found\n",
                nr->name.c_str()
            );

            std::exit(1);
        }

        return c->v;
    } else {
        return var->v;
    }
}

koala::interpreter::value* koala::interpreter::evaluate_expression(member_access* ma) {
    value* expr_eval = evaluate_expression(ma->expr);

    if (expr_eval->get_class() != TP_STRUCT) {
        printf("Used member access operator on a non-struct type\n");

        std::exit(1);
    }

    struct_value* sv = (struct_value*)expr_eval;

    variable* m = search_member(ma->member, sv);

    return m->v;
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
            return evaluate_expression((function_call_expr*)expr);
        } break;
    }

    printf("Unknown expression\n");

    std::exit(1);

    return nullptr;
}

void koala::interpreter::execute_statement(variable_def* vd) {
    if (search_variable(vd->name)) {
        printf("Attempting to define a variable with existing name \'%s\'\n",
            vd->name.c_str()
        );

        std::exit(1);
    } else if (search_constant(vd->name)) {
        printf("Attempting to define a variable with existing name \'%s\'\n",
            vd->name.c_str()
        );

        std::exit(1);
    }

    variable var;

    var.mut = vd->mut;
    var.name = vd->name;
    var.t = vd->t;

    if (vd->init) {
        var.v = evaluate_expression(vd->init);
    }

    // To-do: Default construct types

    add_variable(var);
}

void koala::interpreter::execute_statement(assignment* a) {
    variable* var = search_variable(a->dest);

    if (!var) {
        printf("Variable with name \'%s\' was not found\n",
            a->dest.c_str()
        );

        std::exit(1);
    }

    if (!var->mut) {
        printf("Cannot assign a value to an immutable variable\n");

        std::exit(1);
    }

    if (a->op == "=") {
        value* old_value = var->v;

        var->v = evaluate_expression(a->src);

        return;
    }

    value* eval_rhs = evaluate_expression(a->src);

    if ((var->t->get_class() != TP_INTEGRAL) || (eval_rhs->get_class() != TP_INTEGRAL)) {
        printf("Cannot perform a binary operation between non-integral types\n");

        std::exit(1);
    }

    uint64_t lhs = ((integral_value*)var->v)->value;
    uint64_t rhs = ((integral_value*)eval_rhs)->value;

    // All assignment operators end with '='
    // Popping the last character results in the desired
    // binary operator

    std::string op = a->op;

    op.pop_back();

    binary_operation(g_binary_op_map[op], ((integral_value*)var->v)->value, lhs, rhs);
}

void koala::interpreter::execute_statement(function_call* fc) {
    variable* var = search_variable(fc->callee);

    std::vector <value*> args;

    if (!var) {
        constant* c = search_constant(fc->callee);

        if (!c) {
            printf("Name \'%s\' was not found\n",
                fc->callee.c_str()
            );

            std::exit(1);
        }

        for (expression* expr : fc->args)
            args.push_back(evaluate_expression(expr));

        call_function(fc->callee, args);
    } else {
        for (expression* expr : fc->args)
            args.push_back(evaluate_expression(expr));

        call_function(fc->callee, args);
    }
}

void koala::interpreter::execute_statement(return_expr* re) {
    if (m_return_value.top())
        delete m_return_value.top();

    m_return_value.push(evaluate_expression(re->expr));
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

        case ST_FUNCTION_CALL: {
            execute_statement((function_call*)s);
        } break;

        case ST_FUNCTION_DEF: {
            execute_statement((function_def*)s);
        } break;

        case ST_ASSIGNMENT: {
            execute_statement((assignment*)s);
        } break;

        case ST_RETURN_EXPR: {
            execute_statement((return_expr*)s);

            return false;
        } break;
    }

    return true;
}

void koala::interpreter::init() {
    for (statement* s : *m_ast) {
        if (s->get_tag() == ST_VARIABLE_DEF) {
            variable_def* vd = (variable_def*)s;

            variable var;

            var.t = vd->t;
            var.mut = vd->mut;
            var.name = vd->name;
            
            if (vd->init) {
                var.v = evaluate_expression(vd->init);
            }

            m_global_variables.push_back(var);
        } else if (s->get_tag() == ST_FUNCTION_DEF) {
            function_def* fd = (function_def*)s;
            function_value fv;

            fv.def = fd;

            constant c;

            c.name = fd->name;
            c.v = new function_value(fv);

            m_constants.push_back(c);
        } else {
            printf("Statements in global scope are not allowed\n");

            std::exit(1);
        }
    }
}

koala::interpreter::variable* koala::interpreter::search_member(std::string name, struct_value* sv) {
    for (variable& v : sv->members)
        if (v.name == name)
            return &v;

    printf("Could not find member with name \'%s\'\n",
        name.c_str()
    );

    std::exit(1);
}

koala::interpreter::variable* koala::interpreter::search_variable(std::string name) {
    if (!m_local_variables.size())
        return nullptr;

    for (variable& v : m_local_variables.top())
        if (v.name == name)
            return &v;
    
    for (variable& v : m_global_variables)
        if (v.name == name)
            return &v;
    
    return nullptr;
}

koala::interpreter::constant* koala::interpreter::search_constant(std::string name) {
    for (constant& c : m_constants)
        if (c.name == name)
            return &c;

    return nullptr;
}

void koala::interpreter::add_variable(koala::interpreter::variable& v) {
    m_local_variables.top().push_back(v);
}

koala::function_def* koala::interpreter::search_function(std::string name) {
    variable* v = search_variable(name);

    if (!v) {
        constant* c = search_constant(name);

        if (!c) {
            printf("Could not find function \'%s\'\n", name.c_str());

            std::exit(1);

            return nullptr;
        }

        if (c->v->get_class() != TP_FUNCTION) {
            printf("Symbol \'%s\' is not a function type", c->name.c_str());

            *(int*)0 = 0;

            std::exit(1);

            return nullptr;
        }

        return ((function_value*)c->v)->def;
    } else {
        if (v->v->get_class() != TP_FUNCTION) {
            printf("Symbol \'%s\' is not a function type", v->name.c_str());
            *(int*)0 = 0;

            std::exit(1);

            return nullptr;
        }

        return ((function_value*)v->v)->def;
    }
}

koala::interpreter::value* koala::interpreter::call_function(std::string name, std::vector <koala::interpreter::value*> args) {
    function_def* fd = search_function(name);

    m_local_variables.push(std::vector <variable>());
    m_return_value.push(nullptr);

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

            variable var;

            var.mut = fd->args[i].mut;
            var.name = fd->args[i].name;
            var.t = fd->args[i].t;
            var.v = args[i];

            add_variable(var);
        }
    }

    for (statement* s : fd->body) {
        if (!execute_statement(s))
            break;
    }

    m_local_variables.pop();

    value* return_value = m_return_value.top();

    m_return_value.pop();

    return return_value;
}