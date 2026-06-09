/* k_ser.h — Katlans Serialization (ser prefix) */
#ifndef K_SER_H
#define K_SER_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Simple binary serialization for KVal types.
   Format: type_byte | data_bytes
   0=int(8bytes), 1=flt(8bytes), 2=str(4bytes_len+data), 3=bool(1byte),
   4=void(0bytes), 5=list(recurse), 6=dict(recurse) */

static inline KVal *k_serencode(KVal *data);
static inline KVal *k_serdecode(KVal *bytes);
static inline KVal *k_serbytes(KVal *str);
static inline KVal *k_serfrombytes(KVal *bytes);

/* Helper: create a string with explicit length (binary-safe) */
static KVal *kv_str_len(const char *s, size_t len);

/* Encode a KVal to a byte string (represented as a KVal string) */
static KVal *_ser_encode_val(KVal *v, size_t *out_len) {
    /* Estimate size: type(1) + largest possible */
    size_t cap = 1024;
    unsigned char *buf = (unsigned char*)malloc(cap);
    size_t pos = 0;

    switch (v->type) {
    case KT_INT:
        buf = realloc(buf, 10);
        buf[pos++] = 0;
        long long i = v->i;
        for (int b = 0; b < 8; b++) { buf[pos++] = (unsigned char)(i & 0xFF); i >>= 8; }
        break;
    case KT_FLT:
        buf = realloc(buf, 10);
        buf[pos++] = 1;
        double d = v->f;
        unsigned char *dp = (unsigned char*)&d;
        for (int b = 0; b < 8; b++) buf[pos++] = dp[b];
        break;
    case KT_STR: {
        size_t slen = strlen(v->s);
        buf = realloc(buf, 6 + slen);
        buf[pos++] = 2;
        unsigned int len32 = (unsigned int)slen;
        for (int b = 0; b < 4; b++) { buf[pos++] = (unsigned char)(len32 & 0xFF); len32 >>= 8; }
        memcpy(buf + pos, v->s, slen);
        pos += slen;
        break;
    }
    case KT_BOOL:
        buf = realloc(buf, 3);
        buf[pos++] = 3;
        buf[pos++] = v->b ? 1 : 0;
        break;
    case KT_VOID:
        buf[pos++] = 4;
        break;
    case KT_LIST: {
        buf = realloc(buf, 6 + v->list->len * 16);
        buf[pos++] = 5;
        unsigned int llen = (unsigned int)v->list->len;
        for (int b = 0; b < 4; b++) { buf[pos++] = (unsigned char)(llen & 0xFF); llen >>= 8; }
        for (size_t i = 0; i < v->list->len; i++) {
            size_t elem_len;
            unsigned char *elem = (unsigned char*)_ser_encode_val(v->list->items[i], &elem_len)->s;
            buf = realloc(buf, pos + elem_len + 4);
            unsigned int elen32 = (unsigned int)elem_len;
            for (int b = 0; b < 4; b++) { buf[pos++] = (unsigned char)(elen32 & 0xFF); elen32 >>= 8; }
            memcpy(buf + pos, elem, elem_len);
            pos += elem_len;
        }
        break;
    }
    case KT_DICT: {
        buf = realloc(buf, 6 + v->dict->len * 32);
        buf[pos++] = 6;
        unsigned int dlen = (unsigned int)v->dict->len;
        for (int b = 0; b < 4; b++) { buf[pos++] = (unsigned char)(dlen & 0xFF); dlen >>= 8; }
        for (size_t i = 0; i < v->dict->len; i++) {
            /* key as string */
            size_t klen = strlen(v->dict->entries[i].key);
            buf = realloc(buf, pos + klen + 4);
            unsigned int klen32 = (unsigned int)klen;
            for (int b = 0; b < 4; b++) { buf[pos++] = (unsigned char)(klen32 & 0xFF); klen32 >>= 8; }
            memcpy(buf + pos, v->dict->entries[i].key, klen);
            pos += klen;
            /* value */
            size_t vlen;
            unsigned char *vbytes = (unsigned char*)_ser_encode_val(v->dict->entries[i].val, &vlen)->s;
            buf = realloc(buf, pos + vlen + 4);
            unsigned int vlen32 = (unsigned int)vlen;
            for (int b = 0; b < 4; b++) { buf[pos++] = (unsigned char)(vlen32 & 0xFF); vlen32 >>= 8; }
            memcpy(buf + pos, vbytes, vlen);
            pos += vlen;
        }
        break;
    }
    }

    char *result = (char*)malloc(pos + 1);
    memcpy(result, buf, pos);
    result[pos] = '\0';
    *out_len = pos;
    free(buf);
    return kv_str_len(result, pos);
}

/* Helper: kv_str_len for binary-safe strings */
static KVal *kv_str_len(const char *s, size_t len);
static KVal *kv_str_len(const char *s, size_t len) {
    KVal *k = (KVal*)malloc(sizeof(KVal));
    k->type = KT_STR;
    k->s = (char*)malloc(len + 1);
    memcpy(k->s, s, len);
    k->s[len] = '\0';
    return k;
}

static inline KVal *k_serencode(KVal *data) {
    size_t len;
    return _ser_encode_val(data, &len);
}

/* Decode bytes back to KVal */
static KVal *_ser_decode_bytes(const unsigned char *buf, size_t *pos, size_t total_len) {
    if (*pos >= total_len) return kv_void();

    unsigned char type = buf[(*pos)++];
    (void)type;
    /* Re-read since pos advanced */
    if (*pos > total_len) return kv_void();

    /* We need to rewind to re-read the type */
    (*pos)--;

    switch (buf[*pos]) {
    case 0: { /* int */
        if (*pos + 9 > total_len) return kv_void();
        (*pos)++;
        unsigned long long val = 0;
        for (int b = 0; b < 8; b++) val |= ((unsigned long long)buf[(*pos)++]) << (b * 8);
        return kv_int((long long)val);
    }
    case 1: { /* flt */
        if (*pos + 9 > total_len) return kv_void();
        (*pos)++;
        double d; unsigned char *dp = (unsigned char*)&d;
        for (int b = 0; b < 8; b++) dp[b] = buf[(*pos)++];
        return kv_flt(d);
    }
    case 2: { /* str */
        if (*pos + 5 > total_len) return kv_void();
        (*pos)++;
        unsigned int slen = 0;
        for (int b = 0; b < 4; b++) slen |= ((unsigned int)buf[(*pos)++]) << (b * 8);
        if (*pos + slen > total_len) return kv_void();
        char *s = (char*)malloc(slen + 1);
        memcpy(s, buf + *pos, slen); *pos += slen; s[slen] = '\0';
        KVal *r = kv_str(s); free(s); return r;
    }
    case 3: { /* bool */
        (*pos)++;
        return kv_bool(buf[(*pos)++] != 0);
    }
    case 4: /* void */
        (*pos)++;
        return kv_void();
    case 5: { /* list */
        (*pos)++;
        unsigned int llen = 0;
        for (int b = 0; b < 4; b++) llen |= ((unsigned int)buf[(*pos)++]) << (b * 8);
        KVal *lst = kv_list();
        for (unsigned int i = 0; i < llen && *pos < total_len; i++) {
            KVal *elem = _ser_decode_bytes(buf, pos, total_len);
            klist_push(lst->list, elem);
        }
        return lst;
    }
    case 6: { /* dict */
        (*pos)++;
        unsigned int dlen = 0;
        for (int b = 0; b < 4; b++) dlen |= ((unsigned int)buf[(*pos)++]) << (b * 8);
        KVal *d = kv_dict();
        for (unsigned int i = 0; i < dlen && *pos < total_len; i++) {
            unsigned int klen = 0;
            for (int b = 0; b < 4; b++) klen |= ((unsigned int)buf[(*pos)++]) << (b * 8);
            char *key = (char*)malloc(klen + 1);
            memcpy(key, buf + *pos, klen); *pos += klen; key[klen] = '\0';
            KVal *val = _ser_decode_bytes(buf, pos, total_len);
            kdict_set(d->dict, key, val);
            free(key);
        }
        return d;
    }
    default:
        return kv_void();
    }
}

static inline KVal *k_serdecode(KVal *bytes) {
    size_t pos = 0;
    return _ser_decode_bytes((const unsigned char*)bytes->s, &pos, strlen(bytes->s));
}

static inline KVal *k_serbytes(KVal *str) {
    return kv_str(str->s); /* Return as-is (strings are already binary-safe) */
}

static inline KVal *k_serfrombytes(KVal *bytes) {
    return kv_str(bytes->s); /* Return as string */
}

#endif /* K_SER_H */
