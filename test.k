fn puts(s: koala_string) {
    mut i = 0;

    while (i != s.size) {
        putchar(s.ptr[i++]);
    }

    putchar('\n');

    return i + 1;
}

fn print_integer(mut n: int) {
    if (n == 0) {
        putchar('0');
        putchar('\n');

        return 0;
    }

    mut rev = 0;
    mut rem = 0;
    mut i = 0;

    while (n) {
        rem = n % 10;
        rev = rev * 10 + rem;

        n /= 10;

        ++i;
    }

    while (i--) {
        putchar(0x30 + (rev % 10));

        rev /= 10;
    }

    putchar('\n');

    return 0;
}

fn fib(n: u32) {
    if (n == 0) {
        return 0;
    }

    if (n == 1)
        return 1;

    return fib(n - 1) + fib(n - 2);
}

fn main() {
    puts("Fibonacci sequence:");

    mut i = 0;
    mut f = fib;

    while (i != 20) {
        print_integer(f(i++));
    }
    
    return 0;
}