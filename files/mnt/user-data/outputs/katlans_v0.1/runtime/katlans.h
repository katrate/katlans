/*
 * katlans.h  —  Katlans Runtime Library
 * Included in every compiled .kl program.
 */

#ifndef KATLANS_H
#define KATLANS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

/* ── Type tags ──────────────────────────────────────────────────────────── */
typedef enum {
    KT_INT  = 0,
    KT_FLT  = 1,
    KT_STR  = 2,
    KT_BOOL = 3,
    KT_VOID = 4,
    KT_LIST = 5,
    KT_DICT = 6,
} KType;

/* ── Forward declarations ────────────────────────────────────────────────── */
typedef struct KVal  KVal;
typedef struct KList KList;
typedef struct KDict KDict;

/* ── Dynamic list ────────────────────────────────────────────────────────── */
struct KList {
    KVal   **items;
    size_t   len;
    size_t   cap;
};

/* ── Dict entry ──────────────────────────────────────────────────────────── */
typedef struct {
    char *key;
    KVal *val;
} KDictEntry;

struct KDict {
    KDictEntry *entries;
    size_t      len;
    size_t      cap;
};

/* ── Universal value ─────────────────────────────────────────────────────── */
struct KVal {
    KType type;
    union {
        long long  i;
        double     f;
        char      *s;
        bool       b;
        KList     *list;
        KDict     *dict;
    };
};

/* ══════════════════════════════════════════════════════════════════════════
 *  Constructors
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal *kv_int(long long v) {
    KVal *k = (KVal*)malloc(sizeof(KVal));
    k->type = KT_INT; k->i = v; return k;
}
static inline KVal *kv_flt(double v) {
    KVal *k = (KVal*)malloc(sizeof(KVal));
    k->type = KT_FLT; k->f = v; return k;
}
static inline KVal *kv_str(const char *v) {
    KVal *k = (KVal*)malloc(sizeof(KVal));
    k->type = KT_STR; k->s = strdup(v); return k;
}
static inline KVal *kv_bool(bool v) {
    KVal *k = (KVal*)malloc(sizeof(KVal));
    k->type = KT_BOOL; k->b = v; return k;
}
static inline KVal *kv_void(void) {
    KVal *k = (KVal*)malloc(sizeof(KVal));
    k->type = KT_VOID; return k;
}

/* ── List constructors ───────────────────────────────────────────────────── */
static inline KList *klist_new(void) {
    KList *l = (KList*)malloc(sizeof(KList));
    l->cap = 8; l->len = 0;
    l->items = (KVal**)malloc(l->cap * sizeof(KVal*));
    return l;
}
static inline void klist_push(KList *l, KVal *v) {
    if (l->len >= l->cap) {
        l->cap *= 2;
        l->items = (KVal**)realloc(l->items, l->cap * sizeof(KVal*));
    }
    l->items[l->len++] = v;
}
static inline KVal *kv_list(void) {
    KVal *k = (KVal*)malloc(sizeof(KVal));
    k->type = KT_LIST; k->list = klist_new(); return k;
}

/* ── Dict constructors ───────────────────────────────────────────────────── */
static inline KDict *kdict_new(void) {
    KDict *d = (KDict*)malloc(sizeof(KDict));
    d->cap = 8; d->len = 0;
    d->entries = (KDictEntry*)malloc(d->cap * sizeof(KDictEntry));
    return d;
}
static inline void kdict_set(KDict *d, const char *key, KVal *val) {
    for (size_t i = 0; i < d->len; i++) {
        if (strcmp(d->entries[i].key, key) == 0) {
            d->entries[i].val = val; return;
        }
    }
    if (d->len >= d->cap) {
        d->cap *= 2;
        d->entries = (KDictEntry*)realloc(d->entries, d->cap * sizeof(KDictEntry));
    }
    d->entries[d->len].key = strdup(key);
    d->entries[d->len].val = val;
    d->len++;
}
static inline KVal *kdict_get(KDict *d, const char *key) {
    for (size_t i = 0; i < d->len; i++)
        if (strcmp(d->entries[i].key, key) == 0)
            return d->entries[i].val;
    return kv_void();
}
static inline KVal *kv_dict(void) {
    KVal *k = (KVal*)malloc(sizeof(KVal));
    k->type = KT_DICT; k->dict = kdict_new(); return k;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Display  (Katlans display x ::)
 * ══════════════════════════════════════════════════════════════════════════ */

static inline void k_display(KVal *v) {
    if (!v) { printf("void\n"); return; }
    switch (v->type) {
        case KT_INT:  printf("%lld\n",  v->i); break;
        case KT_FLT:  printf("%g\n",    v->f); break;
        case KT_STR:  printf("%s\n",    v->s); break;
        case KT_BOOL: printf("%s\n",    v->b ? "true" : "false"); break;
        case KT_VOID: printf("void\n"); break;
        case KT_LIST: {
            printf("[");
            for (size_t i = 0; i < v->list->len; i++) {
                if (i) printf(", ");
                KVal *item = v->list->items[i];
                switch (item->type) {
                    case KT_INT:  printf("%lld",  item->i); break;
                    case KT_FLT:  printf("%g",    item->f); break;
                    case KT_STR:  printf("\"%s\"",item->s); break;
                    case KT_BOOL: printf("%s",    item->b ? "true" : "false"); break;
                    case KT_VOID: printf("void"); break;
                    case KT_LIST: {
                        printf("[");
                        for(size_t j=0;j<item->list->len;j++){
                            if(j) printf(", ");
                            KVal *ii=item->list->items[j];
                            if(ii->type==KT_INT) printf("%lld",ii->i);
                            else if(ii->type==KT_STR) printf("\"%s\"",ii->s);
                            else if(ii->type==KT_FLT) printf("%g",ii->f);
                            else if(ii->type==KT_BOOL) printf("%s",ii->b?"true":"false");
                            else printf("?");
                        }
                        printf("]"); break;
                    }
                    default: printf("?"); break;
                }
            }
            printf("]\n");
            break;
        }
        case KT_DICT: {
            /* Check if it's a datetime dict — show _str field */
            int has_ts=0;
            for(size_t i=0;i<v->dict->len;i++)
                if(strcmp(v->dict->entries[i].key,"_str")==0){has_ts=1;break;}
            if(has_ts){
                for(size_t i=0;i<v->dict->len;i++)
                    if(strcmp(v->dict->entries[i].key,"_str")==0){
                        printf("%s\n",v->dict->entries[i].val->s); return;
                    }
            }
            printf("{");
            int first=1;
            for (size_t i = 0; i < v->dict->len; i++) {
                /* Skip internal keys */
                if(v->dict->entries[i].key[0]=='_') continue;
                if(!first) printf(", ");
                first=0;
                printf("%s: ", v->dict->entries[i].key);
                KVal *dv = v->dict->entries[i].val;
                switch (dv->type) {
                    case KT_INT:  printf("%lld",  dv->i); break;
                    case KT_FLT:  printf("%g",    dv->f); break;
                    case KT_STR:  printf("\"%s\"",dv->s); break;
                    case KT_BOOL: printf("%s",    dv->b ? "true" : "false"); break;
                    case KT_VOID: printf("void");  break;
                    case KT_LIST: printf("[...]");  break;
                    case KT_DICT: printf("{...}");  break;
                }
            }
            printf("}\n");
            break;
        }
    }
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Arithmetic helpers
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal *k_add(KVal *a, KVal *b) {
    if (a->type == KT_INT && b->type == KT_INT) return kv_int(a->i + b->i);
    if (a->type == KT_STR && b->type == KT_STR) {
        size_t n = strlen(a->s) + strlen(b->s) + 1;
        char *buf = (char*)malloc(n);
        strcpy(buf, a->s); strcat(buf, b->s);
        KVal *r = kv_str(buf); free(buf); return r;
    }
    double fa = (a->type == KT_INT) ? (double)a->i : a->f;
    double fb = (b->type == KT_INT) ? (double)b->i : b->f;
    return kv_flt(fa + fb);
}
static inline KVal *k_sub(KVal *a, KVal *b) {
    if (a->type == KT_INT && b->type == KT_INT) return kv_int(a->i - b->i);
    double fa = (a->type==KT_INT)?(double)a->i:a->f;
    double fb = (b->type==KT_INT)?(double)b->i:b->f;
    return kv_flt(fa - fb);
}
static inline KVal *k_mul(KVal *a, KVal *b) {
    if (a->type == KT_INT && b->type == KT_INT) return kv_int(a->i * b->i);
    double fa = (a->type==KT_INT)?(double)a->i:a->f;
    double fb = (b->type==KT_INT)?(double)b->i:b->f;
    return kv_flt(fa * fb);
}
static inline KVal *k_div(KVal *a, KVal *b) {
    double fa = (a->type==KT_INT)?(double)a->i:a->f;
    double fb = (b->type==KT_INT)?(double)b->i:b->f;
    return kv_flt(fa / fb);
}
static inline KVal *k_idiv(KVal *a, KVal *b) {
    return kv_int(a->i / b->i);
}
static inline KVal *k_mod(KVal *a, KVal *b) {
    return kv_int(a->i % b->i);
}
static inline KVal *k_pow(KVal *a, KVal *b) {
    double fa = (a->type==KT_INT)?(double)a->i:a->f;
    double fb = (b->type==KT_INT)?(double)b->i:b->f;
    double r  = pow(fa, fb);
    if (a->type==KT_INT && b->type==KT_INT && fb>=0) return kv_int((long long)r);
    return kv_flt(r);
}
static inline KVal *k_pct(KVal *a, KVal *b) {
    double fa = (a->type==KT_INT)?(double)a->i:a->f;
    double fb = (b->type==KT_INT)?(double)b->i:b->f;
    double r  = (fa / 100.0) * fb;
    return kv_flt(r);
}
static inline KVal *k_concat(KVal *a, KVal *b) {
    /* ++ string concat */
    char *sa = (a->type==KT_STR) ? a->s : "";
    char *sb = (b->type==KT_STR) ? b->s : "";
    size_t n = strlen(sa)+strlen(sb)+1;
    char *buf = (char*)malloc(n);
    strcpy(buf,sa); strcat(buf,sb);
    KVal *r = kv_str(buf); free(buf); return r;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Comparison helpers  (return KVal bool)
 * ══════════════════════════════════════════════════════════════════════════ */

static inline double _knum(KVal *v) {
    return (v->type==KT_INT)?(double)v->i:v->f;
}
static inline KVal *k_eq(KVal *a, KVal *b) {
    if (a->type==KT_STR && b->type==KT_STR) return kv_bool(strcmp(a->s,b->s)==0);
    if (a->type==KT_BOOL && b->type==KT_BOOL) return kv_bool(a->b==b->b);
    return kv_bool(_knum(a)==_knum(b));
}
static inline KVal *k_neq(KVal *a, KVal *b) { KVal *r=k_eq(a,b); r->b=!r->b; return r; }
static inline KVal *k_lt(KVal *a, KVal *b)  { return kv_bool(_knum(a)<_knum(b));  }
static inline KVal *k_gt(KVal *a, KVal *b)  { return kv_bool(_knum(a)>_knum(b));  }
static inline KVal *k_lte(KVal *a, KVal *b) { return kv_bool(_knum(a)<=_knum(b)); }
static inline KVal *k_gte(KVal *a, KVal *b) { return kv_bool(_knum(a)>=_knum(b)); }

/* ══════════════════════════════════════════════════════════════════════════
 *  Logical helpers
 * ══════════════════════════════════════════════════════════════════════════ */

static inline bool k_truthy(KVal *v) {
    if (!v) return false;
    switch(v->type) {
        case KT_BOOL: return v->b;
        case KT_INT:  return v->i != 0;
        case KT_FLT:  return v->f != 0.0;
        case KT_STR:  return v->s && v->s[0] != '\0';
        case KT_VOID: return false;
        default:      return true;
    }
}
static inline KVal *k_and(KVal *a, KVal *b) { return kv_bool(k_truthy(a) && k_truthy(b)); }
static inline KVal *k_or(KVal *a, KVal *b)  { return kv_bool(k_truthy(a) || k_truthy(b)); }
static inline KVal *k_not(KVal *a)           { return kv_bool(!k_truthy(a)); }

/* ══════════════════════════════════════════════════════════════════════════
 *  Built-in math functions
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal *k_sqrt(KVal *a)   { return kv_flt(sqrt(_knum(a)));  }
static inline KVal *k_abs(KVal *a)    {
    if (a->type==KT_INT) return kv_int(llabs(a->i));
    return kv_flt(fabs(a->f));
}
static inline KVal *k_floor(KVal *a)  { return kv_int((long long)floor(_knum(a))); }
static inline KVal *k_ceil(KVal *a)   { return kv_int((long long)ceil(_knum(a)));  }
static inline KVal *k_round(KVal *a, KVal *digits) {
    int d = (int)digits->i;
    double mul = pow(10, d);
    return kv_flt(round(_knum(a)*mul)/mul);
}
static inline KVal *k_log(KVal *a)   { return kv_flt(log10(_knum(a))); }
static inline KVal *k_ln(KVal *a)    { return kv_flt(log(_knum(a)));   }
static inline KVal *k_exp(KVal *a)   { return kv_flt(exp(_knum(a)));   }
static inline KVal *k_sin(KVal *a)   { return kv_flt(sin(_knum(a)));   }
static inline KVal *k_cos(KVal *a)   { return kv_flt(cos(_knum(a)));   }
static inline KVal *k_tan(KVal *a)   { return kv_flt(tan(_knum(a)));   }
static inline KVal *k_asin(KVal *a)  { return kv_flt(asin(_knum(a)));  }
static inline KVal *k_acos(KVal *a)  { return kv_flt(acos(_knum(a)));  }
static inline KVal *k_atan(KVal *a)  { return kv_flt(atan(_knum(a)));  }
static inline KVal *k_min2(KVal *a, KVal *b) { return _knum(a)<=_knum(b)?a:b; }
static inline KVal *k_max2(KVal *a, KVal *b) { return _knum(a)>=_knum(b)?a:b; }

static inline KVal *k_sum(KVal *lst) {
    if (lst->type != KT_LIST) return kv_int(0);
    double s = 0; bool is_int = true;
    for (size_t i = 0; i < lst->list->len; i++) {
        KVal *v = lst->list->items[i];
        if (v->type == KT_FLT) is_int = false;
        s += _knum(v);
    }
    return is_int ? kv_int((long long)s) : kv_flt(s);
}
static inline KVal *k_len(KVal *v) {
    if (v->type == KT_STR)  return kv_int((long long)strlen(v->s));
    if (v->type == KT_LIST) return kv_int((long long)v->list->len);
    if (v->type == KT_DICT) return kv_int((long long)v->dict->len);
    return kv_int(0);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  String built-ins (S prefix)
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal *k_Scap(KVal *v) {
    char *r = strdup(v->s);
    for (int i = 0; r[i]; i++) r[i] = toupper((unsigned char)r[i]);
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Slow(KVal *v) {
    char *r = strdup(v->s);
    for (int i = 0; r[i]; i++) r[i] = tolower((unsigned char)r[i]);
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Slen(KVal *v)  { return kv_int((long long)strlen(v->s)); }
static inline KVal *k_Snip(KVal *v) {
    char *s = v->s;
    while (*s == ' ' || *s == '\t' || *s == '\n') s++;
    char *e = s + strlen(s) - 1;
    while (e > s && (*e==' '||*e=='\t'||*e=='\n')) e--;
    char *r = (char*)malloc(e-s+2);
    strncpy(r, s, e-s+1); r[e-s+1]='\0';
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Sfind(KVal *v, KVal *sub) {
    char *p = strstr(v->s, sub->s);
    return p ? kv_int((long long)(p - v->s)) : kv_int(-1);
}
static inline KVal *k_Scount(KVal *v, KVal *sub) {
    int count = 0; char *p = v->s; size_t n = strlen(sub->s);
    while ((p = strstr(p, sub->s))) { count++; p += n; }
    return kv_int(count);
}
static inline KVal *k_Speeks(KVal *v, KVal *sub) {
    return kv_bool(strstr(v->s, sub->s) != NULL);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  List built-ins (L prefix)
 * ══════════════════════════════════════════════════════════════════════════ */

static inline void k_Ladd(KVal *lst, KVal *v) {
    klist_push(lst->list, v);
}
static inline KVal *k_Llen(KVal *lst) {
    return kv_int((long long)lst->list->len);
}
static inline KVal *k_Lget(KVal *lst, KVal *idx) {
    long long i = idx->i;
    if (i < 0) i = (long long)lst->list->len + i;
    if (i < 0 || i >= (long long)lst->list->len) return kv_void();
    return lst->list->items[i];
}
static inline KVal *k_Lmax(KVal *lst) {
    if (!lst->list->len) return kv_void();
    KVal *m = lst->list->items[0];
    for (size_t i=1;i<lst->list->len;i++)
        if (_knum(lst->list->items[i])>_knum(m)) m=lst->list->items[i];
    return m;
}
static inline KVal *k_Lmin(KVal *lst) {
    if (!lst->list->len) return kv_void();
    KVal *m = lst->list->items[0];
    for (size_t i=1;i<lst->list->len;i++)
        if (_knum(lst->list->items[i])<_knum(m)) m=lst->list->items[i];
    return m;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  I/O helpers
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal *k_ask(const char *prompt) {
    printf("%s", prompt);
    fflush(stdout);
    char buf[4096];
    if (!fgets(buf, sizeof(buf), stdin)) return kv_str("");
    size_t n = strlen(buf);
    if (n && buf[n-1]=='\n') buf[n-1]='\0';
    return kv_str(buf);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Type casting
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal *k_I(KVal *v) {
    if (v->type==KT_INT)  return v;
    if (v->type==KT_FLT)  return kv_int((long long)v->f);
    if (v->type==KT_STR)  return kv_int(atoll(v->s));
    if (v->type==KT_BOOL) return kv_int(v->b?1:0);
    return kv_int(0);
}
static inline KVal *k_F(KVal *v) {
    if (v->type==KT_FLT) return v;
    if (v->type==KT_INT) return kv_flt((double)v->i);
    if (v->type==KT_STR) return kv_flt(atof(v->s));
    return kv_flt(0.0);
}
static inline KVal *k_S(KVal *v) {
    char buf[128];
    switch(v->type) {
        case KT_INT:  snprintf(buf,sizeof(buf),"%lld",v->i); return kv_str(buf);
        case KT_FLT:  snprintf(buf,sizeof(buf),"%g",  v->f); return kv_str(buf);
        case KT_BOOL: return kv_str(v->b?"true":"false");
        case KT_STR:  return v;
        default:      return kv_str("void");
    }
}

/* Constants */
#define K_PI  kv_flt(3.14159265358979323846)
#define K_E   kv_flt(2.71828182845904523536)

/* ══════════════════════════════════════════════════════════════════════════
 *  Phase 2 — Complete String Methods (S prefix)
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal *k_Stitle(KVal *v) {
    char *r = strdup(v->s); int prev_space = 1;
    for (int i = 0; r[i]; i++) {
        if (r[i]==' '||r[i]=='\t'||r[i]=='\n') { prev_space=1; }
        else if (prev_space) { r[i]=toupper((unsigned char)r[i]); prev_space=0; }
        else { r[i]=tolower((unsigned char)r[i]); }
    }
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Scapfirst(KVal *v) {
    if (!v->s || !v->s[0]) return kv_str("");
    char *r = strdup(v->s);
    r[0] = toupper((unsigned char)r[0]);
    for (int i=1; r[i]; i++) r[i]=tolower((unsigned char)r[i]);
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Sswapcase(KVal *v) {
    char *r = strdup(v->s);
    for (int i=0; r[i]; i++)
        r[i] = isupper((unsigned char)r[i]) ?
               tolower((unsigned char)r[i]) : toupper((unsigned char)r[i]);
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Scasefold(KVal *v) { return k_Slow(v); }

static inline KVal *k_Srfind(KVal *v, KVal *sub) {
    char *p = v->s; char *last = NULL; size_t n = strlen(sub->s);
    while ((p = strstr(p, sub->s))) { last = p; p += n; }
    return last ? kv_int((long long)(last - v->s)) : kv_int(-1);
}
static inline KVal *k_Sindex(KVal *v, KVal *sub) {
    char *p = strstr(v->s, sub->s);
    if (!p) return kv_int(-1);
    return kv_int((long long)(p - v->s));
}
static inline KVal *k_Srindex(KVal *v, KVal *sub) { return k_Srfind(v, sub); }

static inline KVal *k_Sswap(KVal *v, KVal *old, KVal *nw) {
    /* replace ALL occurrences of old with nw */
    size_t olen = strlen(old->s), nlen = strlen(nw->s);
    size_t slen = strlen(v->s);
    /* count occurrences */
    int cnt = 0; char *p = v->s;
    if (olen == 0) return kv_str(v->s);
    while ((p = strstr(p, old->s))) { cnt++; p += olen; }
    size_t rlen = slen + cnt * ((long)nlen - (long)olen);
    char *r = (char*)malloc(rlen + 1); char *w = r; p = v->s;
    while (*p) {
        if (olen && strncmp(p, old->s, olen)==0) {
            memcpy(w, nw->s, nlen); w += nlen; p += olen;
        } else { *w++ = *p++; }
    }
    *w = '\0';
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Snipleft(KVal *v) {
    char *s = v->s;
    while (*s==' '||*s=='\t'||*s=='\n') s++;
    return kv_str(s);
}
static inline KVal *k_Snipright(KVal *v) {
    char *r = strdup(v->s); int n = (int)strlen(r)-1;
    while (n>=0 && (r[n]==' '||r[n]=='\t'||r[n]=='\n')) r[n--]='\0';
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Spad(KVal *v, KVal *width) {
    int w = (int)width->i; int len = (int)strlen(v->s);
    if (len >= w) return kv_str(v->s);
    int lpad = (w - len) / 2; int rpad = w - len - lpad;
    char *r = (char*)malloc(w+1);
    memset(r, ' ', w); memcpy(r+lpad, v->s, len); r[w]='\0';
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Spadleft(KVal *v, KVal *width, KVal *ch) {
    int w = (int)width->i; int len = (int)strlen(v->s);
    if (len >= w) return kv_str(v->s);
    char c = ch->s[0]; int pad = w - len;
    char *r = (char*)malloc(w+1);
    memset(r, c, pad); memcpy(r+pad, v->s, len); r[w]='\0';
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Spadright(KVal *v, KVal *width, KVal *ch) {
    int w = (int)width->i; int len = (int)strlen(v->s);
    if (len >= w) return kv_str(v->s);
    char c = ch->s[0]; int pad = w - len;
    char *r = (char*)malloc(w+1);
    memcpy(r, v->s, len); memset(r+len, c, pad); r[w]='\0';
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Szfill(KVal *v, KVal *width) {
    return k_Spadleft(v, width, kv_str("0"));
}
static inline KVal *k_Schop(KVal *v, KVal *sep) {
    KVal *lst = kv_list();
    if (strlen(sep->s)==0) {
        for (int i=0; v->s[i]; i++) {
            char buf[2]={v->s[i],'\0'}; klist_push(lst->list, kv_str(buf));
        }
        return lst;
    }
    char *s = strdup(v->s); char *tok = strtok(s, sep->s);
    while (tok) { klist_push(lst->list, kv_str(tok)); tok = strtok(NULL, sep->s); }
    free(s); return lst;
}
static inline KVal *k_Schoplines(KVal *v) {
    KVal *lst = kv_list(); char *s = strdup(v->s); char *p = s;
    while (*p) {
        char *nl = strchr(p, '\n');
        if (!nl) { klist_push(lst->list, kv_str(p)); break; }
        *nl = '\0'; klist_push(lst->list, kv_str(p)); p = nl+1;
    }
    free(s); return lst;
}
static inline KVal *k_Sjoin(KVal *sep, KVal *lst) {
    if (!lst->list->len) return kv_str("");
    size_t total = 0; size_t slen = strlen(sep->s);
    for (size_t i=0; i<lst->list->len; i++) total += strlen(lst->list->items[i]->s);
    total += slen * (lst->list->len - 1);
    char *r = (char*)malloc(total+1); r[0]='\0';
    for (size_t i=0; i<lst->list->len; i++) {
        if (i) strcat(r, sep->s);
        strcat(r, lst->list->items[i]->s);
    }
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Spartition(KVal *v, KVal *sep) {
    KVal *lst = kv_list(); char *p = strstr(v->s, sep->s);
    if (!p) {
        klist_push(lst->list, kv_str(v->s));
        klist_push(lst->list, kv_str(""));
        klist_push(lst->list, kv_str(""));
    } else {
        size_t before = p - v->s;
        char *b = (char*)malloc(before+1); strncpy(b,v->s,before); b[before]='\0';
        klist_push(lst->list, kv_str(b)); free(b);
        klist_push(lst->list, kv_str(sep->s));
        klist_push(lst->list, kv_str(p + strlen(sep->s)));
    }
    return lst;
}
static inline KVal *k_Sremprefix(KVal *v, KVal *pre) {
    size_t n = strlen(pre->s);
    if (strncmp(v->s, pre->s, n)==0) return kv_str(v->s + n);
    return kv_str(v->s);
}
static inline KVal *k_Sremsuffix(KVal *v, KVal *suf) {
    size_t vlen = strlen(v->s), slen = strlen(suf->s);
    if (vlen >= slen && strcmp(v->s + vlen - slen, suf->s)==0) {
        char *r = (char*)malloc(vlen-slen+1);
        strncpy(r, v->s, vlen-slen); r[vlen-slen]='\0';
        KVal *rv = kv_str(r); free(r); return rv;
    }
    return kv_str(v->s);
}

/* String slice: x[start;end] */
static inline KVal *k_Sslice(KVal *v, KVal *start, KVal *end) {
    int len = (int)strlen(v->s);
    int s = (int)start->i; if (s < 0) s = len + s; if (s < 0) s = 0;
    int e = (int)end->i;   if (e < 0) e = len + e; if (e > len) e = len;
    if (s >= e) return kv_str("");
    char *r = (char*)malloc(e-s+1); strncpy(r, v->s+s, e-s); r[e-s]='\0';
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Schar(KVal *v, KVal *idx) {
    int i = (int)idx->i; int len = (int)strlen(v->s);
    if (i < 0) i = len + i;
    if (i < 0 || i >= len) return kv_str("");
    char buf[2] = {v->s[i], '\0'}; return kv_str(buf);
}

/* Boolean checks */
static inline KVal *k_Sisalpha(KVal *v)   { for(int i=0;v->s[i];i++) if(!isalpha((unsigned char)v->s[i])) return kv_bool(false); return kv_bool(v->s[0]!='\0'); }
static inline KVal *k_Sisdigit(KVal *v)   { for(int i=0;v->s[i];i++) if(!isdigit((unsigned char)v->s[i])) return kv_bool(false); return kv_bool(v->s[0]!='\0'); }
static inline KVal *k_Sisalnum(KVal *v)   { for(int i=0;v->s[i];i++) if(!isalnum((unsigned char)v->s[i])) return kv_bool(false); return kv_bool(v->s[0]!='\0'); }
static inline KVal *k_Sisspace(KVal *v)   { for(int i=0;v->s[i];i++) if(!isspace((unsigned char)v->s[i])) return kv_bool(false); return kv_bool(v->s[0]!='\0'); }
static inline KVal *k_Sislower(KVal *v)   { int has=0; for(int i=0;v->s[i];i++){if(isupper((unsigned char)v->s[i]))return kv_bool(false); if(islower((unsigned char)v->s[i]))has=1;} return kv_bool(has); }
static inline KVal *k_Sisupper(KVal *v)   { int has=0; for(int i=0;v->s[i];i++){if(islower((unsigned char)v->s[i]))return kv_bool(false); if(isupper((unsigned char)v->s[i]))has=1;} return kv_bool(has); }
static inline KVal *k_Sistitle(KVal *v)   { KVal *t=k_Stitle(v); bool eq=strcmp(t->s,v->s)==0; return kv_bool(eq); }
static inline KVal *k_Sisascii(KVal *v)   { for(int i=0;v->s[i];i++) if((unsigned char)v->s[i]>127) return kv_bool(false); return kv_bool(true); }
static inline KVal *k_Sisprintable(KVal *v){ for(int i=0;v->s[i];i++) if(!isprint((unsigned char)v->s[i])) return kv_bool(false); return kv_bool(true); }
static inline KVal *k_Sisnumeric(KVal *v)  { return k_Sisdigit(v); }
static inline KVal *k_Sisdecimal(KVal *v)  { return k_Sisdigit(v); }
static inline KVal *k_Sfront(KVal *v, KVal *pre) { return kv_bool(strncmp(v->s, pre->s, strlen(pre->s))==0); }
static inline KVal *k_Sback(KVal *v, KVal *suf) {
    size_t vl=strlen(v->s), sl=strlen(suf->s);
    return kv_bool(vl>=sl && strcmp(v->s+vl-sl, suf->s)==0);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Phase 2 — Complete List Methods (L prefix)
 * ══════════════════════════════════════════════════════════════════════════ */

static inline void   k_Laddall(KVal *lst, KVal *other) {
    for (size_t i=0;i<other->list->len;i++) klist_push(lst->list, other->list->items[i]);
}
static inline void   k_Linsert(KVal *lst, KVal *idx, KVal *val) {
    size_t i = (size_t)idx->i;
    if (i > lst->list->len) i = lst->list->len;
    klist_push(lst->list, NULL);  /* grow */
    memmove(&lst->list->items[i+1], &lst->list->items[i],
            (lst->list->len-1-i)*sizeof(KVal*));
    lst->list->items[i] = val;
}
static inline void   k_Ldrop(KVal *lst, KVal *val) {
    for (size_t i=0;i<lst->list->len;i++) {
        KVal *item = lst->list->items[i];
        bool match = false;
        if (item->type==KT_INT && val->type==KT_INT) match=(item->i==val->i);
        else if (item->type==KT_STR && val->type==KT_STR) match=strcmp(item->s,val->s)==0;
        else if (item->type==KT_FLT && val->type==KT_FLT) match=(item->f==val->f);
        if (match) {
            memmove(&lst->list->items[i], &lst->list->items[i+1],
                    (lst->list->len-i-1)*sizeof(KVal*));
            lst->list->len--; return;
        }
    }
}
static inline KVal  *k_Lpop(KVal *lst, KVal *idx) {
    if (!lst->list->len) return kv_void();
    long long i = idx ? idx->i : (long long)lst->list->len-1;
    if (i<0) i=(long long)lst->list->len+i;
    if (i<0||i>=(long long)lst->list->len) return kv_void();
    KVal *v = lst->list->items[i];
    memmove(&lst->list->items[i], &lst->list->items[i+1],
            (lst->list->len-i-1)*sizeof(KVal*));
    lst->list->len--;
    return v;
}
static inline void   k_Lclear(KVal *lst) { lst->list->len = 0; }
static inline KVal  *k_Lindex(KVal *lst, KVal *val) {
    for (size_t i=0;i<lst->list->len;i++) {
        KVal *item = lst->list->items[i];
        if (item->type==KT_INT&&val->type==KT_INT&&item->i==val->i) return kv_int(i);
        if (item->type==KT_STR&&val->type==KT_STR&&strcmp(item->s,val->s)==0) return kv_int(i);
        if (item->type==KT_FLT&&val->type==KT_FLT&&item->f==val->f) return kv_int(i);
    }
    return kv_int(-1);
}
static inline KVal  *k_Lcount(KVal *lst, KVal *val) {
    int cnt=0;
    for (size_t i=0;i<lst->list->len;i++) {
        KVal *item=lst->list->items[i];
        if (item->type==KT_INT&&val->type==KT_INT&&item->i==val->i) cnt++;
        else if (item->type==KT_STR&&val->type==KT_STR&&strcmp(item->s,val->s)==0) cnt++;
        else if (item->type==KT_FLT&&val->type==KT_FLT&&item->f==val->f) cnt++;
    }
    return kv_int(cnt);
}
static inline KVal  *k_Lhas(KVal *lst, KVal *val) {
    return kv_bool(k_Lindex(lst,val)->i >= 0);
}
static inline int _kcmp(const void *a, const void *b) {
    KVal *va=*(KVal**)a, *vb=*(KVal**)b;
    if(va->type==KT_INT&&vb->type==KT_INT) return (va->i>vb->i)-(va->i<vb->i);
    if(va->type==KT_STR&&vb->type==KT_STR) return strcmp(va->s,vb->s);
    double fa=_knum(va),fb=_knum(vb); return (fa>fb)-(fa<fb);
}
static inline int _kcmp_desc(const void *a, const void *b) { return _kcmp(b,a); }
static inline void   k_Lsort(KVal *lst, KVal *dir) {
    bool desc = dir && dir->type==KT_STR && strcmp(dir->s,"desc")==0;
    qsort(lst->list->items, lst->list->len, sizeof(KVal*),
          desc ? _kcmp_desc : _kcmp);
}
static inline void   k_Lflip(KVal *lst) {
    size_t n=lst->list->len;
    for(size_t i=0;i<n/2;i++) {
        KVal *t=lst->list->items[i];
        lst->list->items[i]=lst->list->items[n-1-i];
        lst->list->items[n-1-i]=t;
    }
}
static inline KVal  *k_Lcopy(KVal *lst) {
    KVal *r=kv_list();
    for(size_t i=0;i<lst->list->len;i++) klist_push(r->list,lst->list->items[i]);
    return r;
}
static inline KVal  *k_Lslice(KVal *lst, KVal *start, KVal *end) {
    KVal *r=kv_list();
    long long s=(long long)start->i, e=(long long)end->i;
    long long n=(long long)lst->list->len;
    if(s<0)s=n+s; if(e<0)e=n+e;
    if(s<0)s=0; if(e>n)e=n;
    for(long long i=s;i<e;i++) klist_push(r->list,lst->list->items[i]);
    return r;
}
static inline KVal  *k_Ljoin(KVal *sep, KVal *lst) {
    /* join list of strings */
    return k_Sjoin(sep, lst);
}
static inline KVal  *k_Lsum(KVal *lst)   { return k_sum(lst); }
static inline KVal  *k_Lunique(KVal *lst) {
    KVal *r=kv_list();
    for(size_t i=0;i<lst->list->len;i++) {
        if(k_Lindex(r,lst->list->items[i])->i < 0)
            klist_push(r->list,lst->list->items[i]);
    }
    return r;
}
static inline KVal  *k_Lconcat(KVal *a, KVal *b) {
    KVal *r=kv_list();
    for(size_t i=0;i<a->list->len;i++) klist_push(r->list,a->list->items[i]);
    for(size_t i=0;i<b->list->len;i++) klist_push(r->list,b->list->items[i]);
    return r;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Phase 2 — Complete Dict/Adero Methods (A prefix)
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal  *k_Akeys(KVal *d) {
    KVal *r=kv_list();
    for(size_t i=0;i<d->dict->len;i++)
        klist_push(r->list,kv_str(d->dict->entries[i].key));
    return r;
}
static inline KVal  *k_Avals(KVal *d) {
    KVal *r=kv_list();
    for(size_t i=0;i<d->dict->len;i++)
        klist_push(r->list,d->dict->entries[i].val);
    return r;
}
static inline KVal  *k_Aitems(KVal *d) {
    KVal *r=kv_list();
    for(size_t i=0;i<d->dict->len;i++) {
        KVal *pair=kv_list();
        klist_push(pair->list, kv_str(d->dict->entries[i].key));
        klist_push(pair->list, d->dict->entries[i].val);
        klist_push(r->list, pair);
    }
    return r;
}
static inline KVal  *k_Aget(KVal *d, KVal *key, KVal *def) {
    for(size_t i=0;i<d->dict->len;i++)
        if(strcmp(d->dict->entries[i].key,key->s)==0)
            return d->dict->entries[i].val;
    return def ? def : kv_void();
}
static inline void   k_Aset(KVal *d, KVal *key, KVal *val) {
    kdict_set(d->dict, key->s, val);
}
static inline KVal  *k_Adrop(KVal *d, KVal *key) {
    for(size_t i=0;i<d->dict->len;i++) {
        if(strcmp(d->dict->entries[i].key, key->s)==0) {
            KVal *v = d->dict->entries[i].val;
            free(d->dict->entries[i].key);
            memmove(&d->dict->entries[i], &d->dict->entries[i+1],
                    (d->dict->len-i-1)*sizeof(KDictEntry));
            d->dict->len--;
            return v;
        }
    }
    return kv_void();
}
static inline KVal  *k_Apop(KVal *d, KVal *key) { return k_Adrop(d,key); }
static inline void   k_Aupdate(KVal *d, KVal *other) {
    for(size_t i=0;i<other->dict->len;i++)
        kdict_set(d->dict, other->dict->entries[i].key, other->dict->entries[i].val);
}
static inline void   k_Aclear(KVal *d) { d->dict->len = 0; }
static inline KVal  *k_Acopy(KVal *d) {
    KVal *r=kv_dict();
    for(size_t i=0;i<d->dict->len;i++)
        kdict_set(r->dict, d->dict->entries[i].key, d->dict->entries[i].val);
    return r;
}
static inline KVal  *k_Alen(KVal *d)  { return kv_int((long long)d->dict->len); }
static inline KVal  *k_Ahas(KVal *d, KVal *key) {
    for(size_t i=0;i<d->dict->len;i++)
        if(strcmp(d->dict->entries[i].key,key->s)==0) return kv_bool(true);
    return kv_bool(false);
}
static inline KVal  *k_Amerge(KVal *a, KVal *b) {
    KVal *r=k_Acopy(a); k_Aupdate(r,b); return r;
}
static inline KVal  *k_Afromkeys(KVal *lst, KVal *val) {
    KVal *r=kv_dict();
    for(size_t i=0;i<lst->list->len;i++)
        kdict_set(r->dict, lst->list->items[i]->s, val);
    return r;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Phase 2 — Tuple/Zelo Methods (Z prefix)
 * ══════════════════════════════════════════════════════════════════════════ */

/* Tuples stored as KT_LIST — same accessors, just semantically immutable */
static inline KVal  *k_Zcount(KVal *zl, KVal *val) { return k_Lcount(zl,val); }
static inline KVal  *k_Zindex(KVal *zl, KVal *val) { return k_Lindex(zl,val); }
static inline KVal  *k_Zlen(KVal *zl)              { return k_Llen(zl);       }
static inline KVal  *k_Zhas(KVal *zl, KVal *val)   { return k_Lhas(zl,val);   }
static inline KVal  *k_Zmax(KVal *zl)              { return k_Lmax(zl);       }
static inline KVal  *k_Zmin(KVal *zl)              { return k_Lmin(zl);       }
static inline KVal  *k_Zsum(KVal *zl)              { return k_Lsum(zl);       }
static inline KVal  *k_Ztolt(KVal *zl)             { return k_Lcopy(zl);      }
static inline KVal  *k_Zsorted(KVal *zl) {
    KVal *r=k_Lcopy(zl); k_Lsort(r, kv_str("asc")); return r;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Phase 2 — Additional global built-ins
 * ══════════════════════════════════════════════════════════════════════════ */

static inline KVal  *k_all(KVal *lst) {
    for(size_t i=0;i<lst->list->len;i++)
        if(!k_truthy(lst->list->items[i])) return kv_bool(false);
    return kv_bool(true);
}
static inline KVal  *k_any(KVal *lst) {
    for(size_t i=0;i<lst->list->len;i++)
        if(k_truthy(lst->list->items[i])) return kv_bool(true);
    return kv_bool(false);
}
static inline KVal  *k_type(KVal *v) {
    switch(v->type) {
        case KT_INT:  return kv_str("int");
        case KT_FLT:  return kv_str("flt");
        case KT_STR:  return kv_str("str");
        case KT_BOOL: return kv_str("bool");
        case KT_VOID: return kv_str("void");
        case KT_LIST: return kv_str("lt");
        case KT_DICT: return kv_str("ad");
        default:      return kv_str("unknown");
    }
}
static inline KVal  *k_Lreversed(KVal *lst) { KVal *r=k_Lcopy(lst); k_Lflip(r); return r; }
static inline KVal  *k_Lsorted(KVal *lst)   { KVal *r=k_Lcopy(lst); k_Lsort(r,NULL); return r; }
static inline KVal  *k_enumerate(KVal *lst) {
    KVal *r=kv_list();
    for(size_t i=0;i<lst->list->len;i++){
        KVal *pair=kv_list();
        klist_push(pair->list, kv_int((long long)i));
        klist_push(pair->list, lst->list->items[i]);
        klist_push(r->list, pair);
    }
    return r;
}
static inline KVal  *k_zip(KVal *a, KVal *b) {
    KVal *r=kv_list();
    size_t n=a->list->len < b->list->len ? a->list->len : b->list->len;
    for(size_t i=0;i<n;i++){
        KVal *pair=kv_list();
        klist_push(pair->list, a->list->items[i]);
        klist_push(pair->list, b->list->items[i]);
        klist_push(r->list, pair);
    }
    return r;
}
static inline KVal *k_chr(KVal *v) { char buf[2]={(char)v->i,'\0'}; return kv_str(buf); }
static inline KVal *k_ord(KVal *v) { return kv_int((long long)(unsigned char)v->s[0]); }
static inline KVal *k_hex(KVal *v) { char buf[32]; snprintf(buf,sizeof(buf),"0x%llx",v->i); return kv_str(buf); }
static inline KVal *k_oct(KVal *v) { char buf[32]; snprintf(buf,sizeof(buf),"0o%llo",v->i); return kv_str(buf); }
static inline KVal *k_bin(KVal *v) {
    char buf[70]="0b"; long long n=v->i; int pos=2;
    if(n==0){buf[pos++]='0';}
    else{ int bits=63; while(bits>=0&&!((n>>bits)&1))bits--;
          for(;bits>=0;bits--) buf[pos++]='0'+((n>>bits)&1); }
    buf[pos]='\0'; return kv_str(buf);
}
static inline KVal *k_divmod(KVal *a, KVal *b) {
    KVal *r=kv_list();
    klist_push(r->list, kv_int(a->i / b->i));
    klist_push(r->list, kv_int(a->i % b->i));
    return r;
}
static inline KVal *k_fact(KVal *v) {
    long long n=v->i, r=1;
    for(long long i=2;i<=n;i++) r*=i;
    return kv_int(r);
}
static inline KVal *k_gcd2(KVal *a, KVal *b) {
    long long x=llabs(a->i), y=llabs(b->i);
    while(y){long long t=y;y=x%y;x=t;}
    return kv_int(x);
}
static inline KVal *k_lcm2(KVal *a, KVal *b) {
    long long g=k_gcd2(a,b)->i;
    return kv_int(g ? llabs(a->i / g * b->i) : 0);
}
static inline KVal *k_rand_range(KVal *lo, KVal *hi) {
    static int seeded=0;
    if(!seeded){srand((unsigned)time(NULL));seeded=1;}
    long long l=lo->i, h=hi->i;
    return kv_int(l + rand()%(h-l+1));
}

/* ── Phase 3 module includes ─────────────────────────────────────────────── */
#include "k_fileio.h"
#include "k_os.h"
#include "k_regex.h"
#include "k_formats.h"
#include "k_crypto.h"
#include "k_cli.h"

/* ── Phase 4 module includes ─────────────────────────────────────────────── */
#include "k_datetime.h"
#include "k_network.h"
#include "k_concurrency.h"
#include "k_datastructs.h"

#endif /* KATLANS_H */
