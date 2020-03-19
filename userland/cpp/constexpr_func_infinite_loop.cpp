// https://cirosantilli.com/linux-kernel-module-cheat#cpp

constexpr int f() {
    int i = 0;
    while (1)
        i += 1;
    return i;
}

constexpr int g() {
    return g();
}


int main() {
#if 0
    // GCC 9.2.1. error: ‘constexpr’ loop iteration count exceeds limit of 262144 (use ‘-fconstexpr-loop-limit=’ to increase the limit)
    static_assert(f() == 0);
    // GCC 9.2.1. error: ‘constexpr’ evaluation depth exceeds maximum of 512 (use ‘-fconstexpr-depth=’ to increase the maximum)
    static_assert(g() == 0);
#endif
}
