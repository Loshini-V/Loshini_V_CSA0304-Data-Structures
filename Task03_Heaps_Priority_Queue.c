#include <stdio.h>

#define MAX 32

typedef struct { int severity; int patient; int arrival; } Case;
Case heap[MAX];
int size = 0;

void swap(Case *a, Case *b) {
    Case t = *a; *a = *b; *b = t;
}

/* Higher severity wins. For equal severity, earlier arrival wins. */
int higher_priority(Case a, Case b) {
    if (a.severity != b.severity) return a.severity > b.severity;
    return a.arrival < b.arrival;
}

void push(int severity, int patient, int arrival) {
    if (size == MAX) return;
    heap[size] = (Case){severity, patient, arrival};
    int i = size++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (!higher_priority(heap[i], heap[parent])) break;
        swap(&heap[i], &heap[parent]);
        i = parent;
    }
}

Case pop(void) {
    Case top = heap[0];
    heap[0] = heap[--size];
    int i = 0;
    for (;;) {
        int l = 2 * i + 1, r = l + 1, best = i;
        if (l < size && higher_priority(heap[l], heap[best])) best = l;
        if (r < size && higher_priority(heap[r], heap[best])) best = r;
        if (best == i) break;
        swap(&heap[i], &heap[best]);
        i = best;
    }
    return top;
}

void sift_down_min(int a[], int n, int i) {
    for (;;) {
        int l = 2*i+1, r = l+1, smallest = i;
        if (l < n && a[l] < a[smallest]) smallest = l;
        if (r < n && a[r] < a[smallest]) smallest = r;
        if (smallest == i) return;
        int t=a[i]; a[i]=a[smallest]; a[smallest]=t;
        i=smallest;
    }
}

int main(void) {
    int a[] = {9, 4, 7, 1, 8, 2, 6};
    int n = 7;
    for (int i = n/2 - 1; i >= 0; i--) sift_down_min(a, n, i);
    printf("Min-heap array: ");
    for (int i=0; i<n; i++) printf("%d ", a[i]);
    puts("");

    int sev[] = {3, 9, 5, 7, 1, 8};
    for (int i=0; i<6; i++) push(sev[i], 101+i, i);
    printf("Treatment order: ");
    while (size) {
        Case c = pop();
        printf("P%d(%d) ", c.patient, c.severity);
    }
    puts("");
    return 0;
}
