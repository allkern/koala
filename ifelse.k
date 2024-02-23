fn puts(s: koala_string) {
    mut i = 0;

    while (i != s.size) {
        putchar(s.ptr[i++]);
    }

    putchar('\n');

    return i + 1;
}

fn main() {
    mut a = 
    puts("Hello, world!");
}