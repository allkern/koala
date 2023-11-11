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
    mut cnt = 10;

    while (cnt) {
        print("Hello, world!\n");

        cnt -= 1;
    }

    return 0;
}