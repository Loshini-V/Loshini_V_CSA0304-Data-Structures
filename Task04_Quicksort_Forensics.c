#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long comparisons = 0;
int max_depth = 0;

void swap(int *a, int *b) { int t=*a; *a=*b; *b=t; }

int partition(int a[], int lo, int hi) {
    int pivot = a[hi], i = lo - 1;
    for (int j=lo; j<hi; j++) {
        comparisons++;
        if (a[j] <= pivot) { i++; swap(&a[i], &a[j]); }
    }
    swap(&a[i+1], &a[hi]);
    return i+1;
}

void quick(int a[], int lo, int hi, int depth) {
    if (depth > max_depth) max_depth = depth;
    if (lo >= hi) return;
    int p = partition(a, lo, hi);
    quick(a, lo, p-1, depth+1);
    quick(a, p+1, hi, depth+1);
}

int main(void) {
    int sample[] = {7, 2, 9, 4, 3, 8, 5};
    int n = 7, pivot = sample[n-1], i = -1, swaps = 0;
    for (int j=0; j<n-1; j++) {
        if (sample[j] <= pivot) {
            i++;
            swap(&sample[i], &sample[j]);
            swaps++;
        }
    }
    swap(&sample[i+1], &sample[n-1]);
    printf("Partition result: ");
    for (int k=0; k<n; k++) printf("%d ", sample[k]);
    printf("| pivot at %d, swaps %d\n", i+1, swaps);

    const int N = 10000;
    int *a = malloc(sizeof(int)*N);
    if (!a) return 1;

    srand(42);
    for (int i=0; i<N; i++) a[i] = rand();
    comparisons=0; max_depth=0;
    quick(a, 0, N-1, 1);
    printf("Random input: comparisons=%lld, max recursion depth=%d\n",
           comparisons, max_depth);

    for (int i=0; i<N; i++) a[i] = i;
    comparisons=0; max_depth=0;
    /* Last-pivot quicksort is intentionally quadratic on sorted input.
       Keep N=10000 only if your system has enough stack. */
    quick(a, 0, N-1, 1);
    printf("Sorted input: comparisons=%lld, max recursion depth=%d\n",
           comparisons, max_depth);
    free(a);
    return 0;
}
