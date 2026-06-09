/*
 * k_datetime.h  —  Katlans Date & Time  (dt prefix)
 */
#ifndef K_DATETIME_H
#define K_DATETIME_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ── dtnow() → adero with all fields ────────────────────────────────────── */
static inline KVal *k_dtnow(void) {
    time_t t = time(NULL); struct tm *tm = localtime(&t);
    KVal *r = kv_dict();
    kdict_set(r->dict,"year",  kv_int(tm->tm_year+1900));
    kdict_set(r->dict,"month", kv_int(tm->tm_mon+1));
    kdict_set(r->dict,"day",   kv_int(tm->tm_mday));
    kdict_set(r->dict,"hour",  kv_int(tm->tm_hour));
    kdict_set(r->dict,"min",   kv_int(tm->tm_min));
    kdict_set(r->dict,"sec",   kv_int(tm->tm_sec));
    kdict_set(r->dict,"week",  kv_int((tm->tm_yday/7)+1));
    kdict_set(r->dict,"wday",  kv_int(tm->tm_wday));
    kdict_set(r->dict,"yday",  kv_int(tm->tm_yday+1));
    kdict_set(r->dict,"ms",    kv_int(0));
    kdict_set(r->dict,"_ts",   kv_int((long long)t));
    char buf[32]; strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",tm);
    kdict_set(r->dict,"_str",  kv_str(buf));
    return r;
}

static inline KVal *k_dtdate(void) {
    time_t t = time(NULL); struct tm *tm = localtime(&t);
    char buf[16]; strftime(buf,sizeof(buf),"%Y-%m-%d",tm);
    return kv_str(buf);
}

static inline KVal *k_dttime(void) {
    time_t t = time(NULL); struct tm *tm = localtime(&t);
    char buf[12]; strftime(buf,sizeof(buf),"%H:%M:%S",tm);
    return kv_str(buf);
}

/* ── dtformat <dt, "DD-MM-YYYY"> ────────────────────────────────────────── */
static inline KVal *k_dtformat(KVal *dt, KVal *fmt) {
    long long ts = 0;
    if (dt->type == KT_DICT) {
        KVal *v = kdict_get(dt->dict,"_ts"); ts = v->i;
    } else if (dt->type == KT_INT) { ts = dt->i; }
    time_t t = (time_t)ts; struct tm *tm = localtime(&t);
    /* Convert Katlans format to strftime format */
    char sfmt[256] = "";
    const char *p = fmt->s;
    while (*p) {
        if      (strncmp(p,"YYYY",4)==0) { strcat(sfmt,"%Y"); p+=4; }
        else if (strncmp(p,"YY",2)==0)   { strcat(sfmt,"%y"); p+=2; }
        else if (strncmp(p,"MM",2)==0)   { strcat(sfmt,"%m"); p+=2; }
        else if (strncmp(p,"DD",2)==0)   { strcat(sfmt,"%d"); p+=2; }
        else if (strncmp(p,"HH",2)==0)   { strcat(sfmt,"%H"); p+=2; }
        else if (strncmp(p,"mm",2)==0)   { strcat(sfmt,"%M"); p+=2; }
        else if (strncmp(p,"SS",2)==0)   { strcat(sfmt,"%S"); p+=2; }
        else { int n=strlen(sfmt); sfmt[n]=*p; sfmt[n+1]='\0'; p++; }
    }
    char buf[128]; strftime(buf,sizeof(buf),sfmt,tm);
    return kv_str(buf);
}

/* ── dtparse <"2026-05-08","DD-MM-YYYY"> ────────────────────────────────── */
static inline KVal *k_dtparse(KVal *str, KVal *fmt) {
    struct tm tm = {0};
    /* Simple parser for common formats */
    const char *s=str->s, *f=fmt->s;
    while(*s && *f) {
        if      (strncmp(f,"YYYY",4)==0){tm.tm_year=atoi(s)-1900; s+=4; f+=4;}
        else if (strncmp(f,"MM",2)==0)  {tm.tm_mon=atoi(s)-1;     s+=2; f+=2;}
        else if (strncmp(f,"DD",2)==0)  {tm.tm_mday=atoi(s);      s+=2; f+=2;}
        else if (strncmp(f,"HH",2)==0)  {tm.tm_hour=atoi(s);      s+=2; f+=2;}
        else if (strncmp(f,"mm",2)==0)  {tm.tm_min=atoi(s);       s+=2; f+=2;}
        else if (strncmp(f,"SS",2)==0)  {tm.tm_sec=atoi(s);       s+=2; f+=2;}
        else { s++; f++; }
    }
    time_t t = mktime(&tm);
    KVal *r = kv_dict();
    kdict_set(r->dict,"_ts",  kv_int((long long)t));
    kdict_set(r->dict,"year", kv_int(tm.tm_year+1900));
    kdict_set(r->dict,"month",kv_int(tm.tm_mon+1));
    kdict_set(r->dict,"day",  kv_int(tm.tm_mday));
    kdict_set(r->dict,"hour", kv_int(tm.tm_hour));
    kdict_set(r->dict,"min",  kv_int(tm.tm_min));
    kdict_set(r->dict,"sec",  kv_int(tm.tm_sec));
    return r;
}

/* ── dtadd <dt, n, "day"> ────────────────────────────────────────────────── */
static inline KVal *k_dtadd(KVal *dt, KVal *n, KVal *unit) {
    long long ts=0;
    if(dt->type==KT_DICT){KVal*v=kdict_get(dt->dict,"_ts");ts=v->i;}
    else if(dt->type==KT_INT){ts=dt->i;}
    long long secs=n->i;
    const char *u=unit->s;
    if(strcmp(u,"sec")==0)        secs*=1;
    else if(strcmp(u,"min")==0)   secs*=60;
    else if(strcmp(u,"hr")==0)    secs*=3600;
    else if(strcmp(u,"day")==0)   secs*=86400;
    else if(strcmp(u,"week")==0)  secs*=604800;
    else if(strcmp(u,"month")==0) secs*=2592000;
    else if(strcmp(u,"year")==0)  secs*=31536000;
    time_t t=(time_t)(ts+secs); struct tm *tm=localtime(&t);
    KVal *r=kv_dict();
    kdict_set(r->dict,"_ts",  kv_int((long long)t));
    kdict_set(r->dict,"year", kv_int(tm->tm_year+1900));
    kdict_set(r->dict,"month",kv_int(tm->tm_mon+1));
    kdict_set(r->dict,"day",  kv_int(tm->tm_mday));
    kdict_set(r->dict,"hour", kv_int(tm->tm_hour));
    kdict_set(r->dict,"min",  kv_int(tm->tm_min));
    kdict_set(r->dict,"sec",  kv_int(tm->tm_sec));
    char buf[32]; strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",tm);
    kdict_set(r->dict,"_str", kv_str(buf));
    return r;
}

static inline KVal *k_dtsub(KVal *dt, KVal *n, KVal *unit) {
    KVal *neg=kv_int(-n->i); return k_dtadd(dt,neg,unit);
}

/* ── dtdiff <dt1, dt2, "day"> ────────────────────────────────────────────── */
static inline KVal *k_dtdiff(KVal *dt1, KVal *dt2, KVal *unit) {
    long long t1=0,t2=0;
    if(dt1->type==KT_DICT){KVal*v=kdict_get(dt1->dict,"_ts");t1=v->i;}
    else t1=dt1->i;
    if(dt2->type==KT_DICT){KVal*v=kdict_get(dt2->dict,"_ts");t2=v->i;}
    else t2=dt2->i;
    long long diff=t2-t1;
    const char *u=unit->s;
    if(strcmp(u,"min")==0)        diff/=60;
    else if(strcmp(u,"hr")==0)    diff/=3600;
    else if(strcmp(u,"day")==0)   diff/=86400;
    else if(strcmp(u,"week")==0)  diff/=604800;
    else if(strcmp(u,"month")==0) diff/=2592000;
    else if(strcmp(u,"year")==0)  diff/=31536000;
    return kv_int(diff);
}

/* ── dtconv <5, "min", "sec"> → 300 ─────────────────────────────────────── */
static inline KVal *k_dtconv(KVal *val, KVal *from, KVal *to) {
    /* Convert to seconds first, then to target unit */
    long long secs=val->i;
    const char *f=from->s;
    if(strcmp(f,"ms")==0)         secs/=1000;
    else if(strcmp(f,"min")==0)   secs*=60;
    else if(strcmp(f,"hr")==0)    secs*=3600;
    else if(strcmp(f,"day")==0)   secs*=86400;
    else if(strcmp(f,"week")==0)  secs*=604800;
    else if(strcmp(f,"month")==0) secs*=2592000;
    else if(strcmp(f,"year")==0)  secs*=31536000;
    const char *t=to->s;
    if(strcmp(t,"ms")==0)         return kv_int(secs*1000);
    else if(strcmp(t,"min")==0)   return kv_int(secs/60);
    else if(strcmp(t,"hr")==0)    return kv_int(secs/3600);
    else if(strcmp(t,"day")==0)   return kv_int(secs/86400);
    else if(strcmp(t,"week")==0)  return kv_int(secs/604800);
    else if(strcmp(t,"month")==0) return kv_int(secs/2592000);
    else if(strcmp(t,"year")==0)  return kv_int(secs/31536000);
    return kv_int(secs);
}

/* ── dtstamp <dt> → unix timestamp ──────────────────────────────────────── */
static inline KVal *k_dtstamp(KVal *dt) {
    if(dt->type==KT_DICT){KVal*v=kdict_get(dt->dict,"_ts");return kv_int(v->i);}
    return kv_int((long long)time(NULL));
}

/* ── dtfromstamp <ts> ────────────────────────────────────────────────────── */
static inline KVal *k_dtfromstamp(KVal *ts) {
    time_t t=(time_t)ts->i; struct tm *tm=localtime(&t);
    KVal *r=kv_dict();
    kdict_set(r->dict,"_ts",  kv_int(ts->i));
    kdict_set(r->dict,"year", kv_int(tm->tm_year+1900));
    kdict_set(r->dict,"month",kv_int(tm->tm_mon+1));
    kdict_set(r->dict,"day",  kv_int(tm->tm_mday));
    kdict_set(r->dict,"hour", kv_int(tm->tm_hour));
    kdict_set(r->dict,"min",  kv_int(tm->tm_min));
    kdict_set(r->dict,"sec",  kv_int(tm->tm_sec));
    char buf[32]; strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",tm);
    kdict_set(r->dict,"_str", kv_str(buf));
    return r;
}

/* ── dtwait <n> / dtwait <n, "ms"> ──────────────────────────────────────── */
static inline KVal *k_dtwait(KVal *n, KVal *unit) {
    long long us=n->i*1000000LL;
    if(unit && strcmp(unit->s,"ms")==0) us=n->i*1000LL;
    else if(unit && strcmp(unit->s,"ms")!=0) us=n->i*1000000LL;
    usleep((useconds_t)(us>0?us:0));
    return kv_void();
}

/* ── dtleap <year> ───────────────────────────────────────────────────────── */
static inline KVal *k_dtleap(KVal *year) {
    long long y=year->i;
    return kv_bool((y%4==0&&y%100!=0)||(y%400==0));
}

/* ── dtbefore / dtafter / dtequal ───────────────────────────────────────── */
static inline long long _dt_ts(KVal *dt){
    if(dt->type==KT_DICT){KVal*v=kdict_get(dt->dict,"_ts");return v->i;}
    return dt->i;
}
static inline KVal *k_dtbefore(KVal *a, KVal *b){return kv_bool(_dt_ts(a)<_dt_ts(b));}
static inline KVal *k_dtafter(KVal *a,  KVal *b){return kv_bool(_dt_ts(a)>_dt_ts(b));}
static inline KVal *k_dtequal(KVal *a,  KVal *b){return kv_bool(_dt_ts(a)==_dt_ts(b));}

/* ── dtzone <dt, "UTC", "IST"> ───────────────────────────────────────────── */
static inline KVal *k_dtzone(KVal *dt, KVal *from, KVal *to) {
    /* Simplified — apply known offsets */
    (void)from;
    long long ts=_dt_ts(dt);
    if(strcmp(to->s,"IST")==0)       ts+=19800;   /* UTC+5:30 */
    else if(strcmp(to->s,"EST")==0)  ts-=18000;
    else if(strcmp(to->s,"PST")==0)  ts-=28800;
    else if(strcmp(to->s,"CET")==0)  ts+=3600;
    else if(strcmp(to->s,"JST")==0)  ts+=32400;
    return k_dtfromstamp(kv_int(ts));
}

/* display for dt dict: show _str field */
static inline KVal *k_dtdisplay(KVal *dt) {
    if(dt->type==KT_DICT){
        KVal *s=kdict_get(dt->dict,"_str");
        if(s->type==KT_STR) return kv_str(s->s);
    }
    return k_S(dt);
}

#endif /* K_DATETIME_H */
