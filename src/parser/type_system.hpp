#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#define KOALA_ARCH_PTR_SIZE 4

namespace koala {
    enum : int {
        TP_INTEGRAL,
        TP_POINTER,
        TP_FUNCTION,
        TP_STRUCT
    };

    class type {
    public:
        std::string sig;
        int size;

        type(std::string sig, int size) :
            sig(sig), size(size) {};

        virtual int get_type() = 0;
        virtual int get_size() = 0;
        virtual std::string str() = 0;
    };

    class integral_type : public type {
    public:
        bool is_signed;

        integral_type(std::string sig, int size, bool is_signed) :
            type(sig, size), is_signed(is_signed) {};
        
        int get_type() override {
            return TP_INTEGRAL;
        }

        int get_size() override {
            return size;
        }

        std::string str() override {
            return sig;
        }
    };

    class pointer_type : public type {
    public:
        type* target;

        pointer_type(type* target) :
            type("", KOALA_ARCH_PTR_SIZE), target(target) {};
        
        int get_type() override {
            return TP_POINTER;
        }

        int get_size() override {
            return KOALA_ARCH_PTR_SIZE;
        }

        std::string str() override {
            if (sig.size())
                return sig;
            
            sig = target->str() + "*";

            return sig;
        }
    };

    class function_type : public type {
    public:
        function_type() :
            type("", 0) {};

        type* return_type;
        std::vector <type*> arg_types;

        int get_type() override {
            return TP_FUNCTION;
        }

        int get_size() override {
            return 0;
        }

        std::string str() override {
            if (sig.size())
                return sig;

            sig = "((";

            if (arg_types.size()) {
                for (int i = 0; i < arg_types.size() - 1; i++)
                    sig += arg_types[i]->str() + ", ";
                
                sig += arg_types.back()->str();
            }

            sig += ") -> " + return_type->str() + ")";

            return sig;
        }
    };

    class struct_type : public type {
    public:
        std::vector <type*> member_types;

        struct_type() :
            type("", 0) {};

        int get_type() override {
            return TP_STRUCT;
        }

        int get_size() override {
            int size = 0;

            for (type* t : member_types)
                size += t->get_size();

            return size;
        }

        std::string str() override {
            if (sig.size())
                return sig;
            
            sig = "{";

            if (member_types.size()) {
                for (int i = 0; i < member_types.size() - 1; i++)
                    sig += member_types[i]->str() + ", ";
                
                sig += member_types.back()->str();
            }

            sig += "}";

            return sig;
        }
    };

    class type_system {
        std::unordered_map <std::string, type*> m_type_map = {
            { "short", new integral_type("short", 2, true) },
            { "long" , new integral_type("long" , 4, true) },
            { "char" , new integral_type("char" , 1, true) },
            { "int"  , new integral_type("int"  , 4, true) },
            { "i8"   , new integral_type("i8"   , 1, true) },
            { "i16"  , new integral_type("i16"  , 2, true) },
            { "i32"  , new integral_type("i32"  , 4, true) },
            { "u8"   , new integral_type("u8"   , 1, false) },
            { "u16"  , new integral_type("u16"  , 2, false) },
            { "u32"  , new integral_type("u32"  , 4, false) }
        };

    public:
        type* get_type(std::string sig);
        type* add_type(type* t);
        bool is_equal(type* t, type* u);
        bool is_comparable(type* t, type* u);
    };
}