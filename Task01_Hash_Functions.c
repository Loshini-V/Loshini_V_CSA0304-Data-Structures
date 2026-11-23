#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define N 100000
#define TABLE_SIZE 131071
#define OLD_SIZE 1000

typedef struct Node {
    char key[20];
    struct Node *next;
} Node;

static unsigned poor_hash(const char *s) {
    return ((unsigned char)s[0] + (unsigned char)s[1]) % OLD_SIZE;
}

static unsigned polynomial_hash(const char *s) {
    unsigned long h = 0;
    while (*s) h = (h * 31u + (unsigned char)*s++) % TABLE_SIZE;
    return (unsigned)h;
}

static uint32_t fnv1a_hash(const char *s) {
    uint32_t h = 2166136261u;
    while (*s) {
        h ^= (unsigned char)*s++;
        h *= 16777619u;
    }
    return h % TABLE_SIZE;
}

static void make_registration(char *out, int i) {
    /* Similar state prefix, but varying district, series and number. */
    int district = (i % 99) + 1;
    int series = (i / 99) % 26;
    int number = i % 10000;
    snprintf(out, 20, "TN%02d%c%c%04d", district,
             'A' + (series % 26), 'A' + ((series / 26) % 26), number);
}

static void measure(const char *label, unsigned (*hashfn)(const char *),
                    char keys[N][20], unsigned table_size) {
    unsigned *counts = calloc(table_size, sizeof(unsigned));
    if (!counts) { puts("Memory allocation failed"); exit(1); }

    for (int i = 0; i < N; i++) counts[hashfn(keys[i])]++;
    unsigned longest = 0;
    unsigned long long total = 0;
    unsigned used = 0;
    for (unsigned i = 0; i < table_size; i++) {
        if (counts[i] > longest) longest = counts[i];
        if (counts[i]) { used++; total += counts[i]; }
    }
    printf("%-18s longest bucket/chain = %u, average per used bucket = %.2f\n",
           label, longest, used ? (double)total / used : 0.0);
    free(counts);
}

static unsigned poor_wrapper(const char *s) { return poor_hash(s); }

int main(void) {
    char (*keys)[20] = malloc(sizeof(char[20]) * N);
    if (!keys) return 1;
    for (int i = 0; i < N; i++) make_registration(keys[i], i);

    puts("Hash-function comparison for 100,000 generated registrations");
    measure("Poor first-2-char", poor_wrapper, keys, OLD_SIZE);
    measure("Polynomial", polynomial_hash, keys, TABLE_SIZE);
    measure("FNV-1a", (unsigned (*)(const char *))fnv1a_hash, keys, TABLE_SIZE);

    free(keys);
    return 0;
}
