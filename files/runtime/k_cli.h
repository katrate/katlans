/*
 * k_cli.h  —  Katlans CLI Tools + Testing Framework
 */
#ifndef K_CLI_H
#define K_CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ══════════════════════════════════════════════════════════════════════════
 *  CLI Tools  (cli prefix)
 * ══════════════════════════════════════════════════════════════════════════ */

/* ANSI color codes */
#define _K_RED     "\033[31m"
#define _K_GREEN   "\033[32m"
#define _K_YELLOW  "\033[33m"
#define _K_BLUE    "\033[34m"
#define _K_MAGENTA "\033[35m"
#define _K_CYAN    "\033[36m"
#define _K_BOLD    "\033[1m"
#define _K_DIM     "\033[2m"
#define _K_UNDER   "\033[4m"
#define _K_RESET   "\033[0m"

static inline KVal *k_clired(KVal *v)     { printf(_K_RED    "%s" _K_RESET "\n", v->s); return kv_void(); }
static inline KVal *k_cligreen(KVal *v)   { printf(_K_GREEN  "%s" _K_RESET "\n", v->s); return kv_void(); }
static inline KVal *k_cliyellow(KVal *v)  { printf(_K_YELLOW "%s" _K_RESET "\n", v->s); return kv_void(); }
static inline KVal *k_clibold(KVal *v)    { printf(_K_BOLD   "%s" _K_RESET "\n", v->s); return kv_void(); }
static inline KVal *k_cliblue(KVal *v)    { printf(_K_BLUE   "%s" _K_RESET "\n", v->s); return kv_void(); }
static inline KVal *k_climagenta(KVal *v) { printf(_K_MAGENTA"%s" _K_RESET "\n", v->s); return kv_void(); }
static inline KVal *k_clicyan(KVal *v)    { printf(_K_CYAN   "%s" _K_RESET "\n", v->s); return kv_void(); }
static inline KVal *k_clidim(KVal *v)     { printf(_K_DIM    "%s" _K_RESET "\n", v->s); return kv_void(); }
static inline KVal *k_cliunder(KVal *v)   { printf(_K_UNDER  "%s" _K_RESET "\n", v->s); return kv_void(); }

/* ── climenu <"title","A","B","C"> → chosen value ────────────────────────── */
static inline KVal *k_climenu(KVal *lst) {
    if (!lst->list->len) return kv_void();
    printf("\n%s\n", lst->list->items[0]->s);
    for (size_t i = 1; i < lst->list->len; i++)
        printf("  [%zu] %s\n", i, lst->list->items[i]->s);
    printf("Choice: "); fflush(stdout);
    char buf[32]; if (!fgets(buf, sizeof(buf), stdin)) return kv_void();
    int choice = atoi(buf);
    if (choice < 1 || (size_t)choice >= lst->list->len) return kv_void();
    return lst->list->items[choice];
}

/* ── cliprogress <total> ─────────────────────────────────────────────────── */
typedef struct { long long total; long long current; } KProgress;

static inline KVal *k_cliprogress(KVal *total) {
    KProgress *p = (KProgress*)malloc(sizeof(KProgress));
    p->total = total->i; p->current = 0;
    KVal *r = kv_dict();
    kdict_set(r->dict, "_ptr",   kv_int((long long)(size_t)p));
    kdict_set(r->dict, "total",  kv_int(p->total));
    kdict_set(r->dict, "current",kv_int(0));
    return r;
}

static void _clipb_draw(long long cur, long long total) {
    int width = 40;
    int filled = (int)((double)cur / (double)total * width);
    printf("\r[");
    for (int i = 0; i < width; i++) printf(i < filled ? "█" : "░");
    printf("] %lld/%lld (%.1f%%)", cur, total, (double)cur/total*100);
    fflush(stdout);
    if (cur >= total) printf("\n");
}

static inline KVal *k_clipbset(KVal *pb, KVal *val) {
    KVal *ptr_v = kdict_get(pb->dict, "_ptr");
    KProgress *p = (KProgress*)(size_t)ptr_v->i;
    p->current = val->i;
    kdict_set(pb->dict, "current", kv_int(p->current));
    _clipb_draw(p->current, p->total);
    return kv_void();
}

static inline KVal *k_clipbdone(KVal *pb) {
    KVal *total = kdict_get(pb->dict, "total");
    return k_clipbset(pb, total);
}

/* ── cliargs() → list of command-line args ───────────────────────────────── */
/* These globals are set by main() wrapper — see codegen */
static int   _k_argc = 0;
static char **_k_argv = NULL;

static inline KVal *k_cliargs(void) {
    KVal *lst = kv_list();
    for (int i = 1; i < _k_argc; i++)
        klist_push(lst->list, kv_str(_k_argv[i]));
    return lst;
}

static inline KVal *k_cliflag(KVal *flag) {
    for (int i = 1; i < _k_argc; i++)
        if (strcmp(_k_argv[i], flag->s) == 0) return kv_bool(true);
    return kv_bool(false);
}

static inline KVal *k_cliopt(KVal *flag, KVal *def) {
    for (int i = 1; i < _k_argc - 1; i++)
        if (strcmp(_k_argv[i], flag->s) == 0) return kv_str(_k_argv[i+1]);
    return def ? def : kv_void();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Testing Framework  (test prefix)
 * ══════════════════════════════════════════════════════════════════════════ */

typedef struct {
    const char *name;
    int passed;
    int failed;
} KTestSuite;

static KTestSuite _k_test = {NULL, 0, 0};
static const char *_k_current_test = "";

static inline void k_test_begin(const char *name) {
    _k_current_test = name;
    _k_test.name = name;
}

static inline KVal *k_testcheck(KVal *cond, const char *expr_str) {
    if (k_truthy(cond)) {
        _k_test.passed++;
        printf("  " _K_GREEN "✓" _K_RESET " %s\n", expr_str ? expr_str : "check");
    } else {
        _k_test.failed++;
        printf("  " _K_RED "✗" _K_RESET " FAILED: %s (in %s)\n",
               expr_str ? expr_str : "check", _k_current_test);
    }
    return kv_void();
}

static inline KVal *k_testrun_summary(void) {
    printf("\n" _K_BOLD "Test Results:" _K_RESET "\n");
    printf("  " _K_GREEN "Passed: %d" _K_RESET "\n", _k_test.passed);
    if (_k_test.failed > 0)
        printf("  " _K_RED "Failed: %d" _K_RESET "\n", _k_test.failed);
    else
        printf("  " _K_GREEN "All tests passed! 🎉" _K_RESET "\n");
    return kv_void();
}

/* Macro for testcheck with expression string captured */
#define K_TESTCHECK(expr) k_testcheck((expr), #expr)

#endif /* K_CLI_H */
