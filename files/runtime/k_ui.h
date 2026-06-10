/* k_ui.h — Katlans UI (ui prefix) — Win32 GUI (Windows) / ncurses TUI (other) */
#ifndef K_UI_H
#define K_UI_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  Windows — Win32 GUI (real popup windows)
 * ═══════════════════════════════════════════════════════════════════════════ */
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>

typedef enum{KW_WIN,KW_LABEL,KW_BTN,KW_ENTRY,KW_TEXT,KW_CHECK,KW_SLIDER,KW_PROG}KWType;
typedef struct KWidget{
    KWType type;int y,x,h,w;
    char text[1024];char value[1024];
    int checked;int slider_val,slider_min,slider_max;int prog_val,prog_max;
    HWND hwnd;int visible;
}KWidget;
#define K_MAX_W 256
static KWidget *_kw[K_MAX_W];static int _knw=0;
static HWND _kmain_hwnd=NULL;static HINSTANCE _k_hinst=NULL;
static int _kui_initialized=0;static char _k_clip[4096]="";

static LRESULT CALLBACK _kui_wndproc(HWND h,UINT m,WPARAM w,LPARAM l){
    if(m==WM_CLOSE){ShowWindow(h,SW_HIDE);return 0;}
    if(m==WM_DESTROY){_kmain_hwnd=NULL;PostQuitMessage(0);return 0;}
    return DefWindowProc(h,m,w,l);
}

static void _kui_init(void){
    if(_kui_initialized)return;_kui_initialized=1;
    _k_hinst=GetModuleHandle(NULL);
    INITCOMMONCONTROLSEX icc={sizeof(INITCOMMONCONTROLSEX),ICC_PROGRESS_CLASS|ICC_BAR_CLASSES};
    InitCommonControlsEx(&icc);
    WNDCLASSEX wc={0};wc.cbSize=sizeof(WNDCLASSEX);wc.lpfnWndProc=_kui_wndproc;
    wc.hInstance=_k_hinst;wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);wc.lpszClassName="KatlansWindow";
    RegisterClassEx(&wc);
}

static KWidget *_kwa(KWType t){KWidget *w=(KWidget*)calloc(1,sizeof(KWidget));w->type=t;w->visible=1;w->h=1;w->w=40;if(_knw<K_MAX_W)_kw[_knw++]=w;return w;}
static KVal *_w2v(KWidget *w){return kv_int((long long)(size_t)w);}
static KWidget *_v2w(KVal *v){return (KWidget*)(size_t)v->i;}

static HWND _k_child(KWidget *p,const char *cls,const char *txt,DWORD style,int x,int y,int w,int h){
    if(!p||!p->hwnd)return NULL;
    RECT rc;GetClientRect(p->hwnd,&rc);
    int cx=x*8,cy=y*16+24;
    int cw=(w>0)?w*8:120,ch=(h>0)?h*16:24;
    if(cx+cw>rc.right)cw=rc.right-cx-4;if(cw<10)cw=120;
    return CreateWindowEx(0,cls,txt,WS_CHILD|WS_VISIBLE|style,cx,cy,cw,ch,p->hwnd,NULL,_k_hinst,NULL);
}

/* ── Window management ──────────────────────────────────────── */

static inline KVal *k_uiwin(KVal *title,KVal *width,KVal *height){
    _kui_init();KWidget *win=_kwa(KW_WIN);win->w=(int)width->i;win->h=(int)height->i;
    strncpy(win->text,title->s,1023);
    int ww=win->w*8+24,wh=win->h*16+60;
    RECT r={0,0,ww,wh};AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
    win->hwnd=CreateWindowEx(0,"KatlansWindow",win->text,
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,
        r.right-r.left,r.bottom-r.top,NULL,NULL,_k_hinst,NULL);
    _kmain_hwnd=win->hwnd;
    MSG msg;while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessage(&msg);}
    return _w2v(win);
}

static inline KVal *k_uiwin_run(KVal *wv){
    KWidget *w=_v2w(wv);if(w->hwnd){ShowWindow(w->hwnd,SW_SHOW);UpdateWindow(w->hwnd);}
    return kv_void();
}

static inline KVal *k_uiwin_close(KVal *wv){
    KWidget *w=_v2w(wv);
    if(w->hwnd&&IsWindow(w->hwnd))DestroyWindow(w->hwnd);
    w->hwnd=NULL;_kmain_hwnd=NULL;
    MSG msg;while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
        if(msg.message==WM_QUIT)break;
        TranslateMessage(&msg);DispatchMessage(&msg);
    }
    return kv_void();
}

static inline KVal *k_uiwin_title(KVal *wv,KVal *t){
    KWidget *w=_v2w(wv);strncpy(w->text,t->s,1023);
    if(w->hwnd)SetWindowText(w->hwnd,w->text);return kv_void();
}

static inline KVal *k_uiwin_flip(KVal *wv){
    KWidget *w=_v2w(wv);if(!w->hwnd)return kv_void();
    MSG msg;while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
        if(msg.message==WM_QUIT)break;
        TranslateMessage(&msg);DispatchMessage(&msg);
    }
    UpdateWindow(w->hwnd);return kv_void();
}

static inline KVal *k_uiwin_center(KVal *wv){
    KWidget *w=_v2w(wv);if(!w->hwnd)return kv_void();
    RECT r;GetWindowRect(w->hwnd,&r);
    int sw=GetSystemMetrics(SM_CXSCREEN),sh=GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(w->hwnd,NULL,(sw-(r.right-r.left))/2,(sh-(r.bottom-r.top))/2,0,0,SWP_NOSIZE|SWP_NOZORDER);
    return kv_void();
}

static inline KVal *k_uirun(KVal *wv){return k_uiwin_flip(wv);}

/* ── Label ──────────────────────────────────────────────────── */

static inline KVal *k_uilabel(KVal *pv,KVal *t){
    KWidget *p=_v2w(pv);KWidget *l=_kwa(KW_LABEL);strncpy(l->text,t->s,1023);
    l->hwnd=_k_child(p,"STATIC",l->text,SS_LEFT,l->x,l->y,l->w,l->h);return _w2v(l);
}
static inline KVal *k_uilabel_text(KVal *v,KVal *t){
    KWidget *w=_v2w(v);strncpy(w->text,t->s,1023);
    if(w->hwnd)SetWindowText(w->hwnd,w->text);return kv_void();
}

/* ── Button ──────────────────────────────────────────────────── */

static inline KVal *k_uibtn(KVal *pv,KVal *t){
    KWidget *p=_v2w(pv);KWidget *b=_kwa(KW_BTN);strncpy(b->text,t->s,1023);
    b->hwnd=_k_child(p,"BUTTON",b->text,BS_PUSHBUTTON,b->x,b->y,b->w,b->h);return _w2v(b);
}

/* ── Entry (text input) ─────────────────────────────────────── */

static inline KVal *k_uientry(KVal *pv){
    KWidget *p=_v2w(pv);KWidget *e=_kwa(KW_ENTRY);
    e->hwnd=_k_child(p,"EDIT","",WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,e->x,e->y,30,1);return _w2v(e);
}
static inline KVal *k_uientry_get(KVal *v){
    KWidget *w=_v2w(v);if(w->hwnd)GetWindowText(w->hwnd,w->value,1023);return kv_str(w->value);
}
static inline KVal *k_uientry_set(KVal *v,KVal *t){
    KWidget *w=_v2w(v);strncpy(w->value,t->s,1023);
    if(w->hwnd)SetWindowText(w->hwnd,w->value);return kv_void();
}
static inline KVal *k_uientry_clear(KVal *v){
    KWidget *w=_v2w(v);w->value[0]=0;if(w->hwnd)SetWindowText(w->hwnd,"");return kv_void();
}

/* ── Multiline text ─────────────────────────────────────────── */

static inline KVal *k_uitext(KVal *pv,KVal *w_,KVal *h){
    KWidget *p=_v2w(pv);KWidget *t=_kwa(KW_TEXT);t->w=(int)w_->i;t->h=(int)h->i;
    t->hwnd=_k_child(p,"EDIT","",WS_BORDER|ES_LEFT|ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL|ES_READONLY,t->x,t->y,t->w*8,t->h*16);
    return _w2v(t);
}
static inline KVal *k_uitext_insert(KVal *v,KVal *t){
    KWidget *w=_v2w(v);strncat(w->value,t->s,1023-strlen(w->value));
    if(w->hwnd){int n=GetWindowTextLength(w->hwnd);SendMessage(w->hwnd,EM_SETSEL,n,n);SendMessage(w->hwnd,EM_REPLACESEL,0,(LPARAM)t->s);}
    return kv_void();
}
static inline KVal *k_uitext_get(KVal *v){
    KWidget *w=_v2w(v);if(w->hwnd)GetWindowText(w->hwnd,w->value,1023);return kv_str(w->value);
}
static inline KVal *k_uitext_clear(KVal *v){
    KWidget *w=_v2w(v);w->value[0]=0;if(w->hwnd)SetWindowText(w->hwnd,"");return kv_void();
}

/* ── Checkbox ────────────────────────────────────────────────── */

static inline KVal *k_uicheck(KVal *pv,KVal *l){
    KWidget *p=_v2w(pv);KWidget *c=_kwa(KW_CHECK);strncpy(c->text,l->s,1023);c->checked=0;
    c->hwnd=_k_child(p,"BUTTON",c->text,BS_AUTOCHECKBOX|WS_TABSTOP,c->x,c->y,c->w,c->h);return _w2v(c);
}
static inline KVal *k_uicheck_get(KVal *v){
    KWidget *w=_v2w(v);if(w->hwnd)w->checked=(SendMessage(w->hwnd,BM_GETCHECK,0,0)==BST_CHECKED);
    return kv_bool(w->checked);
}
static inline KVal *k_uicheck_set(KVal *v,KVal *b){
    KWidget *w=_v2w(v);w->checked=(int)b->b;
    if(w->hwnd)SendMessage(w->hwnd,BM_SETCHECK,w->checked?BST_CHECKED:BST_UNCHECKED,0);
    return kv_void();
}
static inline KVal *k_uicheck_toggle(KVal *v){
    KWidget *w=_v2w(v);w->checked=!w->checked;
    if(w->hwnd)SendMessage(w->hwnd,BM_SETCHECK,w->checked?BST_CHECKED:BST_UNCHECKED,0);
    return kv_void();
}

/* ── Progress bar ────────────────────────────────────────────── */

static inline KVal *k_uiprog(KVal *pv,KVal *mn,KVal *mx){
    (void)mn;KWidget *p=_v2w(pv);KWidget *pr=_kwa(KW_PROG);pr->prog_val=0;pr->prog_max=(int)mx->i;
    pr->hwnd=_k_child(p,PROGRESS_CLASS,"",0,pr->x,pr->y,20,1);
    if(pr->hwnd){SendMessage(pr->hwnd,PBM_SETRANGE,0,MAKELPARAM(0,pr->prog_max));SendMessage(pr->hwnd,PBM_SETPOS,0,0);}
    return _w2v(pr);
}
static inline KVal *k_uiprog_set(KVal *v,KVal *n){
    KWidget *w=_v2w(v);w->prog_val=(int)n->i;if(w->hwnd)SendMessage(w->hwnd,PBM_SETPOS,w->prog_val,0);
    return kv_void();
}

/* ── Slider (trackbar) ───────────────────────────────────────── */

static inline KVal *k_uislider(KVal *pv,KVal *mn,KVal *mx){
    KWidget *p=_v2w(pv);KWidget *s=_kwa(KW_SLIDER);s->slider_min=(int)mn->i;s->slider_max=(int)mx->i;s->slider_val=0;
    s->hwnd=_k_child(p,TRACKBAR_CLASS,"",TBS_HORZ|TBS_AUTOTICKS,s->x,s->y,20,1);
    if(s->hwnd){SendMessage(s->hwnd,TBM_SETRANGE,TRUE,MAKELPARAM(s->slider_min,s->slider_max));SendMessage(s->hwnd,TBM_SETPOS,TRUE,0);}
    return _w2v(s);
}
static inline KVal *k_uislider_get(KVal *v){
    KWidget *w=_v2w(v);if(w->hwnd)w->slider_val=(int)SendMessage(w->hwnd,TBM_GETPOS,0,0);
    return kv_int(w->slider_val);
}
static inline KVal *k_uislider_set(KVal *v,KVal *n){
    KWidget *w=_v2w(v);w->slider_val=(int)n->i;if(w->hwnd)SendMessage(w->hwnd,TBM_SETPOS,TRUE,w->slider_val);
    return kv_void();
}

/* ── Layout ──────────────────────────────────────────────────── */

static inline KVal *k_uipack(KVal *v){(void)v;return kv_void();}
static inline KVal *k_uigrid(KVal *v,KVal *r,KVal *c){
    KWidget *w=_v2w(v);w->y=(int)r->i;w->x=(int)c->i;
    if(w->hwnd)SetWindowPos(w->hwnd,NULL,w->x*8,w->y*16+24,0,0,SWP_NOSIZE|SWP_NOZORDER);
    return kv_void();
}
static inline KVal *k_uiplace(KVal *v,KVal *x,KVal *y){
    KWidget *w=_v2w(v);w->x=(int)x->i;w->y=(int)y->i;
    if(w->hwnd)SetWindowPos(w->hwnd,NULL,w->x*8,w->y*16+24,0,0,SWP_NOSIZE|SWP_NOZORDER);
    return kv_void();
}

/* ── Dialogs (real Windows message boxes) ────────────────────── */

static inline KVal *k_uidialog_info(KVal *title,KVal *msg){
    MessageBox(_kmain_hwnd,msg->s,title->s,MB_OK|MB_ICONINFORMATION);return kv_void();
}
static inline KVal *k_uidialog_ask(KVal *title,KVal *msg){
    int r=MessageBox(_kmain_hwnd,msg->s,title->s,MB_YESNO|MB_ICONQUESTION);
    return kv_bool(r==IDYES);
}
static inline KVal *k_uidialog_input(KVal *prompt){
    printf("%s\n> ",prompt->s);fflush(stdout);
    char buf[256]="";if(fgets(buf,256,stdin)){size_t n=strlen(buf);if(n&&buf[n-1]=='\n')buf[n-1]=0;}
    return kv_str(buf);
}

/* ── Clipboard ────────────────────────────────────────────────── */

static inline KVal *k_uiclip_set(KVal *v){
    strncpy(_k_clip,v->s,4095);
    if(OpenClipboard(_kmain_hwnd)){EmptyClipboard();
        HGLOBAL h=GlobalAlloc(GMEM_MOVEABLE,strlen(v->s)+1);
        if(h){memcpy(GlobalLock(h),v->s,strlen(v->s)+1);GlobalUnlock(h);SetClipboardData(CF_TEXT,h);}
        CloseClipboard();}
    return kv_void();
}
static inline KVal *k_uiclip_get(void){
    if(OpenClipboard(_kmain_hwnd)){
        HANDLE h=GetClipboardData(CF_TEXT);
        if(h){char *p=(char*)GlobalLock(h);if(p){strncpy(_k_clip,p,4095);GlobalUnlock(h);}}
        CloseClipboard();}
    return kv_str(_k_clip);
}

/* ── Variable helpers ────────────────────────────────────────── */

static inline KVal *k_uivar_str(KVal *d){return kv_str(d?d->s:"");}
static inline KVal *k_uivar_int(KVal *d){return kv_int(d?d->i:0);}
static inline KVal *k_uivar_bool(KVal *d){return kv_bool(d&&d->b);}
static inline KVal *k_uivar_get(KVal *v){return v;}
static inline KVal *k_uivar_set(KVal *v,KVal *n){v->type=n->type;v->i=n->i;if(n->type==KT_STR)v->s=strdup(n->s);return kv_void();}

/* ── Screen info ──────────────────────────────────────────────── */

static inline KVal *k_uiinfo_screenwidth(KVal *w){(void)w;return kv_int(GetSystemMetrics(SM_CXSCREEN));}
static inline KVal *k_uiinfo_screenheight(KVal *w){(void)w;return kv_int(GetSystemMetrics(SM_CYSCREEN));}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Non-Windows — ncurses TUI
 * ═══════════════════════════════════════════════════════════════════════════ */
#else

#include <ncurses.h>

typedef enum{KW_WIN,KW_LABEL,KW_BTN,KW_ENTRY,KW_TEXT,KW_CHECK,KW_SLIDER,KW_PROG}KWType;
typedef struct KWidget{KWType type;int y,x,h,w;char text[1024];char value[1024];int checked;int slider_val,slider_min,slider_max;int prog_val,prog_max;WINDOW *win;int visible;}KWidget;
#define K_MAX_W 256
static KWidget *_kw[K_MAX_W]; static int _knw=0; static WINDOW *_kmw=NULL;
static char _k_clip[4096]="";

static void _kui_init(void){if(_kmw)return;_kmw=initscr();cbreak();noecho();keypad(stdscr,TRUE);nodelay(stdscr,TRUE);curs_set(0);if(has_colors()){start_color();init_pair(1,COLOR_RED,COLOR_BLACK);init_pair(2,COLOR_GREEN,COLOR_BLACK);init_pair(3,COLOR_YELLOW,COLOR_BLACK);init_pair(4,COLOR_BLUE,COLOR_BLACK);init_pair(5,COLOR_CYAN,COLOR_BLACK);}}
static KWidget *_kwa(KWType t){KWidget *w=(KWidget*)calloc(1,sizeof(KWidget));w->type=t;w->visible=1;w->h=1;w->w=40;if(_knw<K_MAX_W)_kw[_knw++]=w;return w;}
static KVal *_w2v(KWidget *w){return kv_int((long long)(size_t)w);}
static KWidget *_v2w(KVal *v){return (KWidget*)(size_t)v->i;}

static inline KVal *k_uiwin(KVal *title,KVal *w,KVal *h){_kui_init();KWidget *win=_kwa(KW_WIN);win->w=(int)w->i;win->h=(int)h->i;strncpy(win->text,title->s,1023);win->win=newwin(win->h,win->w,0,0);box(win->win,0,0);mvwprintw(win->win,0,2," %s ",title->s);wrefresh(win->win);return _w2v(win);}
static inline KVal *k_uiwin_run(KVal *wv){KWidget *w=_v2w(wv);if(w->win){refresh();wrefresh(w->win);}return kv_void();}
static inline KVal *k_uiwin_close(KVal *wv){(void)wv;endwin();_kmw=NULL;return kv_void();}
static inline KVal *k_uiwin_title(KVal *wv,KVal *t){strncpy(_v2w(wv)->text,t->s,1023);return kv_void();}
static inline KVal *k_uiwin_flip(KVal *wv){KWidget *w=_v2w(wv);if(!w->win)return kv_void();wclear(w->win);box(w->win,0,0);mvwprintw(w->win,0,2," %s ",w->text);for(int i=0;i<_knw;i++){KWidget *wi=_kw[i];if(wi==w||!wi->visible)continue;switch(wi->type){case KW_LABEL:mvwprintw(w->win,wi->y,wi->x,"%s",wi->text);break;case KW_BTN:mvwprintw(w->win,wi->y,wi->x,"[ %s ]",wi->text);break;case KW_ENTRY:mvwprintw(w->win,wi->y,wi->x,"[%-30s]",wi->value);break;case KW_CHECK:mvwprintw(w->win,wi->y,wi->x,"[%s] %s",wi->checked?"X":" ",wi->text);break;case KW_PROG:{int f=wi->prog_max>0?wi->prog_val*20/wi->prog_max:0;mvwprintw(w->win,wi->y,wi->x,"[");for(int j=0;j<20;j++)waddch(w->win,j<f?'#':'-');wprintw(w->win,"] %d%%",wi->prog_max>0?wi->prog_val*100/wi->prog_max:0);break;}case KW_SLIDER:{int p2=wi->slider_max>wi->slider_min?(wi->slider_val-wi->slider_min)*20/(wi->slider_max-wi->slider_min):0;mvwprintw(w->win,wi->y,wi->x,"<");for(int j=0;j<20;j++)waddch(w->win,j==p2?'O':'-');wprintw(w->win,"> %d",wi->slider_val);break;}default:break;}}wrefresh(w->win);refresh();return kv_void();}
static inline KVal *k_uiwin_center(KVal *wv){KWidget *w=_v2w(wv);int r,c;getmaxyx(stdscr,r,c);mvwin(w->win,(r-w->h)/2,(c-w->w)/2);return kv_void();}
static inline KVal *k_uirun(KVal *wv){return k_uiwin_flip(wv);}

static inline KVal *k_uilabel(KVal *pv,KVal *t){(void)pv;KWidget *l=_kwa(KW_LABEL);strncpy(l->text,t->s,1023);return _w2v(l);}
static inline KVal *k_uilabel_text(KVal *v,KVal *t){strncpy(_v2w(v)->text,t->s,1023);return kv_void();}
static inline KVal *k_uibtn(KVal *pv,KVal *t){(void)pv;KWidget *b=_kwa(KW_BTN);strncpy(b->text,t->s,1023);return _w2v(b);}
static inline KVal *k_uientry(KVal *pv){(void)pv;KWidget *e=_kwa(KW_ENTRY);return _w2v(e);}
static inline KVal *k_uientry_get(KVal *v){return kv_str(_v2w(v)->value);}
static inline KVal *k_uientry_set(KVal *v,KVal *t){strncpy(_v2w(v)->value,t->s,1023);return kv_void();}
static inline KVal *k_uientry_clear(KVal *v){_v2w(v)->value[0]=0;return kv_void();}
static inline KVal *k_uitext(KVal *pv,KVal *w_,KVal *h){(void)pv;KWidget *t=_kwa(KW_TEXT);t->w=(int)w_->i;t->h=(int)h->i;return _w2v(t);}
static inline KVal *k_uitext_insert(KVal *v,KVal *t){strncat(_v2w(v)->value,t->s,1023-strlen(_v2w(v)->value));return kv_void();}
static inline KVal *k_uitext_get(KVal *v){return kv_str(_v2w(v)->value);}
static inline KVal *k_uitext_clear(KVal *v){_v2w(v)->value[0]=0;return kv_void();}
static inline KVal *k_uicheck(KVal *pv,KVal *l){(void)pv;KWidget *c=_kwa(KW_CHECK);strncpy(c->text,l->s,1023);return _w2v(c);}
static inline KVal *k_uicheck_get(KVal *v){return kv_bool(_v2w(v)->checked);}
static inline KVal *k_uicheck_set(KVal *v,KVal *b){_v2w(v)->checked=(int)b->b;return kv_void();}
static inline KVal *k_uicheck_toggle(KVal *v){KWidget *w=_v2w(v);w->checked=!w->checked;return kv_void();}
static inline KVal *k_uiprog(KVal *pv,KVal *mn,KVal *mx){(void)pv;(void)mn;KWidget *p=_kwa(KW_PROG);p->prog_val=0;p->prog_max=(int)mx->i;return _w2v(p);}
static inline KVal *k_uiprog_set(KVal *v,KVal *n){_v2w(v)->prog_val=(int)n->i;return kv_void();}
static inline KVal *k_uislider(KVal *pv,KVal *mn,KVal *mx){(void)pv;KWidget *s=_kwa(KW_SLIDER);s->slider_min=(int)mn->i;s->slider_max=(int)mx->i;return _w2v(s);}
static inline KVal *k_uislider_get(KVal *v){return kv_int(_v2w(v)->slider_val);}
static inline KVal *k_uislider_set(KVal *v,KVal *n){_v2w(v)->slider_val=(int)n->i;return kv_void();}
static inline KVal *k_uipack(KVal *v){(void)v;return kv_void();}
static inline KVal *k_uigrid(KVal *v,KVal *r,KVal *c){KWidget *w=_v2w(v);w->y=(int)r->i;w->x=(int)c->i;return kv_void();}
static inline KVal *k_uiplace(KVal *v,KVal *x,KVal *y){KWidget *w=_v2w(v);w->x=(int)x->i;w->y=(int)y->i;return kv_void();}
static inline KVal *k_uidialog_info(KVal *title,KVal *msg){_kui_init();int r,c;getmaxyx(stdscr,r,c);int w=(int)strlen(msg->s)+8,h=5;WINDOW *d=newwin(h,w,(r-h)/2,(c-w)/2);box(d,0,0);mvwprintw(d,0,2," %s ",title->s);mvwprintw(d,2,2,"%s",msg->s);mvwprintw(d,3,w/2-2,"[OK]");wrefresh(d);nodelay(stdscr,FALSE);wgetch(d);nodelay(stdscr,TRUE);delwin(d);refresh();return kv_void();}
static inline KVal *k_uidialog_ask(KVal *title,KVal *msg){_kui_init();int r,c;getmaxyx(stdscr,r,c);int w=(int)strlen(msg->s)+12,h=6;WINDOW *d=newwin(h,w,(r-h)/2,(c-w)/2);box(d,0,0);mvwprintw(d,0,2," %s ",title->s);mvwprintw(d,2,2,"%s",msg->s);mvwprintw(d,4,2,"[Y]es  [N]o");wrefresh(d);nodelay(stdscr,FALSE);int ch=wgetch(d);nodelay(stdscr,TRUE);delwin(d);refresh();return kv_bool(ch=='y'||ch=='Y');}
static inline KVal *k_uidialog_input(KVal *prompt){_kui_init();int r,c;getmaxyx(stdscr,r,c);int pw=(int)strlen(prompt->s)+20;WINDOW *d=newwin(5,pw,(r-5)/2,(c-pw)/2);box(d,0,0);mvwprintw(d,1,2,"%s",prompt->s);mvwprintw(d,2,2,"> ");wrefresh(d);char buf[256]="";nodelay(stdscr,FALSE);echo();wgetnstr(d,buf,254);noecho();nodelay(stdscr,TRUE);delwin(d);refresh();return kv_str(buf);}
static inline KVal *k_uiclip_set(KVal *v){strncpy(_k_clip,v->s,4095);return kv_void();}
static inline KVal *k_uiclip_get(void){return kv_str(_k_clip);}
static inline KVal *k_uivar_str(KVal *d){return kv_str(d?d->s:"");}
static inline KVal *k_uivar_int(KVal *d){return kv_int(d?d->i:0);}
static inline KVal *k_uivar_bool(KVal *d){return kv_bool(d&&d->b);}
static inline KVal *k_uivar_get(KVal *v){return v;}
static inline KVal *k_uivar_set(KVal *v,KVal *n){v->type=n->type;v->i=n->i;if(n->type==KT_STR)v->s=strdup(n->s);return kv_void();}
static inline KVal *k_uiinfo_screenwidth(KVal *w){(void)w;int r,c;getmaxyx(stdscr,r,c);(void)r;return kv_int((long long)c);}
static inline KVal *k_uiinfo_screenheight(KVal *w){(void)w;int r,c;getmaxyx(stdscr,r,c);(void)c;return kv_int((long long)r);}

#endif /* _WIN32 */
#endif /* K_UI_H */
