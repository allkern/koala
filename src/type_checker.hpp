#pragma once

#include "parser/parser.hpp"

namespace koala {
    struct symbol {
        type* t = nullptr;

        std::string name;
    };

    class type_checker {
        std::vector <statement*>* m_ast;
        type_system*              m_ts = nullptr;
        std::vector <symbol>      m_local_symbols;
        std::vector <symbol>      m_global_symbols;
        int                       m_scope = 0;
        function_type*            m_current_ft = nullptr;
        symbol*                   m_current_symbol = nullptr;

        type* complete_type(type* t);
        symbol* push_symbol(std::string name, type* t);
        symbol& lookup_symbol(std::string name);
        struct_member& lookup_member(std::string name, struct_type* st);
        type* get_type(function_call_expr* fc);
        type* get_type(array_access* aa);
        type* get_type(integer_constant* ic);
        type* get_type(string_literal* sl);
        type* get_type(binary_op* bo);
        type* get_type(unary_op* uo);
        type* get_type(name_ref* nr);
        type* get_type(member_access* ma);
        type* get_type(expression* e);
        void check_statement(variable_def* vd);
        void check_statement(assignment* a);
        void check_statement(function_call* fc);
        void check_statement(return_expr* re);
        void check_statement(function_def* fd);
        void check_statement(statement* s);

    public:
        type_checker(parser& p) :
            m_ast(p.get_ast()), m_ts(p.get_type_system()), m_scope(0) {};

        void check();
    };
}