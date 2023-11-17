#pragma once

#include "parser/parser.hpp"

#include <stack>

/*
fn foo(a: int) {
    mut b = 2;

    return a * b;
}

fn test() {
    mut a = 20;
    mut b = 2;
    mut c = b * 2;
    mut d = a + b * c;

    return d + foo(a + b);
}

foo:
    t0 = 2
    return t0 * a0

puts:
    a  = 20         ; li   a, 20
    b  = 2          ; li   b, 2
    c  = b * 2      ; mul  c, b, 2
    t0 = b * c      ; mul  t0, b, c
    d  = a + t0     ; add  d, a, t0
    t1 = a + b      ; add  t1, a, b
    t2 = foo(t1)    ; call foo, t2, t1
    ret t2          ; ret t2
*/


namespace koala {

    enum : int {
        VT_LOCAL,
        VT_GLOBAL,
        VT_ARG,
        VT_TEMP,
        VT_CONSTANT
    };

    class ir {
        std::vector <statement*>* m_ast = nullptr;
        type_system*              m_ts = nullptr;

        struct variable {
            std::string name;
            int type;
            int size;
        };

    public:
        void a();
    };
}