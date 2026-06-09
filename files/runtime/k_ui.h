/* k_ui.h — Katlans UI (ui prefix) — ncurses TUI */
#ifndef K_UI_H
#define K_UI_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ncurses is not available on Windows — provide stubs */
#ifdef _WIN32
/* WINDOW is defined in k_game.h, included before this */
#define initscr() NULL
#define cbreak() ((void)0)
#define noecho() ((void)0)
#define keypad(w,b) ((void)0)
#define nodelay(w,b) ((void)0)
#define curs_set(s) ((void)0)
#define has_colors() 0
#define start_color() ((void)0)
#define use_default_colors() ((void)0)
#define init_pair(n,f,b) ((void)0)
#define clear() ((void)0)
#define refresh() ((void)0)
#define endwin() ((void)0)
#define getch() (-1)
#define wgetch(w) (-1)
#define wgetnstr(w,s,n) ((*(s)=0),0)
#define wrefresh(w) ((void)0)
#define wclear(w) ((void)0)
#define newwin(h,w,y,x) NULL
#define delwin(w) ((void)0)
#define box(w,v,h) ((void)0)
#define mvwin(w,y,x) ((void)0)
#define mvaddch(y,x,c) ((void)0)
#define mvprintw(y,x,f,...) ((void)0)
#define mvwprintw(w,y,x,f,...) ((void)0)
#define mvwaddch(w,y,x,c) ((void)0)
#define waddch(w,c) ((void)0)
#define wprintw(w,f,...) ((void)0)
#define getmaxyx(w,r,c) ((r)=24,(c)=80)
#define echo() ((void)0)
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLUE 4
#define COLOR_CYAN 5
#define COLOR_BLACK 0
#else
#include <ncurses.h>
#endif

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
static inline KVal *k_uientry_clear(KVal *v){_v2w(v)->value[0]='\0';return kv_void();}
static inline KVal *k_uitext(KVal *pv,KVal *w,KVal *h){(void)pv;KWidget *t=_kwa(KW_TEXT);t->w=(int)w->i;t->h=(int)h->i;return _w2v(t);}
static inline KVal *k_uitext_insert(KVal *v,KVal *t){strncat(_v2w(v)->value,t->s,1023-strlen(_v2w(v)->value));return kv_void();}
static inline KVal *k_uitext_get(KVal *v){return kv_str(_v2w(v)->value);}
static inline KVal *k_uitext_clear(KVal *v){_v2w(v)->value[0]='\0';return kv_void();}
static inline KVal *k_uicheck(KVal *pv,KVal *l){(void)pv;KWidget *c=_kwa(KW_CHECK);strncpy(c->text,l->s,1023);return _w2v(c);}
static inline KVal *k_uicheck_get(KVal *v){return kv_bool(_v2w(v)->checked);}
static inline KVal *k_uicheck_set(KVal *v,KVal *b){_v2w(v)->checked=(int)b->b;return kv_void();}
static inline KVal *k_uicheck_toggle(KVal *v){KWidget *w=_v2w(v);w->checked=!w->checked;return kv_void();}
static inline KVal *k_uiprog(KVal *pv,KVal *mn,KVal *mx){(void)pv;KWidget *p=_kwa(KW_PROG);p->prog_val=0;p->prog_max=(int)mx->i;return _w2v(p);}
static inline KVal *k_uiprog_set(KVal *v,KVal *n){_v2w(v)->prog_val=(int)n->i;return kv_void();}
static inline KVal *k_uislider(KVal *pv,KVal *mn,KVal *mx){(void)pv;KWidget *s=_kwa(KW_SLIDER);s->slider_min=(int)mn->i;s->slider_max=(int)mx->i;return _w2v(s);}
static inline KVal *k_uislider_get(KVal *v){return kv_int(_v2w(v)->slider_val);}
static inline KVal *k_uislider_set(KVal *v,KVal *n){_v2w(v)->slider_val=(int)n->i;return kv_void();}
static inline KVal *k_uipack(KVal *v){(void)v;return kv_void();}
static inline KVal *k_uigrid(KVal *v,KVal *r,KVal *c){KWidget *w=_v2w(v);w->y=(int)r->i;w->x=(int)c->i;return kv_void();}
static inline KVal *k_uiplace(KVal *v,KVal *x,KVal *y){KWidget *w=_v2w(v);w->x=(int)x->i;w->y=(int)y->i;return kv_void();}
static inline KVal *k_uidialog_info(KVal *title,KVal *msg){_kui_init();int r,c;getmaxyx(stdscr,r,c);int w=(int)strlen(msg->s)+8,h=5;WINDOW *d=newwin(h,w,(r-h)/2,(c-w)/2);box(d,0,0);mvwprintw(d,0,2," %s ",title->s);mvwprintw(d,2,2,"%s",msg->s);mvwprintw(d,3,w/2-2,"[OK]");wrefresh(d);nodelay(stdscr,FALSE);wgetch(d);nodelay(stdscr,TRUE);delwin(d);refresh();return kv_void();}
static inline KVal *k_uidialog_ask(KVal *title,KVal *msg){_kui_init();int r,c;getmaxyx(stdscr,r,c);int w=(int)strlen(msg->s)+12,h=6;WINDOW *d=newwin(h,w,(r-h)/2,(c-w)/2);box(d,0,0);mvwprintw(d,0,2," %s ",title->s);mvwprintw(d,2,2,"%s",msg->s);mvwprintw(d,4,2,"[Y]es  [N]o");wrefresh(d);nodelay(stdscr,FALSE);int ch=wgetch(d);nodelay(stdscr,TRUE);delwin(d);refresh();return kv_bool(ch=='y'||ch=='Y');}
static inline KVal *k_uidialog_input(KVal *prompt){_kui_init();int r,c;getmaxyx(stdscr,r,c);int pw=(int)strlen(prompt->s)+20;WINDOW *d=newwin(5,pw,(r-5)/2,(c-pw)/2);box(d,0,0);mvwprintw(d,1,2,"%s",prompt->s);mvwprintw(d,2,2,"> ");wrefresh(d);char buf[256]="";nodelay(stdscr,FALSE);echo();mvwgetnstr(d,2,4,buf,254);noecho();nodelay(stdscr,TRUE);delwin(d);refresh();return kv_str(buf);}
static inline KVal *k_uiclip_set(KVal *v){strncpy(_k_clip,v->s,4095);return kv_void();}
static inline KVal *k_uiclip_get(void){return kv_str(_k_clip);}
static inline KVal *k_uivar_str(KVal *d){return kv_str(d?d->s:"");}
static inline KVal *k_uivar_int(KVal *d){return kv_int(d?d->i:0);}
static inline KVal *k_uivar_bool(KVal *d){return kv_bool(d&&d->b);}
static inline KVal *k_uivar_get(KVal *v){return v;}
static inline KVal *k_uivar_set(KVal *v,KVal *n){v->type=n->type;v->i=n->i;if(n->type==KT_STR)v->s=strdup(n->s);return kv_void();}
static inline KVal *k_uiinfo_screenwidth(KVal *w){(void)w;int r,c;getmaxyx(stdscr,r,c);(void)r;return kv_int((long long)c);}
static inline KVal *k_uiinfo_screenheight(KVal *w){(void)w;int r,c;getmaxyx(stdscr,r,c);(void)c;return kv_int((long long)r);}

#endif /* K_UI_H */
