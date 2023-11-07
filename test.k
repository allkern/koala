fn foo(a: i32, b: { u32, u8 }) -> char** {
    a = 2;
}

fn main(a: int, b: u32*) -> int {
    let s = foo;
    let f = s;

    f = 10;

    print(s);
}