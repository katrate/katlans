/* k_game.h — Katlans Game Engine (gm prefix) */
#ifndef K_GAME_H
#define K_GAME_H
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  Windows — Win32 GDI Game Engine (real window)
 * ═══════════════════════════════════════════════════════════════════════════ */
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static HWND _gw_hwnd=NULL;static int _gr=1;static int _gfps=60;
static long long _gfs=0;static char _gcap[256]="Katlans Game";static int _glk=-1;
static int _gw_cx=800,_gw_cy=600;
static HDC _g_hdc=NULL,_g_bdc=NULL;static HBITMAP _g_hbmp=NULL;
static int _g_keys[256]={0},_g_newk[256]={0};

static LRESULT CALLBACK _g_wndproc(HWND h,UINT m,WPARAM w,LPARAM l){
    if(m==WM_KEYDOWN&&w<256){_g_keys[w]=1;_g_newk[w]=1;_glk=(int)w;return 0;}
    if(m==WM_KEYUP&&w<256){_g_keys[w]=0;return 0;}
    if(m==WM_CLOSE){_gr=0;_gw_hwnd=NULL;DestroyWindow(h);return 0;}
    if(m==WM_DESTROY){_gw_hwnd=NULL;if(_g_bdc){DeleteDC(_g_bdc);_g_bdc=NULL;}if(_g_hbmp){DeleteObject(_g_hbmp);_g_hbmp=NULL;}if(_g_hdc){ReleaseDC(h,_g_hdc);_g_hdc=NULL;}PostQuitMessage(0);return 0;}
    if(m==WM_PAINT&&_g_bdc){
        PAINTSTRUCT ps;HDC hdc=BeginPaint(h,&ps);
        BitBlt(hdc,0,0,_gw_cx,_gw_cy,_g_bdc,0,0,SRCCOPY);
        EndPaint(h,&ps);return 0;
    }
    return DefWindowProc(h,m,w,l);
}

static long long _gmic(void){
    FILETIME ft;GetSystemTimeAsFileTime(&ft);
    return (((long long)ft.dwHighDateTime<<32)|ft.dwLowDateTime)/10LL;
}

static inline KVal *k_gm_init(void){
    _gr=1;_gfs=_gmic();memset(_g_keys,0,sizeof(_g_keys));memset(_g_newk,0,sizeof(_g_newk));return kv_void();
}

static inline KVal *k_gmwin(KVal *title,KVal *w,KVal *h){
    if(!_gw_hwnd){
        HINSTANCE hi=GetModuleHandle(NULL);
        WNDCLASSEX wc={0};wc.cbSize=sizeof(WNDCLASSEX);wc.lpfnWndProc=_g_wndproc;
        wc.hInstance=hi;wc.hCursor=LoadCursor(NULL,IDC_ARROW);
        wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszClassName="KatlansGame";RegisterClassEx(&wc);
        _gw_cx=(int)w->i;_gw_cy=(int)h->i;strncpy(_gcap,title->s,255);
        RECT r={0,0,_gw_cx,_gw_cy};AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
        _gw_hwnd=CreateWindowEx(0,"KatlansGame",_gcap,
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,
            r.right-r.left,r.bottom-r.top,NULL,NULL,hi,NULL);
        _g_hdc=GetDC(_gw_hwnd);_g_bdc=CreateCompatibleDC(_g_hdc);
        _g_hbmp=CreateCompatibleBitmap(_g_hdc,_gw_cx,_gw_cy);
        SelectObject(_g_bdc,_g_hbmp);
        HBRUSH hb=CreateSolidBrush(RGB(0,0,0));
        RECT rc={0,0,_gw_cx,_gw_cy};FillRect(_g_bdc,&rc,hb);DeleteObject(hb);
        MSG msg;while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessage(&msg);}
    }
    _gr=1;_gfs=_gmic();return kv_int((long long)(size_t)_gw_hwnd);
}

static inline KVal *k_gmwin_clear(KVal *w){(void)w;
    HBRUSH hb=CreateSolidBrush(RGB(0,0,0));RECT rc={0,0,_gw_cx,_gw_cy};
    FillRect(_g_bdc,&rc,hb);DeleteObject(hb);return kv_void();
}

static inline KVal *k_gmwin_flip(KVal *w){(void)w;
    long long now=_gmic(),fu=_gfps>0?1000000LL/_gfps:16667LL,el=now-_gfs;
    if(el<fu)Sleep((DWORD)(fu-el)/1000);
    InvalidateRect(_gw_hwnd,NULL,FALSE);
    MSG msg;while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
        if(msg.message==WM_QUIT)break;
        TranslateMessage(&msg);DispatchMessage(&msg);
    }
    _gfs=_gmic();return kv_void();
}

static inline KVal *k_gmwin_caption(KVal *t){strncpy(_gcap,t->s,255);if(_gw_hwnd)SetWindowText(_gw_hwnd,_gcap);return kv_void();}
static inline KVal *k_gmwin_fps(KVal *f){_gfps=(int)f->i;if(_gfps<1)_gfps=1;return kv_void();}
static inline KVal *k_gmwin_hdc(void){return kv_int((long long)(size_t)_g_bdc);}
static inline KVal *k_gmwin_size(void){KVal *_r=kv_dict();kdict_set(_r->dict,"w",kv_int(_gw_cx));kdict_set(_r->dict,"h",kv_int(_gw_cy));return _r;}
static inline KVal *k_gm_running(void){return kv_bool(_gr);}
static inline KVal *k_gm_stop(void){_gr=0;return kv_void();}
static inline KVal *k_gm_quit(void){_gr=0;if(_gw_hwnd){DestroyWindow(_gw_hwnd);_gw_hwnd=NULL;}return kv_void();}

static inline KVal *k_gm_events(void){
    KVal *lst=kv_list();
    for(int i=0;i<256;i++){
        if(!_g_newk[i])continue;_g_newk[i]=0;
        KVal *ev=kv_dict();char buf[2]={(char)i,0};
        if(i==VK_ESCAPE)kdict_set(ev->dict,"key",kv_str("escape"));
        else if(i==VK_RETURN)kdict_set(ev->dict,"key",kv_str("return"));
        else if(i==VK_SPACE)kdict_set(ev->dict,"key",kv_str("space"));
        else if(i==VK_UP)kdict_set(ev->dict,"key",kv_str("up"));
        else if(i==VK_DOWN)kdict_set(ev->dict,"key",kv_str("down"));
        else if(i==VK_LEFT)kdict_set(ev->dict,"key",kv_str("left"));
        else if(i==VK_RIGHT)kdict_set(ev->dict,"key",kv_str("right"));
        else if(i=='Q')kdict_set(ev->dict,"key",kv_str("q"));
        else{kdict_set(ev->dict,"key",kv_str(buf));}
        kdict_set(ev->dict,"type",kv_str("keydown"));
        klist_push(lst->list,ev);
    }
    return lst;
}

static inline KVal *k_gminput_key_pressed(KVal *k){
    int vk=toupper(k->s[0]);return kv_bool(_g_keys[vk]?true:false);
}

static inline KVal *k_gminput_mouse_pos(void){
    if(!_gw_hwnd)return kv_dict();
    POINT p;GetCursorPos(&p);ScreenToClient(_gw_hwnd,&p);
    KVal *r=kv_dict();kdict_set(r->dict,"x",kv_int(p.x));kdict_set(r->dict,"y",kv_int(p.y));return r;
}

/* ── Drawing (GDI) ────────────────────────────────────────────── */

static COLORREF _g_col(KVal *c){
    if(!c||c->type==KT_VOID)return RGB(255,255,255);
    int ci=(int)_knum(c);return RGB((ci>>16)&255,(ci>>8)&255,ci&255);
}

static inline KVal *k_gmdraw_rect(KVal *w,KVal *col,KVal *x,KVal *y,KVal *dw,KVal *dh){
    (void)w;COLORREF cl=_g_col(col);
    HBRUSH hb=CreateSolidBrush(cl);HPEN hp=CreatePen(PS_SOLID,1,cl);
    SelectObject(_g_bdc,hb);SelectObject(_g_bdc,hp);
    Rectangle(_g_bdc,(int)_knum(x),(int)_knum(y),(int)_knum(x)+_knum(dw),(int)_knum(y)+_knum(dh));
    DeleteObject(hp);DeleteObject(hb);return kv_void();
}

static inline KVal *k_gmdraw_circle(KVal *w,KVal *col,KVal *cx,KVal *cy,KVal *r){
    (void)w;COLORREF cl=_g_col(col);int rr=(int)_knum(r);
    HBRUSH hb=CreateSolidBrush(cl);HPEN hp=CreatePen(PS_SOLID,1,cl);
    SelectObject(_g_bdc,hb);SelectObject(_g_bdc,hp);
    Ellipse(_g_bdc,(int)_knum(cx)-rr,(int)_knum(cy)-rr,(int)_knum(cx)+rr,(int)_knum(cy)+rr);
    DeleteObject(hp);DeleteObject(hb);return kv_void();
}

static inline KVal *k_gmdraw_line(KVal *w,KVal *col,KVal *x1,KVal *y1,KVal *x2,KVal *y2){
    (void)w;COLORREF cl=_g_col(col);
    HPEN hp=CreatePen(PS_SOLID,1,cl);SelectObject(_g_bdc,hp);
    MoveToEx(_g_bdc,(int)_knum(x1),(int)_knum(y1),NULL);
    LineTo(_g_bdc,(int)_knum(x2),(int)_knum(y2));
    DeleteObject(hp);return kv_void();
}

static inline KVal *k_gmdraw_text(KVal *w,KVal *x,KVal *y,KVal *t){
    (void)w;SetBkMode(_g_bdc,TRANSPARENT);SetTextColor(_g_bdc,RGB(255,255,255));
    TextOutA(_g_bdc,(int)_knum(x),(int)_knum(y),t->s,(int)strlen(t->s));return kv_void();
}

static inline KVal *k_gmdraw_point(KVal *w,KVal *col,KVal *x,KVal *y){
    (void)w;SetPixel(_g_bdc,(int)_knum(x),(int)_knum(y),_g_col(col));return kv_void();
}

/* ── Sprites ──────────────────────────────────────────────────── */

typedef struct{double x,y,vx,vy,ax,ay;int w,h;char img[64];int visible;double angle;}KSprite;
static inline KVal *k_gmsprite(KVal *img){
    KSprite *s=(KSprite*)calloc(1,sizeof(KSprite));strncpy(s->img,img->s,63);
    s->w=12;s->h=16;s->visible=1;return kv_int((long long)(size_t)s);
}
static KSprite *_ksp(KVal *v){return(KSprite*)(size_t)v->i;}
static inline KVal *k_gmsp_pos(KVal *sv,KVal *x,KVal *y){KSprite *s=_ksp(sv);s->x=_knum(x);s->y=_knum(y);return kv_void();}
static inline KVal *k_gmsp_x(KVal *sv,KVal *x){_ksp(sv)->x=_knum(x);return kv_void();}
static inline KVal *k_gmsp_y(KVal *sv,KVal *y){_ksp(sv)->y=_knum(y);return kv_void();}
static inline KVal *k_gmsp_vel(KVal *sv,KVal *vx,KVal *vy){KSprite *s=_ksp(sv);s->vx=_knum(vx);s->vy=_knum(vy);return kv_void();}
static inline KVal *k_gmsp_move(KVal *sv,KVal *dx,KVal *dy){KSprite *s=_ksp(sv);s->x+=_knum(dx);s->y+=_knum(dy);return kv_void();}
static inline KVal *k_gmsp_moveto(KVal *sv,KVal *x,KVal *y){KSprite *s=_ksp(sv);s->x=_knum(x);s->y=_knum(y);return kv_void();}
static inline KVal *k_gmsp_visible(KVal *sv,KVal *v){_ksp(sv)->visible=v->b;return kv_void();}
static inline KVal *k_gmsp_accel(KVal *sv,KVal *ax,KVal *ay){KSprite *s=_ksp(sv);s->ax=_knum(ax);s->ay=_knum(ay);return kv_void();}
static inline KVal *k_gmsp_update(KVal *sv,KVal *dt){KSprite *s=_ksp(sv);double d=_knum(dt);s->vx+=s->ax*d;s->vy+=s->ay*d;s->x+=s->vx*d;s->y+=s->vy*d;return kv_void();}

static inline KVal *k_gmsp_draw(KVal *sv,KVal *win){
    (void)win;KSprite *s=_ksp(sv);if(!s->visible)return kv_void();
    SetBkMode(_g_bdc,TRANSPARENT);SetTextColor(_g_bdc,RGB(255,255,0));
    if(strlen(s->img)>1)TextOutA(_g_bdc,(int)s->x,(int)s->y,s->img,(int)strlen(s->img));
    else{char buf[2]={s->img[0]?s->img[0]:'@',0};TextOutA(_g_bdc,(int)s->x,(int)s->y,buf,1);}
    return kv_void();
}

static inline KVal *k_gmsp_kill(KVal *sv){_ksp(sv)->visible=0;return kv_void();}
static inline KVal *k_gmsp_rect(KVal *sv){
    KSprite *s=_ksp(sv);KVal *r=kv_dict();kdict_set(r->dict,"x",kv_flt(s->x));kdict_set(r->dict,"y",kv_flt(s->y));
    kdict_set(r->dict,"w",kv_int(s->w));kdict_set(r->dict,"h",kv_int(s->h));return r;
}

static inline KVal *k_gmgroup(void){return kv_list();}
static inline KVal *k_gmgroup_add(KVal *g,KVal *s){klist_push(g->list,s);return kv_void();}
static inline KVal *k_gmgroup_remove(KVal *g,KVal *s){for(size_t i=0;i<g->list->len;i++)if(g->list->items[i]->i==s->i){memmove(&g->list->items[i],&g->list->items[i+1],(g->list->len-i-1)*sizeof(KVal*));g->list->len--;break;}return kv_void();}
static inline KVal *k_gmgroup_update(KVal *g,KVal *dt){for(size_t i=0;i<g->list->len;i++)k_gmsp_update(g->list->items[i],dt);return kv_void();}
static inline KVal *k_gmgroup_draw(KVal *g,KVal *w){for(size_t i=0;i<g->list->len;i++)k_gmsp_draw(g->list->items[i],w);return kv_void();}
static inline KVal *k_gmgroup_clear(KVal *g){g->list->len=0;return kv_void();}
static inline KVal *k_gmgroup_count(KVal *g){return kv_int((long long)g->list->len);}

static inline KVal *k_gmclock(void){return kv_int(_gmic());}
static inline KVal *k_gmclock_tick(KVal *clock,KVal *fps){
    int _fps_i=(int)fps->i;if(_fps_i<1)_fps_i=1;long long now=_gmic(),last=clock->i,fu=1000000LL/_fps_i,el=now-last;
    if(el<fu)Sleep((DWORD)(fu-el)/1000);clock->i=_gmic();return kv_flt((double)(now-last)/1000.0);
}

static inline KVal *k_gmcol_rect(KVal *av,KVal *bv){KSprite *a=_ksp(av),*b=_ksp(bv);return kv_bool(a->x<b->x+b->w&&a->x+a->w>b->x&&a->y<b->y+b->h&&a->y+a->h>b->y);}
static inline KVal *k_gmcol_rect_point(KVal *sv,KVal *x,KVal *y){KSprite *s=_ksp(sv);double px=_knum(x),py=_knum(y);return kv_bool(px>=s->x&&px<s->x+s->w&&py>=s->y&&py<s->y+s->h);}
static inline KVal *k_gmcol_circle(KVal *av,KVal *bv){KSprite *a=_ksp(av),*b=_ksp(bv);double dx=a->x-b->x,dy=a->y-b->y;return kv_bool(sqrt(dx*dx+dy*dy)<(a->w+b->w)/2.0);}
static inline KVal *k_gmcol_group(KVal *sv,KVal *g){KVal *h=kv_list();for(size_t i=0;i<g->list->len;i++)if(k_gmcol_rect(sv,g->list->items[i])->b)klist_push(h->list,g->list->items[i]);return h;}
static inline KVal *k_gmcol_dist(KVal *av,KVal *bv){KSprite *a=_ksp(av),*b=_ksp(bv);double dx=a->x-b->x,dy=a->y-b->y;return kv_flt(sqrt(dx*dx+dy*dy));}

static inline KVal *k_gmsound(KVal *f){(void)f;return kv_int(0);}
static inline KVal *k_gmsound_play(KVal *s){(void)s;Beep(800,100);return kv_void();}
static inline KVal *k_gmsound_stop(KVal *s){(void)s;return kv_void();}
static inline KVal *k_gmsound_vol(KVal *s,KVal *v){(void)s;(void)v;return kv_void();}
static inline KVal *k_gmmusic_play(void){return kv_void();}
static inline KVal *k_gmmusic_stop(void){return kv_void();}
static inline KVal *k_gmfont(KVal *n,KVal *s){(void)n;(void)s;return kv_int(0);}
static inline KVal *k_gmfont_render(KVal *f,KVal *t,KVal *c){(void)f;(void)c;return kv_str(t->s);}
static inline KVal *k_gmtext_draw(KVal *w,KVal *t,KVal *f,KVal *c,KVal *r){
    (void)w;(void)f;(void)c;KVal *x=kdict_get(r->dict,"x"),*y=kdict_get(r->dict,"y");
    SetBkMode(_g_bdc,TRANSPARENT);SetTextColor(_g_bdc,RGB(255,255,255));
    TextOutA(_g_bdc,(int)_knum(x),(int)_knum(y),t->s,(int)strlen(t->s));return kv_void();
}
static inline KVal *k_gmcam(KVal *w,KVal *h){(void)w;(void)h;return kv_int(0);}
static inline KVal *k_gmcam_follow(KVal *c,KVal *sv){(void)c;(void)sv;return kv_void();}
static inline KVal *k_gmcam_pos(KVal *c,KVal *x,KVal *y){(void)c;(void)x;(void)y;return kv_void();}
static inline KVal *k_gmcam_zoom(KVal *c,KVal *z){(void)c;(void)z;return kv_void();}
static inline KVal *k_gmstate_save(KVal *p,KVal *s){extern KVal*k_jswrite(KVal*,KVal*);return k_jswrite(p,s);}
static inline KVal *k_gmstate_load(KVal *p){extern KVal*k_jsread(KVal*);return k_jsread(p);}
static inline KVal *k_gmstate_exists(KVal *p){extern KVal*k_fexists(KVal*);return k_fexists(p);}
static inline KVal *k_gm_event_post(KVal *t,KVal *d){(void)t;(void)d;return kv_void();}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Non-Windows — ncurses TUI
 * ═══════════════════════════════════════════════════════════════════════════ */
#else

#include <ncurses.h>

static WINDOW *_gw=NULL;static int _gr=1;static int _gfps=60;
static long long _gfs=0;static char _gcap[256]="Katlans Game";static int _glk=-1;

static long long _gmic(void){struct timeval tv;gettimeofday(&tv,NULL);return(long long)tv.tv_sec*1000000LL+tv.tv_usec;}

static inline KVal *k_gm_init(void){_gr=1;_gfs=_gmic();return kv_void();}
static inline KVal *k_gmwin(KVal *title,KVal *w,KVal *h){
    if(!_gw){_gw=initscr();cbreak();noecho();keypad(stdscr,TRUE);nodelay(stdscr,TRUE);curs_set(0);if(has_colors()){start_color();use_default_colors();}}
    strncpy(_gcap,title->s,255);(void)w;(void)h;_gr=1;_gfs=_gmic();return kv_int((long long)(size_t)_gw);
}
static inline KVal *k_gmwin_clear(KVal *w){(void)w;clear();return kv_void();}
static inline KVal *k_gmwin_flip(KVal *w){(void)w;long long now=_gmic();long long fu=1000000LL/_gfps;long long el=now-_gfs;if(el<fu)usleep((useconds_t)(fu-el));refresh();_gfs=_gmic();return kv_void();}
static inline KVal *k_gmwin_caption(KVal *t){strncpy(_gcap,t->s,255);return kv_void();}
static inline KVal *k_gmwin_fps(KVal *f){_gfps=(int)f->i;if(_gfps<1)_gfps=1;return kv_void();}
static inline KVal *k_gm_running(void){return kv_bool(_gr);}
static inline KVal *k_gm_stop(void){_gr=0;return kv_void();}
static inline KVal *k_gm_quit(void){_gr=0;if(_gw){endwin();_gw=NULL;}return kv_void();}
static inline KVal *k_gm_events(void){
    KVal *lst=kv_list();int ch=getch();if(ch==ERR)return lst;_glk=ch;
    KVal *ev=kv_dict();
    if(ch==27){kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str("escape"));}
    else if(ch=='\n'||ch=='\r'){kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str("return"));}
    else if(ch==' '){kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str("space"));}
    else if(ch==KEY_UP){kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str("up"));}
    else if(ch==KEY_DOWN){kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str("down"));}
    else if(ch==KEY_LEFT){kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str("left"));}
    else if(ch==KEY_RIGHT){kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str("right"));}
    else if(ch=='q'||ch=='Q'){kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str("q"));}
    else{char buf[2]={(char)ch,0};kdict_set(ev->dict,"type",kv_str("keydown"));kdict_set(ev->dict,"key",kv_str(buf));}
    klist_push(lst->list,ev);return lst;
}
static inline KVal *k_gminput_key_pressed(KVal *k){return kv_bool(_glk>0&&strlen(k->s)==1&&_glk==k->s[0]);}
static inline KVal *k_gminput_mouse_pos(void){KVal *r=kv_dict();kdict_set(r->dict,"x",kv_int(0));kdict_set(r->dict,"y",kv_int(0));return r;}

static inline KVal *k_gmdraw_rect(KVal *w,KVal *col,KVal *x,KVal *y,KVal *dw,KVal *dh){(void)w;(void)col;int px=(int)x->i,py=(int)y->i,pw=(int)dw->i,ph=(int)dh->i;for(int r=py;r<py+ph;r++)for(int c=px;c<px+pw;c++)mvaddch(r,c,'#');return kv_void();}
static inline KVal *k_gmdraw_circle(KVal *w,KVal *col,KVal *cx,KVal *cy,KVal *r){(void)w;(void)col;int ccx=(int)cx->i,ccy=(int)cy->i,cr=(int)r->i;for(int row=ccy-cr;row<=ccy+cr;row++)for(int col2=ccx-cr*2;col2<=ccx+cr*2;col2++){double dx=(col2-ccx)/2.0,dy=row-ccy;if(dx*dx+dy*dy<=cr*cr)mvaddch(row,col2,'O');}return kv_void();}
static inline KVal *k_gmdraw_line(KVal *w,KVal *col,KVal *x1,KVal *y1,KVal *x2,KVal *y2){(void)w;(void)col;int ax=(int)x1->i,ay=(int)y1->i,bx=(int)x2->i,by=(int)y2->i;int dx=abs(bx-ax),dy=abs(by-ay),sx=ax<bx?1:-1,sy=ay<by?1:-1,err=dx-dy;while(1){mvaddch(ay,ax,'*');if(ax==bx&&ay==by)break;int e2=2*err;if(e2>-dy){err-=dy;ax+=sx;}if(e2<dx){err+=dx;ay+=sy;}}return kv_void();}
static inline KVal *k_gmdraw_text(KVal *w,KVal *x,KVal *y,KVal *t){(void)w;mvprintw((int)y->i,(int)x->i,"%s",t->s);return kv_void();}
static inline KVal *k_gmdraw_point(KVal *w,KVal *col,KVal *x,KVal *y){(void)w;(void)col;mvaddch((int)y->i,(int)x->i,'.');return kv_void();}

typedef struct{double x,y,vx,vy,ax,ay;int w,h;char img[64];int visible;double angle;}KSprite;
static inline KVal *k_gmsprite(KVal *img){KSprite *s=(KSprite*)calloc(1,sizeof(KSprite));strncpy(s->img,img->s,63);s->w=1;s->h=1;s->visible=1;return kv_int((long long)(size_t)s);}
static KSprite *_ksp(KVal *v){return(KSprite*)(size_t)v->i;}
static inline KVal *k_gmsp_pos(KVal *sv,KVal *x,KVal *y){KSprite *s=_ksp(sv);s->x=_knum(x);s->y=_knum(y);return kv_void();}
static inline KVal *k_gmsp_x(KVal *sv,KVal *x){_ksp(sv)->x=_knum(x);return kv_void();}
static inline KVal *k_gmsp_y(KVal *sv,KVal *y){_ksp(sv)->y=_knum(y);return kv_void();}
static inline KVal *k_gmsp_vel(KVal *sv,KVal *vx,KVal *vy){KSprite *s=_ksp(sv);s->vx=_knum(vx);s->vy=_knum(vy);return kv_void();}
static inline KVal *k_gmsp_move(KVal *sv,KVal *dx,KVal *dy){KSprite *s=_ksp(sv);s->x+=_knum(dx);s->y+=_knum(dy);return kv_void();}
static inline KVal *k_gmsp_moveto(KVal *sv,KVal *x,KVal *y){KSprite *s=_ksp(sv);s->x=_knum(x);s->y=_knum(y);return kv_void();}
static inline KVal *k_gmsp_visible(KVal *sv,KVal *v){_ksp(sv)->visible=v->b;return kv_void();}
static inline KVal *k_gmsp_accel(KVal *sv,KVal *ax,KVal *ay){KSprite *s=_ksp(sv);s->ax=_knum(ax);s->ay=_knum(ay);return kv_void();}
static inline KVal *k_gmsp_update(KVal *sv,KVal *dt){KSprite *s=_ksp(sv);double d=_knum(dt);s->vx+=s->ax*d;s->vy+=s->ay*d;s->x+=s->vx*d;s->y+=s->vy*d;return kv_void();}
static inline KVal *k_gmsp_draw(KVal *sv,KVal *win){(void)win;KSprite *s=_ksp(sv);if(!s->visible)return kv_void();if(strlen(s->img)>1)mvprintw((int)s->y,(int)s->x,"%s",s->img);else mvaddch((int)s->y,(int)s->x,s->img[0]?s->img[0]:'@');return kv_void();}
static inline KVal *k_gmsp_kill(KVal *sv){_ksp(sv)->visible=0;return kv_void();}
static inline KVal *k_gmsp_rect(KVal *sv){KSprite *s=_ksp(sv);KVal *r=kv_dict();kdict_set(r->dict,"x",kv_flt(s->x));kdict_set(r->dict,"y",kv_flt(s->y));kdict_set(r->dict,"w",kv_int(s->w));kdict_set(r->dict,"h",kv_int(s->h));return r;}

static inline KVal *k_gmgroup(void){return kv_list();}
static inline KVal *k_gmgroup_add(KVal *g,KVal *s){klist_push(g->list,s);return kv_void();}
static inline KVal *k_gmgroup_remove(KVal *g,KVal *s){for(size_t i=0;i<g->list->len;i++)if(g->list->items[i]->i==s->i){memmove(&g->list->items[i],&g->list->items[i+1],(g->list->len-i-1)*sizeof(KVal*));g->list->len--;break;}return kv_void();}
static inline KVal *k_gmgroup_update(KVal *g,KVal *dt){for(size_t i=0;i<g->list->len;i++)k_gmsp_update(g->list->items[i],dt);return kv_void();}
static inline KVal *k_gmgroup_draw(KVal *g,KVal *w){for(size_t i=0;i<g->list->len;i++)k_gmsp_draw(g->list->items[i],w);return kv_void();}
static inline KVal *k_gmgroup_clear(KVal *g){g->list->len=0;return kv_void();}
static inline KVal *k_gmgroup_count(KVal *g){return kv_int((long long)g->list->len);}

static inline KVal *k_gmclock(void){return kv_int((long long)_gmic());}
static inline KVal *k_gmclock_tick(KVal *clock,KVal *fps){    int _fps_i=(int)fps->i;if(_fps_i<1)_fps_i=1;long long now=_gmic(),last=clock->i,fu=1000000LL/_fps_i,el=now-last;if(el<fu)usleep((useconds_t)(fu-el));clock->i=_gmic();return kv_flt((double)(now-last)/1000.0);}

static inline KVal *k_gmcol_rect(KVal *av,KVal *bv){KSprite *a=_ksp(av),*b=_ksp(bv);return kv_bool(a->x<b->x+b->w&&a->x+a->w>b->x&&a->y<b->y+b->h&&a->y+a->h>b->y);}
static inline KVal *k_gmcol_rect_point(KVal *sv,KVal *x,KVal *y){KSprite *s=_ksp(sv);double px=_knum(x),py=_knum(y);return kv_bool(px>=s->x&&px<s->x+s->w&&py>=s->y&&py<s->y+s->h);}
static inline KVal *k_gmcol_circle(KVal *av,KVal *bv){KSprite *a=_ksp(av),*b=_ksp(bv);double dx=a->x-b->x,dy=a->y-b->y;return kv_bool(sqrt(dx*dx+dy*dy)<(a->w+b->w)/2.0);}
static inline KVal *k_gmcol_group(KVal *sv,KVal *g){KVal *h=kv_list();for(size_t i=0;i<g->list->len;i++)if(k_gmcol_rect(sv,g->list->items[i])->b)klist_push(h->list,g->list->items[i]);return h;}
static inline KVal *k_gmcol_dist(KVal *av,KVal *bv){KSprite *a=_ksp(av),*b=_ksp(bv);double dx=a->x-b->x,dy=a->y-b->y;return kv_flt(sqrt(dx*dx+dy*dy));}

static inline KVal *k_gmsound(KVal *f){(void)f;return kv_int(0);}
static inline KVal *k_gmsound_play(KVal *s){(void)s;beep();return kv_void();}
static inline KVal *k_gmsound_stop(KVal *s){(void)s;return kv_void();}
static inline KVal *k_gmsound_vol(KVal *s,KVal *v){(void)s;(void)v;return kv_void();}
static inline KVal *k_gmmusic_play(void){return kv_void();}
static inline KVal *k_gmmusic_stop(void){return kv_void();}
static inline KVal *k_gmfont(KVal *n,KVal *s){(void)n;(void)s;return kv_int(0);}
static inline KVal *k_gmfont_render(KVal *f,KVal *t,KVal *c){(void)f;(void)c;return kv_str(t->s);}
static inline KVal *k_gmtext_draw(KVal *w,KVal *t,KVal *f,KVal *c,KVal *r){(void)w;(void)f;(void)c;KVal *x=kdict_get(r->dict,"x"),*y=kdict_get(r->dict,"y");mvprintw((int)_knum(y),(int)_knum(x),"%s",t->s);return kv_void();}
static inline KVal *k_gmcam(KVal *w,KVal *h){(void)w;(void)h;return kv_int(0);}
static inline KVal *k_gmcam_follow(KVal *c,KVal *sv){(void)c;(void)sv;return kv_void();}
static inline KVal *k_gmcam_pos(KVal *c,KVal *x,KVal *y){(void)c;(void)x;(void)y;return kv_void();}
static inline KVal *k_gmcam_zoom(KVal *c,KVal *z){(void)c;(void)z;return kv_void();}
static inline KVal *k_gmstate_save(KVal *p,KVal *s){extern KVal*k_jswrite(KVal*,KVal*);return k_jswrite(p,s);}
static inline KVal *k_gmstate_load(KVal *p){extern KVal*k_jsread(KVal*);return k_jsread(p);}
static inline KVal *k_gmstate_exists(KVal *p){extern KVal*k_fexists(KVal*);return k_fexists(p);}
static inline KVal *k_gm_event_post(KVal *t,KVal *d){(void)t;(void)d;return kv_void();}

#endif /* _WIN32 */
#endif /* K_GAME_H */
