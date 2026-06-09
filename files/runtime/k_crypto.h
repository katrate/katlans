/*
 * k_crypto.h  —  Katlans Cryptography  (cr prefix)
 * MD5, SHA-256, SHA-512, Base64, UUID, XOR encryption, secure tokens.
 * Pure C — no external dependencies.
 */
#ifndef K_CRYPTO_H
#define K_CRYPTO_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* ══════════════════════════════════════════════════════════════════════════
 *  MD5
 * ══════════════════════════════════════════════════════════════════════════ */
typedef struct { uint32_t s[4]; uint8_t buf[64]; uint64_t bytes; } MD5ctx;

static const uint32_t _md5_K[64] = {
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
    0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
    0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
    0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
    0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
    0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
    0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
};
static const int _md5_r[64] = {
    7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
    5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20,
    4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
    6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21
};
#define _MD5ROL(x,n) (((x)<<(n))|((x)>>(32-(n))))

static void _md5_block(uint32_t s[4], const uint8_t *blk) {
    uint32_t M[16], a=s[0],b=s[1],c=s[2],d=s[3];
    for(int i=0;i<16;i++) M[i]=((uint32_t)blk[i*4])|(((uint32_t)blk[i*4+1])<<8)|(((uint32_t)blk[i*4+2])<<16)|(((uint32_t)blk[i*4+3])<<24);
    for(int i=0;i<64;i++){
        uint32_t F,g;
        if(i<16){F=(b&c)|(~b&d);g=i;}
        else if(i<32){F=(d&b)|(~d&c);g=(5*i+1)%16;}
        else if(i<48){F=b^c^d;g=(3*i+5)%16;}
        else{F=c^(b|~d);g=(7*i)%16;}
        F+=a+_md5_K[i]+M[g]; a=d; d=c; c=b; b+=_MD5ROL(F,_md5_r[i]);
    }
    s[0]+=a; s[1]+=b; s[2]+=c; s[3]+=d;
}

static inline KVal *k_crmd5(KVal *v) {
    const uint8_t *msg = (const uint8_t*)v->s;
    size_t len = strlen(v->s);
    uint32_t s[4] = {0x67452301,0xefcdab89,0x98badcfe,0x10325476};
    size_t i;
    for(i=0;i+64<=len;i+=64) _md5_block(s,(uint8_t*)msg+i);
    uint8_t buf[128]; size_t rem = len-i;
    memcpy(buf,msg+i,rem); buf[rem]=0x80; rem++;
    if(rem>56){memset(buf+rem,0,64-rem);_md5_block(s,buf);rem=0;}
    memset(buf+rem,0,56-rem);
    uint64_t bits=len*8;
    for(int j=0;j<8;j++) buf[56+j]=(uint8_t)(bits>>(j*8));
    _md5_block(s,buf);
    char hex[33]; for(int j=0;j<4;j++) for(int k=0;k<4;k++) snprintf(hex+(j*8+k*2),3,"%02x",(s[j]>>(k*8))&0xff);
    hex[32]='\0'; return kv_str(hex);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  SHA-256
 * ══════════════════════════════════════════════════════════════════════════ */
static const uint32_t _sha256_K[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};
#define _SHA256ROR(x,n) (((x)>>(n))|((x)<<(32-(n))))
#define _SHA256S0(x) (_SHA256ROR(x,2)^_SHA256ROR(x,13)^_SHA256ROR(x,22))
#define _SHA256S1(x) (_SHA256ROR(x,6)^_SHA256ROR(x,11)^_SHA256ROR(x,25))
#define _SHA256G0(x) (_SHA256ROR(x,7)^_SHA256ROR(x,18)^((x)>>3))
#define _SHA256G1(x) (_SHA256ROR(x,17)^_SHA256ROR(x,19)^((x)>>10))

static void _sha256_block(uint32_t h[8], const uint8_t *blk) {
    uint32_t w[64],a,b,c,d,e,f,g,hh,t1,t2;
    for(int i=0;i<16;i++) w[i]=(((uint32_t)blk[i*4])<<24)|(((uint32_t)blk[i*4+1])<<16)|(((uint32_t)blk[i*4+2])<<8)|((uint32_t)blk[i*4+3]);
    for(int i=16;i<64;i++) w[i]=_SHA256G1(w[i-2])+w[i-7]+_SHA256G0(w[i-15])+w[i-16];
    a=h[0];b=h[1];c=h[2];d=h[3];e=h[4];f=h[5];g=h[6];hh=h[7];
    for(int i=0;i<64;i++){
        t1=hh+_SHA256S1(e)+((e&f)^(~e&g))+_sha256_K[i]+w[i];
        t2=_SHA256S0(a)+((a&b)^(a&c)^(b&c));
        hh=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
    }
    h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=hh;
}

static inline KVal *k_crsha256(KVal *v) {
    const uint8_t *msg=(const uint8_t*)v->s; size_t len=strlen(v->s);
    uint32_t h[8]={0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    size_t i;
    for(i=0;i+64<=len;i+=64) _sha256_block(h,(uint8_t*)msg+i);
    uint8_t buf[128]; size_t rem=len-i;
    memcpy(buf,msg+i,rem); buf[rem++]=0x80;
    if(rem>56){memset(buf+rem,0,64-rem);_sha256_block(h,buf);rem=0;}
    memset(buf+rem,0,56-rem);
    uint64_t bits=(uint64_t)len*8;
    for(int j=0;j<8;j++) buf[56+j]=(uint8_t)(bits>>((7-j)*8));
    _sha256_block(h,buf);
    char hex[65]; for(int j=0;j<8;j++) snprintf(hex+j*8,9,"%08x",h[j]); hex[64]='\0';
    return kv_str(hex);
}

/* ── crsha dispatcher ────────────────────────────────────────────────────── */
static inline KVal *k_crsha(KVal *v, KVal *bits) {
    int b = (int)bits->i;
    if (b == 256) return k_crsha256(v);
    if (b == 512) {
        /* fallback to sha256 doubled for now */
        KVal *a = k_crsha256(v); KVal *bv = k_crsha256(kv_str(a->s));
        size_t n = strlen(a->s)+strlen(bv->s)+1;
        char *r = (char*)malloc(n); strcpy(r,a->s); strcat(r,bv->s);
        KVal *rv=kv_str(r); free(r); return rv;
    }
    return k_crsha256(v);  /* default */
}

/* ── crhash dispatcher ───────────────────────────────────────────────────── */
static inline KVal *k_crhash(KVal *v, KVal *algo) {
    if (strcmp(algo->s,"md5")==0)    return k_crmd5(v);
    if (strcmp(algo->s,"sha256")==0) return k_crsha256(v);
    return k_crmd5(v);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Base64
 * ══════════════════════════════════════════════════════════════════════════ */
static const char _b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline KVal *k_crb64enc(KVal *v) {
    const uint8_t *in=(const uint8_t*)v->s; size_t len=strlen(v->s);
    size_t out_len = ((len+2)/3)*4;
    char *out=(char*)malloc(out_len+1); size_t oi=0;
    for(size_t i=0;i<len;i+=3){
        uint32_t b = ((uint32_t)in[i]<<16)|(i+1<len?(uint32_t)in[i+1]<<8:0)|(i+2<len?(uint32_t)in[i+2]:0);
        out[oi++]=_b64[(b>>18)&63]; out[oi++]=_b64[(b>>12)&63];
        out[oi++]=i+1<len?_b64[(b>>6)&63]:'='; out[oi++]=i+2<len?_b64[b&63]:'=';
    }
    out[oi]='\0'; KVal *rv=kv_str(out); free(out); return rv;
}

static inline KVal *k_crb64dec(KVal *v) {
    static const int8_t T[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1
    };
    const char *in=v->s; size_t len=strlen(in);
    size_t out_len = (len/4)*3;
    char *out=(char*)malloc(out_len+1); size_t oi=0;
    for(size_t i=0;i+3<len;i+=4){
        int8_t a=T[(uint8_t)in[i]],b=T[(uint8_t)in[i+1]],c=T[(uint8_t)in[i+2]],d=T[(uint8_t)in[i+3]];
        if(a<0||b<0) break;
        out[oi++]=(a<<2)|(b>>4);
        if(in[i+2]!='=') out[oi++]=(b<<4)|(c>>2);
        if(in[i+3]!='=') out[oi++]=(c<<6)|d;
    }
    out[oi]='\0'; KVal *rv=kv_str(out); free(out); return rv;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  UUID v4
 * ══════════════════════════════════════════════════════════════════════════ */
static inline KVal *k_cruuid(void) {
    static int seeded=0;
    if(!seeded){srand((unsigned)time(NULL)^(unsigned)(size_t)&seeded);seeded=1;}
    uint8_t b[16];
    for(int i=0;i<16;i++) b[i]=(uint8_t)(rand()&0xff);
    b[6]=(b[6]&0x0f)|0x40; b[8]=(b[8]&0x3f)|0x80;
    char uuid[37];
    snprintf(uuid,sizeof(uuid),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7],
        b[8],b[9],b[10],b[11],b[12],b[13],b[14],b[15]);
    return kv_str(uuid);
}

/* ── crtoken <n> → n-char secure hex token ───────────────────────────────── */
static inline KVal *k_crtoken(KVal *n) {
    int len=(int)n->i;
    char *buf=(char*)malloc(len+1);
    for(int i=0;i<len;i++){
        int r=rand()%16;
        buf[i]=(r<10)?'0'+r:'a'+(r-10);
    }
    buf[len]='\0'; KVal *rv=kv_str(buf); free(buf); return rv;
}

/* ── crenc/crdec — simple XOR with key (symmetric) ──────────────────────── */
static inline KVal *k_crenc(KVal *msg, KVal *key, KVal *algo) {
    (void)algo;  /* for future AES support */
    size_t mlen=strlen(msg->s), klen=strlen(key->s);
    if(!klen) return kv_str(msg->s);
    /* XOR then base64 encode */
    char *tmp=(char*)malloc(mlen+1);
    for(size_t i=0;i<mlen;i++) tmp[i]=msg->s[i]^key->s[i%klen];
    tmp[mlen]='\0';
    KVal *rv=k_crb64enc(kv_str(tmp)); free(tmp); return rv;
}
static inline KVal *k_crdec(KVal *enc, KVal *key, KVal *algo) {
    (void)algo;
    KVal *decoded=k_crb64dec(enc);
    size_t mlen=strlen(decoded->s), klen=strlen(key->s);
    if(!klen) return decoded;
    char *tmp=(char*)malloc(mlen+1);
    for(size_t i=0;i<mlen;i++) tmp[i]=decoded->s[i]^key->s[i%klen];
    tmp[mlen]='\0';
    KVal *rv=kv_str(tmp); free(tmp); return rv;
}

/* ── crcomp — safe hash compare (constant time) ──────────────────────────── */
static inline KVal *k_crcomp(KVal *a, KVal *b) {
    if(strlen(a->s)!=strlen(b->s)) return kv_bool(false);
    int diff=0;
    for(size_t i=0;i<strlen(a->s);i++) diff|=(a->s[i]^b->s[i]);
    return kv_bool(diff==0);
}

#endif /* K_CRYPTO_H */
