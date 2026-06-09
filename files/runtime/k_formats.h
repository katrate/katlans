/*
 * k_formats.h  —  Katlans Data Formats  (js / csv / xml prefix)
 * JSON parser, CSV reader/writer, basic XML parser — zero dependencies.
 */
#ifndef K_FORMATS_H
#define K_FORMATS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* ══════════════════════════════════════════════════════════════════════════
 *  JSON
 * ══════════════════════════════════════════════════════════════════════════ */

static const char *_json_skip_ws(const char *p) {
    while (*p && isspace((unsigned char)*p)) p++;
    return p;
}

static KVal *_json_parse_value(const char **p);

static KVal *_json_parse_string(const char **p) {
    (*p)++;  /* skip opening " */
    char buf[65536]; int i = 0;
    while (**p && **p != '"') {
        if (**p == '\\') {
            (*p)++;
            switch(**p) {
                case 'n': buf[i++]='\n'; break;
                case 't': buf[i++]='\t'; break;
                case 'r': buf[i++]='\r'; break;
                case '"': buf[i++]='"';  break;
                case '\\':buf[i++]='\\'; break;
                default:  buf[i++]=**p;  break;
            }
        } else { buf[i++] = **p; }
        (*p)++;
    }
    if (**p == '"') (*p)++;
    buf[i] = '\0';
    return kv_str(buf);
}

static KVal *_json_parse_number(const char **p) {
    char buf[64]; int i = 0; bool is_flt = false;
    if (**p == '-') buf[i++] = *(*p)++;
    while (isdigit((unsigned char)**p)) buf[i++] = *(*p)++;
    if (**p == '.') { is_flt = true; buf[i++] = *(*p)++; while(isdigit((unsigned char)**p)) buf[i++]=*(*p)++; }
    if (**p=='e'||**p=='E') { is_flt=true; buf[i++]=*(*p)++; if(**p=='+'||**p=='-') buf[i++]=*(*p)++; while(isdigit((unsigned char)**p)) buf[i++]=*(*p)++; }
    buf[i] = '\0';
    if (is_flt) return kv_flt(atof(buf));
    return kv_int(atoll(buf));
}

static KVal *_json_parse_array(const char **p) {
    (*p)++;  /* skip [ */
    KVal *lst = kv_list();
    *p = _json_skip_ws(*p);
    if (**p == ']') { (*p)++; return lst; }
    while (**p) {
        *p = _json_skip_ws(*p);
        klist_push(lst->list, _json_parse_value(p));
        *p = _json_skip_ws(*p);
        if (**p == ',') { (*p)++; continue; }
        if (**p == ']') { (*p)++; break; }
    }
    return lst;
}

static KVal *_json_parse_object(const char **p) {
    (*p)++;  /* skip { */
    KVal *d = kv_dict();
    *p = _json_skip_ws(*p);
    if (**p == '}') { (*p)++; return d; }
    while (**p) {
        *p = _json_skip_ws(*p);
        if (**p != '"') break;
        KVal *key = _json_parse_string(p);
        *p = _json_skip_ws(*p);
        if (**p == ':') (*p)++;
        *p = _json_skip_ws(*p);
        KVal *val = _json_parse_value(p);
        kdict_set(d->dict, key->s, val);
        *p = _json_skip_ws(*p);
        if (**p == ',') { (*p)++; continue; }
        if (**p == '}') { (*p)++; break; }
    }
    return d;
}

static KVal *_json_parse_value(const char **p) {
    *p = _json_skip_ws(*p);
    if (**p == '"') return _json_parse_string(p);
    if (**p == '{') return _json_parse_object(p);
    if (**p == '[') return _json_parse_array(p);
    if (**p=='-' || isdigit((unsigned char)**p)) return _json_parse_number(p);
    if (strncmp(*p,"true",4)==0)  { *p+=4; return kv_bool(true);  }
    if (strncmp(*p,"false",5)==0) { *p+=5; return kv_bool(false); }
    if (strncmp(*p,"null",4)==0)  { *p+=4; return kv_void();      }
    return kv_void();
}

/* ── jsparse <jsonStr> → KVal ────────────────────────────────────────────── */
static inline KVal *k_jsparse(KVal *json) {
    const char *p = json->s;
    return _json_parse_value(&p);
}

/* ── jsstring <KVal> → JSON string ──────────────────────────────────────── */
static KVal *_json_stringify(KVal *v, int depth);
static KVal *_json_stringify(KVal *v, int depth) {
    if (!v) return kv_str("null");
    char buf[65536] = "";
    switch(v->type) {
        case KT_VOID: return kv_str("null");
        case KT_BOOL: return kv_str(v->b ? "true" : "false");
        case KT_INT:  { char b[32]; snprintf(b,sizeof(b),"%lld",v->i); return kv_str(b); }
        case KT_FLT:  { char b[32]; snprintf(b,sizeof(b),"%g",v->f);   return kv_str(b); }
        case KT_STR:  {
            char b[65536] = "\"";
            for(char *s=v->s; *s; s++) {
                if(*s=='"') strcat(b,"\\\"");
                else if(*s=='\n') strcat(b,"\\n");
                else if(*s=='\t') strcat(b,"\\t");
                else if(*s=='\\') strcat(b,"\\\\");
                else { int l=strlen(b); b[l]=*s; b[l+1]='\0'; }
            }
            strcat(b,"\""); return kv_str(b);
        }
        case KT_LIST: {
            strcpy(buf,"[");
            for(size_t i=0;i<v->list->len;i++){
                if(i) strcat(buf,",");
                strcat(buf, _json_stringify(v->list->items[i], depth+1)->s);
            }
            strcat(buf,"]"); return kv_str(buf);
        }
        case KT_DICT: {
            strcpy(buf,"{");
            for(size_t i=0;i<v->dict->len;i++){
                if(i) strcat(buf,",");
                strcat(buf,"\""); strcat(buf,v->dict->entries[i].key); strcat(buf,"\":");
                strcat(buf, _json_stringify(v->dict->entries[i].val, depth+1)->s);
            }
            strcat(buf,"}"); return kv_str(buf);
        }
    }
    return kv_str("null");
}

static inline KVal *k_jsstring(KVal *v) { return _json_stringify(v, 0); }

/* ── jsread <"file.json"> ────────────────────────────────────────────────── */
static inline KVal *k_jsread(KVal *path) {

    KVal *content = k_fread(path);
    if (content->type == KT_VOID) return kv_void();
    return k_jsparse(content);
}

/* ── jswrite <"file.json", data> ─────────────────────────────────────────── */
static inline KVal *k_jswrite(KVal *path, KVal *data) {

    return k_fwrite(path, k_jsstring(data));
}

/* ══════════════════════════════════════════════════════════════════════════
 *  CSV
 * ══════════════════════════════════════════════════════════════════════════ */

/* ── csvread <"file.csv"> → list of lists ────────────────────────────────── */
static inline KVal *k_csvread(KVal *path) {

    KVal *content = k_fread(path);
    if (content->type == KT_VOID) return kv_list();

    KVal *rows = kv_list();
    char *text = strdup(content->s);
    char *line = strtok(text, "\n");
    while (line) {
        KVal *row = kv_list();
        char *field = line;
        while (*field) {
            if (*field == '"') {
                field++;
                char buf[4096]; int i = 0;
                while (*field && !(*field=='"' && *(field+1)!='"'))
                    buf[i++] = (*field=='"') ? (field++, '"') : *field++;
                if (*field=='"') field++;
                if (*field==',') field++;
                buf[i]='\0'; klist_push(row->list, kv_str(buf));
            } else {
                char buf[4096]; int i = 0;
                while (*field && *field != ',' && *field != '\r')
                    buf[i++] = *field++;
                if (*field==',') field++;
                buf[i]='\0'; klist_push(row->list, kv_str(buf));
            }
        }
        klist_push(rows->list, row);
        line = strtok(NULL, "\n");
    }
    free(text);
    return rows;
}

/* ── csvheads <"file.csv"> → list of header names ───────────────────────── */
static inline KVal *k_csvheads(KVal *path) {
    KVal *rows = k_csvread(path);
    if (!rows->list->len) return kv_list();
    return rows->list->items[0];
}

/* ── csvwrite <"file.csv", list_of_lists> ────────────────────────────────── */
static inline KVal *k_csvwrite(KVal *path, KVal *data) {
    FILE *f = fopen(path->s, "wb");
    if (!f) return kv_bool(false);
    for (size_t i = 0; i < data->list->len; i++) {
        KVal *row = data->list->items[i];
        if (row->type != KT_LIST) { fprintf(f, "%s\n", k_S(row)->s); continue; }
        for (size_t j = 0; j < row->list->len; j++) {
            if (j) fprintf(f, ",");
            KVal *cell = row->list->items[j];
            if (cell->type == KT_STR && (strchr(cell->s,',')||strchr(cell->s,'"')||strchr(cell->s,'\n')))
                fprintf(f, "\"%s\"", cell->s);
            else fprintf(f, "%s", k_S(cell)->s);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return kv_bool(true);
}

/* ── csvparse <str> → list of lists ──────────────────────────────────────── */
static inline KVal *k_csvparse(KVal *str) {
    /* Same as csvread but from string */
    KVal *tmp = kv_str(str->s);
    /* write to temp file and read */
    char tmpf[64] = "/tmp/k_csv_XXXXXX"; int fd = mkstemp(tmpf);
    write(fd, str->s, strlen(str->s)); close(fd);
    KVal *r = k_csvread(kv_str(tmpf));
    remove(tmpf);
    return r;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  XML (basic tag parsing)
 * ══════════════════════════════════════════════════════════════════════════ */

/* ── xmlget <xml_str, "tag"> → content of first matching tag ────────────── */
static inline KVal *k_xmlget(KVal *xml, KVal *tag) {
    char open[256], close_t[256];
    snprintf(open,  sizeof(open),  "<%s",  tag->s);
    snprintf(close_t, sizeof(close_t), "</%s>", tag->s);

    char *start = strstr(xml->s, open);
    if (!start) return kv_void();
    start = strchr(start, '>');
    if (!start) return kv_void();
    start++;  /* skip > */

    char *end = strstr(start, close_t);
    if (!end) return kv_void();

    int len = (int)(end - start);
    char *buf = (char*)malloc(len + 1);
    strncpy(buf, start, len); buf[len] = '\0';
    KVal *rv = kv_str(buf); free(buf);
    return rv;
}

/* ── xmlattr <xml_str, "tag", "attr"> → attribute value ─────────────────── */
static inline KVal *k_xmlattr(KVal *xml, KVal *tag, KVal *attr) {
    char open[256]; snprintf(open, sizeof(open), "<%s", tag->s);
    char *start = strstr(xml->s, open);
    if (!start) return kv_void();

    char *end_tag = strchr(start, '>');
    if (!end_tag) return kv_void();

    /* look for attr="value" within the tag */
    char search[256]; snprintf(search, sizeof(search), "%s=\"", attr->s);
    char *a = strstr(start, search);
    if (!a || a > end_tag) return kv_void();
    a += strlen(search);

    char *close_q = strchr(a, '"');
    if (!close_q) return kv_void();

    int len = (int)(close_q - a);
    char *buf = (char*)malloc(len + 1);
    strncpy(buf, a, len); buf[len] = '\0';
    KVal *rv = kv_str(buf); free(buf);
    return rv;
}

/* ── xmlparse <str> → adero with tag→content mapping ────────────────────── */
static inline KVal *k_xmlparse(KVal *xml) {
    KVal *d = kv_dict();
    char *s = xml->s;
    while ((s = strchr(s, '<'))) {
        if (s[1]=='/') { s++; continue; }  /* closing tag */
        if (s[1]=='!') { s++; continue; }  /* comment/doctype */
        s++;
        /* read tag name */
        char tag[256]; int i = 0;
        while (*s && *s!='>' && *s!='/' && *s!=' ' && i<255) tag[i++]=*s++;
        tag[i] = '\0';
        /* skip to end of opening tag */
        while (*s && *s!='>') s++;
        if (!*s) break;
        s++;  /* past > */
        /* read content until closing tag */
        char close_t[264]; snprintf(close_t, sizeof(close_t), "</%s>", tag);
        char *end = strstr(s, close_t);
        if (!end) continue;
        int len = (int)(end - s);
        char *buf = (char*)malloc(len+1);
        strncpy(buf, s, len); buf[len]='\0';
        kdict_set(d->dict, tag, kv_str(buf));
        free(buf);
        s = end + strlen(close_t);
    }
    return d;
}

/* ── xmlstring <adero> → XML string ──────────────────────────────────────── */
static inline KVal *k_xmlstring(KVal *d) {
    char buf[65536] = "";
    if (d->type != KT_DICT) return kv_str("");
    for (size_t i = 0; i < d->dict->len; i++) {
        char tmp[4096];
        snprintf(tmp, sizeof(tmp), "<%s>%s</%s>",
                 d->dict->entries[i].key,
                 k_S(d->dict->entries[i].val)->s,
                 d->dict->entries[i].key);
        strcat(buf, tmp);
    }
    return kv_str(buf);
}

#endif /* K_FORMATS_H */
