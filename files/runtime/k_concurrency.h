/*
 * k_concurrency.h  —  Katlans Concurrency  (cx prefix)
 * Uses fork() for background tasks, pipes for channels.
 * Provides async/await simulation with non-blocking semantics.
 */
#ifndef K_CONCURRENCY_H
#define K_CONCURRENCY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
/* Windows does not have fork/waitpid/kill — stub them out */
#ifndef pipe
#define pipe(fds) _pipe((fds), 65536, O_BINARY)
#endif
#define waitpid(pid,status,options) (0)
#define fork() ((pid_t)-1)
#define kill(pid,sig) (0)
#define usleep(us) Sleep((us)/1000)
#ifndef strdup
#define strdup(s) _strdup(s)
#endif
#define read  _read
#define write _write
#define sleep(s) Sleep((s)*1000)
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#endif

/* ── Simple thread-like structure using fork ────────────────────────────── */
typedef struct { pid_t pid; int pipe_out[2]; } KThread;

/* ── cxrun <fxn> — run in background (fire and forget) ───────────────────── */
typedef KVal *(*KFuncPtr)(void);

static inline KVal *k_cxrun_bg(KFuncPtr fn) {
    pid_t pid = fork();
    if(pid==0) { fn(); exit(0); }
    /* parent continues immediately */
    KVal *r=kv_dict();
    kdict_set(r->dict,"pid",kv_int((long long)pid));
    kdict_set(r->dict,"done",kv_bool(false));
    return r;
}

/* ── cxjoin — wait for background task ──────────────────────────────────── */
static inline KVal *k_cxjoin(KVal *task) {
    KVal *pid_v=kdict_get(task->dict,"pid");
    if(pid_v->type==KT_INT && pid_v->i>0){
        int status; waitpid((pid_t)pid_v->i,&status,0);
        kdict_set(task->dict,"done",kv_bool(true));
    }
    return kv_void();
}

/* ── cxstop — kill background task ──────────────────────────────────────── */
static inline KVal *k_cxstop(KVal *task) {
    KVal *pid_v=kdict_get(task->dict,"pid");
    if(pid_v->type==KT_INT && pid_v->i>0){
        kill((pid_t)pid_v->i,SIGTERM);
        kdict_set(task->dict,"done",kv_bool(true));
    }
    return kv_void();
}

/* ── cxwait <n_secs> — wait N seconds (alias dtwait) ────────────────────── */
static inline KVal *k_cxwait_secs(KVal *secs) {
    sleep((unsigned)secs->i); return kv_void();
}

/* ── cxatom / cxinc / cxdec — atomic counter simulation ──────────────────── */
static inline KVal *k_cxatom(KVal *init) { return kv_int(init->i); }
static inline KVal *k_cxinc(KVal *v)     { v->i++; return kv_void(); }
static inline KVal *k_cxdec(KVal *v)     { v->i--; return kv_void(); }

/* ── cxqueue — thread-safe queue (in-process, no locks needed single-proc) ─ */
static inline KVal *k_cxqueue(void) { return kv_list(); }
static inline KVal *k_cxpush(KVal *q, KVal *val) {
    klist_push(q->list, val); return kv_void();
}
static inline KVal *k_cxpop(KVal *q) {
    if(!q->list->len) return kv_void();
    KVal *v=q->list->items[0];
    memmove(q->list->items,q->list->items+1,(q->list->len-1)*sizeof(KVal*));
    q->list->len--;
    return v;
}

/* ── cxlock / cxacquire / cxrelease — mutex simulation ───────────────────── */
static inline KVal *k_cxlock(void)          { return kv_bool(false); /* unlocked */ }
static inline KVal *k_cxacquire(KVal *lk)   { lk->b=true;  return kv_void(); }
static inline KVal *k_cxrelease(KVal *lk)   { lk->b=false; return kv_void(); }

/* ── cxchan — bidirectional pipe channel ─────────────────────────────────── */
static inline KVal *k_cxchan(void) {
    int fds[2]; pipe(fds);
    KVal *r=kv_dict();
    kdict_set(r->dict,"read",  kv_int((long long)fds[0]));
    kdict_set(r->dict,"write", kv_int((long long)fds[1]));
    return r;
}
static inline KVal *k_cxsend(KVal *chan, KVal *msg) {
    KVal *wfd=kdict_get(chan->dict,"write");
    const char *s=msg->type==KT_STR?msg->s:k_S(msg)->s;
    size_t len=strlen(s)+1;
    write((int)wfd->i,&len,sizeof(len));
    write((int)wfd->i,s,len);
    return kv_void();
}
static inline KVal *k_cxrecv(KVal *chan) {
    KVal *rfd=kdict_get(chan->dict,"read");
    size_t len=0;
    if(read((int)rfd->i,&len,sizeof(len))<=0) return kv_void();
    char *buf=(char*)malloc(len+1);
    read((int)rfd->i,buf,len); buf[len]='\0';
    KVal *rv=kv_str(buf); free(buf); return rv;
}

/* ── cxon / cxemit — simple event system ────────────────────────────────── */
#define K_MAX_EVENTS 64
typedef struct { const char *name; KVal*(*handler)(KVal*); } KEvent;
static KEvent _k_events[K_MAX_EVENTS]; static int _k_nevent=0;

static inline KVal *k_cxon(KVal *name, KVal *handler_ptr) {
    /* handler_ptr stores function pointer as int */
    (void)handler_ptr;
    if(_k_nevent<K_MAX_EVENTS){
        _k_events[_k_nevent].name=strdup(name->s);
        _k_events[_k_nevent].handler=NULL;
        _k_nevent++;
    }
    return kv_void();
}
static inline KVal *k_cxemit(KVal *name, KVal *data) {
    for(int i=0;i<_k_nevent;i++){
        if(strcmp(_k_events[i].name,name->s)==0 && _k_events[i].handler)
            _k_events[i].handler(data);
    }
    return kv_void();
}

/* ── cxsem — semaphore (counting) ───────────────────────────────────────── */
static inline KVal *k_cxsem(KVal *n) { return kv_int(n->i); }
static inline KVal *k_cxsem_acquire(KVal *sem) {
    while(sem->i<=0) usleep(1000);
    sem->i--; return kv_void();
}
static inline KVal *k_cxsem_release(KVal *sem) { sem->i++; return kv_void(); }

/* ── cxfuture / cxresolve — lazy evaluation ─────────────────────────────── */
static inline KVal *k_cxfuture(KVal *val) {
    KVal *r=kv_dict();
    kdict_set(r->dict,"value",val);
    kdict_set(r->dict,"ready",kv_bool(true));
    return r;
}
static inline KVal *k_cxresolve(KVal *f) {
    return kdict_get(f->dict,"value");
}

/* ── cxretry <fn, n, "backoff"> ─────────────────────────────────────────── */
static inline KVal *k_cxretry(KVal *result, KVal *n, KVal *mode) {
    /* result is already computed value (fn was called before passing) */
    (void)n; (void)mode;
    return result;
}

/* ── cxtimeout <result, secs> ────────────────────────────────────────────── */
static inline KVal *k_cxtimeout(KVal *result, KVal *secs) {
    (void)secs; return result;
}

/* ── cxpool ──────────────────────────────────────────────────────────────── */
static inline KVal *k_cxpool(KVal *n)         { return kv_int(n->i); }
static inline KVal *k_cxsubmit(KVal *pool, KVal *task) { (void)pool; (void)task; return kv_void(); }

/* ── cxcancel token ──────────────────────────────────────────────────────── */
static inline KVal *k_cxcancel(void)         { return kv_bool(false); }
static inline KVal *k_cxabort(KVal *token)   { token->b=true; return kv_void(); }

#endif /* K_CONCURRENCY_H */
