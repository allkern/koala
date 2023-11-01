#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "type_signature.hpp"

#define KOALA_ARCH_PTR_SIZE 4

namespace koala {
    enum : int {
        TP_INTEGRAL,
        TP_POINTER
    };

    class type {
    public:
        type_signature sig;
        int size;
        bool is_const;
        bool is_static;

        type(type_signature sig, int size, bool is_const, bool is_static) :
            sig(sig), size(size), is_const(is_const), is_static(is_static) {};

        virtual int get_type() = 0;
    };

    class integral_type : public type {
    public:
        bool is_signed;

        integral_type(type_signature sig, int size, bool is_signed) :
            type(sig, size, false, false), is_signed(is_signed) {};
        
        int get_type() override {
            return TP_INTEGRAL;
        }
    };

    class pointer_type : public type {
    public:
        type* target;

        pointer_type(type_signature sig, type* target) :
            type(sig, KOALA_ARCH_PTR_SIZE, false, false), target(target) {};
        
        int get_type() override {
            return TP_POINTER;
        }
    };

    class type_system {
        std::unordered_map <std::string, type*> m_type_map = {
            { "short", new integral_type(type_signature("short"), 2, true) },
            { "long" , new integral_type(type_signature("long" ), 4, true) },
            { "char" , new integral_type(type_signature("char" ), 1, true) },
            { "int"  , new integral_type(type_signature("int"  ), 4, true) },
            { "i8"   , new integral_type(type_signature("i8"   ), 1, true) },
            { "i16"  , new integral_type(type_signature("i16"  ), 2, true) },
            { "i32"  , new integral_type(type_signature("i32"  ), 4, true) },
            { "u8"   , new integral_type(type_signature("u8"   ), 1, false) },
            { "u16"  , new integral_type(type_signature("u16"  ), 2, false) },
            { "u32"  , new integral_type(type_signature("u32"  ), 4, false) },
        };
    
    public:
        bool is_type(std::string ident);
        type* get_type(type_signature sig);
        bool is_equal(type* t, type* u);
        bool is_comparable(type* t, type* u);
    };
}