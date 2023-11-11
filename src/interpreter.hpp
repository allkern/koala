#pragma once

#include "parser/parser.hpp"

#include <stack>

namespace koala {
    enum : int {
        BN_COMPARE_EQ,
        BN_COMPARE_NE,
        BN_COMPARE_LE,
        BN_COMPARE_GE,
        BN_COMPARE_LT,
        BN_COMPARE_GT,
        BN_LOGICAL_AND,
        BN_LOGICAL_OR,
        BN_LOGICAL_XOR,
        BN_ARITH_ADD,
        BN_ARITH_SUB,
        BN_ARITH_MUL,
        BN_ARITH_DIV,
        BN_ARITH_MOD,
        BN_BITWISE_AND,
        BN_BITWISE_NAND,
        BN_BITWISE_OR,
        BN_BITWISE_NOR,
        BN_BITWISE_XOR,
        BN_BITWISE_XNOR,
        BN_BITWISE_SL,
        BN_BITWISE_SR
    };

    enum : int {
        UN_BITWISE_NOT,
        UN_LOGICAL_NOT
    };

    enum : int {
        AP_EQ,
        AP_EQ_ADD,
        AP_EQ_SUB,
        AP_EQ_MUL,
        AP_EQ_DIV,
        AP_EQ_MOD,
        AP_EQ_BITWISE_AND,
        AP_EQ_BITWISE_NAND,
        AP_EQ_BITWISE_OR,
        AP_EQ_BITWISE_NOR,
        AP_EQ_BITWISE_XOR,
        AP_EQ_BITWISE_XNOR,
        AP_EQ_BITWISE_SL,
        AP_EQ_BITWISE_SR,
        AP_EQ_LOGICAL_AND,
        AP_EQ_LOGICAL_OR,
        AP_EQ_LOGICAL_XOR
    };

    class interpreter {
        std::vector <statement*>* m_ast;
        type_system*              m_ts = nullptr;

    public:
        struct symbol;

        class value {
        public:
            virtual int get_class();
            virtual std::string str();
        };

        class integral_value : public value {
        public:
            integral_type* t = nullptr;
            uint64_t value = 0;

            int get_class() override {
                return TP_INTEGRAL;
            }

            std::string str() override {
                return std::to_string(value);
            }
        };

        class struct_value : public value {
        public:
            struct_type* t = nullptr;

            std::vector <symbol> members;

            int get_class() override {
                return TP_STRUCT;
            }

            std::string str() override {
                return "{}";
            }
        };

        class pointer_value : public value {
        public:
            pointer_type* t = nullptr;
            uintptr_t value = 0;

            int get_class() override {
                return TP_POINTER;
            }

            std::string str() override {
                char buf[128];

                sprintf(buf, "<%p: %s>", value, t->str().c_str());

                return std::string(buf);
            }
        };

        class function_value : public value {
        public:
            function_def* def = nullptr;

            int get_class() override {
                return TP_FUNCTION;
            }

            std::string str() override {
                return "<" + def->name + ">";
            }
        };

        struct symbol {
            bool mut = false;
            value* v = nullptr;
            std::string name;
        };

    private:
        std::stack <std::vector <symbol>>   m_locals;
        std::vector <symbol>                m_globals;
        std::stack <value*>                 m_return_stack;

        value* get_default_value(type* t);
        value* evaluate_expression(function_call* fc);
        value* evaluate_expression(array_access* aa);
        value* evaluate_expression(integer_constant* ic);
        value* evaluate_expression(string_literal* sl);
        value* evaluate_expression(binary_op* bo);
        value* evaluate_expression(unary_op* uo);
        value* evaluate_expression(name_ref* nr);
        value* evaluate_expression(member_access* ma);
        value* evaluate_expression(expression* expr);
        void execute_statement(expression_statement* es);
        void execute_statement(variable_def* vd);
        void execute_statement(assignment* a);
        void execute_statement(return_expr* re);
        void execute_statement(function_def* fd);
        void execute_statement(while_loop* wl);
        bool execute_statement(statement* s);
        symbol* search_symbol(expression* expr);
        symbol* search_member(std::string name, struct_value* sv);
        symbol* search_symbol(std::string name);
        void add_local_symbol(std::string name, value* v, bool mut);

    public:
        interpreter(parser& p) :
            m_ast(p.get_ast()), m_ts(p.get_type_system()) {};

        void init();
        value* call_function(std::string name, std::vector <value*> args);
        value* call_function(function_def* fd, std::vector <value*> args);

        value* make_value(int v) {
            integral_value iv;

            iv.t = (integral_type*)m_ts->get_type("int");
            iv.value = v;

            return new integral_value(iv);
        }

        value* make_value(std::string s) {
            integral_value size_iv;
            pointer_value ptr_pv;
            struct_value sv;
            struct_type* st = (struct_type*)m_ts->get_type("koala_string");

            symbol size, ptr;

            size_iv.t = (integral_type*)st->members[0].t;
            size_iv.value = s.size();
            ptr_pv.t = (pointer_type*)st->members[1].t;
            ptr_pv.value = (uintptr_t)s.c_str();

            size.mut = false;
            size.name = st->members[0].name;
            size.v = new integral_value(size_iv);

            ptr.mut = false;
            ptr.name = "ptr";
            ptr.v = new pointer_value(ptr_pv);

            sv.t = st;
            sv.members.push_back(size);
            sv.members.push_back(ptr);

            return new struct_value(sv);
        }
    };
}