fn main(int x, int y) -> int {
    int a = x / y;
    int b = a + 2 * 3;
    int c = a << b & 1;

    a <<= 4;

    strcpy(a, 2 * b + 2 * c);
}

fn strcpy(char* dst, char* src) -> char* {
    static const char* temp = 10;
}