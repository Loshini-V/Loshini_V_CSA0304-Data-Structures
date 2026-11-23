#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { int order_id; int amount; } Order;
long long comparisons = 0;

static void swap_int(int *a, int *b) { int t=*a; *a=*b; *b=t; }

void bubble_sort(int a[], int n) {
    for (int i=0; i<n-1; i++) {
        int changed=0;
        for (int j=0; j<n-i-1; j++) {
            comparisons++;
            if (a[j] > a[j+1]) { swap_int(&a[j],&a[j+1]); changed=1; }
        }
        if (!changed) break;
    }
}
void insertion_sort(int a[], int n) {
    for (int i=1; i<n; i++) {
        int key=a[i], j=i-1;
        while (j>=0) {
            comparisons++;
            if (a[j] <= key) break;
            a[j+1]=a[j]; j--;
        }
        a[j+1]=key;
    }
}
void selection_sort(int a[], int n) {
    for (int i=0; i<n-1; i++) {
        int m=i;
        for (int j=i+1; j<n; j++) {
            comparisons++;
            if (a[j] < a[m]) m=j;
        }
        swap_int(&a[i],&a[m]);
    }
}
void merge_int(int a[], int tmp[], int lo, int mid, int hi) {
    int i=lo,j=mid+1,k=lo;
    while (i<=mid && j<=hi) {
        comparisons++;
        if (a[i] <= a[j]) tmp[k++]=a[i++];
        else tmp[k++]=a[j++];
    }
    while (i<=mid) tmp[k++]=a[i++];
    while (j<=hi) tmp[k++]=a[j++];
    for (i=lo;i<=hi;i++) a[i]=tmp[i];
}
void merge_sort_rec(int a[], int tmp[], int lo, int hi) {
    if (lo>=hi) return;
    int mid=lo+(hi-lo)/2;
    merge_sort_rec(a,tmp,lo,mid);
    merge_sort_rec(a,tmp,mid+1,hi);
    merge_int(a,tmp,lo,mid,hi);
}
void merge_sort(int a[], int n) {
    int *tmp=malloc(sizeof(int)*n);
    if (!tmp) { perror("malloc"); exit(1); }
    merge_sort_rec(a,tmp,0,n-1); free(tmp);
}
int partition(int a[], int lo, int hi) {
    int p=a[hi], i=lo-1;
    for (int j=lo;j<hi;j++) {
        comparisons++;
        if (a[j]<=p) { i++; swap_int(&a[i],&a[j]); }
    }
    swap_int(&a[i+1],&a[hi]); return i+1;
}
void quick_rec(int a[], int lo, int hi) {
    if (lo>=hi) return;
    int p=partition(a,lo,hi);
    quick_rec(a,lo,p-1); quick_rec(a,p+1,hi);
}
void quick_sort(int a[], int n) { quick_rec(a,0,n-1); }
void heap_down(int a[], int n, int i) {
    for (;;) {
        int l=2*i+1,r=l+1,m=i;
        if(l<n && a[l]>a[m]) m=l;
        if(r<n && a[r]>a[m]) m=r;
        if(m==i) break;
        swap_int(&a[i],&a[m]); i=m;
    }
}
void heap_sort(int a[], int n) {
    for(int i=n/2-1;i>=0;i--) heap_down(a,n,i);
    for(int i=n-1;i>0;i--) { swap_int(&a[0],&a[i]); heap_down(a,i,0); }
}

static int is_sorted(int a[], int n) {
    for(int i=1;i<n;i++) if(a[i-1]>a[i]) return 0;
    return 1;
}
static void run_one(const char *name, void (*sortfn)(int[],int),
                    int *source, int n) {
    int *a=malloc(sizeof(int)*n);
    memcpy(a,source,sizeof(int)*n);
    comparisons=0;
    clock_t start=clock();
    sortfn(a,n);
    double seconds=(double)(clock()-start)/CLOCKS_PER_SEC;
    printf("%-12s n=%d time=%.4f sec comparisons=%lld sorted=%s\n",
           name,n,seconds,comparisons,is_sorted(a,n)?"yes":"NO");
    free(a);
}
int main(void) {
    const int sizes[]={1000,100000,1000000};
    srand(7);
    for(int s=0;s<3;s++) {
        int n=sizes[s];
        int *a=malloc(sizeof(int)*n);
        if(!a) return 1;
        for(int i=0;i<n;i++) a[i]=rand();
        printf("\nRANDOM INPUT, n=%d\n",n);
        /* Quadratic algorithms are only run at 1,000 to avoid impractical runtimes. */
        if(n==1000) {
            run_one("Bubble",bubble_sort,a,n);
            run_one("Insertion",insertion_sort,a,n);
            run_one("Selection",selection_sort,a,n);
        }
        run_one("Merge",merge_sort,a,n);
        run_one("Quick",quick_sort,a,n);
        run_one("Heap",heap_sort,a,n);

        /* sorted and reverse input are demonstrated with scalable algorithms */
        merge_sort(a,n);
        printf("SORTED INPUT, n=%d\n",n);
        run_one("Merge",merge_sort,a,n);
        run_one("Heap",heap_sort,a,n);
        for(int i=0;i<n/2;i++) swap_int(&a[i],&a[n-1-i]);
        printf("REVERSE INPUT, n=%d\n",n);
        run_one("Merge",merge_sort,a,n);
        run_one("Heap",heap_sort,a,n);
        free(a);
    }

    /* Stable merge example from the assignment */
    Order o[8]={{1,450},{2,120},{3,450},{4,990},
                {5,120},{6,300},{7,780},{8,300}};
    Order tmp[8];
    /* small direct stable merge-sort */
    for(int width=1;width<8;width*=2) {
        for(int lo=0;lo<8;lo+=2*width) {
            int mid=lo+width-1, hi=lo+2*width-1;
            if(mid>=8) continue;
            if(hi>=8) hi=7;
            int i=lo,j=mid+1,k=lo;
            while(i<=mid && j<=hi) {
                if(o[i].amount<=o[j].amount) tmp[k++]=o[i++];
                else tmp[k++]=o[j++];
            }
            while(i<=mid) tmp[k++]=o[i++];
            while(j<=hi) tmp[k++]=o[j++];
            for(i=lo;i<=hi;i++) o[i]=tmp[i];
        }
    }
    puts("\nStable merge-sort example:");
    for(int i=0;i<8;i++) printf("#%d:%d ",o[i].order_id,o[i].amount);
    puts("");
    return 0;
}
