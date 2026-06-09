/* k_os.h — Katlans OS Operations (os prefix) — cross-platform */
#ifndef K_OS_H
#define K_OS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>
#include <lmcons.h>
#define getcwd _getcwd
#define chdir _chdir
#define F_OK 0
#define access _access
#define WEXITSTATUS(s) ((s) & 0xff)
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#endif

static inline KVal *k_osrun(KVal *cmd) {
    char full[4096];
#ifdef _WIN32
    snprintf(full, sizeof(full), "%s", cmd->s);
#else
    char tmp_out[64] = "/tmp/k_out_XXXXXX";
    char tmp_err[64] = "/tmp/k_err_XXXXXX";
    int fo = mkstemp(tmp_out); int fe = mkstemp(tmp_err);
    close(fo); close(fe);
    snprintf(full, sizeof(full), "%s >%s 2>%s", cmd->s, tmp_out, tmp_err);
#endif
    int code = system(full);
    KVal *r = kv_dict();
#ifdef _WIN32
    kdict_set(r->dict, "out",  kv_str(""));
    kdict_set(r->dict, "err",  kv_str(""));
    kdict_set(r->dict, "code", kv_int(code));
#else
    KVal *out_v = k_fread(kv_str(tmp_out));
    KVal *err_v = k_fread(kv_str(tmp_err));
    remove(tmp_out); remove(tmp_err);
    kdict_set(r->dict, "out",  out_v->type==KT_VOID ? kv_str("") : out_v);
    kdict_set(r->dict, "err",  err_v->type==KT_VOID ? kv_str("") : err_v);
    kdict_set(r->dict, "code", kv_int(WEXITSTATUS(code)));
#endif
    return r;
}

static inline KVal *k_osget(KVal *var) {
    char *v = getenv(var->s);
    return v ? kv_str(v) : kv_void();
}

static inline KVal *k_onset(KVal *var, KVal *val) {
#ifdef _WIN32
    char buf[8192]; snprintf(buf,sizeof(buf),"%s=%s",var->s,val->s);
    return kv_bool(_putenv(buf) == 0);
#else
    return kv_bool(setenv(var->s, val->s, 1) == 0);
#endif
}

static inline KVal *k_ospwd(void) {
    char buf[4096];
    return getcwd(buf, sizeof(buf)) ? kv_str(buf) : kv_void();
}

static inline KVal *k_oscd(KVal *path) {
    return kv_bool(chdir(path->s) == 0);
}

static inline KVal *k_ossys(void) {
    KVal *r = kv_dict();
#ifdef _WIN32
    OSVERSIONINFOW osvi; ZeroMemory(&osvi, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionExW(&osvi);
    SYSTEM_INFO si; GetSystemInfo(&si);
    const char *arch = "x86_64";
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64) arch = "arm64";
    else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) arch = "x86";
    char buf[128]; snprintf(buf,sizeof(buf),"%d.%d",(int)osvi.dwMajorVersion,(int)osvi.dwMinorVersion);
    kdict_set(r->dict,"os",kv_str("Windows"));
    kdict_set(r->dict,"arch",kv_str(arch));
    kdict_set(r->dict,"release",kv_str(buf));
    kdict_set(r->dict,"hostname",kv_str("localhost"));
    kdict_set(r->dict,"version",kv_str("Windows"));
    kdict_set(r->dict,"home",kv_str(getenv("USERPROFILE")?getenv("USERPROFILE"):""));
    kdict_set(r->dict,"shell",kv_str("cmd.exe"));
    kdict_set(r->dict,"lang",kv_str(getenv("LANG")?getenv("LANG"):""));
    kdict_set(r->dict,"timezone",kv_str("UTC"));
    char username[UNLEN+1]; DWORD unlen = UNLEN+1;
    GetUserNameA(username,&unlen);
    kdict_set(r->dict,"username",kv_str(username));
#else
    struct utsname u; uname(&u);
    kdict_set(r->dict,"os",kv_str(u.sysname));
    kdict_set(r->dict,"hostname",kv_str(u.nodename));
    kdict_set(r->dict,"arch",kv_str(u.machine));
    kdict_set(r->dict,"release",kv_str(u.release));
    kdict_set(r->dict,"version",kv_str(u.version));
    char *home = getenv("HOME");
    kdict_set(r->dict,"home",home?kv_str(home):kv_str(""));
    char *shell = getenv("SHELL");
    kdict_set(r->dict,"shell",shell?kv_str(shell):kv_str(""));
    char *lang = getenv("LANG");
    kdict_set(r->dict,"lang",lang?kv_str(lang):kv_str(""));
    char *tz = getenv("TZ");
    kdict_set(r->dict,"timezone",tz?kv_str(tz):kv_str("UTC"));
    struct passwd *pw = getpwuid(getuid());
    kdict_set(r->dict,"username",pw?kv_str(pw->pw_name):kv_str(""));
#endif
    return r;
}

static inline KVal *k_osram(void) {
    KVal *r = kv_dict();
#ifdef _WIN32
    MEMORYSTATUSEX ms; ms.dwLength = sizeof(ms);
    GlobalMemoryStatusEx(&ms);
    char buf[32];
    snprintf(buf,sizeof(buf),"%.1fGB",(double)ms.ullTotalPhys/1e9); kdict_set(r->dict,"total",kv_str(buf));
    snprintf(buf,sizeof(buf),"%.1fGB",(double)(ms.ullTotalPhys-ms.ullAvailPhys)/1e9); kdict_set(r->dict,"used",kv_str(buf));
    snprintf(buf,sizeof(buf),"%.1fGB",(double)ms.ullAvailPhys/1e9); kdict_set(r->dict,"free",kv_str(buf));
    kdict_set(r->dict,"usage",kv_flt(ms.ullTotalPhys>0?(double)(ms.ullTotalPhys-ms.ullAvailPhys)/ms.ullTotalPhys*100:0));
    snprintf(buf,sizeof(buf),"%.1fGB",(double)ms.ullTotalPageFile/1e9); kdict_set(r->dict,"swap",kv_str(buf));
    snprintf(buf,sizeof(buf),"%.1fGB",(double)(ms.ullTotalPageFile-ms.ullAvailPageFile)/1e9); kdict_set(r->dict,"swapused",kv_str(buf));
#else
    struct sysinfo si; sysinfo(&si);
    long long total = (long long)si.totalram * si.mem_unit;
    long long free_ = (long long)si.freeram * si.mem_unit;
    long long used  = total - free_;
    char buf[32];
    snprintf(buf,sizeof(buf),"%.1fGB", total/1e9); kdict_set(r->dict,"total",kv_str(buf));
    snprintf(buf,sizeof(buf),"%.1fGB", used /1e9); kdict_set(r->dict,"used", kv_str(buf));
    snprintf(buf,sizeof(buf),"%.1fGB", free_/1e9); kdict_set(r->dict,"free", kv_str(buf));
    kdict_set(r->dict,"usage", kv_flt(total>0?(double)used/total*100:0));
    long long stotal = (long long)si.totalswap * si.mem_unit;
    long long sfree  = (long long)si.freeswap * si.mem_unit;
    snprintf(buf,sizeof(buf),"%.1fGB",stotal/1e9); kdict_set(r->dict,"swap",kv_str(buf));
    snprintf(buf,sizeof(buf),"%.1fGB",(stotal-sfree)/1e9); kdict_set(r->dict,"swapused",kv_str(buf));
#endif
    return r;
}

static inline KVal *k_osuptime(void) {
    KVal *r = kv_dict();
#ifdef _WIN32
    long long up = (long long)(GetTickCount64() / 1000ULL);
#else
    struct sysinfo si; sysinfo(&si);
    long long up = (long long)si.uptime;
#endif
    long long d  = up / 86400; up %= 86400;
    long long h  = up / 3600;  up %= 3600;
    long long m  = up / 60;    up %= 60;
    char buf[64];
    snprintf(buf, sizeof(buf), "%lldd %lldh %lldm %llds", d, h, m, up);
    kdict_set(r->dict,"days",kv_int(d));
    kdict_set(r->dict,"hours",kv_int(h));
    kdict_set(r->dict,"mins",kv_int(m));
    kdict_set(r->dict,"secs",kv_int(up));
    kdict_set(r->dict,"total",kv_str(buf));
    return r;
}

static inline KVal *k_osboot(void) {
#ifdef _WIN32
    long long up = (long long)(GetTickCount64() / 1000ULL);
#else
    struct sysinfo si; sysinfo(&si);
    long long up = (long long)si.uptime;
#endif
    time_t boot = time(NULL) - (time_t)up;
    char buf[64]; struct tm *t = localtime(&boot);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return kv_str(buf);
}

static inline KVal *k_osuser(void) {
    KVal *r = kv_dict();
#ifdef _WIN32
    char username[UNLEN+1]; DWORD unlen = UNLEN+1;
    GetUserNameA(username, &unlen);
    kdict_set(r->dict,"name",kv_str(username));
    kdict_set(r->dict,"id",kv_int(0));
    kdict_set(r->dict,"home",kv_str(getenv("USERPROFILE")?getenv("USERPROFILE"):""));
    kdict_set(r->dict,"shell",kv_str("cmd.exe"));
    kdict_set(r->dict,"admin",kv_bool(false));
#else
    struct passwd *pw = getpwuid(getuid());
    kdict_set(r->dict,"name",pw?kv_str(pw->pw_name):kv_str(""));
    kdict_set(r->dict,"id",kv_int((long long)getuid()));
    kdict_set(r->dict,"home",pw?kv_str(pw->pw_dir):kv_str(""));
    kdict_set(r->dict,"shell",pw?kv_str(pw->pw_shell):kv_str(""));
    kdict_set(r->dict,"admin",kv_bool(getuid()==0));
#endif
    return r;
}

static inline KVal *k_oshalt(KVal *code) {
    exit((int)code->i); return kv_void();
}

static inline KVal *k_osjoin(KVal *lst) {
    char buf[4096] = "";
    for (size_t i = 0; i < lst->list->len; i++) {
        char *s = lst->list->items[i]->s;
        if (i > 0 && buf[strlen(buf)-1] != '/' && s[0] != '/')
            strcat(buf, "/");
        strcat(buf, s);
    }
    return kv_str(buf);
}

extern KVal *k_fbase(KVal*);
extern KVal *k_fdir(KVal*);
extern KVal *k_fext(KVal*);
static inline KVal *k_osbase(KVal *p) { return k_fbase(p); }
static inline KVal *k_osdir(KVal *p)  { return k_fdir(p);  }
static inline KVal *k_osext(KVal *p)  { return k_fext(p);  }
static inline KVal *k_ospath(KVal *p) {
    return kv_bool(access(p->s, F_OK) == 0);
}

static inline KVal *k_osopen(KVal *target) {
    char cmd[4096];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "start \"\" \"%s\"", target->s);
#else
    snprintf(cmd, sizeof(cmd), "xdg-open \"%s\" 2>/dev/null || open \"%s\" 2>/dev/null &", target->s, target->s);
#endif
    system(cmd);
    return kv_void();
}

static inline KVal *k_osclipget(void) {
#ifdef _WIN32
    FILE *p = popen("powershell -command \"Get-Clipboard\"", "r");
#else
    FILE *p = popen("xclip -o -selection clipboard 2>/dev/null || pbpaste 2>/dev/null", "r");
#endif
    if (!p) return kv_str("");
    char buf[65536]; size_t n = fread(buf, 1, sizeof(buf)-1, p);
    buf[n] = '\0'; pclose(p);
    return kv_str(buf);
}
static inline KVal *k_osclipset(KVal *val) {
    char cmd[65600];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "echo %s | clip", val->s);
#else
    snprintf(cmd, sizeof(cmd), "echo -n '%s' | xclip -selection clipboard 2>/dev/null || echo -n '%s' | pbcopy 2>/dev/null", val->s, val->s);
#endif
    system(cmd);
    return kv_void();
}

static inline KVal *k_osnoti(KVal *title, KVal *body) {
    char cmd[1024];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "powershell -command \"New-BaloonTip -Title '%s' -Text '%s'\" 2>/dev/null", title->s, body->s);
    (void)cmd; /* Windows notification not trivial without a helper app */
#else
    snprintf(cmd, sizeof(cmd), "notify-send \"%s\" \"%s\" 2>/dev/null &", title->s, body->s);
    system(cmd);
#endif
    return kv_void();
}

#endif /* K_OS_H */
