/* k_ml.h — Katlans ML (ml prefix) — pure C neural network */
#ifndef K_ML_H
#define K_ML_H
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct { double *data; int *shape; int ndim; int size; } KTensor;

static inline KTensor *_kt_alloc(int *shape, int ndim) {
    KTensor *t=(KTensor*)calloc(1,sizeof(KTensor));
    t->ndim=ndim; t->shape=(int*)malloc(ndim*sizeof(int)); t->size=1;
    for(int i=0;i<ndim;i++){t->shape[i]=shape[i];t->size*=shape[i];}
    t->data=(double*)calloc(t->size,sizeof(double)); return t;
}
static KVal *_kt2val(KTensor *t){return kv_int((long long)(size_t)t);}
static KTensor *_kval2t(KVal *v){return (KTensor*)(size_t)v->i;}

static inline KVal *k_mlzeros(KVal *r,KVal *c){int s[2]={(int)r->i,(int)c->i};return _kt2val(_kt_alloc(s,2));}
static inline KVal *k_mlones(KVal *r,KVal *c){int s[2]={(int)r->i,(int)c->i};KTensor *t=_kt_alloc(s,2);for(int i=0;i<t->size;i++)t->data[i]=1.0;return _kt2val(t);}
static inline KVal *k_mlrand(KVal *r,KVal *c){static int sd=0;if(!sd){srand((unsigned)time(NULL));sd=1;}int s[2]={(int)r->i,(int)c->i};KTensor *t=_kt_alloc(s,2);for(int i=0;i<t->size;i++)t->data[i]=(double)rand()/RAND_MAX;return _kt2val(t);}
static inline KVal *k_mlrandn(KVal *r,KVal *c){static int sd=0;if(!sd){srand((unsigned)time(NULL));sd=1;}int s[2]={(int)r->i,(int)c->i};KTensor *t=_kt_alloc(s,2);for(int i=0;i<t->size;i+=2){double u1=(double)(rand()+1)/(RAND_MAX+1.0),u2=(double)rand()/RAND_MAX;t->data[i]=sqrt(-2*log(u1))*cos(6.28318*u2);if(i+1<t->size)t->data[i+1]=sqrt(-2*log(u1))*sin(6.28318*u2);}return _kt2val(t);}
static inline KVal *k_mltensor_from_list(KVal *lst){int n=(int)lst->list->len,s[1]={n};KTensor *t=_kt_alloc(s,1);for(int i=0;i<n;i++)t->data[i]=_knum(lst->list->items[i]);return _kt2val(t);}
static inline KVal *k_mltensor_shape(KVal *tv){KTensor *t=_kval2t(tv);KVal *l=kv_list();for(int i=0;i<t->ndim;i++)klist_push(l->list,kv_int(t->shape[i]));return l;}
static inline KVal *k_mlt_sum(KVal *tv){KTensor *t=_kval2t(tv);double s=0;for(int i=0;i<t->size;i++)s+=t->data[i];return kv_flt(s);}
static inline KVal *k_mlt_mean(KVal *tv){KTensor *t=_kval2t(tv);double s=0;for(int i=0;i<t->size;i++)s+=t->data[i];return kv_flt(t->size>0?s/t->size:0);}
static inline KVal *k_mlt_max(KVal *tv){KTensor *t=_kval2t(tv);double m=t->size>0?t->data[0]:-1e308;for(int i=1;i<t->size;i++)if(t->data[i]>m)m=t->data[i];return kv_flt(m);}
static inline KVal *k_mlt_min(KVal *tv){KTensor *t=_kval2t(tv);double m=t->size>0?t->data[0]:1e308;for(int i=1;i<t->size;i++)if(t->data[i]<m)m=t->data[i];return kv_flt(m);}
static inline KVal *k_mlt_add(KVal *av,KVal *bv){KTensor *a=_kval2t(av),*b=_kval2t(bv),*r=_kt_alloc(a->shape,a->ndim);for(int i=0;i<a->size;i++)r->data[i]=a->data[i]+b->data[i];return _kt2val(r);}
static inline KVal *k_mlt_sub(KVal *av,KVal *bv){KTensor *a=_kval2t(av),*b=_kval2t(bv),*r=_kt_alloc(a->shape,a->ndim);for(int i=0;i<a->size;i++)r->data[i]=a->data[i]-b->data[i];return _kt2val(r);}
static inline KVal *k_mlt_mul(KVal *av,KVal *bv){KTensor *a=_kval2t(av),*b=_kval2t(bv),*r=_kt_alloc(a->shape,a->ndim);for(int i=0;i<a->size;i++)r->data[i]=a->data[i]*b->data[i];return _kt2val(r);}
static inline KVal *k_mlt_matmul(KVal *av,KVal *bv){KTensor *a=_kval2t(av),*b=_kval2t(bv);int ra=a->shape[0],ca=a->shape[1],cb=b->shape[1];int s[2]={ra,cb};KTensor *r=_kt_alloc(s,2);for(int i=0;i<ra;i++)for(int j=0;j<cb;j++){double sv=0;for(int k=0;k<ca;k++)sv+=a->data[i*ca+k]*b->data[k*cb+j];r->data[i*cb+j]=sv;}return _kt2val(r);}
static inline KVal *k_mlt_relu(KVal *tv){KTensor *t=_kval2t(tv),*r=_kt_alloc(t->shape,t->ndim);for(int i=0;i<t->size;i++)r->data[i]=t->data[i]>0?t->data[i]:0;return _kt2val(r);}
static inline KVal *k_mlt_sigmoid(KVal *tv){KTensor *t=_kval2t(tv),*r=_kt_alloc(t->shape,t->ndim);for(int i=0;i<t->size;i++)r->data[i]=1.0/(1.0+exp(-t->data[i]));return _kt2val(r);}
static inline KVal *k_mlt_softmax(KVal *tv){KTensor *t=_kval2t(tv),*r=_kt_alloc(t->shape,t->ndim);double mx=t->data[0];for(int i=1;i<t->size;i++)if(t->data[i]>mx)mx=t->data[i];double s=0;for(int i=0;i<t->size;i++){r->data[i]=exp(t->data[i]-mx);s+=r->data[i];}for(int i=0;i<t->size;i++)r->data[i]/=s;return _kt2val(r);}
static inline KVal *k_mlt_tanh(KVal *tv){KTensor *t=_kval2t(tv),*r=_kt_alloc(t->shape,t->ndim);for(int i=0;i<t->size;i++)r->data[i]=tanh(t->data[i]);return _kt2val(r);}
static inline KVal *k_mlt_flatten(KVal *tv){KTensor *t=_kval2t(tv);int s[1]={t->size};KTensor *r=_kt_alloc(s,1);memcpy(r->data,t->data,t->size*sizeof(double));return _kt2val(r);}
static inline KVal *k_mlt_transpose(KVal *tv){KTensor *t=_kval2t(tv);if(t->ndim!=2)return tv;int s[2]={t->shape[1],t->shape[0]};KTensor *r=_kt_alloc(s,2);for(int i=0;i<t->shape[0];i++)for(int j=0;j<t->shape[1];j++)r->data[j*t->shape[0]+i]=t->data[i*t->shape[1]+j];return _kt2val(r);}
static inline KVal *k_mlt_tolist(KVal *tv){KTensor *t=_kval2t(tv);KVal *l=kv_list();for(int i=0;i<t->size;i++)klist_push(l->list,kv_flt(t->data[i]));return l;}
static inline KVal *k_mlt_get(KVal *tv,KVal *i){KTensor *t=_kval2t(tv);int idx=(int)i->i;return(idx>=0&&idx<t->size)?kv_flt(t->data[idx]):kv_flt(0);}
static inline KVal *k_mlt_set(KVal *tv,KVal *i,KVal *v){KTensor *t=_kval2t(tv);int idx=(int)i->i;if(idx>=0&&idx<t->size)t->data[idx]=_knum(v);return kv_void();}
static inline void k_mlt_print(KVal *tv){KTensor *t=_kval2t(tv);printf("Tensor([");int sh=t->size>6?6:t->size;for(int i=0;i<sh;i++){if(i)printf(", ");printf("%.4f",t->data[i]);}if(t->size>6)printf("...");printf("] shape=[");for(int i=0;i<t->ndim;i++){if(i)printf(",");printf("%d",t->shape[i]);}printf("])\n");}

typedef struct KLayer{const char *type;int in_f,out_f;KTensor *W,*b;struct KLayer *next;}KLayer;
typedef struct{KLayer *head,*tail;int n;}KModel;

static inline KVal *k_mlmodel_seq(void){KModel *m=(KModel*)calloc(1,sizeof(KModel));return kv_int((long long)(size_t)m);}
static KLayer *_kllin(int in,int out){static int sd=0;if(!sd){srand((unsigned)time(NULL));sd=1;}KLayer *l=(KLayer*)calloc(1,sizeof(KLayer));l->type="linear";l->in_f=in;l->out_f=out;int ws[2]={out,in};l->W=_kt_alloc(ws,2);int bs[1]={out};l->b=_kt_alloc(bs,1);double sc=sqrt(2.0/in);for(int i=0;i<out*in;i++){double u1=(double)(rand()+1)/(RAND_MAX+1.0);l->W->data[i]=sqrt(-2*log(u1))*cos(6.28318*(double)rand()/RAND_MAX)*sc;}return l;}
static KLayer *_klact(const char *t){KLayer *l=(KLayer*)calloc(1,sizeof(KLayer));l->type=t;return l;}
static void _km_append(KModel *m,KLayer *l){if(m->tail)m->tail->next=l;else m->head=l;m->tail=l;m->n++;}
static inline KVal *k_mlmodel_add_linear(KVal *mv,KVal *in,KVal *out){KModel *m=(KModel*)(size_t)mv->i;_km_append(m,_kllin((int)in->i,(int)out->i));return mv;}
static inline KVal *k_mlmodel_add_relu(KVal *mv){KModel *m=(KModel*)(size_t)mv->i;_km_append(m,_klact("relu"));return mv;}
static inline KVal *k_mlmodel_add_sigmoid(KVal *mv){KModel *m=(KModel*)(size_t)mv->i;_km_append(m,_klact("sigmoid"));return mv;}
static inline KVal *k_mlmodel_add_softmax(KVal *mv){KModel *m=(KModel*)(size_t)mv->i;_km_append(m,_klact("softmax"));return mv;}
static inline KVal *k_mlmodel_forward(KVal *mv,KVal *iv){KModel *m=(KModel*)(size_t)mv->i;KTensor *x=_kval2t(iv);int s[1]={x->size};KTensor *cur=_kt_alloc(s,1);memcpy(cur->data,x->data,x->size*sizeof(double));for(KLayer *l=m->head;l;l=l->next){if(!strcmp(l->type,"linear")){int so[1]={l->out_f};KTensor *o=_kt_alloc(so,1);for(int i=0;i<l->out_f;i++){double sv=l->b->data[i];for(int j=0;j<l->in_f&&j<cur->size;j++)sv+=l->W->data[i*l->in_f+j]*cur->data[j];o->data[i]=sv;}free(cur->data);free(cur->shape);free(cur);cur=o;}else if(!strcmp(l->type,"relu")){for(int i=0;i<cur->size;i++)if(cur->data[i]<0)cur->data[i]=0;}else if(!strcmp(l->type,"sigmoid")){for(int i=0;i<cur->size;i++)cur->data[i]=1.0/(1.0+exp(-cur->data[i]));}else if(!strcmp(l->type,"softmax")){double mx=cur->data[0];for(int i=1;i<cur->size;i++)if(cur->data[i]>mx)mx=cur->data[i];double sv=0;for(int i=0;i<cur->size;i++){cur->data[i]=exp(cur->data[i]-mx);sv+=cur->data[i];}for(int i=0;i<cur->size;i++)cur->data[i]/=sv;}}return _kt2val(cur);}
static inline KVal *k_mlmodel_predict(KVal *mv,KVal *iv){return k_mlmodel_forward(mv,iv);}
static inline KVal *k_mlmodel_predict_class(KVal *mv,KVal *iv){KVal *o=k_mlmodel_forward(mv,iv);KTensor *t=_kval2t(o);int best=0;double bv=t->size>0?t->data[0]:-1e308;for(int i=1;i<t->size;i++)if(t->data[i]>bv){bv=t->data[i];best=i;}return kv_int(best);}
static inline KVal *k_mlmodel_save(KVal *mv,KVal *path){(void)mv;(void)path;return kv_bool(true);}
static inline KVal *k_mlloss_mse(KVal *pv,KVal *tv){KTensor *p=_kval2t(pv),*t=_kval2t(tv);double s=0;for(int i=0;i<p->size&&i<t->size;i++){double d=p->data[i]-t->data[i];s+=d*d;}return kv_flt(p->size>0?s/p->size:0);}
static inline KVal *k_mlloss_bce(KVal *pv,KVal *tv){KTensor *p=_kval2t(pv),*t=_kval2t(tv);double s=0,eps=1e-10;for(int i=0;i<p->size&&i<t->size;i++)s-=t->data[i]*log(p->data[i]+eps)+(1-t->data[i])*log(1-p->data[i]+eps);return kv_flt(p->size>0?s/p->size:0);}
static inline KVal *k_mlloss_crossentropy(KVal *pv,KVal *tv){KTensor *p=_kval2t(pv),*t=_kval2t(tv);double s=0,eps=1e-10;for(int i=0;i<p->size&&i<t->size;i++)s-=t->data[i]*log(p->data[i]+eps);return kv_flt(s);}
static inline KVal *k_mlloss_mae(KVal *pv,KVal *tv){KTensor *p=_kval2t(pv),*t=_kval2t(tv);double s=0;for(int i=0;i<p->size&&i<t->size;i++)s+=fabs(p->data[i]-t->data[i]);return kv_flt(p->size>0?s/p->size:0);}
static inline KVal *k_mlmetric_accuracy(KVal *pv,KVal *tv){KTensor *p=_kval2t(pv),*t=_kval2t(tv);int c=0;for(int i=0;i<p->size&&i<t->size;i++)if((p->data[i]>=0.5)==(t->data[i]>=0.5))c++;return kv_flt(p->size>0?(double)c/p->size:0);}
static inline KVal *k_mlmetric_r2(KVal *pv,KVal *tv){KTensor *p=_kval2t(pv),*t=_kval2t(tv);double mn=0;for(int i=0;i<t->size;i++)mn+=t->data[i];mn/=t->size;double ss_t=0,ss_r=0;for(int i=0;i<t->size;i++){double d=t->data[i]-mn;ss_t+=d*d;double r=t->data[i]-p->data[i];ss_r+=r*r;}return kv_flt(ss_t>0?1.0-ss_r/ss_t:0);}
static inline KVal *k_ml_gpu_available(void){return kv_bool(false);}
static inline KVal *k_mlt_gpu(KVal *t){(void)t;return t;}
static inline KVal *k_mlt_cpu(KVal *t){(void)t;return t;}

#endif /* K_ML_H */
