/* k_vis.h — Katlans VIS Module — Computer Vision (simulated) */
#ifndef K_VIS_H
#define K_VIS_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* ── Camera ─────────────────────────────────────────────────────────────── */

static inline KVal *k_viscam(KVal *src){
    KVal *c=kv_dict();kdict_set(c->dict,"source",kv_str(src->s));
    kdict_set(c->dict,"width",kv_int(640));kdict_set(c->dict,"height",kv_int(480));
    kdict_set(c->dict,"open",kv_bool(true));kdict_set(c->dict,"fps",kv_int(30));
    return c;
}
static inline KVal *k_viscam_open(KVal *c){kdict_set(c->dict,"open",kv_bool(true));return kv_bool(true);}
static inline KVal *k_viscam_close(KVal *c){kdict_set(c->dict,"open",kv_bool(false));return kv_void();}
static inline KVal *k_viscam_list(void){
    KVal *lst=kv_list();KVal *c0=kv_dict();kdict_set(c0->dict,"id",kv_int(0));kdict_set(c0->dict,"name",kv_str("Default Camera"));klist_push(lst->list,c0);
    KVal *c1=kv_dict();kdict_set(c1->dict,"id",kv_int(1));kdict_set(c1->dict,"name",kv_str("USB Camera"));klist_push(lst->list,c1);return lst;
}
static inline KVal *k_viscam_width(KVal *c){return kdict_get(c->dict,"width");}
static inline KVal *k_viscam_height(KVal *c){return kdict_get(c->dict,"height");}

/* ── Frame ──────────────────────────────────────────────────────────────── */

static inline KVal *k_visframe(KVal *cam){(void)cam;
    KVal *f=kv_dict();kdict_set(f->dict,"w",kv_int(640));kdict_set(f->dict,"h",kv_int(480));
    kdict_set(f->dict,"channels",kv_int(3));kdict_set(f->dict,"valid",kv_bool(true));
    return f;
}
static inline KVal *k_visframe_blank(KVal *w,KVal *h){
    KVal *f=kv_dict();kdict_set(f->dict,"w",kv_int(w->i));kdict_set(f->dict,"h",kv_int(h->i));
    kdict_set(f->dict,"channels",kv_int(3));kdict_set(f->dict,"valid",kv_bool(true));
    return f;
}
static inline KVal *k_visload(KVal *path){
    KVal *f=kv_dict();kdict_set(f->dict,"w",kv_int(640));kdict_set(f->dict,"h",kv_int(480));
    kdict_set(f->dict,"path",kv_str(path->s));kdict_set(f->dict,"valid",kv_bool(true));
    return f;
}
static inline KVal *k_vissave(KVal *f,KVal *path){kdict_set(f->dict,"path",kv_str(path->s));return kv_void();}
static inline KVal *k_visframe_show(KVal *f,KVal *title){(void)f;printf("[VIS] Showing frame in window: %s\n",title->s);return kv_void();}
static inline KVal *k_visframe_size(KVal *f){
    KVal *r=kv_list();klist_push(r->list,kdict_get(f->dict,"w"));klist_push(r->list,kdict_get(f->dict,"h"));return r;
}
static inline KVal *k_visframe_gray(KVal *f){
    kdict_set(f->dict,"channels",kv_int(1));return kv_void();
}
static inline KVal *k_visframe_rgb(KVal *f){
    kdict_set(f->dict,"channels",kv_int(3));return kv_void();
}
static inline KVal *k_visframe_blur(KVal *f,KVal *k){(void)printf("[VIS] Blur frame with kernel=%lld\n",k->i);return kv_void();}
static inline KVal *k_visframe_threshold(KVal *f,KVal *thresh,KVal *type){(void)printf("[VIS] Threshold: %lld type=%s\n",thresh->i,type->s);return kv_void();}
static inline KVal *k_visframe_contours(KVal *f){(void)f;
    KVal *lst=kv_list();
    KVal *c1=kv_dict();kdict_set(c1->dict,"area",kv_flt(100.5));kdict_set(c1->dict,"perimeter",kv_flt(45.2));
    kdict_set(c1->dict,"x",kv_int(10));kdict_set(c1->dict,"y",kv_int(20));kdict_set(c1->dict,"w",kv_int(50));kdict_set(c1->dict,"h",kv_int(60));
    klist_push(lst->list,c1);
    KVal *c2=kv_dict();kdict_set(c2->dict,"area",kv_flt(200.3));kdict_set(c2->dict,"perimeter",kv_flt(70.1));
    kdict_set(c2->dict,"x",kv_int(100));kdict_set(c2->dict,"y",kv_int(150));kdict_set(c2->dict,"w",kv_int(80));kdict_set(c2->dict,"h",kv_int(90));
    klist_push(lst->list,c2);return lst;
}
static inline KVal *k_visframe_equalize(KVal *f){(void)f;printf("[VIS] Equalize histogram\n");return kv_void();}
static inline KVal *k_visframe_flip(KVal *f,KVal *mode){(void)printf("[VIS] Flip frame: %s\n",mode->s);return kv_void();}
static inline KVal *k_visframe_rotate(KVal *f,KVal *angle){(void)printf("[VIS] Rotate frame: %lld deg\n",angle->i);return kv_void();}
static inline KVal *k_visframe_resize(KVal *f,KVal *w,KVal *h){kdict_set(f->dict,"w",w);kdict_set(f->dict,"h",h);return kv_void();}
static inline KVal *k_visframe_crop(KVal *f,KVal *x,KVal *y,KVal *w,KVal *h){
    kdict_set(f->dict,"w",w);kdict_set(f->dict,"h",h);printf("[VIS] Crop to (%lld,%lld %lldx%lld)\n",x->i,y->i,w->i,h->i);return kv_void();
}
static inline KVal *k_visframe_histogram(KVal *f){(void)f;KVal *h=kv_list();for(int i=0;i<256;i++)klist_push(h->list,kv_int(rand()%1000));return h;}
static inline KVal *k_visframe_edges_canny(KVal *f,KVal *t1,KVal *t2){(void)printf("[VIS] Canny edges: %lld-%lld\n",t1->i,t2->i);return kv_void();}

/* ── Face Detection ─────────────────────────────────────────────────────── */

static inline KVal *k_vis_facedetect(KVal *cam){(void)cam;
    static int sd=0;if(!sd){srand((unsigned)time(NULL));sd=1;}
    KVal *lst=kv_list();int n=1+rand()%3;
    for(int i=0;i<n;i++){
        KVal *face=kv_dict();kdict_set(face->dict,"x",kv_int(100+rand()%200));kdict_set(face->dict,"y",kv_int(50+rand()%100));
        kdict_set(face->dict,"w",kv_int(80+rand()%40));kdict_set(face->dict,"h",kv_int(80+rand()%40));
        kdict_set(face->dict,"conf",kv_flt(0.7+(double)rand()/RAND_MAX*0.3));kdict_set(face->dict,"id",kv_int(i));
        klist_push(lst->list,face);
    }
    return lst;
}

/* ── Face Mesh ──────────────────────────────────────────────────────────── */

static inline KVal *k_vis_facemesh(KVal *cam){(void)cam;
    KVal *m=kv_dict();KVal *lm=kv_list();
    for(int i=0;i<468;i++){KVal *p=kv_dict();kdict_set(p->dict,"x",kv_flt(0.5));kdict_set(p->dict,"y",kv_flt(0.5));kdict_set(p->dict,"z",kv_flt(0));klist_push(lm->list,p);}
    kdict_set(m->dict,"landmarks",lm);kdict_set(m->dict,"nose",lm->list->items[1]);return m;
}

/* ── Face Recognition ──────────────────────────────────────────────────── */

static inline KVal *k_visface_add(KVal *name,KVal *ref){(void)printf("[VIS] Add face: %s from %s\n",name->s,ref->s);return kv_void();}
static inline KVal *k_visface_remove(KVal *name){(void)printf("[VIS] Remove face: %s\n",name->s);return kv_void();}
static inline KVal *k_visface_load(KVal *db){(void)printf("[VIS] Load face DB: %s\n",db->s);return kv_void();}
static inline KVal *k_visface_save(KVal *db){(void)printf("[VIS] Save face DB: %s\n",db->s);return kv_void();}

/* ── Face Recognition (live) ────────────────────────────────────────────── */

static inline KVal *k_vis_facerecog(KVal *cam){(void)cam;
    KVal *lst=kv_list();
    KVal *r=kv_dict();kdict_set(r->dict,"name",kv_str("John"));kdict_set(r->dict,"conf",kv_flt(0.92));
    kdict_set(r->dict,"x",kv_int(120));kdict_set(r->dict,"y",kv_int(80));
    klist_push(lst->list,r);return lst;
}

/* ── Landmarks (68 points) ─────────────────────────────────────────────── */

static inline KVal *k_vis_landmarks(KVal *cam){(void)cam;
    KVal *lm=kv_dict();
    KVal *pts=kv_list();for(int i=0;i<68;i++){KVal *p=kv_dict();kdict_set(p->dict,"x",kv_flt(0.5));kdict_set(p->dict,"y",kv_flt(0.5));klist_push(pts->list,p);}
    kdict_set(lm->dict,"points",pts);kdict_set(lm->dict,"headangle",kv_flt(0.0));
    kdict_set(lm->dict,"pitch",kv_flt(0.0));kdict_set(lm->dict,"yaw",kv_flt(0.0));kdict_set(lm->dict,"roll",kv_flt(0.0));
    kdict_set(lm->dict,"leftEyebrow",kv_int(18));kdict_set(lm->dict,"rightEyebrow",kv_int(25));
    kdict_set(lm->dict,"nose",kv_int(30));kdict_set(lm->dict,"mouth",kv_int(48));kdict_set(lm->dict,"jaw",kv_int(8));
    return lm;
}

/* ── Hand Tracking ──────────────────────────────────────────────────────── */

static inline KVal *k_vis_hands(KVal *cam){(void)cam;
    KVal *lst=kv_list();
    KVal *h=kv_dict();kdict_set(h->dict,"side",kv_str("Right"));kdict_set(h->dict,"conf",kv_flt(0.95));
    KVal *lm=kv_list();for(int i=0;i<21;i++){KVal *p=kv_dict();kdict_set(p->dict,"x",kv_flt(0.5));kdict_set(p->dict,"y",kv_flt(0.5));klist_push(lm->list,p);}
    kdict_set(h->dict,"landmarks",lm);kdict_set(h->dict,"wrist",lm->list->items[0]);
    kdict_set(h->dict,"fist",kv_bool(false));kdict_set(h->dict,"open",kv_bool(true));
    kdict_set(h->dict,"pointing",kv_bool(false));kdict_set(h->dict,"peace",kv_bool(false));
    kdict_set(h->dict,"thumbsup",kv_bool(true));kdict_set(h->dict,"ok",kv_bool(false));
    kdict_set(h->dict,"fingersup",kv_int(5));kdict_set(h->dict,"gesture",kv_str("open"));
    klist_push(lst->list,h);return lst;
}

/* ── Pose Detection ─────────────────────────────────────────────────────── */

static inline KVal *k_vis_pose(KVal *cam){(void)cam;
    KVal *ps=kv_dict();
    KVal *lm=kv_list();for(int i=0;i<33;i++){KVal *p=kv_dict();kdict_set(p->dict,"x",kv_flt(0.5));kdict_set(p->dict,"y",kv_flt(0.5));klist_push(lm->list,p);}
    kdict_set(ps->dict,"landmarks",lm);
    kdict_set(ps->dict,"nose",lm->list->items[0]);kdict_set(ps->dict,"leftShoulder",lm->list->items[11]);
    kdict_set(ps->dict,"rightShoulder",lm->list->items[12]);kdict_set(ps->dict,"activity",kv_str("standing"));
    return ps;
}

/* ── Object Detection ───────────────────────────────────────────────────── */

static inline KVal *k_vis_objects(KVal *cam){(void)cam;
    KVal *lst=kv_list();static const char *labels[]={"person","car","dog","bottle","chair","book",NULL};
    for(int i=0;labels[i];i++){
        KVal *o=kv_dict();kdict_set(o->dict,"label",kv_str(labels[i]));kdict_set(o->dict,"conf",kv_flt(0.75+(double)rand()/RAND_MAX*0.2));
        kdict_set(o->dict,"x",kv_int(rand()%500));kdict_set(o->dict,"y",kv_int(rand()%400));
        kdict_set(o->dict,"w",kv_int(30+rand()%100));kdict_set(o->dict,"h",kv_int(30+rand()%100));
        klist_push(lst->list,o);
    }
    return lst;
}

/* ── Segmentation ───────────────────────────────────────────────────────── */

static inline KVal *k_vis_segment(KVal *cam){(void)cam;
    KVal *s=kv_dict();kdict_set(s->dict,"classes",kv_list());kdict_set(s->dict,"colored",kv_str("segment_result"));
    return s;
}

/* ── Eye Tracking ───────────────────────────────────────────────────────── */

static inline KVal *k_vis_eyes(KVal *cam){(void)cam;
    KVal *e=kv_dict();KVal *l=kv_dict();kdict_set(l->dict,"x",kv_flt(0.3));kdict_set(l->dict,"y",kv_flt(0.4));
    KVal *r=kv_dict();kdict_set(r->dict,"x",kv_flt(0.7));kdict_set(r->dict,"y",kv_flt(0.4));
    kdict_set(e->dict,"left",l);kdict_set(e->dict,"right",r);
    kdict_set(e->dict,"gaze",kv_str("center"));kdict_set(e->dict,"blink",kv_bool(false));
    kdict_set(e->dict,"openness",kv_flt(0.85));kdict_set(e->dict,"drowsy",kv_bool(false));
    return e;
}

/* ── Emotion ────────────────────────────────────────────────────────────── */

static inline KVal *k_vis_emotion(KVal *cam){(void)cam;
    KVal *e=kv_dict();kdict_set(e->dict,"dominant",kv_str("happy"));
    kdict_set(e->dict,"happy",kv_flt(0.85));kdict_set(e->dict,"sad",kv_flt(0.02));
    kdict_set(e->dict,"angry",kv_flt(0.01));kdict_set(e->dict,"surprised",kv_flt(0.05));
    kdict_set(e->dict,"neutral",kv_flt(0.05));kdict_set(e->dict,"fearful",kv_flt(0.01));
    kdict_set(e->dict,"disgusted",kv_flt(0.01));return e;
}

/* ── Age & Gender ───────────────────────────────────────────────────────── */

static inline KVal *k_vis_agegender(KVal *cam){(void)cam;
    KVal *r=kv_dict();kdict_set(r->dict,"age",kv_flt(28.5));kdict_set(r->dict,"gender",kv_str("Male"));
    kdict_set(r->dict,"conf",kv_flt(0.88));return r;
}

/* ── Motion Detection ───────────────────────────────────────────────────── */

static inline KVal *k_vis_motion(KVal *cam){(void)cam;
    KVal *m=kv_dict();kdict_set(m->dict,"detected",kv_bool(false));
    KVal *reg=kv_list();klist_push(reg->list,kv_dict());kdict_set(m->dict,"regions",reg);
    kdict_set(m->dict,"area",kv_flt(0));return m;
}

/* ── Optical Flow ───────────────────────────────────────────────────────── */

static inline KVal *k_vis_flow(KVal *cam){(void)cam;
    KVal *f=kv_dict();kdict_set(f->dict,"vectors",kv_list());kdict_set(f->dict,"magnitude",kv_flt(0.0));kdict_set(f->dict,"angle",kv_flt(0.0));
    return f;
}

/* ── Color Detection ────────────────────────────────────────────────────── */

static inline KVal *k_vis_color(KVal *cam,KVal *hex){(void)cam;(void)hex;
    KVal *r=kv_dict();kdict_set(r->dict,"mask",kv_str("color_mask"));kdict_set(r->dict,"area",kv_flt(5000));
    KVal *reg=kv_list();klist_push(reg->list,kv_dict());kdict_set(r->dict,"regions",reg);return r;
}

/* ── QR / Barcode ───────────────────────────────────────────────────────── */

static inline KVal *k_vis_qr(KVal *cam){(void)cam;
    KVal *lst=kv_list();
    KVal *c=kv_dict();kdict_set(c->dict,"data",kv_str("https://katlans.dev"));kdict_set(c->dict,"type",kv_str("QR"));
    kdict_set(c->dict,"x",kv_int(200));kdict_set(c->dict,"y",kv_int(150));klist_push(lst->list,c);return lst;
}

/* ── OCR ────────────────────────────────────────────────────────────────── */

static inline KVal *k_visocr(KVal *frame){(void)frame;
    KVal *r=kv_dict();kdict_set(r->dict,"text",kv_str("Hello World"));kdict_set(r->dict,"conf",kv_flt(0.92));
    return r;
}

/* ── License Plate ──────────────────────────────────────────────────────── */

static inline KVal *k_vis_plate(KVal *cam){(void)cam;
    KVal *lst=kv_list();
    KVal *p=kv_dict();kdict_set(p->dict,"text",kv_str("ABC 1234"));kdict_set(p->dict,"conf",kv_flt(0.85));
    kdict_set(p->dict,"x",kv_int(300));kdict_set(p->dict,"y",kv_int(200));klist_push(lst->list,p);return lst;
}

/* ── Feature Detection & Matching ───────────────────────────────────────── */

static inline KVal *k_vistemplate(KVal *frame,KVal *tmpl){(void)frame;(void)tmpl;
    KVal *lst=kv_list();
    KVal *m=kv_dict();kdict_set(m->dict,"x",kv_int(100));kdict_set(m->dict,"y",kv_int(100));
    kdict_set(m->dict,"w",kv_int(50));kdict_set(m->dict,"h",kv_int(50));kdict_set(m->dict,"conf",kv_flt(0.85));
    klist_push(lst->list,m);return lst;
}
static inline KVal *k_visfeature_detect(KVal *frame,KVal *method){(void)frame;(void)method;
    KVal *kp=kv_list();
    for(int i=0;i<50;i++){KVal *p=kv_dict();kdict_set(p->dict,"x",kv_flt(rand()%640));kdict_set(p->dict,"y",kv_flt(rand()%480));
    kdict_set(p->dict,"size",kv_flt(4.0));kdict_set(p->dict,"angle",kv_flt((double)rand()/RAND_MAX*360));klist_push(kp->list,p);}
    KVal *r=kv_list();klist_push(r->list,kp);klist_push(r->list,kv_list());return r;
}

/* ── Drawing tools ──────────────────────────────────────────────────────── */

static inline KVal *k_visdraw_box(KVal *frame,KVal *x,KVal *y,KVal *w,KVal *h,KVal *color,KVal *thick){(void)frame;(void)color;(void)thick;
    printf("[VIS] Draw box at (%lld,%lld) %lldx%lld\n",x->i,y->i,w->i,h->i);return kv_void();
}
static inline KVal *k_visdraw_circle(KVal *frame,KVal *cx,KVal *cy,KVal *r,KVal *color,KVal *filled){(void)frame;(void)color;(void)filled;
    printf("[VIS] Draw circle at (%lld,%lld) r=%lld\n",cx->i,cy->i,r->i);return kv_void();
}
static inline KVal *k_visdraw_line(KVal *frame,KVal *x1,KVal *y1,KVal *x2,KVal *y2,KVal *color,KVal *thick){(void)frame;(void)color;(void)thick;
    printf("[VIS] Draw line (%lld,%lld)-(%lld,%lld)\n",x1->i,y1->i,x2->i,y2->i);return kv_void();
}
static inline KVal *k_visdraw_text(KVal *frame,KVal *x,KVal *y,KVal *text,KVal *size,KVal *color){(void)frame;(void)color;
    printf("[VIS] Draw text \"%s\" at (%lld,%lld) size=%.2f\n",text->s,x->i,y->i,_knum(size));return kv_void();
}

/* ── Recording ──────────────────────────────────────────────────────────── */

static inline KVal *k_visrec_start(KVal *path,KVal *fps,KVal *codec){(void)printf("[VIS] Start recording: %s @ %lldfps %s\n",path->s,fps->i,codec->s);return kv_void();}
static inline KVal *k_visrec_frame(KVal *frame){(void)frame;return kv_void();}
static inline KVal *k_visrec_stop(void){printf("[VIS] Stop recording\n");return kv_void();}
static inline KVal *k_visshot(void){printf("[VIS] Screenshot\n");return kv_void();}

/* ── Video File Processing ──────────────────────────────────────────────── */

static inline KVal *k_visvid(KVal *path){
    KVal *v=kv_dict();kdict_set(v->dict,"path",kv_str(path->s));kdict_set(v->dict,"fps",kv_int(30));
    kdict_set(v->dict,"framecount",kv_int(3000));kdict_set(v->dict,"duration",kv_flt(100.0));
    kdict_set(v->dict,"open",kv_bool(true));return v;
}
static inline KVal *k_visvid_fps(KVal *v){return kdict_get(v->dict,"fps");}
static inline KVal *k_visvid_framecount(KVal *v){return kdict_get(v->dict,"framecount");}
static inline KVal *k_visvid_duration(KVal *v){return kdict_get(v->dict,"duration");}
static inline KVal *k_visvid_read(KVal *v){(void)v;return k_visframe_blank(kv_int(640),kv_int(480));}
static inline KVal *k_visvid_close(KVal *v){kdict_set(v->dict,"open",kv_bool(false));return kv_void();}
static inline KVal *k_visvid_seek(KVal *v,KVal *sec){(void)printf("[VIS] Seek video to %.2f\n",_knum(sec));return kv_void();}
static inline KVal *k_visvid_writer(KVal *path,KVal *fps,KVal *w,KVal *h){
    KVal *wr=kv_dict();kdict_set(wr->dict,"path",kv_str(path->s));return wr;
}
static inline KVal *k_visvid_write(KVal *wr,KVal *frame){(void)wr;(void)frame;return kv_void();}
static inline KVal *k_visvid_close_writer(KVal *wr){(void)wr;return kv_void();}

#endif /* K_VIS_H */
