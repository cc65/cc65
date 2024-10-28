#if (0u - 1) < 0
#error
#endif

#if !1u - 1 > 0
#error
#endif

#if (1 & 1u) - 2 < 0
#error
#endif

#if (1 | 1u) - 2 < 0
#error
#endif

#if (1 ^ 1u) - 2 < 0
#error
#endif

#if (1u >> 1) - 2 < 0
#error
#endif

#if (0u << 1) - 1 < 0
#error
#endif

int main() { return 0; }
