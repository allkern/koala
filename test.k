fn foo(a: int) {
    return a * a;
}

fn bar(a: int) {
    let square = foo(a);

    return square * a;
}

fn print(str: koala_string) {
    let ptr = str.ptr;
    mut i = 0;

    while (i != str.size) {
        putchar(ptr[i]);

        i += 1;
    }

    return i;
}

fn main(a: int, str: koala_string) {
    return print("Hello, world!");
}