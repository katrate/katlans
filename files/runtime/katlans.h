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
                    default:      printf("?"); break;
                }
            }
            printf("]\n");
            break;
        }
        case KT_DICT: {
            printf("{");
            for (size_t i = 0; i < v->dict->len; i++) {
                if (i) printf(", ");
                printf("%s: ", v->dict->entries[i].key);
                KVal *dv = v->dict->entries[i].val;
                switch (dv->type) {
                    case KT_INT:  printf("%lld",  dv->i); break;
                    case KT_FLT:  printf("%g",    dv->f); break;
                    case KT_STR:  printf("\"%s\"",dv->s); break;
                    case KT_BOOL: printf("%s",    dv->b ? "true" : "false"); break;
                    default:      printf("?"); break;
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
static inline char* _kstringify(KVal *v) {
    if (!v) return strdup("void");
    char buf[256];
    switch (v->type) {
        case KT_INT: snprintf(buf, 256, "%lld", v->i); return strdup(buf);
        case KT_FLT: snprintf(buf, 256, "%g", v->f); return strdup(buf);
        case KT_STR: return strdup(v->s);
        case KT_BOOL: return strdup(v->b ? "true" : "false");
        default: return strdup("?");
    }
}

static inline KVal *k_concat(KVal *a, KVal *b) {
    /* ++ string concat */
    char *sa = _kstringify(a);
    char *sb = _kstringify(b);
    size_t n = strlen(sa)+strlen(sb)+1;
    char *buf = (char*)malloc(n);
    strcpy(buf, sa); strcat(buf, sb);
    KVal *r = kv_str(buf);
    free(sa); free(sb); free(buf);
    return r;
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
    /* String indexing: return single-char string */
    if (lst->type == KT_STR) {
        long long i = idx->i;
        long long slen = (long long)strlen(lst->s);
        if (i < 0) i = slen + i;
        if (i < 0 || i >= slen) return kv_void();
        char buf[2] = { lst->s[i], '\0' };
        return kv_str(buf);
    }
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

/* ── Additional string built-ins for Phase 2 ───────────────────────────── */

static inline KVal *k_Stitle(KVal *v) {
    char *r = strdup(v->s);
    bool cap = true;
    for (int i = 0; r[i]; i++) {
        if (r[i] == ' ') { cap = true; }
        else if (cap) { r[i] = toupper((unsigned char)r[i]); cap = false; }
        else { r[i] = tolower((unsigned char)r[i]); }
    }
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Scapfirst(KVal *v) {
    char *r = strdup(v->s);
    if (r[0]) r[0] = toupper((unsigned char)r[0]);
    for (int i = 1; r[i]; i++) r[i] = tolower((unsigned char)r[i]);
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Sswapcase(KVal *v) {
    char *r = strdup(v->s);
    for (int i = 0; r[i]; i++) {
        if (isupper((unsigned char)r[i])) r[i] = tolower((unsigned char)r[i]);
        else r[i] = toupper((unsigned char)r[i]);
    }
    KVal *rv = kv_str(r); free(r); return rv;
}
static inline KVal *k_Scasefold(KVal *v) {
    return k_Slow(v);
}

/* Schop: split by delimiter */
static inline KVal *k_Schop(KVal *v, KVal *delim) {
    KVal *lst = kv_list();
    char *s = v->s;
    char *d = delim->s;
    size_t dlen = strlen(d);
    char *p;
    while ((p = strstr(s, d))) {
        int len = (int)(p - s);
        char *buf = (char*)malloc(len + 1);
        strncpy(buf, s, len); buf[len] = '\0';
        klist_push(lst->list, kv_str(buf)); free(buf);
        s = p + dlen;
    }
    klist_push(lst->list, kv_str(s));
    return lst;
}

/* Schoplines: split by newlines */
static inline KVal *k_Schoplines(KVal *v) {
    return k_Schop(v, kv_str("\n"));
}

/* Srchop: rsplit - split from right */
static inline KVal *k_Srchop(KVal *v, KVal *delim) {
    KVal *lst = k_Schop(v, delim);
    /* reverse the list */
    for (size_t i = 0; i < lst->list->len / 2; i++) {
        KVal *tmp = lst->list->items[i];
        lst->list->items[i] = lst->list->items[lst->list->len-1-i];
        lst->list->items[lst->list->len-1-i] = tmp;
    }
    return lst;
}

/* Sfront: starts with */
static inline KVal *k_Sfront(KVal *v, KVal *sub) {
    size_t n = strlen(sub->s);
    return kv_bool(strlen(v->s) >= n && strncmp(v->s, sub->s, n) == 0);
}

/* Sback: ends with */
static inline KVal *k_Sback(KVal *v, KVal *sub) {
    size_t n = strlen(sub->s);
    size_t m = strlen(v->s);
    return kv_bool(m >= n && strcmp(v->s + m - n, sub->s) == 0);
}

/* Sswap: replace substring */
static inline KVal *k_Sswap(KVal *v, KVal *old, KVal *new_str) {
    char *s = v->s;
    char *o = old->s;
    char *n = new_str->s;
    size_t olen = strlen(o), nlen = strlen(n);
    char result[65536] = "";
    char *p;
    while ((p = strstr(s, o))) {
        strncat(result, s, p - s);
        strcat(result, n);
        s = p + olen;
    }
    strcat(result, s);
    return kv_str(result);
}

/* Sremprefix: remove prefix */
static inline KVal *k_Sremprefix(KVal *v, KVal *pre) {
    size_t n = strlen(pre->s);
    if (strlen(v->s) >= n && strncmp(v->s, pre->s, n) == 0)
        return kv_str(v->s + n);
    return kv_str(v->s);
}

/* Sremsuffix: remove suffix */
static inline KVal *k_Sremsuffix(KVal *v, KVal *suf) {
    size_t n = strlen(suf->s);
    size_t m = strlen(v->s);
    if (m >= n && strcmp(v->s + m - n, suf->s) == 0) {
        char *buf = (char*)malloc(m - n + 1);
        strncpy(buf, v->s, m - n); buf[m - n] = '\0';
        KVal *rv = kv_str(buf); free(buf); return rv;
    }
    return kv_str(v->s);
}

/* Srfind: find last occurrence */
static inline KVal *k_Srfind(KVal *v, KVal *sub) {
    char *last = NULL;
    char *p = v->s;
    while ((p = strstr(p, sub->s))) { last = p; p++; }
    return last ? kv_int((long long)(last - v->s)) : kv_int(-1);
}

/* Sindex: like find but raises error (returns -1 for now) */
static inline KVal *k_Sindex(KVal *v, KVal *sub) { return k_Sfind(v, sub); }
static inline KVal *k_Srindex(KVal *v, KVal *sub) { return k_Srfind(v, sub); }

/* Szfill: zero-fill left */
static inline KVal *k_Szfill(KVal *v, KVal *width) {
    long long w = width->i;
    size_t n = strlen(v->s);
    if ((long long)n >= w) return kv_str(v->s);
    char *buf = (char*)malloc((size_t)w + 1);
    for (long long i = 0; i < w - (long long)n; i++) buf[i] = '0';
    strcpy(buf + w - n, v->s);
    KVal *rv = kv_str(buf); free(buf); return rv;
}

/* Sjoin: join list with separator */
static inline KVal *k_Sjoin(KVal *sep, KVal *lst) {
    char buf[65536] = "";
    for (size_t i = 0; i < lst->list->len; i++) {
        if (i) strcat(buf, sep->s);
        KVal *item = lst->list->items[i];
        char *s = (item->type == KT_STR) ? item->s : k_S(item)->s;
        strcat(buf, s);
    }
    return kv_str(buf);
}

/* Sisalpha / Sisdigit / Sisalnum / Sisspace / Sislower / Sisupper / Sistitle / Sisascii */
static inline KVal *k_Sisalpha(KVal *v) {
    for (char *p = v->s; *p; p++) if (!isalpha((unsigned char)*p)) return kv_bool(false);
    return kv_bool(strlen(v->s) > 0);
}
static inline KVal *k_Sisdigit(KVal *v) {
    for (char *p = v->s; *p; p++) if (!isdigit((unsigned char)*p)) return kv_bool(false);
    return kv_bool(strlen(v->s) > 0);
}
static inline KVal *k_Sisalnum(KVal *v) {
    for (char *p = v->s; *p; p++) if (!isalnum((unsigned char)*p)) return kv_bool(false);
    return kv_bool(strlen(v->s) > 0);
}
static inline KVal *k_Sisspace(KVal *v) {
    for (char *p = v->s; *p; p++) if (!isspace((unsigned char)*p)) return kv_bool(false);
    return kv_bool(strlen(v->s) > 0);
}
static inline KVal *k_Sislower(KVal *v) {
    bool has_letter = false;
    for (char *p = v->s; *p; p++) {
        if (isalpha((unsigned char)*p)) { has_letter = true; if (!islower((unsigned char)*p)) return kv_bool(false); }
    }
    return kv_bool(has_letter);
}
static inline KVal *k_Sisupper(KVal *v) {
    bool has_letter = false;
    for (char *p = v->s; *p; p++) {
        if (isalpha((unsigned char)*p)) { has_letter = true; if (!isupper((unsigned char)*p)) return kv_bool(false); }
    }
    return kv_bool(has_letter);
}
static inline KVal *k_Sistitle(KVal *v) {
    bool cap = true;
    for (char *p = v->s; *p; p++) {
        if (*p == ' ') { cap = true; }
        else if (cap) { if (!isupper((unsigned char)*p)) return kv_bool(false); cap = false; }
        else { if (!islower((unsigned char)*p)) return kv_bool(false); }
    }
    return kv_bool(strlen(v->s) > 0);
}
static inline KVal *k_Sisascii(KVal *v) {
    for (char *p = v->s; *p; p++) if ((unsigned char)*p > 127) return kv_bool(false);
    return kv_bool(true);
}

/* ── Additional List built-ins ──────────────────────────────────────────── */

/* Lsort: sort list in-place */
static inline int _k_compare(const void *a, const void *b) {
    KVal *va = *(KVal**)a, *vb = *(KVal**)b;
    double da = _knum(va), db = _knum(vb);
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}
static inline KVal *k_Lsort(KVal *lst) {
    qsort(lst->list->items, lst->list->len, sizeof(KVal*), _k_compare);
    return kv_void();
}

/* Lflip: reverse list in-place */
static inline KVal *k_Lflip(KVal *lst) {
    for (size_t i = 0; i < lst->list->len / 2; i++) {
        KVal *tmp = lst->list->items[i];
        lst->list->items[i] = lst->list->items[lst->list->len-1-i];
        lst->list->items[lst->list->len-1-i] = tmp;
    }
    return kv_void();
}

/* Ldrop: remove first occurrence of value */
static inline KVal *k_Ldrop(KVal *lst, KVal *val) {
    for (size_t i = 0; i < lst->list->len; i++) {
        KVal *e = lst->list->items[i];
        bool match = false;
        if (e->type == KT_INT && val->type == KT_INT && e->i == val->i) match = true;
        else if (e->type == KT_STR && val->type == KT_STR && strcmp(e->s, val->s) == 0) match = true;
        else if (_knum(e) == _knum(val)) match = true;
        if (match) {
            memmove(&lst->list->items[i], &lst->list->items[i+1], (lst->list->len-i-1)*sizeof(KVal*));
            lst->list->len--; break;
        }
    }
    return kv_void();
}

/* Lpop: remove and return last (or at index) */
static inline KVal *k_Lpop(KVal *lst, KVal *idx) {
    if (!lst->list->len) return kv_void();
    long long i = idx ? idx->i : (long long)lst->list->len - 1;
    if (i < 0) i = (long long)lst->list->len + i;
    if (i < 0 || (size_t)i >= lst->list->len) return kv_void();
    KVal *v = lst->list->items[i];
    memmove(&lst->list->items[i], &lst->list->items[i+1], (lst->list->len-i-1)*sizeof(KVal*));
    lst->list->len--;
    return v;
}

/* Lindex: find first index of value */
static inline KVal *k_Lindex(KVal *lst, KVal *val) {
    for (size_t i = 0; i < lst->list->len; i++) {
        KVal *e = lst->list->items[i];
        if (e->type == KT_INT && val->type == KT_INT && e->i == val->i) return kv_int((long long)i);
        if (e->type == KT_STR && val->type == KT_STR && strcmp(e->s, val->s) == 0) return kv_int((long long)i);
        if (_knum(e) == _knum(val)) return kv_int((long long)i);
    }
    return kv_int(-1);
}

/* Lcount: count occurrences */
static inline KVal *k_Lcount(KVal *lst, KVal *val) {
    long long count = 0;
    for (size_t i = 0; i < lst->list->len; i++) {
        KVal *e = lst->list->items[i];
        if (e->type == KT_INT && val->type == KT_INT && e->i == val->i) count++;
        else if (e->type == KT_STR && val->type == KT_STR && strcmp(e->s, val->s) == 0) count++;
        else if (_knum(e) == _knum(val)) count++;
    }
    return kv_int(count);
}

/* Lhas: check if value exists */
static inline KVal *k_Lhas(KVal *lst, KVal *val) {
    for (size_t i = 0; i < lst->list->len; i++) {
        KVal *e = lst->list->items[i];
        if (e->type == KT_INT && val->type == KT_INT && e->i == val->i) return kv_bool(true);
        if (e->type == KT_STR && val->type == KT_STR && strcmp(e->s, val->s) == 0) return kv_bool(true);
        if (_knum(e) == _knum(val)) return kv_bool(true);
    }
    return kv_bool(false);
}

/* Lcopy: shallow copy */
static inline KVal *k_Lcopy(KVal *lst) {
    KVal *c = kv_list();
    for (size_t i = 0; i < lst->list->len; i++)
        klist_push(c->list, lst->list->items[i]);
    return c;
}

/* Lclear: remove all items */
static inline KVal *k_Lclear(KVal *lst) {
    lst->list->len = 0;
    return kv_void();
}

/* Linsert: insert at index */
static inline KVal *k_Linsert(KVal *lst, KVal *idx, KVal *val) {
    long long i = idx->i;
    if (i < 0) i = 0;
    if ((size_t)i >= lst->list->len) { klist_push(lst->list, val); return kv_void(); }
    klist_push(lst->list, NULL);
    memmove(&lst->list->items[i+1], &lst->list->items[i], (lst->list->len-i-1)*sizeof(KVal*));
    lst->list->items[i] = val;
    return kv_void();
}

/* Lextend: extend with another list */
static inline KVal *k_Lextend(KVal *lst, KVal *other) {
    for (size_t i = 0; i < other->list->len; i++)
        klist_push(lst->list, other->list->items[i]);
    return kv_void();
}

/* Lunique: remove duplicates, return new list */
static inline KVal *k_Lunique(KVal *lst) {
    KVal *r = kv_list();
    for (size_t i = 0; i < lst->list->len; i++) {
        bool found = false;
        for (size_t j = 0; j < r->list->len; j++) {
            KVal *a = lst->list->items[i], *b = r->list->items[j];
            if (a->type == KT_INT && b->type == KT_INT && a->i == b->i) { found = true; break; }
            if (a->type == KT_STR && b->type == KT_STR && strcmp(a->s, b->s) == 0) { found = true; break; }
            if (_knum(a) == _knum(b)) { found = true; break; }
        }
        if (!found) klist_push(r->list, lst->list->items[i]);
    }
    return r;
}

/* Lslice: slice list */
static inline KVal *k_Lslice(KVal *lst, KVal *start, KVal *end) {
    long long s = start ? start->i : 0;
    long long e = end ? end->i : (long long)lst->list->len;
    if (s < 0) s = (long long)lst->list->len + s;
    if (e < 0) e = (long long)lst->list->len + e;
    if (s < 0) s = 0; if ((size_t)e > lst->list->len) e = (long long)lst->list->len;
    KVal *r = kv_list();
    for (long long i = s; i < e; i++) klist_push(r->list, lst->list->items[i]);
    return r;
}

/* Lreversed: return new reversed list */
static inline KVal *k_Lreversed(KVal *lst) {
    KVal *r = kv_list();
    for (size_t i = lst->list->len; i > 0; i--)
        klist_push(r->list, lst->list->items[i-1]);
    return r;
}

/* Lsorted: return new sorted list */
static inline KVal *k_Lsorted(KVal *lst) {
    KVal *c = k_Lcopy(lst);
    k_Lsort(c);
    return c;
}

/* ── Adero/Dict built-ins (A prefix) ────────────────────────────────────── */

static inline KVal *k_Alen(KVal *ad) { return kv_int((long long)ad->dict->len); }

static inline KVal *k_Akeys(KVal *ad) {
    KVal *r = kv_list();
    for (size_t i = 0; i < ad->dict->len; i++)
        klist_push(r->list, kv_str(ad->dict->entries[i].key));
    return r;
}

static inline KVal *k_Avals(KVal *ad) {
    KVal *r = kv_list();
    for (size_t i = 0; i < ad->dict->len; i++)
        klist_push(r->list, ad->dict->entries[i].val);
    return r;
}

static inline KVal *k_Aitems(KVal *ad) {
    KVal *r = kv_list();
    for (size_t i = 0; i < ad->dict->len; i++) {
        KVal *pair = kv_list();
        klist_push(pair->list, kv_str(ad->dict->entries[i].key));
        klist_push(pair->list, ad->dict->entries[i].val);
        klist_push(r->list, pair);
    }
    return r;
}

static inline KVal *k_Aget(KVal *ad, KVal *key) {
    return kdict_get(ad->dict, key->s);
}

static inline KVal *k_Ahas(KVal *ad, KVal *key) {
    KVal *v = kdict_get(ad->dict, key->s);
    return kv_bool(v->type != KT_VOID);
}

static inline KVal *k_Aset(KVal *ad, KVal *key, KVal *val) {
    kdict_set(ad->dict, key->s, val);
    return kv_void();
}

static inline KVal *k_Adrop(KVal *ad, KVal *key) {
    for (size_t i = 0; i < ad->dict->len; i++) {
        if (strcmp(ad->dict->entries[i].key, key->s) == 0) {
            free(ad->dict->entries[i].key);
            memmove(&ad->dict->entries[i], &ad->dict->entries[i+1], (ad->dict->len-i-1)*sizeof(KDictEntry));
            ad->dict->len--; break;
        }
    }
    return kv_void();
}

static inline KVal *k_Aclear(KVal *ad) {
    ad->dict->len = 0;
    return kv_void();
}

static inline KVal *k_Acopy(KVal *ad) {
    KVal *c = kv_dict();
    for (size_t i = 0; i < ad->dict->len; i++)
        kdict_set(c->dict, ad->dict->entries[i].key, ad->dict->entries[i].val);
    return c;
}

/* ── Zelo/Tuple built-ins (Z prefix) ────────────────────────────────────── */

static inline KVal *k_Zlen(KVal *zl)    { return kv_int((long long)zl->list->len); }
static inline KVal *k_Zhas(KVal *zl, KVal *v) { return k_Lhas(zl, v); }
static inline KVal *k_Zmax(KVal *zl)    { return k_Lmax(zl); }
static inline KVal *k_Zmin(KVal *zl)    { return k_Lmin(zl); }
static inline KVal *k_Zsum(KVal *zl)    { return k_sum(zl); }
static inline KVal *k_Zindex(KVal *zl, KVal *v) { return k_Lindex(zl, v); }
static inline KVal *k_Zcount(KVal *zl, KVal *v) { return k_Lcount(zl, v); }

/* ── Global built-ins ───────────────────────────────────────────────────── */

/* type(x) → string name of type */
static inline KVal *k_type(KVal *v) {
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

static inline KVal *k_chr(KVal *v)    { char c = (char)v->i; char buf[2] = {c, 0}; return kv_str(buf); }
static inline KVal *k_ord(KVal *v)    { return kv_int((long long)(unsigned char)v->s[0]); }
static inline KVal *k_hex(KVal *v)    { char buf[32]; snprintf(buf,sizeof(buf),"0x%llx",v->i); return kv_str(buf); }
static inline KVal *k_oct(KVal *v)    { char buf[32]; snprintf(buf,sizeof(buf),"0o%llo",v->i); return kv_str(buf); }
static inline KVal *k_bin(KVal *v)    {
    char buf[128] = "0b";
    unsigned long long n = (unsigned long long)v->i;
    int started = 0;
    for (int i = 63; i >= 0; i--) {
        if ((n >> i) & 1) { started = 1; strcat(buf, "1"); }
        else if (started) strcat(buf, "0");
    }
    if (!started) strcat(buf, "0");
    return kv_str(buf);
}

/* divmod: return [quotient, remainder] */
static inline KVal *k_divmod(KVal *a, KVal *b) {
    KVal *r = kv_list();
    klist_push(r->list, kv_int(a->i / b->i));
    klist_push(r->list, kv_int(a->i % b->i));
    return r;
}

/* all: true if all list items are truthy */
static inline KVal *k_all(KVal *lst) {
    for (size_t i = 0; i < lst->list->len; i++)
        if (!k_truthy(lst->list->items[i])) return kv_bool(false);
    return kv_bool(true);
}

/* any: true if any list item is truthy */
static inline KVal *k_any(KVal *lst) {
    for (size_t i = 0; i < lst->list->len; i++)
        if (k_truthy(lst->list->items[i])) return kv_bool(true);
    return kv_bool(false);
}

static inline long long _k_gcd(long long a, long long b) { while(b){long long t=b;b=a%b;a=t;} return a; }
static inline KVal *k_gcd(KVal *a, KVal *b) { return kv_int(_k_gcd(a->i, b->i)); }
static inline KVal *k_lcm(KVal *a, KVal *b) { return kv_int(a->i / _k_gcd(a->i,b->i) * b->i); }

static inline KVal *k_fact(KVal *v) {
    long long n = v->i, r = 1;
    for (long long i = 2; i <= n; i++) r *= i;
    return kv_int(r);
}

static inline KVal *k_rand(KVal *lo, KVal *hi) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }
    long long l = lo->i, h = hi->i;
    return kv_int(l + (rand() % (h - l + 1)));
}

/* enumerate: return list of [index, value] pairs */
static inline KVal *k_ltenum(KVal *lst) {
    KVal *r = kv_list();
    for (size_t i = 0; i < lst->list->len; i++) {
        KVal *pair = kv_list();
        klist_push(pair->list, kv_int((long long)i));
        klist_push(pair->list, lst->list->items[i]);
        klist_push(r->list, pair);
    }
    return r;
}

/* zip: return list of [a_i, b_i] pairs */
static inline KVal *k_ltzip(KVal *a, KVal *b) {
    KVal *r = kv_list();
    size_t n = a->list->len < b->list->len ? a->list->len : b->list->len;
    for (size_t i = 0; i < n; i++) {
        KVal *pair = kv_list();
        klist_push(pair->list, a->list->items[i]);
        klist_push(pair->list, b->list->items[i]);
        klist_push(r->list, pair);
    }
    return r;
}

/* Constants */
#define K_PI  kv_flt(3.14159265358979323846)
#define K_E   kv_flt(2.71828182845904523536)

/* Include all module headers (must be after KVal/kv_* definitions) */
#include "k_fileio.h"
#include "k_os.h"
#include "k_regex.h"
#include "k_formats.h"
#include "k_crypto.h"
#include "k_cli.h"
#include "k_datetime.h"
#include "k_network.h"
#include "k_concurrency.h"
#include "k_datastructs.h"

/* ── Additional Modules ──────────────────────────────────────────────── */
#include "k_zp.h"
#include "k_ser.h"

/* ── Phase 5 — Power Modules ──────────────────────────────────────────── */
#include "k_ml.h"
#include "k_vis.h"
#include "k_game.h"
#include "k_ui.h"
#include "k_finance.h"
#include "k_design.h"

#endif /* KATLANS_H */
