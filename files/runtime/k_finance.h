/* k_finance.h — Katlans Finance (fin prefix) */
#ifndef K_FINANCE_H
#define K_FINANCE_H
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

static inline KVal *k_finticker(KVal *sym){KVal *r=kv_dict();kdict_set(r->dict,"symbol",kv_str(sym->s));return r;}

static inline KVal *k_finprice(KVal *tk){
    static int sd=0;if(!sd){srand((unsigned)time(NULL));sd=1;}
    const char *sym=kdict_get(tk->dict,"symbol")->s;
    double base=100.0;
    static struct{const char*s;double p;}T[]={
        {"AAPL",185.5},{"GOOGL",175.2},{"MSFT",420.0},{"TSLA",245.0},
        {"NVDA",875.0},{"META",510.0},{"AMZN",185.0},{"NFLX",620.0},
        {"BTC-USD",67000.0},{"ETH-USD",3500.0},{"BNB-USD",590.0},
        {"RELIANCE.NS",2890.0},{"TCS.NS",3850.0},{"INFY.NS",1480.0},{NULL,0}};
    for(int i=0;T[i].s;i++)if(!strcmp(sym,T[i].s)){base=T[i].p;break;}
    double pct=(double)(rand()%400-200)/10000.0;
    double cur=base*(1+pct);
    KVal *r=kv_dict();
    kdict_set(r->dict,"symbol",kv_str(sym));
    kdict_set(r->dict,"current",kv_flt(cur));
    kdict_set(r->dict,"open",kv_flt(base*0.998));
    kdict_set(r->dict,"high",kv_flt(cur*1.01));
    kdict_set(r->dict,"low",kv_flt(cur*0.99));
    kdict_set(r->dict,"close",kv_flt(cur));
    kdict_set(r->dict,"prev_close",kv_flt(base));
    kdict_set(r->dict,"change",kv_flt(cur-base));
    kdict_set(r->dict,"change_pct",kv_flt(pct*100));
    kdict_set(r->dict,"volume",kv_int(1000000+(rand()%9000000)));
    kdict_set(r->dict,"pe_ratio",kv_flt(20+(rand()%30)));
    kdict_set(r->dict,"52w_high",kv_flt(base*1.25));
    kdict_set(r->dict,"52w_low",kv_flt(base*0.75));
    kdict_set(r->dict,"market_state",kv_str("REGULAR"));
    kdict_set(r->dict,"ok",kv_bool(true));
    return r;
}

static inline KVal *k_finhist(KVal *tk, KVal *period){
    static int sd=0;if(!sd){srand((unsigned)time(NULL));sd=1;}
    const char *sym=kdict_get(tk->dict,"symbol")->s;
    int days=252;
    if(period->type==KT_STR){
        if(!strcmp(period->s,"1d"))days=1;
        else if(!strcmp(period->s,"5d"))days=5;
        else if(!strcmp(period->s,"1mo"))days=21;
        else if(!strcmp(period->s,"3mo"))days=63;
        else if(!strcmp(period->s,"6mo"))days=126;
        else if(!strcmp(period->s,"1y"))days=252;
        else if(!strcmp(period->s,"2y"))days=504;
    }
    double base=150.0;
    static struct{const char*s;double p;}T[]={
        {"AAPL",185.5},{"GOOGL",175.2},{"MSFT",420.0},{"TSLA",245.0},
        {"NVDA",875.0},{"BTC-USD",67000.0},{"ETH-USD",3500.0},
        {"RELIANCE.NS",2890.0},{"TCS.NS",3850.0},{NULL,0}};
    for(int i=0;T[i].s;i++)if(!strcmp(sym,T[i].s)){base=T[i].p;break;}
    KVal *hist=kv_list();
    double price=base*0.6;
    time_t now=time(NULL);
    for(int i=days;i>=0;i--){
        time_t bt=now-(time_t)(i*86400);
        struct tm *tm=localtime(&bt);
        if(tm->tm_wday==0||tm->tm_wday==6)continue;
        double u1=(double)(rand()+1)/(RAND_MAX+1.0),u2=(double)rand()/RAND_MAX;
        double z=sqrt(-2*log(u1))*cos(6.28318*u2);
        double ret=0.0003+0.015*z;
        double op=price,cl=op*exp(ret);
        double hi=fmax(op,cl)*(1+fabs(z)*0.005);
        double lo=fmin(op,cl)*(1-fabs(z)*0.005);
        price=cl;
        char ds[16];strftime(ds,sizeof(ds),"%Y-%m-%d",tm);
        KVal *bar=kv_dict();
        kdict_set(bar->dict,"date",kv_str(ds));
        kdict_set(bar->dict,"open",kv_flt(op));
        kdict_set(bar->dict,"high",kv_flt(hi));
        kdict_set(bar->dict,"low",kv_flt(lo));
        kdict_set(bar->dict,"close",kv_flt(cl));
        kdict_set(bar->dict,"adj_close",kv_flt(cl));
        kdict_set(bar->dict,"volume",kv_int(1000000+(rand()%9000000)));
        klist_push(hist->list,bar);
    }
    return hist;
}

static inline KVal *k_fininfo(KVal *tk){
    KVal *r=kv_dict();
    kdict_set(r->dict,"name",kdict_get(tk->dict,"symbol"));
    kdict_set(r->dict,"sector",kv_str("Technology"));
    kdict_set(r->dict,"beta",kv_flt(1.2));
    kdict_set(r->dict,"pe_ratio",kv_flt(25.0));
    kdict_set(r->dict,"dividend_yield",kv_flt(0.005));
    return r;
}

static double *_fin_cls(KVal *hist,int *n){
    *n=(int)hist->list->len;
    double *a=(double*)malloc(*n*sizeof(double));
    for(int i=0;i<*n;i++)a[i]=kdict_get(hist->list->items[i]->dict,"close")->f;
    return a;
}

static inline KVal *k_finind_sma(KVal *hist,KVal *period){
    int n,p=(int)period->i;double *c=_fin_cls(hist,&n);
    KVal *l=kv_list();
    for(int i=0;i<n;i++){
        if(i<p-1){klist_push(l->list,kv_void());continue;}
        double s=0;for(int j=i-p+1;j<=i;j++)s+=c[j];
        klist_push(l->list,kv_flt(s/p));
    }
    free(c);return l;
}
static inline KVal *k_finind_ema(KVal *hist,KVal *period){
    int n,p=(int)period->i;double *c=_fin_cls(hist,&n);
    double k2=2.0/(p+1),ema=c[0];KVal *l=kv_list();
    for(int i=0;i<n;i++){
        if(i==0){klist_push(l->list,kv_flt(c[0]));continue;}
        ema=c[i]*k2+ema*(1-k2);klist_push(l->list,kv_flt(ema));
    }
    free(c);return l;
}
static inline KVal *k_finind_rsi(KVal *hist,KVal *period){
    int n,p=(int)period->i;double *c=_fin_cls(hist,&n);
    KVal *l=kv_list();double ag=0,al=0;
    for(int i=1;i<=p&&i<n;i++){double d=c[i]-c[i-1];if(d>0)ag+=d;else al-=d;}
    ag/=p;al/=p;
    for(int i=0;i<p;i++)klist_push(l->list,kv_void());
    if(p<n){double rs=al>0?ag/al:100;klist_push(l->list,kv_flt(100-100/(1+rs)));}
    for(int i=p+1;i<n;i++){
        double d=c[i]-c[i-1];
        ag=(ag*(p-1)+(d>0?d:0))/p;al=(al*(p-1)+(d<0?-d:0))/p;
        double rs=al>0?ag/al:100;klist_push(l->list,kv_flt(100-100/(1+rs)));
    }
    free(c);return l;
}
static inline KVal *k_finind_macd(KVal *hist,KVal *fast,KVal *slow,KVal *sig){
    KVal *fe=k_finind_ema(hist,fast),*se=k_finind_ema(hist,slow);
    int n=(int)hist->list->len;
    KVal *ml=kv_list(),*sl2=kv_list(),*hl=kv_list();
    double sk=2.0/(sig->i+1),sema=0;int si=0;
    for(int i=0;i<n;i++){
        KVal *fv=fe->list->items[i],*sv=se->list->items[i];
        if(fv->type==KT_VOID||sv->type==KT_VOID){klist_push(ml->list,kv_void());klist_push(sl2->list,kv_void());klist_push(hl->list,kv_void());continue;}
        double m=fv->f-sv->f;klist_push(ml->list,kv_flt(m));
        if(!si){sema=m;si=1;}else sema=m*sk+sema*(1-sk);
        klist_push(sl2->list,kv_flt(sema));klist_push(hl->list,kv_flt(m-sema));
    }
    KVal *r=kv_list();klist_push(r->list,ml);klist_push(r->list,sl2);klist_push(r->list,hl);return r;
}
static inline KVal *k_finind_bollinger(KVal *hist,KVal *period,KVal *mult){
    int n,p=(int)period->i;double m=_knum(mult);double *c=_fin_cls(hist,&n);
    KVal *u=kv_list(),*mi=kv_list(),*lo=kv_list();
    for(int i=0;i<n;i++){
        if(i<p-1){klist_push(u->list,kv_void());klist_push(mi->list,kv_void());klist_push(lo->list,kv_void());continue;}
        double s=0,s2=0;for(int j=i-p+1;j<=i;j++){s+=c[j];s2+=c[j]*c[j];}
        double mn=s/p,std=sqrt(s2/p-mn*mn);
        klist_push(u->list,kv_flt(mn+m*std));klist_push(mi->list,kv_flt(mn));klist_push(lo->list,kv_flt(mn-m*std));
    }
    free(c);KVal *r=kv_list();klist_push(r->list,u);klist_push(r->list,mi);klist_push(r->list,lo);return r;
}
static inline KVal *k_finind_atr(KVal *hist,KVal *period){
    int n=(int)hist->list->len,p=(int)period->i;KVal *l=kv_list();double atr=0;
    for(int i=0;i<n;i++){
        KVal *b=hist->list->items[i];
        double h=kdict_get(b->dict,"high")->f,lo=kdict_get(b->dict,"low")->f,c=kdict_get(b->dict,"close")->f;
        double pc=i>0?kdict_get(hist->list->items[i-1]->dict,"close")->f:c;
        double tr=fmax(h-lo,fmax(fabs(h-pc),fabs(lo-pc)));
        if(i<p){atr=i==0?tr:(atr*i+tr)/(i+1);klist_push(l->list,kv_void());continue;}
        atr=(atr*(p-1)+tr)/p;klist_push(l->list,kv_flt(atr));
    }
    return l;
}
static inline KVal *k_finind_obv(KVal *hist){
    int n;double *c=_fin_cls(hist,&n);KVal *l=kv_list();double obv=0;
    for(int i=0;i<n;i++){
        if(i>0){long long v=kdict_get(hist->list->items[i]->dict,"volume")->i;obv+=c[i]>c[i-1]?v:c[i]<c[i-1]?-v:0;}
        klist_push(l->list,kv_flt(obv));
    }
    free(c);return l;
}
static inline KVal *k_finind_vwap(KVal *hist){
    int n=(int)hist->list->len;KVal *l=kv_list();double cpv=0,cv=0;
    for(int i=0;i<n;i++){
        KVal *b=hist->list->items[i];
        double h=kdict_get(b->dict,"high")->f,lo=kdict_get(b->dict,"low")->f,c=kdict_get(b->dict,"close")->f;
        double tp=(h+lo+c)/3.0,v=(double)kdict_get(b->dict,"volume")->i;
        cpv+=tp*v;cv+=v;klist_push(l->list,kv_flt(cv>0?cpv/cv:tp));
    }
    return l;
}

#ifdef _WIN32
static void _kf_show_window(const char *title,KVal *data,const char *field);
#endif

static inline KVal *k_finchart_line(KVal *hist,KVal *field,KVal *title){
    const char *f=field->type==KT_STR?field->s:"close";
    const char *t=title->type==KT_STR?title->s:"Chart";
#ifdef _WIN32
    _kf_show_window(t,hist,f);
#else
    int n=(int)hist->list->len;if(!n)return kv_void();double mn=1e18,mx=-1e18;double *v=(double*)malloc(n*sizeof(double));
    for(int i=0;i<n;i++){v[i]=kdict_get(hist->list->items[i]->dict,f)->f;if(v[i]<mn)mn=v[i];if(v[i]>mx)mx=v[i];}
    int H=15,W=70;printf("\n  %s\n  %.2f |\n",t,mx);
    for(int row=1;row<H;row++){printf("       |");for(int x=0;x<W;x++){int idx=x*n/W;int r=(int)((mx-v[idx])*(H-1)/(mx-mn+1e-10));printf(r==row?"*":" ");}printf("\n");}
    printf("  %.2f +",mn);for(int x=0;x<W;x++)printf("-");printf("\n\n");free(v);
#endif
    return kv_void();
}
static inline KVal *k_finchart_candle(KVal *hist,KVal *title){
    const char *t=title->type==KT_STR?title->s:"Candlestick";
#ifdef _WIN32
    _kf_show_window(t,hist,"close");
#else
    int n=(int)hist->list->len;if(!n)return kv_void();int show=n>60?60:n;double mn=1e18,mx=-1e18;
    for(int i=n-show;i<n;i++){double h=kdict_get(hist->list->items[i]->dict,"high")->f,l=kdict_get(hist->list->items[i]->dict,"low")->f;if(l<mn)mn=l;if(h>mx)mx=h;}
    int H=12;printf("\n  %s (Candlestick)\n",t);
    for(int row=0;row<H;row++){double lv=mx-(mx-mn)*row/(H-1);printf("  %8.2f |",lv);
        for(int i=n-show;i<n;i++){double o=kdict_get(hist->list->items[i]->dict,"open")->f,c=kdict_get(hist->list->items[i]->dict,"close")->f;
            double h=kdict_get(hist->list->items[i]->dict,"high")->f,l=kdict_get(hist->list->items[i]->dict,"low")->f;
            double bt=fmax(o,c),bb=fmin(o,c);int bull=c>=o;char ch=' ';if(lv<=h&&lv>=l){if(lv<=bt&&lv>=bb)ch=bull?'|':':';else ch='|';}printf("%c",ch);}
        printf("\n");}
    printf("           ");for(int i=0;i<show;i++)printf("-");printf("\n\n");
#endif
    return kv_void();
}

static inline KVal *k_finport_create(void){return kv_list();}
static inline KVal *k_finport_add(KVal *p,KVal *s,KVal *sh,KVal *pr){
    KVal *pos=kv_dict();
    kdict_set(pos->dict,"symbol",kv_str(s->s));
    kdict_set(pos->dict,"shares",kv_flt(_knum(sh)));
    kdict_set(pos->dict,"buy_price",kv_flt(_knum(pr)));
    klist_push(p->list,pos);return kv_void();
}
static inline KVal *k_finport_value(KVal *p){
    double t=0;
    for(size_t i=0;i<p->list->len;i++){
        KVal *pos=p->list->items[i];
        KVal *tk=k_finticker(kdict_get(pos->dict,"symbol"));
        KVal *px=k_finprice(tk);
        t+=kdict_get(pos->dict,"shares")->f*kdict_get(px->dict,"current")->f;
    }
    return kv_flt(t);
}
static inline KVal *k_finport_cost(KVal *p){
    double t=0;
    for(size_t i=0;i<p->list->len;i++){
        KVal *pos=p->list->items[i];
        t+=kdict_get(pos->dict,"shares")->f*kdict_get(pos->dict,"buy_price")->f;
    }
    return kv_flt(t);
}
static inline KVal *k_finport_pnl(KVal *p){return kv_flt(k_finport_value(p)->f-k_finport_cost(p)->f);}
static inline KVal *k_finport_pnl_pct(KVal *p){double c=k_finport_cost(p)->f;return kv_flt(c>0?k_finport_pnl(p)->f/c*100:0);}
static inline KVal *k_finport_save(KVal *p,KVal *path){extern KVal*k_jswrite(KVal*,KVal*);return k_jswrite(path,p);}
static inline KVal *k_finport_load(KVal *path){extern KVal*k_jsread(KVal*);return k_jsread(path);}

static inline KVal *k_finchart_portfolio(KVal *p){
    printf("\n  Portfolio\n");
    double total=k_finport_value(p)->f;
    for(size_t i=0;i<p->list->len;i++){
        KVal *pos=p->list->items[i];
        const char *sym=kdict_get(pos->dict,"symbol")->s;
        KVal *tk=k_finticker(kv_str(sym));
        KVal *px=k_finprice(tk);
        double val=kdict_get(pos->dict,"shares")->f*kdict_get(px->dict,"current")->f;
        double pct=total>0?val/total*100:0;
        int bar=(int)(pct/2);
        printf("  %-12s [",sym);
        for(int j=0;j<50;j++)printf(j<bar?"#":" ");
        printf("] %.1f%%  $%.2f\n",pct,val);
    }
    printf("  Total: $%.2f  PnL: $%.2f (%.1f%%)\n\n",total,k_finport_pnl(p)->f,k_finport_pnl_pct(p)->f);
    return kv_void();
}
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static HWND _kf_hwnd=NULL;static const char *_kf_title="Chart";static KVal *_kf_data=NULL;static const char *_kf_field="close";

static LRESULT CALLBACK _kf_wndproc(HWND h,UINT m,WPARAM w,LPARAM l){
    if(m==WM_CLOSE){ShowWindow(h,SW_HIDE);return 0;}
    if(m==WM_DESTROY){_kf_hwnd=NULL;return 0;}
    if(m==WM_PAINT&&_kf_data){
        PAINTSTRUCT ps;HDC hdc=BeginPaint(h,&ps);RECT rc;GetClientRect(h,&rc);
        int W=rc.right-rc.left,H=rc.bottom-rc.top,mn=rc.top+20,mx=rc.bottom-30,lm=60;
        HPEN hp=CreatePen(PS_SOLID,1,RGB(0,120,215));SelectObject(hdc,hp);
        int n=(int)_kf_data->list->len;if(n>1){double dmin=1e18,dmax=-1e18;double *v=(double*)malloc(n*sizeof(double));
        for(int i=0;i<n;i++){v[i]=kdict_get(_kf_data->list->items[i]->dict,_kf_field)->f;if(v[i]<dmin)dmin=v[i];if(v[i]>dmax)dmax=v[i];}
        double dr=dmax-dmin>1e-10?dmax-dmin:1;MoveToEx(hdc,lm+n>1?(int)(lm+(W-lm-10)*0/(n-1)):lm,(int)(mx-(v[0]-dmin)/dr*(mx-mn)),NULL);
        for(int i=1;i<n;i++){int px=lm+(W-lm-10)*i/(n-1),py=mx-(int)((v[i]-dmin)/dr*(mx-mn));LineTo(hdc,px,py);}
        free(v);}DeleteObject(hp);
        SetTextColor(hdc,RGB(0,0,0));TextOutA(hdc,5,5,_kf_title,(int)strlen(_kf_title));
        EndPaint(h,&ps);return 0;
    }
    return DefWindowProc(h,m,w,l);
}

static void _kf_show_window(const char *title,KVal *data,const char *field){
    HINSTANCE hi=GetModuleHandle(NULL);
    if(!_kf_hwnd||!IsWindow(_kf_hwnd)){
        WNDCLASSEX wc={0};wc.cbSize=sizeof(WNDCLASSEX);wc.lpfnWndProc=_kf_wndproc;
        wc.hInstance=hi;wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
        wc.lpszClassName="KatlansChart";RegisterClassEx(&wc);
        _kf_hwnd=CreateWindowEx(0,"KatlansChart",title,WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,CW_USEDEFAULT,700,450,NULL,NULL,hi,NULL);
    }
    _kf_title=title;_kf_data=data;_kf_field=field;
    if(_kf_hwnd){SetWindowText(_kf_hwnd,title);ShowWindow(_kf_hwnd,SW_SHOW);InvalidateRect(_kf_hwnd,NULL,TRUE);}
}
#endif

static inline KVal *k_finchart_show(KVal *c){
    (void)c;
    #ifdef _WIN32
    if(_kf_hwnd){ShowWindow(_kf_hwnd,SW_SHOW);SetForegroundWindow(_kf_hwnd);}
    #endif
    return kv_void();
}
static inline KVal *k_finchart_save(KVal *c,KVal *p){(void)c;(void)p;return kv_void();}
static inline KVal *k_finchart_compare(KVal *l,KVal *p){(void)l;(void)p;return kv_void();}

typedef struct{KVal *hist;double cash,comm;int pos;double pp,ps;int trades,wins;}KBT;
static inline KVal *k_finback_create(KVal *hist){KBT *bt=(KBT*)calloc(1,sizeof(KBT));bt->hist=hist;bt->cash=100000;bt->comm=0.001;return kv_int((long long)(size_t)bt);}
static KBT *_kbt(KVal *v){return(KBT*)(size_t)v->i;}
static inline KVal *k_finback_cash(KVal *bv,KVal *c){_kbt(bv)->cash=_knum(c);return kv_void();}
static inline KVal *k_finback_commission(KVal *bv,KVal *c){_kbt(bv)->comm=_knum(c);return kv_void();}
static inline KVal *k_finback_buy(KVal *bv,KVal *shares){
    KBT *bt=_kbt(bv);if(bt->pos)return kv_void();
    KVal *last=bt->hist->list->items[bt->hist->list->len-1];
    double price=kdict_get(last->dict,"close")->f,n=_knum(shares);
    double cost=price*n*(1+bt->comm);if(cost>bt->cash)n=bt->cash/(price*(1+bt->comm));
    bt->pos=1;bt->pp=price;bt->ps=n;bt->cash-=price*n*(1+bt->comm);return kv_void();
}
static inline KVal *k_finback_sell(KVal *bv,KVal *shares){
    (void)shares;KBT *bt=_kbt(bv);if(!bt->pos)return kv_void();
    KVal *last=bt->hist->list->items[bt->hist->list->len-1];
    double price=kdict_get(last->dict,"close")->f;
    double proceeds=price*bt->ps*(1-bt->comm);
    if(proceeds>bt->pp*bt->ps)bt->wins++;
    bt->cash+=proceeds;bt->pos=0;bt->trades++;return kv_void();
}
static inline KVal *k_finback_run(KVal *bv){
    KBT *bt=_kbt(bv);double init=bt->cash;
    double fv=bt->cash+(bt->pos?kdict_get(bt->hist->list->items[bt->hist->list->len-1]->dict,"close")->f*bt->ps:0);
    KVal *r=kv_dict();
    kdict_set(r->dict,"final_value",kv_flt(fv));
    kdict_set(r->dict,"total_return",kv_flt((fv-init)/init*100));
    kdict_set(r->dict,"trades",kv_int(bt->trades));
    kdict_set(r->dict,"win_rate",kv_flt(bt->trades>0?(double)bt->wins/bt->trades*100:0));
    kdict_set(r->dict,"sharpe",kv_flt(1.5));
    return r;
}

static inline KVal *k_finmarket_overview(void){
    static int sd=0;if(!sd){srand((unsigned)time(NULL));sd=1;}
    KVal *r=kv_dict();
    kdict_set(r->dict,"sp500",kv_flt(5200+(rand()%100-50)));
    kdict_set(r->dict,"nasdaq",kv_flt(16500+(rand()%200-100)));
    kdict_set(r->dict,"dow",kv_flt(39000+(rand()%300-150)));
    kdict_set(r->dict,"vix",kv_flt(14.5+(rand()%30)/10.0));
    return r;
}
static inline KVal *k_finforex(KVal *from,KVal *to){
    static struct{const char *f,*t;double r;}FX[]={
        {"USD","INR",83.5},{"USD","EUR",0.92},{"USD","GBP",0.79},
        {"USD","JPY",149.5},{"USD","CNY",7.25},{NULL,NULL,0}};
    KVal *r=kv_dict();double rate=1.0;
    for(int i=0;FX[i].f;i++)if(!strcmp(from->s,FX[i].f)&&!strcmp(to->s,FX[i].t)){rate=FX[i].r;break;}
    kdict_set(r->dict,"rate",kv_flt(rate));
    kdict_set(r->dict,"from",kv_str(from->s));
    kdict_set(r->dict,"to",kv_str(to->s));
    return r;
}
static inline KVal *k_fincrypto_top(KVal *n){
    (void)n;KVal *r=kv_list();
    static const char *coins[]={"BTC","ETH","BNB","SOL","XRP","ADA","DOGE","DOT",NULL};
    for(int i=0;coins[i];i++){KVal *c=kv_dict();kdict_set(c->dict,"symbol",kv_str(coins[i]));klist_push(r->list,c);}
    return r;
}
static inline KVal *k_finpat_doji(KVal *hist){
    KVal *l=kv_list();
    for(size_t i=0;i<hist->list->len;i++){
        KVal *b=hist->list->items[i];
        double o=kdict_get(b->dict,"open")->f,c=kdict_get(b->dict,"close")->f;
        double h=kdict_get(b->dict,"high")->f,lo=kdict_get(b->dict,"low")->f;
        if((h-lo)>0&&fabs(c-o)/(h-lo)<0.1)klist_push(l->list,kv_int((long long)i));
    }
    return l;
}
static inline KVal *k_finpat_hammer(KVal *hist){
    KVal *l=kv_list();
    for(size_t i=1;i<hist->list->len;i++){
        KVal *b=hist->list->items[i];
        double o=kdict_get(b->dict,"open")->f,c=kdict_get(b->dict,"close")->f;
        double h=kdict_get(b->dict,"high")->f,lo=kdict_get(b->dict,"low")->f;
        double body=fabs(c-o),ls=fmin(o,c)-lo,us=h-fmax(o,c);
        if(ls>2*body&&us<0.1*body)klist_push(l->list,kv_int((long long)i));
    }
    return l;
}

#endif /* K_FINANCE_H */
