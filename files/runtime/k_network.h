/*
 * k_network.h  —  Katlans Networking  (net prefix)
 * HTTP/1.1 client, DNS, ping — works on POSIX (BSD sockets) and Windows (Winsock2).
 */
#ifndef K_NETWORK_H
#define K_NETWORK_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
/* Winsock2 must be included before windows.h (if used elsewhere) */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

/* One-time Winsock initialisation */
static inline int _knet_init(void) {
    static int done = 0;
    if (!done) {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) return -1;
        done = 1;
    }
    return 0;
}
#define _knet_close(fd)  closesocket(fd)
#define KNET_ERRNO       WSAGetLastError()
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#define _knet_init()      0
#define _knet_close(fd)   close(fd)
#define KNET_ERRNO        errno
#endif

#define K_NET_BUFSZ (1024*1024)   /* 1 MB response buffer */
#define K_NET_HDRSZ (65536)

/* ── Parse URL → host, path, port, https ────────────────────────────────── */
typedef struct {
    char host[512]; char path[2048]; int port; int https;
} KURL;

static int _kurl_parse(const char *url, KURL *out) {
    memset(out,0,sizeof(*out)); strcpy(out->path,"/");
    out->port = 80;
    const char *p = url;
    if(strncmp(p,"https://",8)==0){out->https=1;out->port=443;p+=8;}
    else if(strncmp(p,"http://",7)==0){p+=7;}
    else return -1;
    const char *slash=strchr(p,'/');
    const char *colon=strchr(p,':');
    if(colon && (!slash || colon<slash)){
        size_t hl=colon-p; if(hl>=512) return -1;
        strncpy(out->host,p,hl); out->host[hl]='\0';
        out->port=atoi(colon+1);
        if(slash) strcpy(out->path,slash);
    } else {
        size_t hl=slash ? (size_t)(slash-p) : strlen(p);
        if(hl>=512) return -1;
        strncpy(out->host,p,hl); out->host[hl]='\0';
        if(slash) strcpy(out->path,slash);
    }
    return 0;
}

/* ── Open TCP socket to host:port ────────────────────────────────────────── */
static int _knet_connect(const char *host, int port) {
    if (_knet_init() != 0) return -1;
    struct addrinfo hints={0}, *res;
    hints.ai_family=AF_INET; hints.ai_socktype=SOCK_STREAM;
    char portstr[8]; snprintf(portstr,sizeof(portstr),"%d",port);
    int gai = getaddrinfo(host,portstr,&hints,&res);
    if(gai!=0) return -1;
    int fd=(int)socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(fd<0){freeaddrinfo(res);return -1;}
    if(connect(fd,res->ai_addr,(int)res->ai_addrlen)!=0){_knet_close(fd);freeaddrinfo(res);return -1;}
    freeaddrinfo(res);
    return fd;
}

/* ── Send HTTP/1.1 request, return full response ─────────────────────────── */
static KVal *_knet_request(const char *method, const char *url,
                           const char *body, const char *headers_extra,
                           int timeout_secs) {
    KURL u; if(_kurl_parse(url,&u)<0) {
        KVal *r=kv_dict();
        kdict_set(r->dict,"status",kv_int(0));
        kdict_set(r->dict,"body",  kv_str("Invalid URL"));
        kdict_set(r->dict,"ok",    kv_bool(false));
        return r;
    }

    int fd = _knet_connect(u.host, u.port);
    if(fd<0) {
        KVal *r=kv_dict();
        kdict_set(r->dict,"status",kv_int(0));
        kdict_set(r->dict,"body",  kv_str("Connection failed"));
        kdict_set(r->dict,"ok",    kv_bool(false));
        return r;
    }

    /* Set timeout */
    if(timeout_secs>0){
#ifdef _WIN32
        DWORD tv = timeout_secs * 1000; /* Windows uses milliseconds */
        setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv));
        setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&tv,sizeof(tv));
#else
        struct timeval tv={timeout_secs,0};
        setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
        setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(tv));
#endif
    }

    /* Build request */
    char req[K_NET_HDRSZ];
    int body_len = body ? (int)strlen(body) : 0;
    int rlen = snprintf(req,sizeof(req),
        "%s %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: Katlans/0.1\r\n"
        "Connection: close\r\n"
        "Accept: */*\r\n"
        "%s"
        "%s%s%s"
        "\r\n",
        method, u.path, u.host,
        headers_extra ? headers_extra : "",
        body_len>0 ? "Content-Type: application/json\r\nContent-Length: " : "",
        body_len>0 ? (char[32]){0} : "",
        "");
    if(body_len>0){
        char clen[32]; snprintf(clen,sizeof(clen),"%d\r\n\r\n",body_len);
        int cl=strlen(req); snprintf(req+cl,sizeof(req)-cl,"%s",clen);
    }
    send(fd,req,(int)strlen(req),0);
    if(body && body_len>0) send(fd,body,(int)body_len,0);

    /* Read response */
    char *buf=(char*)malloc(K_NET_BUFSZ); int total=0;
    while(total<K_NET_BUFSZ-1){
        int n=recv(fd,buf+total,K_NET_BUFSZ-1-total,0);
        if(n<=0) break; total+=n;
    }
    buf[total]='\0'; _knet_close(fd);

    /* Parse status line */
    int status=0;
    if(strncmp(buf,"HTTP/",5)==0){
        char *sp=strchr(buf,' ');
        if(sp) status=atoi(sp+1);
    }

    /* Split headers and body */
    char *body_start=strstr(buf,"\r\n\r\n");
    char *resp_body = body_start ? body_start+4 : buf;

    /* Extract headers as dict */
    KVal *hdrs=kv_dict();
    if(body_start){
        char hdrbuf[K_NET_HDRSZ]; int hl=(int)(body_start-buf);
        if(hl>K_NET_HDRSZ-1) hl=K_NET_HDRSZ-1;
        strncpy(hdrbuf,buf,hl); hdrbuf[hl]='\0';
        char *line=strtok(hdrbuf,"\r\n");
        if (line) line=strtok(NULL,"\r\n");  /* skip status line */
        while(line){
            char *colon=strchr(line,':');
            if(colon){
                *colon='\0'; char *val=colon+1;
                while(*val==' ') val++;
                kdict_set(hdrs->dict,line,kv_str(val));
            }
            line=strtok(NULL,"\r\n");
        }
    }

    KVal *r=kv_dict();
    kdict_set(r->dict,"status",  kv_int(status));
    kdict_set(r->dict,"body",    kv_str(resp_body));
    kdict_set(r->dict,"headers", hdrs);
    kdict_set(r->dict,"url",     kv_str(url));
    kdict_set(r->dict,"ok",      kv_bool(status>=200&&status<300));
    free(buf);
    return r;
}

/* ── Public HTTP methods ─────────────────────────────────────────────────── */
static inline KVal *k_netget(KVal *url)              { return _knet_request("GET",  url->s, NULL, NULL, 30); }
static inline KVal *k_netdel(KVal *url)              { return _knet_request("DELETE",url->s,NULL, NULL, 30); }
static inline KVal *k_netpost(KVal *url, KVal *body) { return _knet_request("POST", url->s, body->type==KT_STR?body->s:NULL, NULL, 30); }
static inline KVal *k_netput(KVal *url,  KVal *body) { return _knet_request("PUT",  url->s, body->type==KT_STR?body->s:NULL, NULL, 30); }
static inline KVal *k_netpatch(KVal *url,KVal *body) { return _knet_request("PATCH",url->s, body->type==KT_STR?body->s:NULL, NULL, 30); }

/* netget with options: timeout, auth, etc. */
static inline KVal *k_netget_timeout(KVal *url, KVal *secs) {
    return _knet_request("GET", url->s, NULL, NULL, (int)_knum(secs));
}

/* ── netjson <url> → dict ────────────────────────────────────────────────── */
static inline KVal *k_netjson(KVal *url) {
    KVal *resp = k_netget(url);
    KVal *body = kdict_get(resp->dict, "body");
    if(!body || body->type != KT_STR) return kv_void();
    extern KVal *k_jsparse(KVal*);
    return k_jsparse(body);
}

/* ── netdns <"domain"> → IP string ──────────────────────────────────────── */
static inline KVal *k_netdns(KVal *host) {
    if (_knet_init() != 0) return kv_str("init failed");
    struct addrinfo hints={0}, *res;
    hints.ai_family=AF_INET;
    if(getaddrinfo(host->s,"80",&hints,&res)!=0) return kv_str("not found");
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in *sa=(struct sockaddr_in*)res->ai_addr;
    inet_ntop(AF_INET,&sa->sin_addr,ip,sizeof(ip));
    freeaddrinfo(res);
    return kv_str(ip);
}

/* ── netping <"host"> → true/false (checks if TCP port 80 is reachable) ──── */
static inline KVal *k_netping(KVal *host) {
    int fd = _knet_connect(host->s, 80);
    if(fd<0) return kv_bool(false);
    _knet_close(fd); return kv_bool(true);
}

/* ── netdl <url, path> — download file ──────────────────────────────────── */
static inline KVal *k_netdl(KVal *url, KVal *path) {
    KVal *resp = k_netget(url);
    KVal *body = kdict_get(resp->dict, "body");
    if(!body || body->type!=KT_STR) return kv_bool(false);
    FILE *f=fopen(path->s,"wb");
    if(!f) return kv_bool(false);
    fwrite(body->s,1,strlen(body->s),f); fclose(f);
    return kv_bool(true);
}

#endif /* K_NETWORK_H */
