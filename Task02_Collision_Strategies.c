#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUCKETS 7
#define M 10

typedef struct Member {
    char id[12];
    char name[20];
    struct Member *next;
} Member;

Member *table[BUCKETS] = {NULL};

unsigned hash(const char *s) {
    unsigned h = 0;
    while (*s) h = h * 31u + (unsigned char)*s++;
    return h % BUCKETS;
}

void put(const char *id, const char *name) {
    unsigned b = hash(id);
    Member *m = malloc(sizeof(Member));
    if (!m) { perror("malloc"); exit(1); }
    snprintf(m->id, sizeof m->id, "%s", id);
    snprintf(m->name, sizeof m->name, "%s", name);
    m->next = table[b];              /* insert at front */
    table[b] = m;
}

const char *get(const char *id, int *probes) {
    unsigned b = hash(id);
    *probes = 0;
    for (Member *m = table[b]; m != NULL; m = m->next) {
        (*probes)++;
        if (strcmp(m->id, id) == 0) return m->name;
    }
    return NULL;
}

static void chaining_demo(void) {
    const char *ids[] = {"CSE1043", "ECE2210", "MEC3307",
                         "CSE1044", "BME4120", "CIV5009"};
    const char *names[] = {"Kavya", "Arjun", "Fathima",
                           "Rahul", "Deepa", "Manoj"};
    for (int i = 0; i < 6; i++) put(ids[i], names[i]);

    puts("Separate chaining bucket lengths:");
    for (int b = 0; b < BUCKETS; b++) {
        int len = 0;
        for (Member *m = table[b]; m; m = m->next) len++;
        printf("%d", len);
    }
    int p;
    const char *who = get("CSE1044", &p);
    printf("\nCSE1044 -> %s (%d probes)\n", who ? who : "not found", p);
    who = get("CSE9999", &p);
    printf("CSE9999 -> %s (%d probes)\n", who ? who : "not found", p);

    for (int b = 0; b < BUCKETS; b++) {
        while (table[b]) {
            Member *next = table[b]->next;
            free(table[b]);
            table[b] = next;
        }
    }
}

static void linear_probing_demo(void) {
    int t[M];
    int keys[] = {42, 17, 52, 27, 99, 62};
    for (int i = 0; i < M; i++) t[i] = -1;
    puts("\nLinear probing (key@position(probes)):");
    for (int k = 0; k < 6; k++) {
        int i = keys[k] % M, probes = 1;
        while (t[i] != -1) {
            i = (i + 1) % M;
            probes++;
        }
        t[i] = keys[k];
        printf("%d@%d(%d) ", keys[k], i, probes);
    }
    puts("\nFinal table:");
    for (int i = 0; i < M; i++)
        printf("[%d] %s\n", i, t[i] == -1 ? "empty" : "");
}

int main(void) {
    chaining_demo();
    linear_probing_demo();
    return 0;
}
