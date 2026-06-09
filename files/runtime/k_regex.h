/*
 * k_regex.h  —  Katlans Regex  (rx prefix)
 * Uses POSIX extended regex (regex.h) — available on all Linux/macOS systems.
 * On Windows/MinGW, provides a substring-based fallback.
 */
#ifndef K_REGEX_H
#define K_REGEX_H

#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
/* Minimal regex fallback for Windows — substring-based matching */
#define K_RX_MAXGROUPS 10
#define K_RX_MAXMATCH  4096

/* Simple substring-based pattern matcher (handles exact and anchored patterns) */
static inline int _k_rx_match(const char *str, const char *pat) {
    if (strstr(str, pat)) return 1;
    if (pat[0] == '^' && strlen(pat) > 1 && pat[strlen(pat)-1] == '$') {
        size_t plen = strlen(pat) - 2;
        return (strncmp(str, pat + 1, plen) == 0 && str[plen] == '\0') ? 1 : 0;
    }
    return 0;
}

/* regcomp: stores pattern, always succeeds */
#define regcomp(re,pat,flags) ({ strncpy((re)->s, pat, 4095); (re)->s[4095]='\0'; (void)flags; 0; })
/* regexec: simple match using stored pattern (pmatch ignored — use k_rxfind for positions on Win) */
#define regexec(re,str,nmatch,pmatch,flags) ({ (void)re;(void)nmatch;(void)pmatch;(void)flags; _k_rx_match(str,(re)->s) ? 0 : 1; })
#define regfree(re) ((void)0)
#define REG_EXTENDED 0
typedef struct { char s[4096]; } regex_t;
typedef struct { int rm_so, rm_eo; } regmatch_t;
#else
#include <regex.h>
#define K_RX_MAXGROUPS 32
#define K_RX_MAXMATCH  4096
#endif

/* ── rxmatch <str, pattern> → true/false ────────────────────────────────── */
static inline KVal *k_rxmatch(KVal *str, KVal *pat) {
    regex_t re; int r = regcomp(&re, pat->s, REG_EXTENDED);
    if (r) return kv_bool(false);
    bool found = regexec(&re, str->s, 0, NULL, 0) == 0;
    regfree(&re);
    return kv_bool(found);
}

/* ── rxfull <str, pattern> → true/false  (full-string match) ────────────── */
static inline KVal *k_rxfull(KVal *str, KVal *pat) {
    char anchored[4096];
    snprintf(anchored, sizeof(anchored), "^(%s)$", pat->s);
    KVal *p = kv_str(anchored);
    return k_rxmatch(str, p);
}

/* ── rxfind <str, pattern> → list of all matches ────────────────────────── */
static inline KVal *k_rxfind(KVal *str, KVal *pat) {
    KVal *lst = kv_list();
#ifdef _WIN32
    /* Substring search — computes positions manually */
    char *s = str->s;
    size_t plen = strlen(pat->s);
    if (plen == 0) { klist_push(lst->list, kv_str("")); return lst; }
    while ((s = strstr(s, pat->s)) != NULL) {
        char *buf = (char*)malloc(plen + 1);
        strncpy(buf, s, plen); buf[plen] = '\0';
        klist_push(lst->list, kv_str(buf)); free(buf);
        s += plen;
    }
#else
    regex_t re;
    if (regcomp(&re, pat->s, REG_EXTENDED) != 0) return lst;
    regmatch_t m[1];
    char *s = str->s;
    while (regexec(&re, s, 1, m, 0) == 0) {
        int len = (int)(m[0].rm_eo - m[0].rm_so);
        char *buf = (char*)malloc(len + 1);
        strncpy(buf, s + m[0].rm_so, len); buf[len] = '\0';
        klist_push(lst->list, kv_str(buf)); free(buf);
        s += m[0].rm_eo;
        if (m[0].rm_eo == 0) s++;  /* avoid infinite loop on zero-length match */
    }
    regfree(&re);
#endif
    return lst;
}

/* ── rxswap <str, pattern, replacement> → string ────────────────────────── */
static inline KVal *k_rxswap(KVal *str, KVal *pat, KVal *rep) {
#ifdef _WIN32
    char result[K_RX_MAXMATCH] = "";
    char *s = str->s;
    size_t plen = strlen(pat->s);
    if (plen == 0) return kv_str(str->s);
    char *found;
    while ((found = strstr(s, pat->s)) != NULL) {
        strncat(result, s, (size_t)(found - s));
        strcat(result, rep->s);
        s = found + plen;
    }
    strcat(result, s);
    return kv_str(result);
#else
    regex_t re;
    if (regcomp(&re, pat->s, REG_EXTENDED) != 0) return kv_str(str->s);
    char result[K_RX_MAXMATCH] = "";
    char *s = str->s;
    regmatch_t m[1];
    while (regexec(&re, s, 1, m, 0) == 0) {
        int before = (int)m[0].rm_so;
        strncat(result, s, before);
        strcat(result, rep->s);
        s += m[0].rm_eo;
        if (m[0].rm_eo == 0) { strncat(result, s, 1); s++; }
    }
    strcat(result, s);
    regfree(&re);
    return kv_str(result);
#endif
}

/* ── rxchop <str, pattern> → list (split by pattern) ────────────────────── */
static inline KVal *k_rxchop(KVal *str, KVal *pat) {
    KVal *lst = kv_list();
#ifdef _WIN32
    char *s = str->s;
    size_t plen = strlen(pat->s);
    if (plen == 0) { klist_push(lst->list, kv_str(str->s)); return lst; }
    char *found;
    while ((found = strstr(s, pat->s)) != NULL) {
        int len = (int)(found - s);
        char *buf = (char*)malloc(len + 1);
        strncpy(buf, s, len); buf[len] = '\0';
        klist_push(lst->list, kv_str(buf)); free(buf);
        s = found + plen;
    }
    klist_push(lst->list, kv_str(s));
#else
    regex_t re;
    if (regcomp(&re, pat->s, REG_EXTENDED) != 0) {
        klist_push(lst->list, kv_str(str->s)); return lst;
    }
    char *s = str->s;
    regmatch_t m[1];
    while (regexec(&re, s, 1, m, 0) == 0) {
        int len = (int)m[0].rm_so;
        char *buf = (char*)malloc(len + 1);
        strncpy(buf, s, len); buf[len] = '\0';
        klist_push(lst->list, kv_str(buf)); free(buf);
        s += m[0].rm_eo;
        if (m[0].rm_eo == 0) s++;
    }
    klist_push(lst->list, kv_str(s));
    regfree(&re);
#endif
    return lst;
}

/* ── rxgroups <str, pattern> → list of captured groups ──────────────────── */
static inline KVal *k_rxgroups(KVal *str, KVal *pat) {
    KVal *lst = kv_list();
#ifdef _WIN32
    /* No real regex groups on Windows — just return the full match */
    (void)pat;
    /* Substring match the full pattern heuristically */
    KVal *m = k_rxfind(str, pat);
    if (m->list->len > 0) {
        KVal *first = (KVal*)m->list->items[0];
        klist_push(lst->list, kv_str(first->s));
    }
    return lst;
#else
    regex_t re;
    if (regcomp(&re, pat->s, REG_EXTENDED) != 0) return lst;
    regmatch_t m[K_RX_MAXGROUPS];
    if (regexec(&re, str->s, K_RX_MAXGROUPS, m, 0) == 0) {
        for (int i = 1; i < K_RX_MAXGROUPS && m[i].rm_so >= 0; i++) {
            int len = (int)(m[i].rm_eo - m[i].rm_so);
            char *buf = (char*)malloc(len + 1);
            strncpy(buf, str->s + m[i].rm_so, len); buf[len] = '\0';
            klist_push(lst->list, kv_str(buf)); free(buf);
        }
    }
    regfree(&re);
    return lst;
#endif
}

/* ── rxcount <str, pattern> → int ────────────────────────────────────────── */
static inline KVal *k_rxcount(KVal *str, KVal *pat) {
    KVal *matches = k_rxfind(str, pat);
    long long count = (long long)(matches ? matches->list->len : 0);
    return kv_int(count);
}

#endif /* K_REGEX_H */
