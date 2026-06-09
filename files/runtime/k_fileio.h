/* k_fileio.h — Katlans File I/O (f prefix) */
#ifndef K_FILEIO_H
#define K_FILEIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define F_OK 0
#define mkdir(p,m) _mkdir(p)
#define access _access
#else
#include <unistd.h>
#include <dirent.h>
#endif

static inline KVal *k_fread(KVal *path) {
    FILE *f = fopen(path->s, "rb");
    if (!f) return kv_void();
    fseek(f,0,SEEK_END); long sz=ftell(f); rewind(f);
    char *buf=(char*)malloc(sz+1);
    size_t n=fread(buf,1,sz,f); fclose(f);
    buf[n]='\0'; KVal *rv=kv_str(buf); free(buf); return rv;
}
static inline KVal *k_fwrite(KVal *path, KVal *content) {
    FILE *f=fopen(path->s,"wb"); if(!f) return kv_bool(false);
    fwrite(content->s,1,strlen(content->s),f); fclose(f); return kv_bool(true);
}
static inline KVal *k_fadd(KVal *path, KVal *content) {
    FILE *f=fopen(path->s,"ab"); if(!f) return kv_bool(false);
    fwrite(content->s,1,strlen(content->s),f); fclose(f); return kv_bool(true);
}
static inline KVal *k_fdel(KVal *path)   { return kv_bool(remove(path->s)==0); }
static inline KVal *k_fexists(KVal *path){ return kv_bool(access(path->s,F_OK)==0); }
static inline KVal *k_frename(KVal *o,KVal *n){ return kv_bool(rename(o->s,n->s)==0); }
static inline KVal *k_fcopy(KVal *src,KVal *dst){
    KVal *c=k_fread(src); if(c->type==KT_VOID) return kv_bool(false);
    return k_fwrite(dst,c);
}
static inline KVal *k_fsize(KVal *path){
    struct stat st; if(stat(path->s,&st)!=0) return kv_int(-1);
    return kv_int((long long)st.st_size);
}
static inline KVal *k_flist(KVal *path){
    KVal *lst=kv_list();
#ifdef _WIN32
    /* Windows: use FindFirstFile/FindNextFile */
    char pattern[4096]; snprintf(pattern,sizeof(pattern),"%s/*",path->s);
    struct _finddata_t fd; intptr_t h = _findfirst(pattern, &fd);
    if (h == -1) return lst;
    do {
        if(strcmp(fd.name,".")==0||strcmp(fd.name,"..")==0) continue;
        klist_push(lst->list,kv_str(fd.name));
    } while(_findnext(h, &fd) == 0);
    _findclose(h);
#else
    DIR *d=opendir(path->s); if(!d) return lst;
    struct dirent *e;
    while((e=readdir(d))){
        if(strcmp(e->d_name,".")==0||strcmp(e->d_name,"..")==0) continue;
        klist_push(lst->list,kv_str(e->d_name));
    }
    closedir(d);
#endif
    return lst;
}
static inline KVal *k_fmkdir(KVal *path){ return kv_bool(mkdir(path->s,0755)==0||errno==EEXIST); }
static inline KVal *k_frmdir(KVal *path){ return kv_bool(rmdir(path->s)==0); }
static inline KVal *k_fisdir(KVal *path){
    struct stat st; if(stat(path->s,&st)!=0) return kv_bool(false);
    return kv_bool(S_ISDIR(st.st_mode));
}
static inline KVal *k_fisfile(KVal *path){
    struct stat st; if(stat(path->s,&st)!=0) return kv_bool(false);
    return kv_bool(S_ISREG(st.st_mode));
}
static inline KVal *k_fext(KVal *path){
    char *dot=strrchr(path->s,'.'); if(!dot||dot==path->s) return kv_str("");
    return kv_str(dot);
}
static inline KVal *k_fbase(KVal *path){
    char *sl=strrchr(path->s,'/'); if(!sl) sl=strrchr(path->s,'\\');
    return kv_str(sl?sl+1:path->s);
}
static inline KVal *k_fdir(KVal *path){
    char *r=strdup(path->s); char *sl=strrchr(r,'/');
    if(!sl) sl=strrchr(r,'\\');
    if(sl) *sl='\0'; else r[0]='\0';
    KVal *rv=kv_str(r); free(r); return rv;
}
static inline KVal *k_fjoin(KVal *lst){
    char buf[4096]="";
    for(size_t i=0;i<lst->list->len;i++){
        if(i&&strlen(buf)&&buf[strlen(buf)-1]!='/') strcat(buf,"/");
        strcat(buf,lst->list->items[i]->s);
    }
    return kv_str(buf);
}
static inline KVal *k_freadlines(KVal *path){
    KVal *c=k_fread(path); if(c->type==KT_VOID) return kv_list();
    extern KVal *k_Schop(KVal*,KVal*);
    return k_Schop(c,kv_str("\n"));
}
static inline KVal *k_fwritelines(KVal *path,KVal *lines){
    FILE *f=fopen(path->s,"wb"); if(!f) return kv_bool(false);
    for(size_t i=0;i<lines->list->len;i++) fprintf(f,"%s\n",lines->list->items[i]->s);
    fclose(f); return kv_bool(true);
}

#endif /* K_FILEIO_H */
