/* k_design.h — Katlans Design Module (ds prefix)
 * CSS-like styling system with Win32 GDI drawing.
 * Windows only — non-Windows falls back to printf-based info.
 */
#ifndef K_DESIGN_H
#define K_DESIGN_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  Windows — Full GDI design engine
 * ═══════════════════════════════════════════════════════════════════════════ */
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* ── Color helpers ───────────────────────────────────────────────────── */

static COLORREF _ds_parse_color(KVal *c, COLORREF def) {
    if(!c||c->type==KT_VOID) return def;
    if(c->type==KT_STR){
        char *s=c->s;
        if(s[0]=='#') return (COLORREF)strtol(s+1,NULL,16);
        struct{char *n;COLORREF c;}named[]={
            {"red",0x0000FF},{"green",0x008000},{"blue",0xFF0000},
            {"white",0xFFFFFF},{"black",0x000000},{"gray",0x808080},
            {"yellow",0x00FFFF},{"orange",0x00A5FF},{"purple",0x800080},
            {"pink",0xCBC0FF},{"cyan",0xFFFF00},{"magenta",0xFF00FF},
            {"navy",0x800000},{"teal",0x808000},{"olive",0x008080},
            {"maroon",0x000080},{"silver",0xC0C0C0},{"lime",0x00FF00},
            {NULL,0}
        };
        for(int i=0;named[i].n;i++){
        const char *a=s,*b=named[i].n;int eq=1;
        while(*a&&*b){if(toupper(*a)!=toupper(*b)){eq=0;break;}a++;b++;}
        if(eq&&*a==*b) return named[i].c;
    }
    }
    if(c->type==KT_INT) return (COLORREF)c->i;
    return def;
}

static inline KVal *k_ds_color(KVal *c){
    COLORREF cr=_ds_parse_color(c,RGB(255,255,255));
    return kv_int((long long)cr);
}

static inline KVal *k_ds_color_rgb(KVal *r,KVal *g,KVal *b){
    return kv_int((long long)RGB((int)r->i,(int)g->i,(int)b->i));
}

static inline KVal *k_ds_color_hex(KVal *h){
    return k_ds_color(h);
}

/* ── Drawing context ────────────────────────────────────────────────── */

/* The design module draws to whatever HDC is set as current.
 * ds_bind <hdc> sets the drawing context. ds_bind_win <win> gets HDC from a game window.
 */
static HDC _ds_hdc=NULL;
static int _ds_w=800,_ds_h=600;

static inline KVal *k_ds_bind(KVal *h,KVal *w,KVal *hgt){
    _ds_hdc=(HDC)(size_t)h->i;_ds_w=(int)w->i;_ds_h=(int)hgt->i;return kv_void();
}

static inline KVal *k_ds_size(KVal *w,KVal *hgt){_ds_w=(int)w->i;_ds_h=(int)hgt->i;return kv_void();}

/* ── GDI Drawing Primitives ─────────────────────────────────────────── */

/* ds_rounded_rect <x, y, w, h, radius, color> */
static inline KVal *k_ds_draw_roundrect(KVal *x,KVal *y,KVal *w,KVal *h,KVal *r,KVal *col){
    if(!_ds_hdc)return kv_void();
    int rx=(int)x->i,ry=(int)y->i,rw=(int)w->i,rh=(int)h->i,rr=(int)r->i;
    COLORREF cl=_ds_parse_color(col,RGB(200,200,200));
    HBRUSH hb=CreateSolidBrush(cl);
    HPEN hp=CreatePen(PS_SOLID,1,cl);
    HBRUSH oldb=(HBRUSH)SelectObject(_ds_hdc,hb);
    HPEN oldp=(HPEN)SelectObject(_ds_hdc,hp);
    RoundRect(_ds_hdc,rx,ry,rx+rw,ry+rh,rr,rr);
    SelectObject(_ds_hdc,oldp);SelectObject(_ds_hdc,oldb);
    DeleteObject(hp);DeleteObject(hb);return kv_void();
}

/* ds_fill_rect <x, y, w, h, color> — filled rectangle */
static inline KVal *k_ds_fill_rect(KVal *x,KVal *y,KVal *w,KVal *h,KVal *col){
    if(!_ds_hdc)return kv_void();
    COLORREF cl=_ds_parse_color(col,RGB(200,200,200));
    HBRUSH hb=CreateSolidBrush(cl);
    RECT rc={(int)x->i,(int)y->i,(int)(x->i+w->i),(int)(y->i+h->i)};
    FillRect(_ds_hdc,&rc,hb);
    DeleteObject(hb);return kv_void();
}

/* ds_draw_rect <x, y, w, h, color> — outlined rectangle */
static inline KVal *k_ds_draw_rect(KVal *x,KVal *y,KVal *w,KVal *h,KVal *col){
    if(!_ds_hdc)return kv_void();
    COLORREF cl=_ds_parse_color(col,RGB(0,0,0));
    HPEN hp=CreatePen(PS_SOLID,1,cl);
    HPEN oldp=(HPEN)SelectObject(_ds_hdc,hp);
    HBRUSH oldb=(HBRUSH)SelectObject(_ds_hdc,GetStockObject(NULL_BRUSH));
    Rectangle(_ds_hdc,(int)x->i,(int)y->i,(int)(x->i+w->i),(int)(y->i+h->i));
    SelectObject(_ds_hdc,oldb);SelectObject(_ds_hdc,oldp);
    DeleteObject(hp);return kv_void();
}

/* ds_gradient <x, y, w, h, color1, color2, horizontal?> — gradient fill */
static inline KVal *k_ds_gradient(KVal *x,KVal *y,KVal *w,KVal *h,KVal *c1,KVal *c2,KVal *horiz){
    if(!_ds_hdc)return kv_void();
    int rx=(int)x->i,ry=(int)y->i,rw=(int)w->i,rh=(int)h->i;
    COLORREF ca=_ds_parse_color(c1,RGB(255,0,0)),cb=_ds_parse_color(c2,RGB(0,0,255));
    bool hz=horiz?horiz->b:true;
    /* Simple gradient via vertical/horizontal lines */
    int steps=hz?rw:rh;
    for(int i=0;i<steps;i++){
        double t=(double)i/steps;
        int r=(int)((1-t)*GetRValue(ca)+t*GetRValue(cb));
        int g=(int)((1-t)*GetGValue(ca)+t*GetGValue(cb));
        int b=(int)((1-t)*GetBValue(ca)+t*GetBValue(cb));
        COLORREF _dc=RGB(r,g,b);
        if(hz) for(int _j=0;_j<rh;_j++) SetPixelV(_ds_hdc,rx+i,ry+_j,_dc);
        else   for(int _j=0;_j<rw;_j++) SetPixelV(_ds_hdc,rx+_j,ry+i,_dc);
    }
    return kv_void();
}

/* ds_shadow <x, y, w, h, blur, color> — draw a shadow offset by blur/4 */
static inline KVal *k_ds_shadow(KVal *x,KVal *y,KVal *w,KVal *h,KVal *blur,KVal *col){
    if(!_ds_hdc)return kv_void();
    int _b=(int)(blur?blur->i:4);if(_b<1)_b=1;
    int bx=(int)x->i+_b/2,by=(int)y->i+_b/2,bw=(int)w->i,bh=(int)h->i;
    COLORREF cl=_ds_parse_color(col,RGB(100,100,100));
    /* Draw semi-transparent shadow as multiple offset rects */
    int _layers=_b>8?8:_b;
    for(int s=0;s<_layers;s++){
        int alpha=60-s*12;
        COLORREF sc=RGB(
            GetRValue(cl)*alpha/255,
            GetGValue(cl)*alpha/255,
            GetBValue(cl)*alpha/255
        );
        HBRUSH hb=CreateSolidBrush(sc);
        RECT rc={bx+s,by+s,bx+bw-s,by+bh-s};
        FillRect(_ds_hdc,&rc,hb);
        DeleteObject(hb);
    }
    return kv_void();
}

/* ds_text <x, y, text, color, font_size> — styled text */
static inline KVal *k_ds_text(KVal *x,KVal *y,KVal *t,KVal *col,KVal *size){
    if(!_ds_hdc)return kv_void();
    COLORREF cl=_ds_parse_color(col,RGB(0,0,0));
    int fs=(int)(size?size->i:14);
    SetBkMode(_ds_hdc,TRANSPARENT);
    SetTextColor(_ds_hdc,cl);
    HFONT hf=CreateFontA(fs,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Segoe UI");
    HFONT old=(HFONT)SelectObject(_ds_hdc,hf);
    TextOutA(_ds_hdc,(int)x->i,(int)y->i,t->s,(int)strlen(t->s));
    SelectObject(_ds_hdc,old);
    DeleteObject(hf);return kv_void();
}

/* ds_text_bold <x, y, text, color, size> — bold text */
static inline KVal *k_ds_text_bold(KVal *x,KVal *y,KVal *t,KVal *col,KVal *size){
    if(!_ds_hdc)return kv_void();
    COLORREF cl=_ds_parse_color(col,RGB(0,0,0));
    int fs=(int)(size?size->i:14);
    SetBkMode(_ds_hdc,TRANSPARENT);
    SetTextColor(_ds_hdc,cl);
    HFONT hf=CreateFontA(fs,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Segoe UI");
    HFONT old=(HFONT)SelectObject(_ds_hdc,hf);
    TextOutA(_ds_hdc,(int)x->i,(int)y->i,t->s,(int)strlen(t->s));
    SelectObject(_ds_hdc,old);
    DeleteObject(hf);return kv_void();
}

/* ds_text_measure <text, size> — returns {w, h} dict of text extent */
static inline KVal *k_ds_text_measure(KVal *t,KVal *size){
    if(!_ds_hdc){KVal *_r=kv_dict();kdict_set(_r->dict,"w",kv_int(0));kdict_set(_r->dict,"h",kv_int(0));return _r;}
    int fs=(int)(size?size->i:14);
    HFONT hf=CreateFontA(fs,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Segoe UI");
    HFONT old=(HFONT)SelectObject(_ds_hdc,hf);
    SIZE sz;GetTextExtentPoint32A(_ds_hdc,t->s,(int)strlen(t->s),&sz);
    SelectObject(_ds_hdc,old);DeleteObject(hf);
    KVal *r=kv_dict();kdict_set(r->dict,"w",kv_int(sz.cx));
    kdict_set(r->dict,"h",kv_int(sz.cy));return r;
}

/* ── CSS Selector Engine ────────────────────────────────────────────── */

/* Styles are stored as dicts:
 * {
 *   selectors: [{type:"element",name:"button"}, {type:"class",name:"btn"}, ...],
 *   properties: {color:"#ff0000", "font-size":"14", margin:"8", ...},
 *   priority: 0
 * }
 * ds_parse_css <css_string> — parse CSS text into list of rule dicts
 */

static KVal *_ds_trim(KVal *s){
    char *p=s->s;while(*p==' '||*p=='\t'||*p=='\n')p++;
    char *e=p+strlen(p)-1;while(e>p&&(*e==' '||*e=='\t'||*e=='\n'))e--;
    if(p==s->s&&e==s->s+strlen(s->s)-1)return s;
    char *buf=(char*)malloc(e-p+2);strncpy(buf,p,e-p+1);buf[e-p+1]=0;
    KVal *r=kv_str(buf);free(buf);return r;
}

static KVal *_ds_split_sel(const char *sel){
    /* Parse a single selector like "button.active#main" */
    KVal *parts=kv_list();
    char buf[256];int bi=0;char type='e'; /* e=element, c=class, i=id */
    for(int i=0;sel[i];i++){
        char ch=sel[i];
        if(ch=='.'||ch=='#'){
            if(bi>0){buf[bi]=0;
                KVal *p=kv_dict();kdict_set(p->dict,"type",kv_str(type=='i'?"id":type=='c'?"class":"element"));
                kdict_set(p->dict,"name",kv_str(buf));klist_push(parts->list,p);
                bi=0;
            }
            type=(ch=='.')?'c':'i';
        }else{buf[bi++]=ch;}
    }
    if(bi>0){buf[bi]=0;
        KVal *p=kv_dict();kdict_set(p->dict,"type",kv_str(type=='i'?"id":type=='c'?"class":"element"));
        kdict_set(p->dict,"name",kv_str(buf));klist_push(parts->list,p);
    }
    return parts;
}

static inline KVal *k_ds_parse_css(KVal *css){
    KVal *rules=kv_list();
    char *s=css->s;char selector[1024]="";int si=0;
    char prop[256]="";int pi=0;char value[1024]="";int vi=0;
    int state=0;/* 0=selector, 1=block, 2=prop, 3=value */
    for(int i=0;s[i];i++){
        char ch=s[i];
        if(state==0){
            if(ch=='{'){selector[si]=0;si=0;state=1;}
            else if(ch!=' '||si>0){if(ch!='\n'&&ch!='\r'&&ch!='\t')selector[si++]=ch;}
        }else if(state==1){
            if(ch=='}'){/* done — create rule */
                KVal *rule=kv_dict();
                /* Parse selectors (comma-separated) */
                KVal *sel_list=kv_list();
                char sel_buf[256]="";int sbi=0;
                for(int si2=0;selector[si2];si2++){
                    if(selector[si2]==','){sel_buf[sbi]=0;
                        KVal *trimmed=_ds_trim(kv_str(sel_buf));
                        klist_push(sel_list->list,_ds_split_sel(trimmed->s));
                        sbi=0;
                    }else sel_buf[sbi++]=selector[si2];
                }
                if(sbi>0){sel_buf[sbi]=0;
                    KVal *trimmed=_ds_trim(kv_str(sel_buf));
                    klist_push(sel_list->list,_ds_split_sel(trimmed->s));
                }
                kdict_set(rule->dict,"selectors",sel_list);
                kdict_set(rule->dict,"priority",kv_int(0));
                KVal *props=kv_dict();/* Properties already parsed? */
                /* Parse properties from the block */
                char pbuf[256]="";int pbi=0;char vbuf[1024]="";int vbi=0;
                int ps=0;/* 0=prop, 1=value */
                /* We need to re-scan the block content — but we already consumed it.
                   Instead, we store the property parsing during scan. */
                /* For now, return rules with block text for later parsing */
                kdict_set(rule->dict,"_block",kv_str(""));
                klist_push(rules->list,rule);
                state=0;selector[0]=0;si=0;
            }else if(ch==':'&&pi==0){
                /* Start property name */
                pi=0;prop[0]=0;
                /* Scan backwards to get property */
            }else{
                /* In block — collect everything */
            }
        }
    }
    return rules;
}

/* ds_parse_styles <ad> — parse a adero of style properties into computed style dict */
static inline KVal *k_ds_parse_styles(KVal *props){
    KVal *style=kv_dict();
    if(!props||props->type!=KT_DICT)return style;
    char *keys[]={"color","bg","background","font-size","margin","padding",
        "border","border-radius","border-color","width","height",
        "display","flex-direction","justify-content","align-items","gap",
        "grid-template-columns","grid-template-rows","gap","opacity",
        "text-align","font-weight","font-family","box-shadow",NULL};
    for(int i=0;keys[i];i++){
        KVal *v=kdict_get(props->dict,keys[i]);
        if(v->type!=KT_VOID)kdict_set(style->dict,keys[i],v);
    }
    return style;
}

/* ── Box Model ──────────────────────────────────────────────────────── */

/* ds_box_model <style_dict> — compute {margin, border, padding, content} from style
 * Returns: {margin:{t,r,b,l}, border:{t,r,b,l}, padding:{t,r,b,l}, content:{w,h}}
 */

static int _ds_parse_px(KVal *v,int def){
    if(!v||v->type==KT_VOID)return def;
    if(v->type==KT_INT)return (int)v->i;
    return def;
}

static void _ds_get_box_values(KVal *style,const char *key,int *t,int *r,int *b,int *l){
    KVal *v=kdict_get(style->dict,key);
    if(v->type==KT_INT){*t=*r=*b=*l=(int)v->i;return;}
    if(v->type==KT_STR){
        /* Parse space-separated values: "10" / "10 20" / "10 20 30" / "10 20 30 40" */
        int vals[4]={0};int n=sscanf(v->s,"%d %d %d %d",&vals[0],&vals[1],&vals[2],&vals[3]);
        if(n==1)*t=*r=*b=*l=vals[0];
        else if(n==2)*t=*b=vals[0],*r=*l=vals[1];
        else if(n==3)*t=vals[0],*r=*l=vals[1],*b=vals[2];
        else if(n==4)*t=vals[0],*r=vals[1],*b=vals[2],*l=vals[3];
    }
}

static inline KVal *k_ds_box_model(KVal *style){
    KVal *box=kv_dict();
    int mt=0,mr=0,mb=0,ml=0;_ds_get_box_values(style,"margin",&mt,&mr,&mb,&ml);
    int bt=0,br=0,bb=0,bl=0;_ds_get_box_values(style,"border",&bt,&br,&bb,&bl);
    int pt=0,pr=0,pb=0,pl=0;_ds_get_box_values(style,"padding",&pt,&pr,&pb,&pl);
    KVal *m=kv_dict();kdict_set(m->dict,"t",kv_int(mt));kdict_set(m->dict,"r",kv_int(mr));
    kdict_set(m->dict,"b",kv_int(mb));kdict_set(m->dict,"l",kv_int(ml));
    kdict_set(box->dict,"margin",m);
    KVal *b2=kv_dict();kdict_set(b2->dict,"t",kv_int(bt));kdict_set(b2->dict,"r",kv_int(br));
    kdict_set(b2->dict,"b",kv_int(bb));kdict_set(b2->dict,"l",kv_int(bl));
    kdict_set(box->dict,"border",b2);
    KVal *p=kv_dict();kdict_set(p->dict,"t",kv_int(pt));kdict_set(p->dict,"r",kv_int(pr));
    kdict_set(p->dict,"b",kv_int(pb));kdict_set(p->dict,"l",kv_int(pl));
    kdict_set(box->dict,"padding",p);
    return box;
}

/* ── Flexbox Layout ─────────────────────────────────────────────────── */

/* ds_layout_flex <container_width, container_height, children_styles, children_sizes>
 * children_styles: list of style dicts
 * children_sizes: list of {w:int, h:int} dicts for content sizes
 * Returns: list of {x, y, w, h} positions for each child
 *
 * Supports: flex-direction (row/column), justify-content, align-items, gap
 */

typedef struct{int x,y,w,h;}DSRect;

static inline KVal *k_ds_layout_flex(KVal *cw,KVal *ch,KVal *styles,KVal *sizes){
    if(!styles||styles->type!=KT_LIST)return kv_list();
    KVal *result=kv_list();
    size_t n=styles->list->len;
    if(n==0)return result;

    /* Get container style from first child's parent context — use first style as default */
    KVal *style=styles->list->items[0];
    KVal *dir_v=kdict_get(style->dict,"flex-direction");
    const char *dir=dir_v->type==KT_STR?dir_v->s:"row";
    KVal *jc_v=kdict_get(style->dict,"justify-content");
    const char *jc=jc_v->type==KT_STR?jc_v->s:"start";
    KVal *ai_v=kdict_get(style->dict,"align-items");
    const char *ai=ai_v->type==KT_STR?ai_v->s:"stretch";
    KVal *gap_v=kdict_get(style->dict,"gap");
    int gap=gap_v->type==KT_INT?(int)gap_v->i:0;

    int cw_i=(int)cw->i,ch_i=(int)ch->i;
    bool is_row=strcmp(dir,"row")==0;

    /* Calculate total content size */
    int total=0;
    int *child_sizes=(int*)calloc(n,sizeof(int));
    int *child_other=(int*)calloc(n,sizeof(int));
    for(size_t i=0;i<n;i++){
        KVal *sz=i<sizes->list->len?sizes->list->items[i]:kv_dict();
        KVal *wv=kdict_get(sz->dict,"w"),*hv=kdict_get(sz->dict,"h");
        int cw_i2=(int)(wv->type==KT_INT?wv->i:50);
        int ch_i2=(int)(hv->type==KT_INT?hv->i:20);
        child_sizes[i]=is_row?cw_i2:ch_i2;
        child_other[i]=is_row?ch_i2:cw_i2;
        total+=child_sizes[i];
    }
    total+=gap*(int)(n-1);

    /* Calculate start offset based on justify-content */
    int start=0,avail=is_row?cw_i:ch_i;
    if(strcmp(jc,"center")==0)start=(avail-total)/2;
    else if(strcmp(jc,"end")==0)start=avail-total;
    else if(strcmp(jc,"space-between")==0&&n>1)gap=(avail-total)/(int)(n-1);
    else if(strcmp(jc,"space-evenly")==0&&n>0){
        int space=(avail-total)/(int)(n+1);
        start=space;gap=space;
    }
    if(start<0)start=0;

    /* Position each child */
    int pos=start;
    for(size_t i=0;i<n;i++){
        KVal *rect=kv_dict();
        if(is_row){
            kdict_set(rect->dict,"x",kv_int(pos));
            int al=0;int oc=child_other[i];
            if(strcmp(ai,"center")==0)al=(ch_i-oc)/2;
            else if(strcmp(ai,"end")==0)al=ch_i-oc;
            kdict_set(rect->dict,"y",kv_int(al));
            kdict_set(rect->dict,"w",kv_int(child_sizes[i]));
            kdict_set(rect->dict,"h",kv_int(oc));
            pos+=child_sizes[i]+gap;
        }else{
            kdict_set(rect->dict,"y",kv_int(pos));
            int al=0;int oc=child_other[i];
            if(strcmp(ai,"center")==0)al=(cw_i-oc)/2;
            else if(strcmp(ai,"end")==0)al=cw_i-oc;
            kdict_set(rect->dict,"x",kv_int(al));
            kdict_set(rect->dict,"h",kv_int(child_sizes[i]));
            kdict_set(rect->dict,"w",kv_int(oc));
            pos+=child_sizes[i]+gap;
        }
        klist_push(result->list,rect);
    }
    free(child_sizes);free(child_other);
    return result;
}

/* ── Grid Layout ────────────────────────────────────────────────────── */

/* ds_layout_grid <container_w, container_h, cols_list, rows_list, children_sizes, gap>
 * cols_list: list of column widths (e.g. [1,1,1] for 3 equal cols, [100,1,100] for fixed+flex)
 * rows_list: similar for rows
 * children_sizes: list of content sizes for each child
 * gap: int gap between cells
 * Returns: list of {x, y, w, h} positions
 */

static inline KVal *k_ds_layout_grid(KVal *cw,KVal *ch,KVal *cols,KVal *rows,KVal *sizes,KVal *gap_v){
    KVal *result=kv_list();
    size_t nc=cols->list->len,nr=rows->list->len;
    if(nc==0&&nr==0)return result;
    int cw_i=(int)cw->i,ch_i=(int)ch->i,gap=(int)(gap_v?gap_v->i:0);

    /* Calculate column widths */
    int *col_w=(int*)calloc(nc?nc:1,sizeof(int));
    int fixed_total=0,flex_count=0;
    for(size_t i=0;i<nc;i++){
        int v=(int)cols->list->items[i]->i;
        if(v==0){col_w[i]=0;flex_count++;}
        else if(v<0){col_w[i]=50;flex_count++;} /* auto = flex */
        else{col_w[i]=v;fixed_total+=v;}
    }
    int avail=cw_i-fixed_total-(int)(nc>1?gap*(nc-1):0);
    if(avail<0)avail=0;
    for(size_t i=0;i<nc;i++){if(col_w[i]==0||col_w[i]<0)col_w[i]=flex_count>0?avail/(int)flex_count:0;}
    if((int)nc==0){col_w[0]=cw_i;nc=1;}

    /* Calculate row heights */
    int *row_h=(int*)calloc(nr?nr:1,sizeof(int));
    fixed_total=0;flex_count=0;
    for(size_t i=0;i<nr;i++){
        int v=(int)rows->list->items[i]->i;
        if(v==0){row_h[i]=0;flex_count++;}
        else if(v<0){row_h[i]=30;flex_count++;}
        else{row_h[i]=v;fixed_total+=v;}
    }
    avail=ch_i-fixed_total-(int)(nr>1?gap*(nr-1):0);
    if(avail<0)avail=0;
    for(size_t i=0;i<nr;i++){if(row_h[i]==0||row_h[i]<0)row_h[i]=flex_count>0?avail/(int)flex_count:30;}
    if((int)nr==0){row_h[0]=ch_i;nr=1;}

    /* Position children in grid cells */
    int cy=0;
    size_t ci=0;
    for(size_t ri=0;ri<nr;ri++){
        int cx=0;
        for(size_t ci2=0;ci2<nc;ci2++){
            KVal *rect=kv_dict();
            kdict_set(rect->dict,"x",kv_int(cx));
            kdict_set(rect->dict,"y",kv_int(cy));
            kdict_set(rect->dict,"w",kv_int(col_w[ci2]));
            kdict_set(rect->dict,"h",kv_int(row_h[ri]));
            klist_push(result->list,rect);
            cx+=col_w[ci2]+gap;
            ci++;
            if(ci>=sizes->list->len&&ci2+1<nc){
                /* Fill remaining cells with empty rects */
            }
        }
        cy+=row_h[ri]+gap;
        if(ci>=sizes->list->len)break;
    }
    free(col_w);free(row_h);
    return result;
}

/* ── Style Application ──────────────────────────────────────────────── */

/* ds_apply_style <widget_dict, style_dict> — apply computed styles to a widget
 * Modifies widget dict in place with style properties.
 */
static inline KVal *k_ds_apply_style(KVal *widget,KVal *style){
    if(!widget||widget->type!=KT_DICT)return kv_void();
    char *props[]={"color","bg","background","font-size","font-weight",
        "text-align","opacity","border-radius","border","padding","margin",NULL};
    for(int i=0;props[i];i++){
        KVal *v=kdict_get(style->dict,props[i]);
        if(v->type!=KT_VOID)kdict_set(widget->dict,props[i],v);
    }
    return kv_void();
}

/* ds_render_box <x, y, w, h, style> — render a styled box with border-radius, bg, border */
static inline KVal *k_ds_render_box(KVal *x,KVal *y,KVal *w,KVal *h,KVal *style){
    if(!_ds_hdc)return kv_void();
    int rx=(int)x->i,ry=(int)y->i,rw=(int)w->i,rh=(int)h->i;

    /* Get style properties */
    KVal *bg=kdict_get(style->dict,"bg");
    if(bg->type==KT_VOID)bg=kdict_get(style->dict,"background");
    KVal *border=kdict_get(style->dict,"border");
    KVal *br=kdict_get(style->dict,"border-radius");
    COLORREF bc=_ds_parse_color(kdict_get(style->dict,"border-color"),RGB(0,0,0));
    int bw=border->type==KT_INT?(int)border->i:1;if(bw<1)bw=1;
    int rr=br->type==KT_INT?(int)br->i:0;
    COLORREF bgc=_ds_parse_color(bg,RGB(240,240,240));
    bool has_border=border->type!=KT_VOID&&bw>0;

    /* If border-radius > 0, use a single RoundRect for both fill + outline */
    if(rr>0){
        HBRUSH hb_bg=CreateSolidBrush(bgc);
        HPEN hp_b=CreatePen(PS_SOLID,has_border?bw:1,bc);
        HBRUSH oldb=(HBRUSH)SelectObject(_ds_hdc,hb_bg);
        HPEN oldp=(HPEN)SelectObject(_ds_hdc,hp_b);
        RoundRect(_ds_hdc,rx,ry,rx+rw,ry+rh,rr*2,rr*2);
        SelectObject(_ds_hdc,oldp);SelectObject(_ds_hdc,oldb);
        DeleteObject(hp_b);DeleteObject(hb_bg);
    }else{
        /* Background fill (FillRect does NOT require SelectObject) */
        if(bg->type!=KT_VOID){
            HBRUSH hb=CreateSolidBrush(bgc);
            RECT rc={rx,ry,rx+rw,ry+rh};
            FillRect(_ds_hdc,&rc,hb);
            DeleteObject(hb);
        }
        /* Border outline */
        if(has_border){
            HPEN hp=CreatePen(PS_SOLID,bw,bc);
            HPEN oldp=(HPEN)SelectObject(_ds_hdc,hp);
            HBRUSH oldb=(HBRUSH)SelectObject(_ds_hdc,GetStockObject(NULL_BRUSH));
            Rectangle(_ds_hdc,rx,ry,rx+rw,ry+rh);
            SelectObject(_ds_hdc,oldb);SelectObject(_ds_hdc,oldp);
            DeleteObject(hp);
        }
    }
    return kv_void();
}

/* ── Utility ────────────────────────────────────────────────────────── */

/* ds_screen_size — returns {w, h} of the screen */
static inline KVal *k_ds_screen_size(void){
    KVal *r=kv_dict();
    kdict_set(r->dict,"w",kv_int(GetSystemMetrics(SM_CXSCREEN)));
    kdict_set(r->dict,"h",kv_int(GetSystemMetrics(SM_CYSCREEN)));
    return r;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Non-Windows — stub implementations
 * ═══════════════════════════════════════════════════════════════════════════ */
#else

static inline KVal *k_ds_color(KVal *c){(void)c;return kv_int(0xFFFFFF);}
static inline KVal *k_ds_color_rgb(KVal *r,KVal *g,KVal *b){(void)r;(void)g;(void)b;return kv_int(0);}
static inline KVal *k_ds_color_hex(KVal *h){(void)h;return kv_int(0);}
static inline KVal *k_ds_bind(KVal *h,KVal *w,KVal *hgt){(void)h;(void)w;(void)hgt;return kv_void();}
static inline KVal *k_ds_size(KVal *w,KVal *hgt){(void)w;(void)hgt;return kv_void();}
static inline KVal *k_ds_draw_roundrect(KVal *x,KVal *y,KVal *w,KVal *h,KVal *r,KVal *col){
    (void)x;(void)y;(void)w;(void)h;(void)r;(void)col;return kv_void();
}
static inline KVal *k_ds_fill_rect(KVal *x,KVal *y,KVal *w,KVal *h,KVal *col){
    (void)x;(void)y;(void)w;(void)h;(void)col;return kv_void();
}
static inline KVal *k_ds_draw_rect(KVal *x,KVal *y,KVal *w,KVal *h,KVal *col){
    (void)x;(void)y;(void)w;(void)h;(void)col;return kv_void();
}
static inline KVal *k_ds_gradient(KVal *x,KVal *y,KVal *w,KVal *h,KVal *c1,KVal *c2,KVal *horiz){
    (void)x;(void)y;(void)w;(void)h;(void)c1;(void)c2;(void)horiz;return kv_void();
}
static inline KVal *k_ds_shadow(KVal *x,KVal *y,KVal *w,KVal *h,KVal *blur,KVal *col){
    (void)x;(void)y;(void)w;(void)h;(void)blur;(void)col;return kv_void();
}
static inline KVal *k_ds_text(KVal *x,KVal *y,KVal *t,KVal *col,KVal *size){
    (void)x;(void)y;(void)col;(void)size;
    if(t)printf("%s",t->s);return kv_void();
}
static inline KVal *k_ds_text_bold(KVal *x,KVal *y,KVal *t,KVal *col,KVal *size){
    (void)x;(void)y;(void)col;(void)size;
    if(t)printf("%s",t->s);return kv_void();
}
static inline KVal *k_ds_text_measure(KVal *t,KVal *size){
    (void)size;KVal *r=kv_dict();
    kdict_set(r->dict,"w",kv_int(t?strlen(t->s)*8:0));
    kdict_set(r->dict,"h",kv_int(16));return r;
}
static inline KVal *k_ds_parse_css(KVal *css){(void)css;return kv_list();}
static inline KVal *k_ds_parse_styles(KVal *props){(void)props;return kv_dict();}
static inline KVal *k_ds_box_model(KVal *style){(void)style;KVal *b=kv_dict();
    kdict_set(b->dict,"margin",kv_dict());kdict_set(b->dict,"border",kv_dict());
    kdict_set(b->dict,"padding",kv_dict());return b;
}
static inline KVal *k_ds_layout_flex(KVal *cw,KVal *ch,KVal *styles,KVal *sizes){
    (void)cw;(void)ch;(void)styles;(void)sizes;return kv_list();
}
static inline KVal *k_ds_layout_grid(KVal *cw,KVal *ch,KVal *cols,KVal *rows,KVal *sizes,KVal *gap_v){
    (void)cw;(void)ch;(void)cols;(void)rows;(void)sizes;(void)gap_v;return kv_list();
}
static inline KVal *k_ds_apply_style(KVal *widget,KVal *style){
    (void)widget;(void)style;return kv_void();
}
static inline KVal *k_ds_render_box(KVal *x,KVal *y,KVal *w,KVal *h,KVal *style){
    (void)x;(void)y;(void)w;(void)h;(void)style;return kv_void();
}
static inline KVal *k_ds_screen_size(void){KVal *r=kv_dict();
    kdict_set(r->dict,"w",kv_int(80));kdict_set(r->dict,"h",kv_int(24));return r;
}

#endif /* _WIN32 */
#endif /* K_DESIGN_H */
