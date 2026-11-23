#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 262147
#define MAX_ORDERS 100000

typedef struct Order {
    int id;
    int restaurant;
    int amount;
    int delay;
    int status;       /* 0 active, 1 assigned, 2 cancelled */
    int heap_index;
    struct Order *next;
} Order;

Order *table[TABLE_SIZE];
Order *heap[MAX_ORDERS];
int heap_size=0;

unsigned hash_id(int id) {
    unsigned x=(unsigned)id;
    x ^= x >> 16; x *= 0x7feb352dU;
    x ^= x >> 15; x *= 0x846ca68bU;
    x ^= x >> 16;
    return x % TABLE_SIZE;
}
Order *find_order(int id) {
    for(Order *p=table[hash_id(id)];p;p=p->next)
        if(p->id==id) return p;
    return NULL;
}
void hash_insert(Order *o) {
    unsigned b=hash_id(o->id);
    o->next=table[b]; table[b]=o;
}
void hash_remove(int id) {
    unsigned b=hash_id(id);
    Order **p=&table[b];
    while(*p) {
        if((*p)->id==id) { *p=(*p)->next; return; }
        p=&(*p)->next;
    }
}
int higher(Order *a, Order *b) {
    if(a->delay!=b->delay) return a->delay>b->delay;
    return a->id<b->id;
}
void swap_heap(int a,int b) {
    Order *t=heap[a]; heap[a]=heap[b]; heap[b]=t;
    heap[a]->heap_index=a; heap[b]->heap_index=b;
}
void sift_up(int i) {
    while(i>0) {
        int p=(i-1)/2;
        if(!higher(heap[i],heap[p])) break;
        swap_heap(i,p); i=p;
    }
}
void sift_down(int i) {
    for(;;) {
        int l=2*i+1,r=l+1,m=i;
        if(l<heap_size && higher(heap[l],heap[m])) m=l;
        if(r<heap_size && higher(heap[r],heap[m])) m=r;
        if(m==i) break;
        swap_heap(i,m); i=m;
    }
}
void heap_add(Order *o) {
    if(heap_size>=MAX_ORDERS) return;
    heap[heap_size]=o; o->heap_index=heap_size;
    sift_up(heap_size++);
}
void heap_remove_at(int i) {
    if(i<0 || i>=heap_size) return;
    heap_size--;
    if(i==heap_size) return;
    heap[i]=heap[heap_size]; heap[i]->heap_index=i;
    if(i>0 && higher(heap[i],heap[(i-1)/2])) sift_up(i);
    else sift_down(i);
}
void place_order(int id,int restaurant,int amount,int delay) {
    Order *o=malloc(sizeof(Order));
    if(!o) { perror("malloc"); exit(1); }
    o->id=id; o->restaurant=restaurant; o->amount=amount;
    o->delay=delay; o->status=0; o->heap_index=-1; o->next=NULL;
    hash_insert(o); heap_add(o);
}
void cancel_order(int id) {
    Order *o=find_order(id);
    if(!o || o->status!=0) return;
    heap_remove_at(o->heap_index);
    o->status=2;
    hash_remove(id);
}
Order *assign_order(void) {
    if(heap_size==0) return NULL;
    Order *o=heap[0];
    heap_remove_at(0);
    o->status=1;
    return o;
}
int compare_order(const void *a,const void *b) {
    const Order *x=*(const Order * const *)a;
    const Order *y=*(const Order * const *)b;
    if(x->restaurant!=y->restaurant) return x->restaurant-y->restaurant;
    if(x->amount!=y->amount) return x->amount-y->amount;
    return x->id-y->id;
}
static double seconds(clock_t a,clock_t b) {
    return (double)(b-a)/CLOCKS_PER_SEC;
}
int main(void) {
    const int counts[]={50000,100000};
    for(int test=0;test<2;test++) {
        memset(table,0,sizeof table); heap_size=0;
        int n=counts[test];
        clock_t t0=clock();
        for(int i=0;i<n;i++)
            place_order(1000000+i, i%500, 100+(i%900), i%240);
        clock_t t1=clock();

        clock_t s0=clock();
        for(int i=0;i<n;i++) (void)find_order(1000000+i);
        clock_t s1=clock();

        /* Cancel every 10th order. */
        clock_t c0=clock();
        for(int i=0;i<n;i+=10) cancel_order(1000000+i);
        clock_t c1=clock();

        clock_t a0=clock();
        int assigned=0;
        while(heap_size>0) { (void)assign_order(); assigned++; }
        clock_t a1=clock();

        printf("\nOrders: %d\n",n);
        printf("Place all: %.4f sec\n",seconds(t0,t1));
        printf("Search all: %.4f sec\n",seconds(s0,s1));
        printf("Cancel 10%%: %.4f sec\n",seconds(c0,c1));
        printf("Assign remaining (%d): %.4f sec\n",assigned,seconds(a0,a1));
    }
    /* Note: report sorting can be timed separately on a copied array of orders.
       This simulation focuses on hash lookup, heap assignment and cancellation. */
    return 0;
}
