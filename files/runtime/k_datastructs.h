/*
 * k_datastructs.h — Katlans Data Structures (ds prefix)
 * Stack, Queue, Set, LinkedList, Tree, Graph, Heap, Deque, Matrix
 */
#ifndef K_DATASTRUCTS_H
#define K_DATASTRUCTS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ══════════════════════════════════════════════════════════════════════════
 *  Stack  (dsstack / dspush / dspop / dspeek / dssize)
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_dsstack(void)            { return kv_list(); }
static inline KVal *k_dspush(KVal *s,KVal *v)  { klist_push(s->list,v); return kv_void(); }
static inline KVal *k_dspop(KVal *s) {
    if(!s->list->len) return kv_void();
    KVal *v=s->list->items[--s->list->len]; return v;
}
static inline KVal *k_dspeek(KVal *s) {
    return s->list->len ? s->list->items[s->list->len-1] : kv_void();
}
static inline KVal *k_dssize(KVal *s) { return kv_int((long long)s->list->len); }

/* ══════════════════════════════════════════════════════════════════════════
 *  Queue  (dsqueue / dsenqueue / dsdequeue)
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_dsqueue(void)              { return kv_list(); }
static inline KVal *k_dsenqueue(KVal *q,KVal *v) { klist_push(q->list,v); return kv_void(); }
static inline KVal *k_dsdequeue(KVal *q) {
    if(!q->list->len) return kv_void();
    KVal *v=q->list->items[0];
    memmove(q->list->items,q->list->items+1,(q->list->len-1)*sizeof(KVal*));
    q->list->len--; return v;
}
static inline KVal *k_dsqpeek(KVal *q) {
    return q->list->len ? q->list->items[0] : kv_void();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Set  (dsset / dsadd / dsrem / dshas / dsunion / dsinter / dsdiff)
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_dsset(KVal *lst) {
    /* Build set from list, removing duplicates */
    KVal *s=kv_list();
    for(size_t i=0;i<lst->list->len;i++){
        KVal *v=lst->list->items[i]; bool found=false;
        for(size_t j=0;j<s->list->len;j++){
            KVal *e=s->list->items[j];
            if(e->type==KT_INT&&v->type==KT_INT&&e->i==v->i){found=true;break;}
            if(e->type==KT_STR&&v->type==KT_STR&&strcmp(e->s,v->s)==0){found=true;break;}
            if(e->type==KT_FLT&&v->type==KT_FLT&&e->f==v->f){found=true;break;}
        }
        if(!found) klist_push(s->list,v);
    }
    return s;
}
static inline KVal *k_dsadd(KVal *s,KVal *v) {
    /* Only add if not already present */
    for(size_t i=0;i<s->list->len;i++){
        KVal *e=s->list->items[i];
        if(e->type==KT_INT&&v->type==KT_INT&&e->i==v->i) return kv_void();
        if(e->type==KT_STR&&v->type==KT_STR&&strcmp(e->s,v->s)==0) return kv_void();
    }
    klist_push(s->list,v); return kv_void();
}
static inline KVal *k_dsrem(KVal *s, KVal *v) {
    /* Remove from set — inline copy of Ldrop logic */
    for(size_t i=0;i<s->list->len;i++){
        KVal *e=s->list->items[i]; bool match=false;
        if(e->type==KT_INT&&v->type==KT_INT&&e->i==v->i) match=true;
        else if(e->type==KT_STR&&v->type==KT_STR&&strcmp(e->s,v->s)==0) match=true;
        else if(e->type==KT_FLT&&v->type==KT_FLT&&e->f==v->f) match=true;
        if(match){
            memmove(&s->list->items[i],&s->list->items[i+1],
                    (s->list->len-i-1)*sizeof(KVal*));
            s->list->len--; return kv_void();
        }
    }
    return kv_void();
}
static inline KVal *k_dshas(KVal *s, KVal *v) {
    for(size_t i=0;i<s->list->len;i++){
        KVal *e=s->list->items[i];
        if(e->type==KT_INT&&v->type==KT_INT&&e->i==v->i) return kv_bool(true);
        if(e->type==KT_STR&&v->type==KT_STR&&strcmp(e->s,v->s)==0) return kv_bool(true);
        if(e->type==KT_FLT&&v->type==KT_FLT&&e->f==v->f) return kv_bool(true);
    }
    return kv_bool(false);
}
static inline KVal *k_dsunion(KVal *a,KVal *b) {
    KVal *r=kv_list();
    for(size_t i=0;i<a->list->len;i++) klist_push(r->list,a->list->items[i]);
    for(size_t i=0;i<b->list->len;i++) k_dsadd(r,b->list->items[i]);
    return r;
}
static inline KVal *k_dsinter(KVal *a,KVal *b) {
    KVal *r=kv_list();
    for(size_t i=0;i<a->list->len;i++)
        if(k_dshas(b,a->list->items[i])->b) klist_push(r->list,a->list->items[i]);
    return r;
}
static inline KVal *k_dsdiff(KVal *a,KVal *b) {
    KVal *r=kv_list();
    for(size_t i=0;i<a->list->len;i++)
        if(!k_dshas(b,a->list->items[i])->b) klist_push(r->list,a->list->items[i]);
    return r;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Linked List  (dsll)
 * ══════════════════════════════════════════════════════════════════════════ */
typedef struct KLLNode { KVal *val; struct KLLNode *next; struct KLLNode *prev; } KLLNode;
typedef struct { KLLNode *head; KLLNode *tail; size_t len; } KLL;

static inline KVal *k_dsll(void) {
    KLL *ll=(KLL*)calloc(1,sizeof(KLL));
    KVal *r=kv_dict();
    kdict_set(r->dict,"_ll",kv_int((long long)(size_t)ll));
    kdict_set(r->dict,"len",kv_int(0));
    return r;
}
static KLL *_kll(KVal *v){ return (KLL*)(size_t)kdict_get(v->dict,"_ll")->i; }
static inline KVal *k_dsllpush(KVal *v, KVal *val) {
    KLL *ll=_kll(v);
    KLLNode *n=(KLLNode*)malloc(sizeof(KLLNode));
    n->val=val; n->next=NULL; n->prev=ll->tail;
    if(ll->tail) ll->tail->next=n; else ll->head=n;
    ll->tail=n; ll->len++;
    kdict_set(v->dict,"len",kv_int((long long)ll->len));
    return kv_void();
}
static inline KVal *k_dsllpop(KVal *v) {
    KLL *ll=_kll(v); if(!ll->tail) return kv_void();
    KLLNode *n=ll->tail; KVal *val=n->val;
    ll->tail=n->prev;
    if(ll->tail) ll->tail->next=NULL; else ll->head=NULL;
    free(n); ll->len--;
    kdict_set(v->dict,"len",kv_int((long long)ll->len));
    return val;
}
static inline KVal *k_dsllget(KVal *v, KVal *idx) {
    KLL *ll=_kll(v); size_t i=(size_t)idx->i;
    KLLNode *n=ll->head;
    while(n && i-->0) n=n->next;
    return n ? n->val : kv_void();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Tree  (dstree / dstadd / dstchildren / dstparent)
 *  Stored as adero of adero: {node→{parent,children:[...]}}
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_dstree(void)  { return kv_dict(); }
static inline KVal *k_dstadd(KVal *tr, KVal *node, KVal *parent) {
    KVal *entry=kv_dict();
    kdict_set(entry->dict,"parent",  parent);
    kdict_set(entry->dict,"children",kv_list());
    kdict_set(tr->dict, node->s, entry);
    /* register as child of parent */
    if(parent->type==KT_STR && strlen(parent->s)>0) {
        KVal *pe=kdict_get(tr->dict,parent->s);
        if(pe->type==KT_DICT){
            KVal *children=kdict_get(pe->dict,"children");
            klist_push(children->list, node);
        }
    }
    return kv_void();
}
static inline KVal *k_dstchildren(KVal *tr, KVal *node) {
    KVal *e=kdict_get(tr->dict,node->s);
    if(e->type!=KT_DICT) return kv_list();
    return kdict_get(e->dict,"children");
}
static inline KVal *k_dstparent(KVal *tr, KVal *node) {
    KVal *e=kdict_get(tr->dict,node->s);
    if(e->type!=KT_DICT) return kv_void();
    return kdict_get(e->dict,"parent");
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Graph  (dsgraph / dsgedge / dsgnbr / dsgpath)
 *  Adjacency list stored as adero of lists
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_dsgraph(void) { return kv_dict(); }
static inline KVal *k_dsgedge(KVal *g, KVal *from, KVal *to) {
    KVal *nbrs=kdict_get(g->dict,from->s);
    if(nbrs->type!=KT_LIST){ nbrs=kv_list(); kdict_set(g->dict,from->s,nbrs); }
    klist_push(nbrs->list,to);
    /* Ensure 'to' also has an entry */
    KVal *tnbrs=kdict_get(g->dict,to->s);
    if(tnbrs->type!=KT_LIST){ tnbrs=kv_list(); kdict_set(g->dict,to->s,tnbrs); }
    return kv_void();
}
static inline KVal *k_dsgnbr(KVal *g, KVal *node) {
    KVal *nbrs=kdict_get(g->dict,node->s);
    return nbrs->type==KT_LIST ? nbrs : kv_list();
}
static inline KVal *k_dsgpath(KVal *g, KVal *from, KVal *to) {
    /* BFS path check */
    KVal *visited=kv_list(); KVal *queue=kv_list();
    klist_push(queue->list, from);
    while(queue->list->len){
        KVal *cur=queue->list->items[0];
        memmove(queue->list->items,queue->list->items+1,(queue->list->len-1)*sizeof(KVal*));
        queue->list->len--;
        if(cur->type==KT_STR&&to->type==KT_STR&&strcmp(cur->s,to->s)==0)
            return kv_bool(true);
        /* Skip if visited */
        bool seen=false;
        for(size_t i=0;i<visited->list->len;i++){
            if(visited->list->items[i]->type==KT_STR&&strcmp(visited->list->items[i]->s,cur->s)==0){seen=true;break;}
        }
        if(seen) continue;
        klist_push(visited->list,cur);
        KVal *nbrs=k_dsgnbr(g,cur);
        for(size_t i=0;i<nbrs->list->len;i++) klist_push(queue->list,nbrs->list->items[i]);
    }
    return kv_bool(false);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Min/Max Heap  (dsheap / dshpush / dshpop)
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_dsheap(KVal *mode) {
    KVal *r=kv_dict();
    kdict_set(r->dict,"_data",kv_list());
    kdict_set(r->dict,"_min", kv_bool(strcmp(mode->s,"min")==0));
    return r;
}
static void _heap_up(KVal *lst, size_t i, bool is_min) {
    while(i>0){
        size_t p=(i-1)/2;
        double ci=_knum(lst->list->items[i]), cp=_knum(lst->list->items[p]);
        bool swap_cond = is_min ? ci<cp : ci>cp;
        if(!swap_cond) break;
        KVal *tmp=lst->list->items[i]; lst->list->items[i]=lst->list->items[p]; lst->list->items[p]=tmp;
        i=p;
    }
}
static void _heap_down(KVal *lst, size_t i, bool is_min) {
    size_t n=lst->list->len;
    while(1){
        size_t l=2*i+1,r=2*i+2,best=i;
        if(l<n){double bl=_knum(lst->list->items[l]),bb=_knum(lst->list->items[best]); if(is_min?bl<bb:bl>bb) best=l;}
        if(r<n){double br=_knum(lst->list->items[r]),bb=_knum(lst->list->items[best]); if(is_min?br<bb:br>bb) best=r;}
        if(best==i) break;
        KVal *tmp=lst->list->items[i]; lst->list->items[i]=lst->list->items[best]; lst->list->items[best]=tmp;
        i=best;
    }
}
static inline KVal *k_dshpush(KVal *h, KVal *v) {
    KVal *lst=kdict_get(h->dict,"_data");
    bool is_min=kdict_get(h->dict,"_min")->b;
    klist_push(lst->list,v);
    _heap_up(lst,lst->list->len-1,is_min);
    return kv_void();
}
static inline KVal *k_dshpop(KVal *h) {
    KVal *lst=kdict_get(h->dict,"_data");
    if(!lst->list->len) return kv_void();
    bool is_min=kdict_get(h->dict,"_min")->b;
    KVal *top=lst->list->items[0];
    lst->list->items[0]=lst->list->items[--lst->list->len];
    _heap_down(lst,0,is_min);
    return top;
}
static inline KVal *k_dshpeek(KVal *h) {
    KVal *lst=kdict_get(h->dict,"_data");
    return lst->list->len ? lst->list->items[0] : kv_void();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Deque  (dsdeque / dqdpushl / dqdpushr / dqdpopl / dqdpopr)
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_dsdeque(void) { return kv_list(); }
static inline KVal *k_dqdpushl(KVal *d, KVal *v) {
    klist_push(d->list,NULL);
    memmove(d->list->items+1,d->list->items,(d->list->len-1)*sizeof(KVal*));
    d->list->items[0]=v; return kv_void();
}
static inline KVal *k_dqdpushr(KVal *d, KVal *v) { klist_push(d->list,v); return kv_void(); }
static inline KVal *k_dqdpopl(KVal *d) {
    if(!d->list->len) return kv_void();
    KVal *v=d->list->items[0];
    memmove(d->list->items,d->list->items+1,(d->list->len-1)*sizeof(KVal*));
    d->list->len--; return v;
}
static inline KVal *k_dqdpopr(KVal *d) {
    if(!d->list->len) return kv_void();
    return d->list->items[--d->list->len];
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Matrix  (dsmat / dsmatset / dsmatget / dsmatmul / dsmattrans)
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_dsmat(KVal *rows, KVal *cols) {
    size_t r=(size_t)rows->i, c=(size_t)cols->i;
    KVal *m=kv_dict();
    kdict_set(m->dict,"rows",kv_int((long long)r));
    kdict_set(m->dict,"cols",kv_int((long long)c));
    KVal *data=kv_list();
    for(size_t i=0;i<r*c;i++) klist_push(data->list,kv_flt(0.0));
    kdict_set(m->dict,"_data",data);
    return m;
}
static inline KVal *k_dsmatset(KVal *m, KVal *row, KVal *col, KVal *val) {
    long long r=kdict_get(m->dict,"rows")->i, c=kdict_get(m->dict,"cols")->i;
    KVal *data=kdict_get(m->dict,"_data");
    long long idx=row->i*c+col->i;
    if(idx>=0 && idx<r*c) data->list->items[idx]=val;
    return kv_void();
}
static inline KVal *k_dsmatget(KVal *m, KVal *row, KVal *col) {
    long long c=kdict_get(m->dict,"cols")->i;
    KVal *data=kdict_get(m->dict,"_data");
    long long idx=row->i*c+col->i;
    if(idx<0||(size_t)idx>=data->list->len) return kv_flt(0.0);
    return data->list->items[idx];
}
static inline KVal *k_dsmattrans(KVal *m) {
    long long r=kdict_get(m->dict,"rows")->i, c=kdict_get(m->dict,"cols")->i;
    KVal *t=k_dsmat(kv_int(c),kv_int(r));
    for(long long i=0;i<r;i++) for(long long j=0;j<c;j++)
        k_dsmatset(t,kv_int(j),kv_int(i), k_dsmatget(m,kv_int(i),kv_int(j)));
    return t;
}
static inline KVal *k_dsmatmul(KVal *a, KVal *b) {
    long long ra=kdict_get(a->dict,"rows")->i, ca=kdict_get(a->dict,"cols")->i;
    long long rb=kdict_get(b->dict,"rows")->i, cb=kdict_get(b->dict,"cols")->i;
    if(ca!=rb) return kv_void();
    KVal *r=k_dsmat(kv_int(ra),kv_int(cb));
    for(long long i=0;i<ra;i++) for(long long j=0;j<cb;j++){
        double sum=0;
        for(long long k=0;k<ca;k++) sum+=_knum(k_dsmatget(a,kv_int(i),kv_int(k)))*_knum(k_dsmatget(b,kv_int(k),kv_int(j)));
        k_dsmatset(r,kv_int(i),kv_int(j),kv_flt(sum));
    }
    return r;
}
static inline KVal *k_dsmatprint(KVal *m) {
    long long r=kdict_get(m->dict,"rows")->i, c=kdict_get(m->dict,"cols")->i;
    for(long long i=0;i<r;i++){
        printf("[");
        for(long long j=0;j<c;j++){
            if(j) printf(", ");
            KVal *v=k_dsmatget(m,kv_int(i),kv_int(j));
            if(v->type==KT_INT) printf("%lld",v->i); else printf("%g",v->f);
        }
        printf("]\n");
    }
    return kv_void();
}

#endif /* K_DATASTRUCTS_H */
