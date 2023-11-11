fn print(str: koala_string) {
    let ptr = str.ptr;
    mut i = 0;

    while (i != str.size) {
        putchar(ptr[i++]);
    }

    return i;
}

fn main(a: int, str: koala_string) {
    print("Hello, world!\n");

    return 0;
}