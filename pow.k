fn power(x: i32, n: i32) -> i32 {
    if (!n) return 1;

    return x * power(x, n - 1);
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

fn main() {
    print_integer(power(2, 8));
}