
/*
* ---- EXPORTED BY TOC.HBAS ----
* Beloved, if God so loved us, we ought also to love one another.
* 1 Jn 4:11
*/

#define SEABASS_64_BIT 1
#define i32 int
#define u32 unsigned int
#define _STRTOU_ strtoull
#define _STRTOI_ strtoll
#define TGT_UMAX unsigned long long
#define TGT_IMAX long long


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
//#include <unistd.h>
/*for unsupported platforms...*/
#ifdef __APPLE__
#define __CBAS_SINGLE_THREADED__
#endif

#ifndef __CBAS_SINGLE_THREADED__
#include <pthread.h>
typedef struct {
    pthread_mutex_t myMutex;
    pthread_barrier_t myBarrier;
    pthread_t myThread;
    int isThreadLive;
    int shouldKillThread;
    int state;
    void (*execute)(unsigned char*);
    unsigned char* argument;
} lsthread;
static inline void init_lsthread(lsthread* t);
static inline void start_lsthread(lsthread* t);
static inline void kill_lsthread(lsthread* t);
static inline void destroy_lsthread(lsthread* t);
static inline void lock(lsthread* t);
static inline void step(lsthread* t);
static void* lsthread_func(void* me_void);
static inline void init_lsthread(lsthread* t){
    pthread_mutex_init(&t->myMutex, NULL);
    pthread_barrier_init(&t->myBarrier, NULL, 2);
    t->isThreadLive = 0;
    t->shouldKillThread = 0;
    t->state = 0;
    t->execute = NULL;
    t->argument = NULL;
}
static inline void destroy_lsthread(lsthread* t){
    pthread_mutex_destroy(&t->myMutex);
    pthread_barrier_destroy(&t->myBarrier);
}
static inline void lock(lsthread* t){
    if(t->state == 1)return;
    if(!t->isThreadLive)return;
    pthread_barrier_wait(&t->myBarrier);
    if(pthread_mutex_lock(&t->myMutex))
        exit(1);
    t->state = 1;
}

static inline void step(lsthread* t){
    if(t->state == -1)return;
    if(!t->isThreadLive)return;
    if(pthread_mutex_unlock(&(t->myMutex)))
        exit(1);
    pthread_barrier_wait(&t->myBarrier);
    t->state = -1;
}
static inline void kill_lsthread(lsthread* t){
    if(!t->isThreadLive)return;
    if(t->state != 1){
        lock(t);
    }
    t->shouldKillThread = 1;
    step(t);
    
    pthread_join(t->myThread,NULL);
    
    t->isThreadLive = 0;
    t->shouldKillThread = 0;
}
static void* lsthread_func(void* me_void){
    lsthread* me = (lsthread*) me_void;
    if (!me)pthread_exit(NULL);
    while (1) {
        pthread_barrier_wait(&me->myBarrier);
        pthread_mutex_lock(&me->myMutex);
        if (!(me->shouldKillThread) && me->execute)
            me->execute(me->argument);
        else if(me->shouldKillThread){
            pthread_mutex_unlock(&me->myMutex);
            pthread_exit(NULL);
        }

        pthread_mutex_unlock(&me->myMutex);
        pthread_barrier_wait(&me->myBarrier);
    }
    pthread_exit(NULL);
}
static inline void start_lsthread(lsthread* t){
    if(t->isThreadLive)return;
    t->isThreadLive = 1;
    t->shouldKillThread = 0;
    if(pthread_mutex_lock(&t->myMutex))
        exit(1);
    t->state = 1;/*LOCKED*/
    pthread_create(
        &t->myThread,
        NULL,
        lsthread_func,
        (void*)t
    );
}

#endif

static inline double __CBAS__atof(unsigned char* text){
    return strtod((char*)text, NULL);
}
static inline TGT_UMAX __CBAS__atou(unsigned char* text){
#ifndef SEABASS_16_BIT
    return _STRTOU_((char*)text, NULL, 0);
#else
    return _STRTOU_((char*)text);
#endif
}
static inline TGT_IMAX __CBAS__atoi(unsigned char* text){
#ifndef SEABASS_16_BIT
    return _STRTOI_((char*)text, NULL, 0);
#else
    return _STRTOI_((char*)text);
#endif
}
static inline void __CBAS__srand(u32 seed){
    srand(seed);
}
static inline i32 __CBAS__rand(){
    return rand();
}
static inline TGT_IMAX __CBAS__strcmp(unsigned char* a, unsigned char* b){
    return strcmp((char*)a,(char*)b);
}
static inline TGT_IMAX __CBAS__memcmp(unsigned char* a, unsigned char* b, TGT_UMAX sz){
    return memcmp((char*)a,(char*)b, sz);
}
static inline TGT_UMAX __CBAS__strlen(unsigned char* s){
    return strlen((char*)s);
}
static inline void __CBAS__strcpy(unsigned char* d, unsigned char* s){
    strcpy((char*)d, (char*)s);
}

static inline TGT_UMAX __CBAS__fopen(unsigned char* fname, unsigned char* mode){
    return (TGT_UMAX)fopen((char*)fname, (char*)mode);
}

static inline i32 __CBAS__fclose(TGT_UMAX p){
    return fclose((FILE*)p);
}
static inline i32 __CBAS__fflush(TGT_UMAX p){
    return fflush((FILE*)p);
}

static inline TGT_UMAX __CBAS__popen(unsigned char* fname, unsigned char* mode){
    return (TGT_UMAX)popen((char*)fname, (char*)mode);
}

static inline i32 __CBAS__pclose(TGT_UMAX p){
    return pclose((FILE*)p);
}

/*fn predecl noexport getdirlist(char* dirname, char*** entry_listing, u32* nentries)->i32;*/
static inline i32 __CBAS__getdirlist(unsigned char* dirname, unsigned char*** entry_listing, u32* nentries){
    DIR* dir;
    struct dirent* de;
    dir = opendir((char*)dirname);
    if(dir == NULL) return 1; 
    nentries[0] = 0;
    entry_listing[0] = 0;
    while(1){
        de = readdir(dir);
        if(de == NULL) break;
        entry_listing[0] = realloc(entry_listing[0], sizeof(void*) * ++(nentries[0]));
        entry_listing[0][nentries[0]-1] = (unsigned char*)strdup(de->d_name);
    }
    closedir(dir);
    return 0;
}

static inline TGT_UMAX __CBAS__system(unsigned char* cmd){
    return system((char*)cmd);
}

static inline TGT_UMAX __CBAS__get_stdout_handle(){
    return (TGT_UMAX)stdout;
}

static inline TGT_UMAX __CBAS__get_stdin_handle(){
    return (TGT_UMAX)stdin;
}

static inline TGT_UMAX __CBAS__get_stderr_handle(){
    return (TGT_UMAX)stderr;
}

static inline TGT_UMAX __CBAS__fwrite(TGT_UMAX fhandle, unsigned char* buf, TGT_UMAX nbytes){
    return fwrite(buf, 1, nbytes,(FILE*)fhandle);
}

static inline TGT_UMAX __CBAS__fread(TGT_UMAX fhandle, unsigned char* buf, TGT_UMAX nbytes){
    return fread(buf, 1, nbytes,(FILE*)fhandle);
}
static inline i32 __CBAS__feof(TGT_UMAX fhandle){
    return feof((FILE*)fhandle);
}
static inline TGT_IMAX __CBAS__ftell(TGT_UMAX fhandle){
    return ftell((FILE*)fhandle);
}
static inline i32 __CBAS__fseek(TGT_UMAX fhandle, TGT_IMAX offset, i32 whence){
    return fseek((FILE*)fhandle, offset, whence);
}
static inline i32 __CBAS__fremove(unsigned char* fname){
    return remove((char*)fname);
}
static inline i32 __CBAS__frename(unsigned char* fname_old, unsigned char* fname_new){
    return rename((char*)fname_old, (char*)fname_new);
}
static inline void __CBAS__frewind(TGT_UMAX fhandle){
    rewind((FILE*)fhandle);
}
static inline void __CBAS__perror(unsigned char* msg){
    perror((char*)msg);
}
static inline void __CBAS__fclearerr(TGT_UMAX fhandle){
    clearerr((FILE*)fhandle);
}
static inline TGT_UMAX __CBAS__L_tmpnam(){
    return L_tmpnam;
}
static inline unsigned char* __CBAS__tmpnam(unsigned char* s){
    return (unsigned char*)tmpnam((char*)s);
}
static inline TGT_UMAX __CBAS__unixtime(){
    return time(0);
}
static inline TGT_UMAX __CBAS__time(TGT_UMAX* a){
    time_t qq;
    qq = *a;
    time_t retval = time(&qq);
    *a = qq;
    return retval;
}
static inline unsigned char* __CBAS__ctime(TGT_UMAX* a){
    time_t qq;
    qq = *a;
    unsigned char* retval = (unsigned char*)ctime(&qq);
    *a = qq;
    return retval;
}
static inline double __CBAS__difftime(TGT_UMAX a, TGT_UMAX b){
    return difftime(a,b);
}
static inline TGT_UMAX __CBAS__clock(){
    return clock();
}
static inline TGT_UMAX __CBAS__CLOCKS_PER_SEC(){
    return CLOCKS_PER_SEC;
}
static inline unsigned char* __CBAS__localtime(TGT_UMAX a){
    time_t tt = a;
    struct tm * timeinfo;
    timeinfo = localtime(&tt);
    return (unsigned char*)timeinfo;
}
static inline unsigned char* __CBAS__gmtime(TGT_UMAX a){
    time_t tt = a;
    struct tm * timeinfo;
    timeinfo = gmtime(&tt);
    return (unsigned char*)timeinfo;
}
static inline unsigned char* __CBAS__asctime(unsigned char* tm_struct){
   struct tm timeinfo;
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return (unsigned char*)asctime(&timeinfo);
}

static inline i32 __CBAS__tm_sec(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_sec;
}
static inline i32 __CBAS__tm_min(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_min;
}
static inline i32 __CBAS__tm_hour(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_hour;
}
static inline i32 __CBAS__tm_mday(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_mday;
}
static inline i32 __CBAS__tm_mon(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_mon;
}
static inline i32 __CBAS__tm_year(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_year;
}
static inline i32 __CBAS__tm_wday(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_wday;
}
static inline i32 __CBAS__tm_yday(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_yday;
}
static inline i32 __CBAS__tm_isdst(unsigned char* tm_struct){
   struct tm timeinfo;
   
   memcpy(&timeinfo, tm_struct, sizeof(struct tm));
   return timeinfo.tm_isdst;
}
#define __CBAS__errno (errno)
/*
static inline i32 __CBAS__pipe(i32* pipefd){
    int pd[2];
    int retval;
    pd[0] = pipefd[0];
    pd[1] = pipefd[1];
    retval = pipe(pd);
    pipefd[0] = pd[0];
    pipefd[1] = pd[1];
    return retval;
}*/
static inline void __CBAS__mcpy(unsigned char* dst, unsigned char* src, TGT_UMAX sz){
    memcpy(dst, src, sz);
}
static inline void __CBAS__mmove(unsigned char* dst, unsigned char* src, TGT_UMAX sz){
    memmove(dst, src, sz);
}
static inline void __CBAS__memclear(unsigned char* buf, TGT_UMAX sz){
    memset(buf, 0, sz);
}
static inline unsigned char* __CBAS__malloc(TGT_UMAX amt){
    return malloc(amt);
}
static inline void __CBAS__free(unsigned char* p){
    free(p);
}
static inline unsigned char* __CBAS__realloc(unsigned char* b, TGT_UMAX amt){
    return realloc(b, amt);
}
static inline void __CBAS__println(unsigned char* s){
    puts((char*)s);
}
static inline void __CBAS__sys_exit(i32 a){
    exit(a);
}
    static inline double __CBAS__sin(double a){
        return sin(a);
    }

    static inline double __CBAS__cos(double a){
        return cos(a);
    }

    static inline double __CBAS__tan(double a){
        return tan(a);
    }

    static inline float __CBAS__sinf(float a){
        return sinf(a);
    }

    static inline float __CBAS__cosf(float a){
        return cosf(a);
    }

    static inline float __CBAS__tanf(float a){
        return tanf(a);
    }

    static inline double __CBAS__asin(double a){
        return asin(a);
    }

    static inline double __CBAS__acos(double a){
        return acos(a);
    }

    static inline double __CBAS__atan(double a){
        return atan(a);
    }

    static inline float __CBAS__asinf(float a){
        return asinf(a);
    }

    static inline float __CBAS__acosf(float a){
        return acosf(a);
    }

    static inline float __CBAS__atanf(float a){
        return atanf(a);
    }
    static inline double __CBAS__atan2(double a, double b){
        return atan2(a, b);
    }
    static inline float __CBAS__atan2f(float a, float b){
        return atan2f(a, b);
    }
    static inline double __CBAS__sinh(double a){
        return sinh(a);
    }
    static inline double __CBAS__cosh(double a){
        return cosh(a);
    }
    static inline double __CBAS__tanh(double a){
        return tanh(a);
    }
    static inline float __CBAS__sinhf(float a){
        return sinhf(a);
    }
    static inline float __CBAS__coshf(float a){
        return coshf(a);
    }
    static inline float __CBAS__tanhf(float a){
        return tanhf(a);
    }
    static inline double __CBAS__asinh(double a){
        return asinh(a);
    }
    static inline double __CBAS__acosh(double a){
        return acosh(a);
    }
    static inline double __CBAS__atanh(double a){
        return atanh(a);
    }
    static inline float __CBAS__asinhf(float a){
        return asinhf(a);
    }
    static inline float __CBAS__acoshf(float a){
        return acoshf(a);
    }
    static inline float __CBAS__atanhf(float a){
        return atanhf(a);
    }
    static inline double __CBAS__exp(double a){
        return exp(a);
    }
    static inline float __CBAS__expf(float a){
        return expf(a);
    }
    
    static inline double __CBAS__frexp(double a, i32* b){
        int cc = *b;
        double retval = frexp(a,&cc);
        *b = cc;
        return retval;
    }
    static inline float __CBAS__frexpf(float a, i32* b){
        int cc = *b;
        float retval = frexpf(a,&cc);
        *b = cc;
        return retval;
    }
    static inline double __CBAS__ldexp(double a, i32 b){
        return ldexp(a,b);
    }
    static inline float __CBAS__ldexpf(float a, i32 b){
        return ldexp(a,b);
    }
    static inline double __CBAS__log(double a){
        return log(a);
    }
    static inline float __CBAS__logf(float a){
        return logf(a);
    }
    static inline double __CBAS__log10(double a){
        return log10(a);
    }
    static inline float __CBAS__log10f(float a){
        return log10f(a);
    }
    static inline double __CBAS__log2(double a){
        return log2(a);
    }
    static inline float __CBAS__log2f(float a){
        return log2f(a);
    }
    static inline double __CBAS__logb(double a){
        return logb(a);
    }
    static inline float __CBAS__logbf(float a){
        return logbf(a);
    }
    static inline double __CBAS__log1p(double a){
        return log1p(a);
    }
    static inline float __CBAS__log1pf(float a){
        return log1pf(a);
    }
    static inline double __CBAS__modf(double a, double* b){
        return modf(a, b);
    }
    static inline float __CBAS__modff(float a, float* b){
        return modff(a, b);
    }
    static inline double __CBAS__exp2(double a){
        return exp2(a);
    }
    static inline float __CBAS__exp2f(float a){
        return exp2f(a);
    }
    static inline double __CBAS__expm1(double a){
        return expm1(a);
    }
    static inline float __CBAS__expm1f(float a){
        return expm1f(a);
    }
    static inline i32 __CBAS__ilogb(double a){
        return ilogb(a);
    }
    static inline i32 __CBAS__ilogbf(float a){
        return ilogbf(a);
    }
    static inline double __CBAS__scalbn(double a, i32 n){
        return scalbn(a,n);
    }
    static inline float __CBAS__scalbnf(float a, i32 n){
        return scalbnf(a,n);
    }
    static inline double __CBAS__pow(double a, double b){
        return pow(a, b);
    }
    static inline float __CBAS__powf(float a, float b){
        return powf(a, b);
    }
    static inline double __CBAS__sqrt(double a){
        return sqrt(a);
    }
    static inline float __CBAS__sqrtf(float a){
        return sqrtf(a);
    }
    static inline double __CBAS__cbrt(double a){
        return cbrt(a);
    }
    static inline float __CBAS__cbrtf(float a){
        return cbrtf(a);
    }
    static inline double __CBAS__hypot(double a, double b){
        return hypot(a, b);
    }
    static inline float __CBAS__hypotf(float a, float b){
        return hypotf(a, b);
    }
    static inline double __CBAS__erf(double a){
        return erf(a);
    }
    static inline float __CBAS__erff(float a){
        return erff(a);
    }
    static inline double __CBAS__erfc(double a){
        return erfc(a);
    }
    static inline float __CBAS__erfcf(float a){
        return erfcf(a);
    }
    static inline double __CBAS__tgamma(double a){
        return tgamma(a);
    }
    static inline float __CBAS__tgammaf(float a){
        return tgammaf(a);
    }
    static inline double __CBAS__lgamma(double a){
        return lgamma(a);
    }
    static inline float __CBAS__lgammaf(float a){
        return lgammaf(a);
    }
    static inline double __CBAS__ceil(double a){
        return ceil(a);
    }
    static inline float __CBAS__ceilf(float a){
        return ceilf(a);
    }
    static inline double __CBAS__floor(double a){
        return floor(a);
    }
    static inline float __CBAS__floorf(float a){
        return floorf(a);
    }    
    static inline double __CBAS__trunc(double a){
        return trunc(a);
    }
    static inline float __CBAS__truncf(float a){
        return truncf(a);
    }
    static inline double __CBAS__round(double a){
        return round(a);
    }
    static inline float __CBAS__roundf(float a){
        return roundf(a);
    }
    static inline double __CBAS__fmod(double a, double b){
        return fmod(a,b);
    }
    static inline float __CBAS__fmodf(float a, float b){
        return fmodf(a,b);
    }
    static inline double __CBAS__nearbyint(double a){
        return nearbyint(a);
    }
    static inline float __CBAS__nearbyintf(float a){
        return nearbyintf(a);
    }
    static inline double __CBAS__remainder(double a, double b){
        return remainder(a,b);
    }
    static inline float __CBAS__remainderf(float a, float b){
        return remainderf(a,b);
    }    
    static inline double __CBAS__remquo(double a, double b, i32* q){
        int p = *q;
        double retval = remquo(a,b,&p);
        *q = p;
        return retval;
    }
    static inline float __CBAS__remquof(float a, float b, i32* q){
        int p = *q;
        float retval = remquof(a,b,&p);
        *q = p;
        return retval;
    }
    static inline double __CBAS__copysign(double a, double b){
        return copysign(a,b);
    }
    static inline float __CBAS__copysignf(float a, float b){
        return copysignf(a,b);
    }
    static inline double __CBAS__nan(unsigned char* tagp){
        return nan((const char*)tagp);
    }
    static inline float __CBAS__nanf(unsigned char* tagp){
        return nanf((const char*)tagp);
    }
    static inline double __CBAS__nextafter(double a, double b){
        return nextafter(a,b);
    }
    static inline float __CBAS__nextafterf(float a, float b){
        return nextafterf(a,b);
    }
    static inline double __CBAS__fdim(double a, double b){
        return fdim(a,b);
    }
    static inline float __CBAS__fdimf(float a, float b){
        return fdimf(a,b);
    }
    static inline double __CBAS__fmax(double a, double b){
        return fmax(a,b);
    }
    static inline float __CBAS__fmaxf(float a, float b){
        return fmaxf(a,b);
    }
    static inline double __CBAS__fmin(double a, double b){
        return fmin(a,b);
    }
    static inline float __CBAS__fminf(float a, float b){
        return fminf(a,b);
    }
    static inline double __CBAS__fabs(double a){
        return fabs(a);
    }
    static inline float __CBAS__fabsf(float a){
        return fabsf(a);
    }
    static inline double __CBAS__fma(double a, double b, double z){
        return fma(a,b,z);
    }
    static inline float __CBAS__fmaf(float a, float b, float z){
        return fmaf(a,b,z);
    }
    static inline i32 __CBAS__fpclassify(double q){
        return fpclassify(q);
    }
    static inline i32 __CBAS__isfinite(double q){
        return isfinite(q);
    }
    static inline i32 __CBAS__isinf(double q){
        return isinf(q);
    }
    static inline i32 __CBAS__isnan(double q){
        return isnan(q);
    }
    static inline i32 __CBAS__isnormal(double q){
        return isnormal(q);
    }
    static inline i32 __CBAS__signbit(double q){
        return signbit(q);
    }
    static inline i32 __CBAS__fpclassifyf(float q){
        return fpclassify(q);
    }
    static inline i32 __CBAS__isfinitef(float q){
        return isfinite(q);
    }
    static inline i32 __CBAS__isinff(float q){
        return isinf(q);
    }
    static inline i32 __CBAS__isnanf(float q){
        return isnan(q);
    }
    static inline i32 __CBAS__isnormalf(float q){
        return isnormal(q);
    }
    static inline i32 __CBAS__signbitf(float q){
        return signbit(q);
    }

    static inline i32 __CBAS__isunordered(double a, double b){
        return isunordered(a,b);
    }
    static inline i32 __CBAS__isunorderedf(float a, float b){
        return isunordered(a,b);
    }

    static inline float __CBAS__INFINITY(){
        return INFINITY;
    }
    static inline float __CBAS__NAN(){
        return NAN;
    }
    static inline double __CBAS__HUGE_VAL(){
        return HUGE_VAL;
    }
    static inline float __CBAS__HUGE_VALF(){
        return HUGE_VALF;
    }

/*MULTITHREADING LIBRARY*/
#ifndef __CBAS_SINGLE_THREADED__


static inline unsigned char* __CBAS__thread_new(){
    lsthread* p = malloc(sizeof(lsthread));
    init_lsthread(p);
    return (unsigned char*)p;
}
static inline void __CBAS__thread_delete(unsigned char* tr){
    lsthread* p = (lsthread*)tr;
    destroy_lsthread(p);
    free(p);
    return;
}

static inline void __CBAS__thread_assign_fn(unsigned char* tr, unsigned char* funk){
    lsthread* p = (lsthread*)tr;
    void* fnk = funk;
    p->execute = fnk;
}

static inline void __CBAS__thread_assign_arg(unsigned char* tr, unsigned char* arg){
    lsthread* p = (lsthread*)tr;
    p->argument = arg;
}

static inline void __CBAS__thread_start(unsigned char* tr){
    lsthread* p = (lsthread*)tr;
    start_lsthread(p);
}
static inline void __CBAS__thread_kill(unsigned char* tr){
    lsthread* p = (lsthread*)tr;
    kill_lsthread(p);
}

static inline void __CBAS__thread_step(unsigned char* tr){
    lsthread* p = (lsthread*)tr;
    step(p);
}

static inline void __CBAS__thread_lock(unsigned char* tr){
    lsthread* p = (lsthread*)tr;
    lock(p);
}

static inline unsigned char* __CBAS__mutex_new(){
    pthread_mutex_t* m = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(m, NULL);
    return (unsigned char*)m;
}

static inline void __CBAS__mutex_delete(unsigned char* mtx){
    pthread_mutex_t* m = (pthread_mutex_t*)mtx;
    pthread_mutex_destroy(m);
    free(m);
}

static inline void __CBAS__mutex_lock(unsigned char* mtx){
    pthread_mutex_t* m = (pthread_mutex_t*)mtx;
    pthread_mutex_lock(m);
}
static inline void __CBAS__mutex_unlock(unsigned char* mtx){
    pthread_mutex_t* m = (pthread_mutex_t*)mtx;
    pthread_mutex_unlock(m);
}
#else
static inline unsigned char* __CBAS__thread_new(){
    return malloc(sizeof(char*) * 2);
}
static inline void __CBAS__thread_delete(unsigned char* tr){
    free(tr);
    return;
}

static inline void __CBAS__thread_assign_fn(unsigned char* tr, unsigned char* funk){
    void** t = (void**)tr;
    t[0] = funk;
}

static inline void __CBAS__thread_assign_arg(unsigned char* tr, unsigned char* arg){
    void** t = (void**)tr;
    t[1] = arg;
}

static inline void __CBAS__thread_start(unsigned char* tr){
    return;
}
static inline void __CBAS__thread_kill(unsigned char* tr){
    return;
}

static inline void __CBAS__thread_step(unsigned char* tr){
    void** t = (void**)tr;
    /*get thread function...*/
    void (*execute)(unsigned char*);
    execute = (void*)(t[0]);
    execute(t[1]);
    return;
}

static inline void __CBAS__thread_lock(unsigned char* tr){
    return;
}

static inline unsigned char* __CBAS__mutex_new(){
    return NULL;
}

static inline void __CBAS__mutex_delete(unsigned char* mtx){
    free(mtx);
}

static inline void __CBAS__mutex_lock(unsigned char* mtx){
    return;
}
static inline void __CBAS__mutex_unlock(unsigned char* mtx){
    return;
}

#endif

#include <GL/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC

/* stb_image - v2.28 - public domain image loader - http://nothings.org/stb
                                  no warranty implied; use at your own risk

   Do this:
      #define STB_IMAGE_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #include ...
   #define STB_IMAGE_IMPLEMENTATION
   #include "stb_image.h"

   You can #define STBI_ASSERT(x) before the #include to avoid using assert.h.
   And #define STBI_MALLOC, STBI_REALLOC, and STBI_FREE to avoid using malloc,realloc,free


   QUICK NOTES:
      Primarily of interest to game developers and other people who can
          avoid problematic images and only need the trivial interface

      JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
      PNG 1/2/4/8/16-bit-per-channel

      TGA (not sure what subset, if a subset)
      BMP non-1bpp, non-RLE
      PSD (composited view only, no extra channels, 8/16 bit-per-channel)

      GIF (*comp always reports as 4-channel)
      HDR (radiance rgbE format)
      PIC (Softimage PIC)
      PNM (PPM and PGM binary only)

      Animated GIF still needs a proper API, but here's one way to do it:
          http://gist.github.com/urraka/685d9a6340b26b830d49

      - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
      - decode from arbitrary I/O callbacks
      - SIMD acceleration on x86/x64 (SSE2) and ARM (NEON)

   Full documentation under "DOCUMENTATION" below.


LICENSE

  See end of file for license information.

RECENT REVISION HISTORY:

      2.28  (2023-01-29) many error fixes, security errors, just tons of stuff
      2.27  (2021-07-11) document stbi_info better, 16-bit PNM support, bug fixes
      2.26  (2020-07-13) many minor fixes
      2.25  (2020-02-02) fix warnings
      2.24  (2020-02-02) fix warnings; thread-local failure_reason and flip_vertically
      2.23  (2019-08-11) fix clang static analysis warning
      2.22  (2019-03-04) gif fixes, fix warnings
      2.21  (2019-02-25) fix typo in comment
      2.20  (2019-02-07) support utf8 filenames in Windows; fix warnings and platform ifdefs
      2.19  (2018-02-11) fix warning
      2.18  (2018-01-30) fix warnings
      2.17  (2018-01-29) bugfix, 1-bit BMP, 16-bitness query, fix warnings
      2.16  (2017-07-23) all functions have 16-bit variants; optimizations; bugfixes
      2.15  (2017-03-18) fix png-1,2,4; all Imagenet JPGs; no runtime SSE detection on GCC
      2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs
      2.13  (2016-12-04) experimental 16-bit API, only for PNG so far; fixes
      2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
      2.11  (2016-04-02) 16-bit PNGS; enable SSE2 in non-gcc x64
                         RGB-format JPEG; remove white matting in PSD;
                         allocate large structures on the stack;
                         correct channel count for PNG & BMP
      2.10  (2016-01-22) avoid warning introduced in 2.09
      2.09  (2016-01-16) 16-bit TGA; comments in PNM files; STBI_REALLOC_SIZED

   See end of file for full revision history.


 ============================    Contributors    =========================

 Image formats                          Extensions, features
    Sean Barrett (jpeg, png, bmp)          Jetro Lauha (stbi_info)
    Nicolas Schulz (hdr, psd)              Martin "SpartanJ" Golini (stbi_info)
    Jonathan Dummer (tga)                  James "moose2000" Brown (iPhone PNG)
    Jean-Marc Lienher (gif)                Ben "Disch" Wenger (io callbacks)
    Tom Seddon (pic)                       Omar Cornut (1/2/4-bit PNG)
    Thatcher Ulrich (psd)                  Nicolas Guillemot (vertical flip)
    Ken Miller (pgm, ppm)                  Richard Mitton (16-bit PSD)
    github:urraka (animated gif)           Junggon Kim (PNM comments)
    Christopher Forseth (animated gif)     Daniel Gibson (16-bit TGA)
                                           socks-the-fox (16-bit PNG)
                                           Jeremy Sawicki (handle all ImageNet JPGs)
 Optimizations & bugfixes                  Mikhail Morozov (1-bit BMP)
    Fabian "ryg" Giesen                    Anael Seghezzi (is-16-bit query)
    Arseny Kapoulkine                      Simon Breuss (16-bit PNM)
    John-Mark Allen
    Carmelo J Fdez-Aguera

 Bug & warning fixes
    Marc LeBlanc            David Woo          Guillaume George     Martins Mozeiko
    Christpher Lloyd        Jerry Jansson      Joseph Thomson       Blazej Dariusz Roszkowski
    Phil Jordan                                Dave Moore           Roy Eltham
    Hayaki Saito            Nathan Reed        Won Chun
    Luke Graham             Johan Duparc       Nick Verigakis       the Horde3D community
    Thomas Ruf              Ronny Chevalier                         github:rlyeh
    Janez Zemva             John Bartholomew   Michal Cichon        github:romigrou
    Jonathan Blow           Ken Hamada         Tero Hanninen        github:svdijk
    Eugene Golushkov        Laurent Gomila     Cort Stratton        github:snagar
    Aruelien Pocheville     Sergio Gonzalez    Thibault Reuille     github:Zelex
    Cass Everitt            Ryamond Barbiero                        github:grim210
    Paul Du Bois            Engin Manap        Aldo Culquicondor    github:sammyhw
    Philipp Wiesemann       Dale Weiler        Oriol Ferrer Mesia   github:phprus
    Josh Tobin              Neil Bickford      Matthew Gregan       github:poppolopoppo
    Julian Raschke          Gregory Mullen     Christian Floisand   github:darealshinji
    Baldur Karlsson         Kevin Schmidt      JR Smith             github:Michaelangel007
                            Brad Weinberger    Matvey Cherevko      github:mosra
    Luca Sas                Alexander Veselov  Zack Middleton       [reserved]
    Ryan C. Gordon          [reserved]                              [reserved]
                     DO NOT ADD YOUR NAME HERE

                     Jacko Dirks

  To add your name to the credits, pick a random blank space in the middle and fill it.
  80% of merge conflicts on stb PRs are due to people adding their name at the end
  of the credits.
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// DOCUMENTATION
//
// Limitations:
//    - no 12-bit-per-channel JPEG
//    - no JPEGs with arithmetic coding
//    - GIF always returns *comp=4
//
// Basic usage (see HDR discussion below for HDR usage):
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you said 0
//    stbi_image_free(data);
//
// Standard parameters:
//    int *x                 -- outputs image width in pixels
//    int *y                 -- outputs image height in pixels
//    int *channels_in_file  -- outputs # of image components in image file
//    int desired_channels   -- if non-zero, # of image components requested in result
//
// The return value from an image loader is an 'unsigned char *' which points
// to the pixel data, or NULL on an allocation failure or if the image is
// corrupt or invalid. The pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components; the first
// pixel pointed to is top-left-most in the image. There is no padding between
// image scanlines or between pixels, regardless of format. The number of
// components N is 'desired_channels' if desired_channels is non-zero, or
// *channels_in_file otherwise. If desired_channels is non-zero,
// *channels_in_file has the number of components that _would_ have been
// output otherwise. E.g. if you set desired_channels to 4, you will always
// get RGBA output, but you can check *channels_in_file to see if it's trivially
// opaque because e.g. there were only 3 channels in the source image.
//
// An output image with N components has the following components interleaved
// in this order in each pixel:
//
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
//
// If image loading fails for any reason, the return value will be NULL,
// and *x, *y, *channels_in_file will be unchanged. The function
// stbi_failure_reason() can be queried for an extremely brief, end-user
// unfriendly explanation of why the load failed. Define STBI_NO_FAILURE_STRINGS
// to avoid compiling these strings at all, and STBI_FAILURE_USERMSG to get slightly
// more user-friendly ones.
//
// Paletted PNG, BMP, GIF, and PIC images are automatically depalettized.
//
// To query the width, height and component count of an image without having to
// decode the full file, you can use the stbi_info family of functions:
//
//   int x,y,n,ok;
//   ok = stbi_info(filename, &x, &y, &n);
//   // returns ok=1 and sets x, y, n if image is a supported format,
//   // 0 otherwise.
//
// Note that stb_image pervasively uses ints in its public API for sizes,
// including sizes of memory buffers. This is now part of the API and thus
// hard to change without causing breakage. As a result, the various image
// loaders all have certain limits on image size; these differ somewhat
// by format but generally boil down to either just under 2GB or just under
// 1GB. When the decoded image would be larger than this, stb_image decoding
// will fail.
//
// Additionally, stb_image will reject image files that have any of their
// dimensions set to a larger value than the configurable STBI_MAX_DIMENSIONS,
// which defaults to 2**24 = 16777216 pixels. Due to the above memory limit,
// the only way to have an image with such dimensions load correctly
// is for it to have a rather extreme aspect ratio. Either way, the
// assumption here is that such larger images are likely to be malformed
// or malicious. If you do need to load an image with individual dimensions
// larger than that, and it still fits in the overall size limit, you can
// #define STBI_MAX_DIMENSIONS on your own to be something larger.
//
// ===========================================================================
//
// UNICODE:
//
//   If compiling for Windows and you wish to use Unicode filenames, compile
//   with
//       #define STBI_WINDOWS_UTF8
//   and pass utf8-encoded filenames. Call stbi_convert_wchar_to_utf8 to convert
//   Windows wchar_t filenames to utf8.
//
// ===========================================================================
//
// Philosophy
//
// stb libraries are designed with the following priorities:
//
//    1. easy to use
//    2. easy to maintain
//    3. good performance
//
// Sometimes I let "good performance" creep up in priority over "easy to maintain",
// and for best performance I may provide less-easy-to-use APIs that give higher
// performance, in addition to the easy-to-use ones. Nevertheless, it's important
// to keep in mind that from the standpoint of you, a client of this library,
// all you care about is #1 and #3, and stb libraries DO NOT emphasize #3 above all.
//
// Some secondary priorities arise directly from the first two, some of which
// provide more explicit reasons why performance can't be emphasized.
//
//    - Portable ("ease of use")
//    - Small source code footprint ("easy to maintain")
//    - No dependencies ("ease of use")
//
// ===========================================================================
//
// I/O callbacks
//
// I/O callbacks allow you to read from arbitrary sources, like packaged
// files or some other source. Data read from callbacks are processed
// through a small internal buffer (currently 128 bytes) to try to reduce
// overhead.
//
// The three functions you must define are "read" (reads some bytes of data),
// "skip" (skips some bytes of data), "eof" (reports if the stream is at the end).
//
// ===========================================================================
//
// SIMD support
//
// The JPEG decoder will try to automatically use SIMD kernels on x86 when
// supported by the compiler. For ARM Neon support, you must explicitly
// request it.
//
// (The old do-it-yourself SIMD API is no longer supported in the current
// code.)
//
// On x86, SSE2 will automatically be used when available based on a run-time
// test; if not, the generic C versions are used as a fall-back. On ARM targets,
// the typical path is to have separate builds for NEON and non-NEON devices
// (at least this is true for iOS and Android). Therefore, the NEON support is
// toggled by a build flag: define STBI_NEON to get NEON loops.
//
// If for some reason you do not want to use any of SIMD code, or if
// you have issues compiling it, you can disable it entirely by
// defining STBI_NO_SIMD.
//
// ===========================================================================
//
// HDR image support   (disable by defining STBI_NO_HDR)
//
// stb_image supports loading HDR images in general, and currently the Radiance
// .HDR file format specifically. You can still load any file through the existing
// interface; if you attempt to load an HDR file, it will be automatically remapped
// to LDR, assuming gamma 2.2 and an arbitrary scale factor defaulting to 1;
// both of these constants can be reconfigured through this interface:
//
//     stbi_hdr_to_ldr_gamma(2.2f);
//     stbi_hdr_to_ldr_scale(1.0f);
//
// (note, do not use _inverse_ constants; stbi_image will invert them
// appropriately).
//
// Additionally, there is a new, parallel interface for loading files as
// (linear) floats to preserve the full dynamic range:
//
//    float *data = stbi_loadf(filename, &x, &y, &n, 0);
//
// If you load LDR images through this interface, those images will
// be promoted to floating point values, run through the inverse of
// constants corresponding to the above:
//
//     stbi_ldr_to_hdr_scale(1.0f);
//     stbi_ldr_to_hdr_gamma(2.2f);
//
// Finally, given a filename (or an open file or memory block--see header
// file for details) containing image data, you can query for the "most
// appropriate" interface to use (that is, whether the image is HDR or
// not), using:
//
//     stbi_is_hdr(char *filename);
//
// ===========================================================================
//
// iPhone PNG support:
//
// We optionally support converting iPhone-formatted PNGs (which store
// premultiplied BGRA) back to RGB, even though they're internally encoded
// differently. To enable this conversion, call
// stbi_convert_iphone_png_to_rgb(1).
//
// Call stbi_set_unpremultiply_on_load(1) as well to force a divide per
// pixel to remove any premultiplied alpha *only* if the image file explicitly
// says there's premultiplied data (currently only happens in iPhone images,
// and only if iPhone convert-to-rgb processing is on).
//
// ===========================================================================
//
// ADDITIONAL CONFIGURATION
//
//  - You can suppress implementation of any of the decoders to reduce
//    your code footprint by #defining one or more of the following
//    symbols before creating the implementation.
//
//        STBI_NO_JPEG
//        STBI_NO_PNG
//        STBI_NO_BMP
//        STBI_NO_PSD
//        STBI_NO_TGA
//        STBI_NO_GIF
//        STBI_NO_HDR
//        STBI_NO_PIC
//        STBI_NO_PNM   (.ppm and .pgm)
//
//  - You can request *only* certain decoders and suppress all other ones
//    (this will be more forward-compatible, as addition of new decoders
//    doesn't require you to disable them explicitly):
//
//        STBI_ONLY_JPEG
//        STBI_ONLY_PNG
//        STBI_ONLY_BMP
//        STBI_ONLY_PSD
//        STBI_ONLY_TGA
//        STBI_ONLY_GIF
//        STBI_ONLY_HDR
//        STBI_ONLY_PIC
//        STBI_ONLY_PNM   (.ppm and .pgm)
//
//   - If you use STBI_NO_PNG (or _ONLY_ without PNG), and you still
//     want the zlib decoder to be available, #define STBI_SUPPORT_ZLIB
//
//  - If you define STBI_MAX_DIMENSIONS, stb_image will reject images greater
//    than that size (in either width or height) without further processing.
//    This is to let programs in the wild set an upper bound to prevent
//    denial-of-service attacks on untrusted data, as one could generate a
//    valid image of gigantic dimensions and force stb_image to allocate a
//    huge block of memory and spend disproportionate time decoding it. By
//    default this is set to (1 << 24), which is 16777216, but that's still
//    very big.

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif // STBI_NO_STDIO

#define STBI_VERSION 1

enum
{
   STBI_default = 0, // only used for desired_channels

   STBI_grey       = 1,
   STBI_grey_alpha = 2,
   STBI_rgb        = 3,
   STBI_rgb_alpha  = 4
};

#include <stdlib.h>
typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STBIDEF
#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
//
// PRIMARY API - works on images of any type
//

//
// load image by filename, open file, or memory buffer
//

typedef struct
{
   int      (*read)  (void *user,char *data,int size);   // fill 'data' with 'size' bytes.  return number of bytes actually read
   void     (*skip)  (void *user,int n);                 // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
   int      (*eof)   (void *user);                       // returns nonzero if we are at end of file/data
} stbi_io_callbacks;

////////////////////////////////////
//
// 8-bits-per-channel interface
//

STBIDEF stbi_uc *stbi_load_from_memory   (stbi_uc           const *buffer, int len   , int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_uc *stbi_load_from_callbacks(stbi_io_callbacks const *clbk  , void *user, int *x, int *y, int *channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF stbi_uc *stbi_load            (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_uc *stbi_load_from_file  (FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);
// for stbi_load_from_file, file pointer is left pointing immediately after image
#endif

#ifndef STBI_NO_GIF
STBIDEF stbi_uc *stbi_load_gif_from_memory(stbi_uc const *buffer, int len, int **delays, int *x, int *y, int *z, int *comp, int req_comp);
#endif

#ifdef STBI_WINDOWS_UTF8
STBIDEF int stbi_convert_wchar_to_utf8(char *buffer, size_t bufferlen, const wchar_t* input);
#endif

////////////////////////////////////
//
// 16-bits-per-channel interface
//

STBIDEF stbi_us *stbi_load_16_from_memory   (stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_us *stbi_load_16_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF stbi_us *stbi_load_16          (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
STBIDEF stbi_us *stbi_load_from_file_16(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);
#endif

////////////////////////////////////
//
// float-per-channel interface
//
#ifndef STBI_NO_LINEAR
   STBIDEF float *stbi_loadf_from_memory     (stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
   STBIDEF float *stbi_loadf_from_callbacks  (stbi_io_callbacks const *clbk, void *user, int *x, int *y,  int *channels_in_file, int desired_channels);

   #ifndef STBI_NO_STDIO
   STBIDEF float *stbi_loadf            (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
   STBIDEF float *stbi_loadf_from_file  (FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);
   #endif
#endif

#ifndef STBI_NO_HDR
   STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma);
   STBIDEF void   stbi_hdr_to_ldr_scale(float scale);
#endif // STBI_NO_HDR

#ifndef STBI_NO_LINEAR
   STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma);
   STBIDEF void   stbi_ldr_to_hdr_scale(float scale);
#endif // STBI_NO_LINEAR

// stbi_is_hdr is always defined, but always returns false if STBI_NO_HDR
STBIDEF int    stbi_is_hdr_from_callbacks(stbi_io_callbacks const *clbk, void *user);
STBIDEF int    stbi_is_hdr_from_memory(stbi_uc const *buffer, int len);
#ifndef STBI_NO_STDIO
STBIDEF int      stbi_is_hdr          (char const *filename);
STBIDEF int      stbi_is_hdr_from_file(FILE *f);
#endif // STBI_NO_STDIO


// get a VERY brief reason for failure
// on most compilers (and ALL modern mainstream compilers) this is threadsafe
STBIDEF const char *stbi_failure_reason  (void);

// free the loaded image -- this is just free()
STBIDEF void     stbi_image_free      (void *retval_from_stbi_load);

// get image dimensions & components without fully decoding
STBIDEF int      stbi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp);
STBIDEF int      stbi_info_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp);
STBIDEF int      stbi_is_16_bit_from_memory(stbi_uc const *buffer, int len);
STBIDEF int      stbi_is_16_bit_from_callbacks(stbi_io_callbacks const *clbk, void *user);

#ifndef STBI_NO_STDIO
STBIDEF int      stbi_info               (char const *filename,     int *x, int *y, int *comp);
STBIDEF int      stbi_info_from_file     (FILE *f,                  int *x, int *y, int *comp);
STBIDEF int      stbi_is_16_bit          (char const *filename);
STBIDEF int      stbi_is_16_bit_from_file(FILE *f);
#endif



// for image formats that explicitly notate that they have premultiplied alpha,
// we just return the colors as stored in the file. set this flag to force
// unpremultiplication. results are undefined if the unpremultiply overflow.
STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

// indicate whether we should process iphone images back to canonical format,
// or just pass them through "as-is"
STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

// flip the image vertically, so the first pixel in the output array is the bottom left
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// as above, but only applies to images loaded on the thread that calls the function
// this function is only available if your compiler supports thread-local variables;
// calling it will fail to link if your compiler doesn't
STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply);
STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert);
STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip);

// ZLIB client - used by PNG, available for other purposes

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen);
STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen, int parse_header);
STBIDEF char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
STBIDEF int   stbi_zlib_decode_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);

STBIDEF char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len, int *outlen);
STBIDEF int   stbi_zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen);


#ifdef __cplusplus
}
#endif

//
//
////   end header file   /////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

#if defined(STBI_ONLY_JPEG) || defined(STBI_ONLY_PNG) || defined(STBI_ONLY_BMP) \
  || defined(STBI_ONLY_TGA) || defined(STBI_ONLY_GIF) || defined(STBI_ONLY_PSD) \
  || defined(STBI_ONLY_HDR) || defined(STBI_ONLY_PIC) || defined(STBI_ONLY_PNM) \
  || defined(STBI_ONLY_ZLIB)
   #ifndef STBI_ONLY_JPEG
   #define STBI_NO_JPEG
   #endif
   #ifndef STBI_ONLY_PNG
   #define STBI_NO_PNG
   #endif
   #ifndef STBI_ONLY_BMP
   #define STBI_NO_BMP
   #endif
   #ifndef STBI_ONLY_PSD
   #define STBI_NO_PSD
   #endif
   #ifndef STBI_ONLY_TGA
   #define STBI_NO_TGA
   #endif
   #ifndef STBI_ONLY_GIF
   #define STBI_NO_GIF
   #endif
   #ifndef STBI_ONLY_HDR
   #define STBI_NO_HDR
   #endif
   #ifndef STBI_ONLY_PIC
   #define STBI_NO_PIC
   #endif
   #ifndef STBI_ONLY_PNM
   #define STBI_NO_PNM
   #endif
#endif

#if defined(STBI_NO_PNG) && !defined(STBI_SUPPORT_ZLIB) && !defined(STBI_NO_ZLIB)
#define STBI_NO_ZLIB
#endif


#include <stdarg.h>
#include <stddef.h> // ptrdiff_t on osx
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR)
#include <math.h>  // ldexp, pow
#endif

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif

#ifndef STBI_ASSERT
#include <assert.h>
#define STBI_ASSERT(x) assert(x)
#endif

#ifdef __cplusplus
#define STBI_EXTERN extern "C"
#else
#define STBI_EXTERN extern
#endif


#ifndef _MSC_VER
   #ifdef __cplusplus
   #define stbi_inline inline
   #else
   #define stbi_inline
   #endif
#else
   #define stbi_inline __forceinline
#endif

#ifndef STBI_NO_THREAD_LOCALS
   #if defined(__cplusplus) &&  __cplusplus >= 201103L
      #define STBI_THREAD_LOCAL       thread_local
   #elif defined(__GNUC__) && __GNUC__ < 5
      #define STBI_THREAD_LOCAL       __thread
   #elif defined(_MSC_VER)
      #define STBI_THREAD_LOCAL       __declspec(thread)
   #elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
      #define STBI_THREAD_LOCAL       _Thread_local
   #endif

   #ifndef STBI_THREAD_LOCAL
      #if defined(__GNUC__)
        #define STBI_THREAD_LOCAL       __thread
      #endif
   #endif
#endif

#if defined(_MSC_VER) || defined(__SYMBIAN32__)
typedef unsigned short stbi__uint16;
typedef   signed short stbi__int16;
typedef unsigned int   stbi__uint32;
typedef   signed int   stbi__int32;
#else
#include <stdint.h>
typedef uint16_t stbi__uint16;
typedef int16_t  stbi__int16;
typedef uint32_t stbi__uint32;
typedef int32_t  stbi__int32;
#endif

// should produce compiler error if size is wrong
typedef unsigned char validate_uint32[sizeof(stbi__uint32)==4 ? 1 : -1];

#ifdef _MSC_VER
#define STBI_NOTUSED(v)  (void)(v)
#else
#define STBI_NOTUSED(v)  (void)sizeof(v)
#endif

#ifdef _MSC_VER
#define STBI_HAS_LROTL
#endif

#ifdef STBI_HAS_LROTL
   #define stbi_lrot(x,y)  _lrotl(x,y)
#else
   #define stbi_lrot(x,y)  (((x) << (y)) | ((x) >> (-(y) & 31)))
#endif

#if defined(STBI_MALLOC) && defined(STBI_FREE) && (defined(STBI_REALLOC) || defined(STBI_REALLOC_SIZED))
// ok
#elif !defined(STBI_MALLOC) && !defined(STBI_FREE) && !defined(STBI_REALLOC) && !defined(STBI_REALLOC_SIZED)
// ok
#else
#error "Must define all or none of STBI_MALLOC, STBI_FREE, and STBI_REALLOC (or STBI_REALLOC_SIZED)."
#endif

#ifndef STBI_MALLOC
#define STBI_MALLOC(sz)           malloc(sz)
#define STBI_REALLOC(p,newsz)     realloc(p,newsz)
#define STBI_FREE(p)              free(p)
#endif

#ifndef STBI_REALLOC_SIZED
#define STBI_REALLOC_SIZED(p,oldsz,newsz) STBI_REALLOC(p,newsz)
#endif

// x86/x64 detection
#if defined(__x86_64__) || defined(_M_X64)
#define STBI__X64_TARGET
#elif defined(__i386) || defined(_M_IX86)
#define STBI__X86_TARGET
#endif

#if defined(__GNUC__) && defined(STBI__X86_TARGET) && !defined(__SSE2__) && !defined(STBI_NO_SIMD)
// gcc doesn't support sse2 intrinsics unless you compile with -msse2,
// which in turn means it gets to use SSE2 everywhere. This is unfortunate,
// but previous attempts to provide the SSE2 functions with runtime
// detection caused numerous issues. The way architecture extensions are
// exposed in GCC/Clang is, sadly, not really suited for one-file libs.
// New behavior: if compiled with -msse2, we use SSE2 without any
// detection; if not, we don't use it at all.
#define STBI_NO_SIMD
#endif

#if defined(__MINGW32__) && defined(STBI__X86_TARGET) && !defined(STBI_MINGW_ENABLE_SSE2) && !defined(STBI_NO_SIMD)
// Note that __MINGW32__ doesn't actually mean 32-bit, so we have to avoid STBI__X64_TARGET
//
// 32-bit MinGW wants ESP to be 16-byte aligned, but this is not in the
// Windows ABI and VC++ as well as Windows DLLs don't maintain that invariant.
// As a result, enabling SSE2 on 32-bit MinGW is dangerous when not
// simultaneously enabling "-mstackrealign".
//
// See https://github.com/nothings/stb/issues/81 for more information.
//
// So default to no SSE2 on 32-bit MinGW. If you've read this far and added
// -mstackrealign to your build settings, feel free to #define STBI_MINGW_ENABLE_SSE2.
#define STBI_NO_SIMD
#endif

#if !defined(STBI_NO_SIMD) && (defined(STBI__X86_TARGET) || defined(STBI__X64_TARGET))
#define STBI_SSE2
#include <emmintrin.h>

#ifdef _MSC_VER

#if _MSC_VER >= 1400  // not VC6
#include <intrin.h> // __cpuid
static int stbi__cpuid3(void)
{
   int info[4];
   __cpuid(info,1);
   return info[3];
}
#else
static int stbi__cpuid3(void)
{
   int res;
   __asm {
      mov  eax,1
      cpuid
      mov  res,edx
   }
   return res;
}
#endif

#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name

#if !defined(STBI_NO_JPEG) && defined(STBI_SSE2)
static int stbi__sse2_available(void)
{
   int info3 = stbi__cpuid3();
   return ((info3 >> 26) & 1) != 0;
}
#endif

#else // assume GCC-style if not VC++
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))

#if !defined(STBI_NO_JPEG) && defined(STBI_SSE2)
static int stbi__sse2_available(void)
{
   // If we're even attempting to compile this on GCC/Clang, that means
   // -msse2 is on, which means the compiler is allowed to use SSE2
   // instructions at will, and so are we.
   return 1;
}
#endif

#endif
#endif

// ARM NEON
#if defined(STBI_NO_SIMD) && defined(STBI_NEON)
#undef STBI_NEON
#endif

#ifdef STBI_NEON
#include <arm_neon.h>
#ifdef _MSC_VER
#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name
#else
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))
#endif
#endif

#ifndef STBI_SIMD_ALIGN
#define STBI_SIMD_ALIGN(type, name) type name
#endif

#ifndef STBI_MAX_DIMENSIONS
#define STBI_MAX_DIMENSIONS (1 << 24)
#endif

///////////////////////////////////////////////
//
//  stbi__context struct and start_xxx functions

// stbi__context structure is our basic context used by all images, so it
// contains all the IO context, plus some basic image information
typedef struct
{
   stbi__uint32 img_x, img_y;
   int img_n, img_out_n;

   stbi_io_callbacks io;
   void *io_user_data;

   int read_from_callbacks;
   int buflen;
   stbi_uc buffer_start[128];
   int callback_already_read;

   stbi_uc *img_buffer, *img_buffer_end;
   stbi_uc *img_buffer_original, *img_buffer_original_end;
} stbi__context;


static void stbi__refill_buffer(stbi__context *s);

// initialize a memory-decode context
static void stbi__start_mem(stbi__context *s, stbi_uc const *buffer, int len)
{
   s->io.read = NULL;
   s->read_from_callbacks = 0;
   s->callback_already_read = 0;
   s->img_buffer = s->img_buffer_original = (stbi_uc *) buffer;
   s->img_buffer_end = s->img_buffer_original_end = (stbi_uc *) buffer+len;
}

// initialize a callback-based context
static void stbi__start_callbacks(stbi__context *s, stbi_io_callbacks *c, void *user)
{
   s->io = *c;
   s->io_user_data = user;
   s->buflen = sizeof(s->buffer_start);
   s->read_from_callbacks = 1;
   s->callback_already_read = 0;
   s->img_buffer = s->img_buffer_original = s->buffer_start;
   stbi__refill_buffer(s);
   s->img_buffer_original_end = s->img_buffer_end;
}

#ifndef STBI_NO_STDIO

static int stbi__stdio_read(void *user, char *data, int size)
{
   return (int) fread(data,1,size,(FILE*) user);
}

static void stbi__stdio_skip(void *user, int n)
{
   int ch;
   fseek((FILE*) user, n, SEEK_CUR);
   ch = fgetc((FILE*) user);  /* have to read a byte to reset feof()'s flag */
   if (ch != EOF) {
      ungetc(ch, (FILE *) user);  /* push byte back onto stream if valid. */
   }
}

static int stbi__stdio_eof(void *user)
{
   return feof((FILE*) user) || ferror((FILE *) user);
}

static stbi_io_callbacks stbi__stdio_callbacks =
{
   stbi__stdio_read,
   stbi__stdio_skip,
   stbi__stdio_eof,
};

static void stbi__start_file(stbi__context *s, FILE *f)
{
   stbi__start_callbacks(s, &stbi__stdio_callbacks, (void *) f);
}

//static void stop_file(stbi__context *s) { }

#endif // !STBI_NO_STDIO

static void stbi__rewind(stbi__context *s)
{
   // conceptually rewind SHOULD rewind to the beginning of the stream,
   // but we just rewind to the beginning of the initial buffer, because
   // we only use it after doing 'test', which only ever looks at at most 92 bytes
   s->img_buffer = s->img_buffer_original;
   s->img_buffer_end = s->img_buffer_original_end;
}

enum
{
   STBI_ORDER_RGB,
   STBI_ORDER_BGR
};

typedef struct
{
   int bits_per_channel;
   int num_channels;
   int channel_order;
} stbi__result_info;

#ifndef STBI_NO_JPEG
static int      stbi__jpeg_test(stbi__context *s);
static void    *stbi__jpeg_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__jpeg_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PNG
static int      stbi__png_test(stbi__context *s);
static void    *stbi__png_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__png_info(stbi__context *s, int *x, int *y, int *comp);
static int      stbi__png_is16(stbi__context *s);
#endif

#ifndef STBI_NO_BMP
static int      stbi__bmp_test(stbi__context *s);
static void    *stbi__bmp_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__bmp_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_TGA
static int      stbi__tga_test(stbi__context *s);
static void    *stbi__tga_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__tga_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PSD
static int      stbi__psd_test(stbi__context *s);
static void    *stbi__psd_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri, int bpc);
static int      stbi__psd_info(stbi__context *s, int *x, int *y, int *comp);
static int      stbi__psd_is16(stbi__context *s);
#endif

#ifndef STBI_NO_HDR
static int      stbi__hdr_test(stbi__context *s);
static float   *stbi__hdr_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__hdr_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PIC
static int      stbi__pic_test(stbi__context *s);
static void    *stbi__pic_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__pic_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_GIF
static int      stbi__gif_test(stbi__context *s);
static void    *stbi__gif_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static void    *stbi__load_gif_main(stbi__context *s, int **delays, int *x, int *y, int *z, int *comp, int req_comp);
static int      stbi__gif_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PNM
static int      stbi__pnm_test(stbi__context *s);
static void    *stbi__pnm_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri);
static int      stbi__pnm_info(stbi__context *s, int *x, int *y, int *comp);
static int      stbi__pnm_is16(stbi__context *s);
#endif

static
#ifdef STBI_THREAD_LOCAL
STBI_THREAD_LOCAL
#endif
const char *stbi__g_failure_reason;

STBIDEF const char *stbi_failure_reason(void)
{
   return stbi__g_failure_reason;
}

#ifndef STBI_NO_FAILURE_STRINGS
static int stbi__err(const char *str)
{
   stbi__g_failure_reason = str;
   return 0;
}
#endif

static void *stbi__malloc(size_t size)
{
    return STBI_MALLOC(size);
}

// stb_image uses ints pervasively, including for offset calculations.
// therefore the largest decoded image size we can support with the
// current code, even on 64-bit targets, is INT_MAX. this is not a
// significant limitation for the intended use case.
//
// we do, however, need to make sure our size calculations don't
// overflow. hence a few helper functions for size calculations that
// multiply integers together, making sure that they're non-negative
// and no overflow occurs.

// return 1 if the sum is valid, 0 on overflow.
// negative terms are considered invalid.
static int stbi__addsizes_valid(int a, int b)
{
   if (b < 0) return 0;
   // now 0 <= b <= INT_MAX, hence also
   // 0 <= INT_MAX - b <= INTMAX.
   // And "a + b <= INT_MAX" (which might overflow) is the
   // same as a <= INT_MAX - b (no overflow)
   return a <= INT_MAX - b;
}

// returns 1 if the product is valid, 0 on overflow.
// negative factors are considered invalid.
static int stbi__mul2sizes_valid(int a, int b)
{
   if (a < 0 || b < 0) return 0;
   if (b == 0) return 1; // mul-by-0 is always safe
   // portable way to check for no overflows in a*b
   return a <= INT_MAX/b;
}

#if !defined(STBI_NO_JPEG) || !defined(STBI_NO_PNG) || !defined(STBI_NO_TGA) || !defined(STBI_NO_HDR)
// returns 1 if "a*b + add" has no negative terms/factors and doesn't overflow
static int stbi__mad2sizes_valid(int a, int b, int add)
{
   return stbi__mul2sizes_valid(a, b) && stbi__addsizes_valid(a*b, add);
}
#endif

// returns 1 if "a*b*c + add" has no negative terms/factors and doesn't overflow
static int stbi__mad3sizes_valid(int a, int b, int c, int add)
{
   return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a*b, c) &&
      stbi__addsizes_valid(a*b*c, add);
}

// returns 1 if "a*b*c*d + add" has no negative terms/factors and doesn't overflow
#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR) || !defined(STBI_NO_PNM)
static int stbi__mad4sizes_valid(int a, int b, int c, int d, int add)
{
   return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a*b, c) &&
      stbi__mul2sizes_valid(a*b*c, d) && stbi__addsizes_valid(a*b*c*d, add);
}
#endif

#if !defined(STBI_NO_JPEG) || !defined(STBI_NO_PNG) || !defined(STBI_NO_TGA) || !defined(STBI_NO_HDR)
// mallocs with size overflow checking
static void *stbi__malloc_mad2(int a, int b, int add)
{
   if (!stbi__mad2sizes_valid(a, b, add)) return NULL;
   return stbi__malloc(a*b + add);
}
#endif

static void *stbi__malloc_mad3(int a, int b, int c, int add)
{
   if (!stbi__mad3sizes_valid(a, b, c, add)) return NULL;
   return stbi__malloc(a*b*c + add);
}

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR) || !defined(STBI_NO_PNM)
static void *stbi__malloc_mad4(int a, int b, int c, int d, int add)
{
   if (!stbi__mad4sizes_valid(a, b, c, d, add)) return NULL;
   return stbi__malloc(a*b*c*d + add);
}
#endif

// returns 1 if the sum of two signed ints is valid (between -2^31 and 2^31-1 inclusive), 0 on overflow.
static int stbi__addints_valid(int a, int b)
{
   if ((a >= 0) != (b >= 0)) return 1; // a and b have different signs, so no overflow
   if (a < 0 && b < 0) return a >= INT_MIN - b; // same as a + b >= INT_MIN; INT_MIN - b cannot overflow since b < 0.
   return a <= INT_MAX - b;
}

// returns 1 if the product of two signed shorts is valid, 0 on overflow.
static int stbi__mul2shorts_valid(short a, short b)
{
   if (b == 0 || b == -1) return 1; // multiplication by 0 is always 0; check for -1 so SHRT_MIN/b doesn't overflow
   if ((a >= 0) == (b >= 0)) return a <= SHRT_MAX/b; // product is positive, so similar to mul2sizes_valid
   if (b < 0) return a <= SHRT_MIN / b; // same as a * b >= SHRT_MIN
   return a >= SHRT_MIN / b;
}

// stbi__err - error
// stbi__errpf - error returning pointer to float
// stbi__errpuc - error returning pointer to unsigned char

#ifdef STBI_NO_FAILURE_STRINGS
   #define stbi__err(x,y)  0
#elif defined(STBI_FAILURE_USERMSG)
   #define stbi__err(x,y)  stbi__err(y)
#else
   #define stbi__err(x,y)  stbi__err(x)
#endif

#define stbi__errpf(x,y)   ((float *)(size_t) (stbi__err(x,y)?NULL:NULL))
#define stbi__errpuc(x,y)  ((unsigned char *)(size_t) (stbi__err(x,y)?NULL:NULL))

STBIDEF void stbi_image_free(void *retval_from_stbi_load)
{
   STBI_FREE(retval_from_stbi_load);
}

#ifndef STBI_NO_LINEAR
static float   *stbi__ldr_to_hdr(stbi_uc *data, int x, int y, int comp);
#endif

#ifndef STBI_NO_HDR
static stbi_uc *stbi__hdr_to_ldr(float   *data, int x, int y, int comp);
#endif

static int stbi__vertically_flip_on_load_global = 0;

STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip)
{
   stbi__vertically_flip_on_load_global = flag_true_if_should_flip;
}

#ifndef STBI_THREAD_LOCAL
#define stbi__vertically_flip_on_load  stbi__vertically_flip_on_load_global
#else
static STBI_THREAD_LOCAL int stbi__vertically_flip_on_load_local, stbi__vertically_flip_on_load_set;

STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip)
{
   stbi__vertically_flip_on_load_local = flag_true_if_should_flip;
   stbi__vertically_flip_on_load_set = 1;
}

#define stbi__vertically_flip_on_load  (stbi__vertically_flip_on_load_set       \
                                         ? stbi__vertically_flip_on_load_local  \
                                         : stbi__vertically_flip_on_load_global)
#endif // STBI_THREAD_LOCAL

static void *stbi__load_main(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri, int bpc)
{
   memset(ri, 0, sizeof(*ri)); // make sure it's initialized if we add new fields
   ri->bits_per_channel = 8; // default is 8 so most paths don't have to be changed
   ri->channel_order = STBI_ORDER_RGB; // all current input & output are this, but this is here so we can add BGR order
   ri->num_channels = 0;

   // test the formats with a very explicit header first (at least a FOURCC
   // or distinctive magic number first)
   #ifndef STBI_NO_PNG
   if (stbi__png_test(s))  return stbi__png_load(s,x,y,comp,req_comp, ri);
   #endif
   #ifndef STBI_NO_BMP
   if (stbi__bmp_test(s))  return stbi__bmp_load(s,x,y,comp,req_comp, ri);
   #endif
   #ifndef STBI_NO_GIF
   if (stbi__gif_test(s))  return stbi__gif_load(s,x,y,comp,req_comp, ri);
   #endif
   #ifndef STBI_NO_PSD
   if (stbi__psd_test(s))  return stbi__psd_load(s,x,y,comp,req_comp, ri, bpc);
   #else
   STBI_NOTUSED(bpc);
   #endif
   #ifndef STBI_NO_PIC
   if (stbi__pic_test(s))  return stbi__pic_load(s,x,y,comp,req_comp, ri);
   #endif

   // then the formats that can end up attempting to load with just 1 or 2
   // bytes matching expectations; these are prone to false positives, so
   // try them later
   #ifndef STBI_NO_JPEG
   if (stbi__jpeg_test(s)) return stbi__jpeg_load(s,x,y,comp,req_comp, ri);
   #endif
   #ifndef STBI_NO_PNM
   if (stbi__pnm_test(s))  return stbi__pnm_load(s,x,y,comp,req_comp, ri);
   #endif

   #ifndef STBI_NO_HDR
   if (stbi__hdr_test(s)) {
      float *hdr = stbi__hdr_load(s, x,y,comp,req_comp, ri);
      return stbi__hdr_to_ldr(hdr, *x, *y, req_comp ? req_comp : *comp);
   }
   #endif

   #ifndef STBI_NO_TGA
   // test tga last because it's a crappy test!
   if (stbi__tga_test(s))
      return stbi__tga_load(s,x,y,comp,req_comp, ri);
   #endif

   return stbi__errpuc("unknown image type", "Image not of any known type, or corrupt");
}

static stbi_uc *stbi__convert_16_to_8(stbi__uint16 *orig, int w, int h, int channels)
{
   int i;
   int img_len = w * h * channels;
   stbi_uc *reduced;

   reduced = (stbi_uc *) stbi__malloc(img_len);
   if (reduced == NULL) return stbi__errpuc("outofmem", "Out of memory");

   for (i = 0; i < img_len; ++i)
      reduced[i] = (stbi_uc)((orig[i] >> 8) & 0xFF); // top half of each byte is sufficient approx of 16->8 bit scaling

   STBI_FREE(orig);
   return reduced;
}

static stbi__uint16 *stbi__convert_8_to_16(stbi_uc *orig, int w, int h, int channels)
{
   int i;
   int img_len = w * h * channels;
   stbi__uint16 *enlarged;

   enlarged = (stbi__uint16 *) stbi__malloc(img_len*2);
   if (enlarged == NULL) return (stbi__uint16 *) stbi__errpuc("outofmem", "Out of memory");

   for (i = 0; i < img_len; ++i)
      enlarged[i] = (stbi__uint16)((orig[i] << 8) + orig[i]); // replicate to high and low byte, maps 0->0, 255->0xffff

   STBI_FREE(orig);
   return enlarged;
}

static void stbi__vertical_flip(void *image, int w, int h, int bytes_per_pixel)
{
   int row;
   size_t bytes_per_row = (size_t)w * bytes_per_pixel;
   stbi_uc temp[2048];
   stbi_uc *bytes = (stbi_uc *)image;

   for (row = 0; row < (h>>1); row++) {
      stbi_uc *row0 = bytes + row*bytes_per_row;
      stbi_uc *row1 = bytes + (h - row - 1)*bytes_per_row;
      // swap row0 with row1
      size_t bytes_left = bytes_per_row;
      while (bytes_left) {
         size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
         memcpy(temp, row0, bytes_copy);
         memcpy(row0, row1, bytes_copy);
         memcpy(row1, temp, bytes_copy);
         row0 += bytes_copy;
         row1 += bytes_copy;
         bytes_left -= bytes_copy;
      }
   }
}

#ifndef STBI_NO_GIF
static void stbi__vertical_flip_slices(void *image, int w, int h, int z, int bytes_per_pixel)
{
   int slice;
   int slice_size = w * h * bytes_per_pixel;

   stbi_uc *bytes = (stbi_uc *)image;
   for (slice = 0; slice < z; ++slice) {
      stbi__vertical_flip(bytes, w, h, bytes_per_pixel);
      bytes += slice_size;
   }
}
#endif

static unsigned char *stbi__load_and_postprocess_8bit(stbi__context *s, int *x, int *y, int *comp, int req_comp)
{
   stbi__result_info ri;
   void *result = stbi__load_main(s, x, y, comp, req_comp, &ri, 8);

   if (result == NULL)
      return NULL;

   // it is the responsibility of the loaders to make sure we get either 8 or 16 bit.
   STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

   if (ri.bits_per_channel != 8) {
      result = stbi__convert_16_to_8((stbi__uint16 *) result, *x, *y, req_comp == 0 ? *comp : req_comp);
      ri.bits_per_channel = 8;
   }

   // @TODO: move stbi__convert_format to here

   if (stbi__vertically_flip_on_load) {
      int channels = req_comp ? req_comp : *comp;
      stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi_uc));
   }

   return (unsigned char *) result;
}

static stbi__uint16 *stbi__load_and_postprocess_16bit(stbi__context *s, int *x, int *y, int *comp, int req_comp)
{
   stbi__result_info ri;
   void *result = stbi__load_main(s, x, y, comp, req_comp, &ri, 16);

   if (result == NULL)
      return NULL;

   // it is the responsibility of the loaders to make sure we get either 8 or 16 bit.
   STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

   if (ri.bits_per_channel != 16) {
      result = stbi__convert_8_to_16((stbi_uc *) result, *x, *y, req_comp == 0 ? *comp : req_comp);
      ri.bits_per_channel = 16;
   }

   // @TODO: move stbi__convert_format16 to here
   // @TODO: special case RGB-to-Y (and RGBA-to-YA) for 8-bit-to-16-bit case to keep more precision

   if (stbi__vertically_flip_on_load) {
      int channels = req_comp ? req_comp : *comp;
      stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi__uint16));
   }

   return (stbi__uint16 *) result;
}

#if !defined(STBI_NO_HDR) && !defined(STBI_NO_LINEAR)
static void stbi__float_postprocess(float *result, int *x, int *y, int *comp, int req_comp)
{
   if (stbi__vertically_flip_on_load && result != NULL) {
      int channels = req_comp ? req_comp : *comp;
      stbi__vertical_flip(result, *x, *y, channels * sizeof(float));
   }
}
#endif

#ifndef STBI_NO_STDIO

#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
STBI_EXTERN __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int cp, unsigned long flags, const char *str, int cbmb, wchar_t *widestr, int cchwide);
STBI_EXTERN __declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int cp, unsigned long flags, const wchar_t *widestr, int cchwide, char *str, int cbmb, const char *defchar, int *used_default);
#endif

#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
STBIDEF int stbi_convert_wchar_to_utf8(char *buffer, size_t bufferlen, const wchar_t* input)
{
	return WideCharToMultiByte(65001 /* UTF8 */, 0, input, -1, buffer, (int) bufferlen, NULL, NULL);
}
#endif

static FILE *stbi__fopen(char const *filename, char const *mode)
{
   FILE *f;
#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
   wchar_t wMode[64];
   wchar_t wFilename[1024];
	if (0 == MultiByteToWideChar(65001 /* UTF8 */, 0, filename, -1, wFilename, sizeof(wFilename)/sizeof(*wFilename)))
      return 0;

	if (0 == MultiByteToWideChar(65001 /* UTF8 */, 0, mode, -1, wMode, sizeof(wMode)/sizeof(*wMode)))
      return 0;

#if defined(_MSC_VER) && _MSC_VER >= 1400
	if (0 != _wfopen_s(&f, wFilename, wMode))
		f = 0;
#else
   f = _wfopen(wFilename, wMode);
#endif

#elif defined(_MSC_VER) && _MSC_VER >= 1400
   if (0 != fopen_s(&f, filename, mode))
      f=0;
#else
   f = fopen(filename, mode);
#endif
   return f;
}


STBIDEF stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)
{
   FILE *f = stbi__fopen(filename, "rb");
   unsigned char *result;
   if (!f) return stbi__errpuc("can't fopen", "Unable to open file");
   result = stbi_load_from_file(f,x,y,comp,req_comp);
   fclose(f);
   return result;
}

STBIDEF stbi_uc *stbi_load_from_file(FILE *f, int *x, int *y, int *comp, int req_comp)
{
   unsigned char *result;
   stbi__context s;
   stbi__start_file(&s,f);
   result = stbi__load_and_postprocess_8bit(&s,x,y,comp,req_comp);
   if (result) {
      // need to 'unget' all the characters in the IO buffer
      fseek(f, - (int) (s.img_buffer_end - s.img_buffer), SEEK_CUR);
   }
   return result;
}

STBIDEF stbi__uint16 *stbi_load_from_file_16(FILE *f, int *x, int *y, int *comp, int req_comp)
{
   stbi__uint16 *result;
   stbi__context s;
   stbi__start_file(&s,f);
   result = stbi__load_and_postprocess_16bit(&s,x,y,comp,req_comp);
   if (result) {
      // need to 'unget' all the characters in the IO buffer
      fseek(f, - (int) (s.img_buffer_end - s.img_buffer), SEEK_CUR);
   }
   return result;
}

STBIDEF stbi_us *stbi_load_16(char const *filename, int *x, int *y, int *comp, int req_comp)
{
   FILE *f = stbi__fopen(filename, "rb");
   stbi__uint16 *result;
   if (!f) return (stbi_us *) stbi__errpuc("can't fopen", "Unable to open file");
   result = stbi_load_from_file_16(f,x,y,comp,req_comp);
   fclose(f);
   return result;
}


#endif //!STBI_NO_STDIO

STBIDEF stbi_us *stbi_load_16_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__load_and_postprocess_16bit(&s,x,y,channels_in_file,desired_channels);
}

STBIDEF stbi_us *stbi_load_16_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *channels_in_file, int desired_channels)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
   return stbi__load_and_postprocess_16bit(&s,x,y,channels_in_file,desired_channels);
}

STBIDEF stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__load_and_postprocess_8bit(&s,x,y,comp,req_comp);
}

STBIDEF stbi_uc *stbi_load_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__load_and_postprocess_8bit(&s,x,y,comp,req_comp);
}

#ifndef STBI_NO_GIF
STBIDEF stbi_uc *stbi_load_gif_from_memory(stbi_uc const *buffer, int len, int **delays, int *x, int *y, int *z, int *comp, int req_comp)
{
   unsigned char *result;
   stbi__context s;
   stbi__start_mem(&s,buffer,len);

   result = (unsigned char*) stbi__load_gif_main(&s, delays, x, y, z, comp, req_comp);
   if (stbi__vertically_flip_on_load) {
      stbi__vertical_flip_slices( result, *x, *y, *z, *comp );
   }

   return result;
}
#endif

#ifndef STBI_NO_LINEAR
static float *stbi__loadf_main(stbi__context *s, int *x, int *y, int *comp, int req_comp)
{
   unsigned char *data;
   #ifndef STBI_NO_HDR
   if (stbi__hdr_test(s)) {
      stbi__result_info ri;
      float *hdr_data = stbi__hdr_load(s,x,y,comp,req_comp, &ri);
      if (hdr_data)
         stbi__float_postprocess(hdr_data,x,y,comp,req_comp);
      return hdr_data;
   }
   #endif
   data = stbi__load_and_postprocess_8bit(s, x, y, comp, req_comp);
   if (data)
      return stbi__ldr_to_hdr(data, *x, *y, req_comp ? req_comp : *comp);
   return stbi__errpf("unknown image type", "Image not of any known type, or corrupt");
}

STBIDEF float *stbi_loadf_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__loadf_main(&s,x,y,comp,req_comp);
}

STBIDEF float *stbi_loadf_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__loadf_main(&s,x,y,comp,req_comp);
}

#ifndef STBI_NO_STDIO
STBIDEF float *stbi_loadf(char const *filename, int *x, int *y, int *comp, int req_comp)
{
   float *result;
   FILE *f = stbi__fopen(filename, "rb");
   if (!f) return stbi__errpf("can't fopen", "Unable to open file");
   result = stbi_loadf_from_file(f,x,y,comp,req_comp);
   fclose(f);
   return result;
}

STBIDEF float *stbi_loadf_from_file(FILE *f, int *x, int *y, int *comp, int req_comp)
{
   stbi__context s;
   stbi__start_file(&s,f);
   return stbi__loadf_main(&s,x,y,comp,req_comp);
}
#endif // !STBI_NO_STDIO

#endif // !STBI_NO_LINEAR

// these is-hdr-or-not is defined independent of whether STBI_NO_LINEAR is
// defined, for API simplicity; if STBI_NO_LINEAR is defined, it always
// reports false!

STBIDEF int stbi_is_hdr_from_memory(stbi_uc const *buffer, int len)
{
   #ifndef STBI_NO_HDR
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__hdr_test(&s);
   #else
   STBI_NOTUSED(buffer);
   STBI_NOTUSED(len);
   return 0;
   #endif
}

#ifndef STBI_NO_STDIO
STBIDEF int      stbi_is_hdr          (char const *filename)
{
   FILE *f = stbi__fopen(filename, "rb");
   int result=0;
   if (f) {
      result = stbi_is_hdr_from_file(f);
      fclose(f);
   }
   return result;
}

STBIDEF int stbi_is_hdr_from_file(FILE *f)
{
   #ifndef STBI_NO_HDR
   long pos = ftell(f);
   int res;
   stbi__context s;
   stbi__start_file(&s,f);
   res = stbi__hdr_test(&s);
   fseek(f, pos, SEEK_SET);
   return res;
   #else
   STBI_NOTUSED(f);
   return 0;
   #endif
}
#endif // !STBI_NO_STDIO

STBIDEF int      stbi_is_hdr_from_callbacks(stbi_io_callbacks const *clbk, void *user)
{
   #ifndef STBI_NO_HDR
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__hdr_test(&s);
   #else
   STBI_NOTUSED(clbk);
   STBI_NOTUSED(user);
   return 0;
   #endif
}

#ifndef STBI_NO_LINEAR
static float stbi__l2h_gamma=2.2f, stbi__l2h_scale=1.0f;

STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma) { stbi__l2h_gamma = gamma; }
STBIDEF void   stbi_ldr_to_hdr_scale(float scale) { stbi__l2h_scale = scale; }
#endif

static float stbi__h2l_gamma_i=1.0f/2.2f, stbi__h2l_scale_i=1.0f;

STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma) { stbi__h2l_gamma_i = 1/gamma; }
STBIDEF void   stbi_hdr_to_ldr_scale(float scale) { stbi__h2l_scale_i = 1/scale; }


//////////////////////////////////////////////////////////////////////////////
//
// Common code used by all image loaders
//

enum
{
   STBI__SCAN_load=0,
   STBI__SCAN_type,
   STBI__SCAN_header
};

static void stbi__refill_buffer(stbi__context *s)
{
   int n = (s->io.read)(s->io_user_data,(char*)s->buffer_start,s->buflen);
   s->callback_already_read += (int) (s->img_buffer - s->img_buffer_original);
   if (n == 0) {
      // at end of file, treat same as if from memory, but need to handle case
      // where s->img_buffer isn't pointing to safe memory, e.g. 0-byte file
      s->read_from_callbacks = 0;
      s->img_buffer = s->buffer_start;
      s->img_buffer_end = s->buffer_start+1;
      *s->img_buffer = 0;
   } else {
      s->img_buffer = s->buffer_start;
      s->img_buffer_end = s->buffer_start + n;
   }
}

stbi_inline static stbi_uc stbi__get8(stbi__context *s)
{
   if (s->img_buffer < s->img_buffer_end)
      return *s->img_buffer++;
   if (s->read_from_callbacks) {
      stbi__refill_buffer(s);
      return *s->img_buffer++;
   }
   return 0;
}

#if defined(STBI_NO_JPEG) && defined(STBI_NO_HDR) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
stbi_inline static int stbi__at_eof(stbi__context *s)
{
   if (s->io.read) {
      if (!(s->io.eof)(s->io_user_data)) return 0;
      // if feof() is true, check if buffer = end
      // special case: we've only got the special 0 character at the end
      if (s->read_from_callbacks == 0) return 1;
   }

   return s->img_buffer >= s->img_buffer_end;
}
#endif

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC)
// nothing
#else
static void stbi__skip(stbi__context *s, int n)
{
   if (n == 0) return;  // already there!
   if (n < 0) {
      s->img_buffer = s->img_buffer_end;
      return;
   }
   if (s->io.read) {
      int blen = (int) (s->img_buffer_end - s->img_buffer);
      if (blen < n) {
         s->img_buffer = s->img_buffer_end;
         (s->io.skip)(s->io_user_data, n - blen);
         return;
      }
   }
   s->img_buffer += n;
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_TGA) && defined(STBI_NO_HDR) && defined(STBI_NO_PNM)
// nothing
#else
static int stbi__getn(stbi__context *s, stbi_uc *buffer, int n)
{
   if (s->io.read) {
      int blen = (int) (s->img_buffer_end - s->img_buffer);
      if (blen < n) {
         int res, count;

         memcpy(buffer, s->img_buffer, blen);

         count = (s->io.read)(s->io_user_data, (char*) buffer + blen, n - blen);
         res = (count == (n-blen));
         s->img_buffer = s->img_buffer_end;
         return res;
      }
   }

   if (s->img_buffer+n <= s->img_buffer_end) {
      memcpy(buffer, s->img_buffer, n);
      s->img_buffer += n;
      return 1;
   } else
      return 0;
}
#endif

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_PSD) && defined(STBI_NO_PIC)
// nothing
#else
static int stbi__get16be(stbi__context *s)
{
   int z = stbi__get8(s);
   return (z << 8) + stbi__get8(s);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD) && defined(STBI_NO_PIC)
// nothing
#else
static stbi__uint32 stbi__get32be(stbi__context *s)
{
   stbi__uint32 z = stbi__get16be(s);
   return (z << 16) + stbi__get16be(s);
}
#endif

#if defined(STBI_NO_BMP) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF)
// nothing
#else
static int stbi__get16le(stbi__context *s)
{
   int z = stbi__get8(s);
   return z + (stbi__get8(s) << 8);
}
#endif

#ifndef STBI_NO_BMP
static stbi__uint32 stbi__get32le(stbi__context *s)
{
   stbi__uint32 z = stbi__get16le(s);
   z += (stbi__uint32)stbi__get16le(s) << 16;
   return z;
}
#endif

#define STBI__BYTECAST(x)  ((stbi_uc) ((x) & 255))  // truncate int to byte without warnings

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
//////////////////////////////////////////////////////////////////////////////
//
//  generic converter from built-in img_n to req_comp
//    individual types do this automatically as much as possible (e.g. jpeg
//    does all cases internally since it needs to colorspace convert anyway,
//    and it never has alpha, so very few cases ). png can automatically
//    interleave an alpha=255 channel, but falls back to this for other cases
//
//  assume data buffer is malloced, so malloc a new one and free that one
//  only failure mode is malloc failing

static stbi_uc stbi__compute_y(int r, int g, int b)
{
   return (stbi_uc) (((r*77) + (g*150) +  (29*b)) >> 8);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
static unsigned char *stbi__convert_format(unsigned char *data, int img_n, int req_comp, unsigned int x, unsigned int y)
{
   int i,j;
   unsigned char *good;

   if (req_comp == img_n) return data;
   STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

   good = (unsigned char *) stbi__malloc_mad3(req_comp, x, y, 0);
   if (good == NULL) {
      STBI_FREE(data);
      return stbi__errpuc("outofmem", "Out of memory");
   }

   for (j=0; j < (int) y; ++j) {
      unsigned char *src  = data + j * x * img_n   ;
      unsigned char *dest = good + j * x * req_comp;

      #define STBI__COMBO(a,b)  ((a)*8+(b))
      #define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
      // convert source image with img_n components to one with req_comp components;
      // avoid switch per pixel, so use switch per scanline and massive macros
      switch (STBI__COMBO(img_n, req_comp)) {
         STBI__CASE(1,2) { dest[0]=src[0]; dest[1]=255;                                     } break;
         STBI__CASE(1,3) { dest[0]=dest[1]=dest[2]=src[0];                                  } break;
         STBI__CASE(1,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=255;                     } break;
         STBI__CASE(2,1) { dest[0]=src[0];                                                  } break;
         STBI__CASE(2,3) { dest[0]=dest[1]=dest[2]=src[0];                                  } break;
         STBI__CASE(2,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=src[1];                  } break;
         STBI__CASE(3,4) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];dest[3]=255;        } break;
         STBI__CASE(3,1) { dest[0]=stbi__compute_y(src[0],src[1],src[2]);                   } break;
         STBI__CASE(3,2) { dest[0]=stbi__compute_y(src[0],src[1],src[2]); dest[1] = 255;    } break;
         STBI__CASE(4,1) { dest[0]=stbi__compute_y(src[0],src[1],src[2]);                   } break;
         STBI__CASE(4,2) { dest[0]=stbi__compute_y(src[0],src[1],src[2]); dest[1] = src[3]; } break;
         STBI__CASE(4,3) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];                    } break;
         default: STBI_ASSERT(0); STBI_FREE(data); STBI_FREE(good); return stbi__errpuc("unsupported", "Unsupported format conversion");
      }
      #undef STBI__CASE
   }

   STBI_FREE(data);
   return good;
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD)
// nothing
#else
static stbi__uint16 stbi__compute_y_16(int r, int g, int b)
{
   return (stbi__uint16) (((r*77) + (g*150) +  (29*b)) >> 8);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD)
// nothing
#else
static stbi__uint16 *stbi__convert_format16(stbi__uint16 *data, int img_n, int req_comp, unsigned int x, unsigned int y)
{
   int i,j;
   stbi__uint16 *good;

   if (req_comp == img_n) return data;
   STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

   good = (stbi__uint16 *) stbi__malloc(req_comp * x * y * 2);
   if (good == NULL) {
      STBI_FREE(data);
      return (stbi__uint16 *) stbi__errpuc("outofmem", "Out of memory");
   }

   for (j=0; j < (int) y; ++j) {
      stbi__uint16 *src  = data + j * x * img_n   ;
      stbi__uint16 *dest = good + j * x * req_comp;

      #define STBI__COMBO(a,b)  ((a)*8+(b))
      #define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
      // convert source image with img_n components to one with req_comp components;
      // avoid switch per pixel, so use switch per scanline and massive macros
      switch (STBI__COMBO(img_n, req_comp)) {
         STBI__CASE(1,2) { dest[0]=src[0]; dest[1]=0xffff;                                     } break;
         STBI__CASE(1,3) { dest[0]=dest[1]=dest[2]=src[0];                                     } break;
         STBI__CASE(1,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=0xffff;                     } break;
         STBI__CASE(2,1) { dest[0]=src[0];                                                     } break;
         STBI__CASE(2,3) { dest[0]=dest[1]=dest[2]=src[0];                                     } break;
         STBI__CASE(2,4) { dest[0]=dest[1]=dest[2]=src[0]; dest[3]=src[1];                     } break;
         STBI__CASE(3,4) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];dest[3]=0xffff;        } break;
         STBI__CASE(3,1) { dest[0]=stbi__compute_y_16(src[0],src[1],src[2]);                   } break;
         STBI__CASE(3,2) { dest[0]=stbi__compute_y_16(src[0],src[1],src[2]); dest[1] = 0xffff; } break;
         STBI__CASE(4,1) { dest[0]=stbi__compute_y_16(src[0],src[1],src[2]);                   } break;
         STBI__CASE(4,2) { dest[0]=stbi__compute_y_16(src[0],src[1],src[2]); dest[1] = src[3]; } break;
         STBI__CASE(4,3) { dest[0]=src[0];dest[1]=src[1];dest[2]=src[2];                       } break;
         default: STBI_ASSERT(0); STBI_FREE(data); STBI_FREE(good); return (stbi__uint16*) stbi__errpuc("unsupported", "Unsupported format conversion");
      }
      #undef STBI__CASE
   }

   STBI_FREE(data);
   return good;
}
#endif

#ifndef STBI_NO_LINEAR
static float   *stbi__ldr_to_hdr(stbi_uc *data, int x, int y, int comp)
{
   int i,k,n;
   float *output;
   if (!data) return NULL;
   output = (float *) stbi__malloc_mad4(x, y, comp, sizeof(float), 0);
   if (output == NULL) { STBI_FREE(data); return stbi__errpf("outofmem", "Out of memory"); }
   // compute number of non-alpha components
   if (comp & 1) n = comp; else n = comp-1;
   for (i=0; i < x*y; ++i) {
      for (k=0; k < n; ++k) {
         output[i*comp + k] = (float) (pow(data[i*comp+k]/255.0f, stbi__l2h_gamma) * stbi__l2h_scale);
      }
   }
   if (n < comp) {
      for (i=0; i < x*y; ++i) {
         output[i*comp + n] = data[i*comp + n]/255.0f;
      }
   }
   STBI_FREE(data);
   return output;
}
#endif

#ifndef STBI_NO_HDR
#define stbi__float2int(x)   ((int) (x))
static stbi_uc *stbi__hdr_to_ldr(float   *data, int x, int y, int comp)
{
   int i,k,n;
   stbi_uc *output;
   if (!data) return NULL;
   output = (stbi_uc *) stbi__malloc_mad3(x, y, comp, 0);
   if (output == NULL) { STBI_FREE(data); return stbi__errpuc("outofmem", "Out of memory"); }
   // compute number of non-alpha components
   if (comp & 1) n = comp; else n = comp-1;
   for (i=0; i < x*y; ++i) {
      for (k=0; k < n; ++k) {
         float z = (float) pow(data[i*comp+k]*stbi__h2l_scale_i, stbi__h2l_gamma_i) * 255 + 0.5f;
         if (z < 0) z = 0;
         if (z > 255) z = 255;
         output[i*comp + k] = (stbi_uc) stbi__float2int(z);
      }
      if (k < comp) {
         float z = data[i*comp+k] * 255 + 0.5f;
         if (z < 0) z = 0;
         if (z > 255) z = 255;
         output[i*comp + k] = (stbi_uc) stbi__float2int(z);
      }
   }
   STBI_FREE(data);
   return output;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  "baseline" JPEG/JFIF decoder
//
//    simple implementation
//      - doesn't support delayed output of y-dimension
//      - simple interface (only one output format: 8-bit interleaved RGB)
//      - doesn't try to recover corrupt jpegs
//      - doesn't allow partial loading, loading multiple at once
//      - still fast on x86 (copying globals into locals doesn't help x86)
//      - allocates lots of intermediate memory (full size of all components)
//        - non-interleaved case requires this anyway
//        - allows good upsampling (see next)
//    high-quality
//      - upsampled channels are bilinearly interpolated, even across blocks
//      - quality integer IDCT derived from IJG's 'slow'
//    performance
//      - fast huffman; reasonable integer IDCT
//      - some SIMD kernels for common paths on targets with SSE2/NEON
//      - uses a lot of intermediate memory, could cache poorly

#ifndef STBI_NO_JPEG

// huffman decoding acceleration
#define FAST_BITS   9  // larger handles more cases; smaller stomps less cache

typedef struct
{
   stbi_uc  fast[1 << FAST_BITS];
   // weirdly, repacking this into AoS is a 10% speed loss, instead of a win
   stbi__uint16 code[256];
   stbi_uc  values[256];
   stbi_uc  size[257];
   unsigned int maxcode[18];
   int    delta[17];   // old 'firstsymbol' - old 'firstcode'
} stbi__huffman;

typedef struct
{
   stbi__context *s;
   stbi__huffman huff_dc[4];
   stbi__huffman huff_ac[4];
   stbi__uint16 dequant[4][64];
   stbi__int16 fast_ac[4][1 << FAST_BITS];

// sizes for components, interleaved MCUs
   int img_h_max, img_v_max;
   int img_mcu_x, img_mcu_y;
   int img_mcu_w, img_mcu_h;

// definition of jpeg image component
   struct
   {
      int id;
      int h,v;
      int tq;
      int hd,ha;
      int dc_pred;

      int x,y,w2,h2;
      stbi_uc *data;
      void *raw_data, *raw_coeff;
      stbi_uc *linebuf;
      short   *coeff;   // progressive only
      int      coeff_w, coeff_h; // number of 8x8 coefficient blocks
   } img_comp[4];

   stbi__uint32   code_buffer; // jpeg entropy-coded buffer
   int            code_bits;   // number of valid bits
   unsigned char  marker;      // marker seen while filling entropy buffer
   int            nomore;      // flag if we saw a marker so must stop

   int            progressive;
   int            spec_start;
   int            spec_end;
   int            succ_high;
   int            succ_low;
   int            eob_run;
   int            jfif;
   int            app14_color_transform; // Adobe APP14 tag
   int            rgb;

   int scan_n, order[4];
   int restart_interval, todo;

// kernels
   void (*idct_block_kernel)(stbi_uc *out, int out_stride, short data[64]);
   void (*YCbCr_to_RGB_kernel)(stbi_uc *out, const stbi_uc *y, const stbi_uc *pcb, const stbi_uc *pcr, int count, int step);
   stbi_uc *(*resample_row_hv_2_kernel)(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
} stbi__jpeg;

static int stbi__build_huffman(stbi__huffman *h, int *count)
{
   int i,j,k=0;
   unsigned int code;
   // build size list for each symbol (from JPEG spec)
   for (i=0; i < 16; ++i) {
      for (j=0; j < count[i]; ++j) {
         h->size[k++] = (stbi_uc) (i+1);
         if(k >= 257) return stbi__err("bad size list","Corrupt JPEG");
      }
   }
   h->size[k] = 0;

   // compute actual symbols (from jpeg spec)
   code = 0;
   k = 0;
   for(j=1; j <= 16; ++j) {
      // compute delta to add to code to compute symbol id
      h->delta[j] = k - code;
      if (h->size[k] == j) {
         while (h->size[k] == j)
            h->code[k++] = (stbi__uint16) (code++);
         if (code-1 >= (1u << j)) return stbi__err("bad code lengths","Corrupt JPEG");
      }
      // compute largest code + 1 for this size, preshifted as needed later
      h->maxcode[j] = code << (16-j);
      code <<= 1;
   }
   h->maxcode[j] = 0xffffffff;

   // build non-spec acceleration table; 255 is flag for not-accelerated
   memset(h->fast, 255, 1 << FAST_BITS);
   for (i=0; i < k; ++i) {
      int s = h->size[i];
      if (s <= FAST_BITS) {
         int c = h->code[i] << (FAST_BITS-s);
         int m = 1 << (FAST_BITS-s);
         for (j=0; j < m; ++j) {
            h->fast[c+j] = (stbi_uc) i;
         }
      }
   }
   return 1;
}

// build a table that decodes both magnitude and value of small ACs in
// one go.
static void stbi__build_fast_ac(stbi__int16 *fast_ac, stbi__huffman *h)
{
   int i;
   for (i=0; i < (1 << FAST_BITS); ++i) {
      stbi_uc fast = h->fast[i];
      fast_ac[i] = 0;
      if (fast < 255) {
         int rs = h->values[fast];
         int run = (rs >> 4) & 15;
         int magbits = rs & 15;
         int len = h->size[fast];

         if (magbits && len + magbits <= FAST_BITS) {
            // magnitude code followed by receive_extend code
            int k = ((i << len) & ((1 << FAST_BITS) - 1)) >> (FAST_BITS - magbits);
            int m = 1 << (magbits - 1);
            if (k < m) k += (~0U << magbits) + 1;
            // if the result is small enough, we can fit it in fast_ac table
            if (k >= -128 && k <= 127)
               fast_ac[i] = (stbi__int16) ((k * 256) + (run * 16) + (len + magbits));
         }
      }
   }
}

static void stbi__grow_buffer_unsafe(stbi__jpeg *j)
{
   do {
      unsigned int b = j->nomore ? 0 : stbi__get8(j->s);
      if (b == 0xff) {
         int c = stbi__get8(j->s);
         while (c == 0xff) c = stbi__get8(j->s); // consume fill bytes
         if (c != 0) {
            j->marker = (unsigned char) c;
            j->nomore = 1;
            return;
         }
      }
      j->code_buffer |= b << (24 - j->code_bits);
      j->code_bits += 8;
   } while (j->code_bits <= 24);
}

// (1 << n) - 1
static const stbi__uint32 stbi__bmask[17]={0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535};

// decode a jpeg huffman value from the bitstream
stbi_inline static int stbi__jpeg_huff_decode(stbi__jpeg *j, stbi__huffman *h)
{
   unsigned int temp;
   int c,k;

   if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);

   // look at the top FAST_BITS and determine what symbol ID it is,
   // if the code is <= FAST_BITS
   c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS)-1);
   k = h->fast[c];
   if (k < 255) {
      int s = h->size[k];
      if (s > j->code_bits)
         return -1;
      j->code_buffer <<= s;
      j->code_bits -= s;
      return h->values[k];
   }

   // naive test is to shift the code_buffer down so k bits are
   // valid, then test against maxcode. To speed this up, we've
   // preshifted maxcode left so that it has (16-k) 0s at the
   // end; in other words, regardless of the number of bits, it
   // wants to be compared against something shifted to have 16;
   // that way we don't need to shift inside the loop.
   temp = j->code_buffer >> 16;
   for (k=FAST_BITS+1 ; ; ++k)
      if (temp < h->maxcode[k])
         break;
   if (k == 17) {
      // error! code not found
      j->code_bits -= 16;
      return -1;
   }

   if (k > j->code_bits)
      return -1;

   // convert the huffman code to the symbol id
   c = ((j->code_buffer >> (32 - k)) & stbi__bmask[k]) + h->delta[k];
   if(c < 0 || c >= 256) // symbol id out of bounds!
       return -1;
   STBI_ASSERT((((j->code_buffer) >> (32 - h->size[c])) & stbi__bmask[h->size[c]]) == h->code[c]);

   // convert the id to a symbol
   j->code_bits -= k;
   j->code_buffer <<= k;
   return h->values[c];
}

// bias[n] = (-1<<n) + 1
static const int stbi__jbias[16] = {0,-1,-3,-7,-15,-31,-63,-127,-255,-511,-1023,-2047,-4095,-8191,-16383,-32767};

// combined JPEG 'receive' and JPEG 'extend', since baseline
// always extends everything it receives.
stbi_inline static int stbi__extend_receive(stbi__jpeg *j, int n)
{
   unsigned int k;
   int sgn;
   if (j->code_bits < n) stbi__grow_buffer_unsafe(j);
   if (j->code_bits < n) return 0; // ran out of bits from stream, return 0s intead of continuing

   sgn = j->code_buffer >> 31; // sign bit always in MSB; 0 if MSB clear (positive), 1 if MSB set (negative)
   k = stbi_lrot(j->code_buffer, n);
   j->code_buffer = k & ~stbi__bmask[n];
   k &= stbi__bmask[n];
   j->code_bits -= n;
   return k + (stbi__jbias[n] & (sgn - 1));
}

// get some unsigned bits
stbi_inline static int stbi__jpeg_get_bits(stbi__jpeg *j, int n)
{
   unsigned int k;
   if (j->code_bits < n) stbi__grow_buffer_unsafe(j);
   if (j->code_bits < n) return 0; // ran out of bits from stream, return 0s intead of continuing
   k = stbi_lrot(j->code_buffer, n);
   j->code_buffer = k & ~stbi__bmask[n];
   k &= stbi__bmask[n];
   j->code_bits -= n;
   return k;
}

stbi_inline static int stbi__jpeg_get_bit(stbi__jpeg *j)
{
   unsigned int k;
   if (j->code_bits < 1) stbi__grow_buffer_unsafe(j);
   if (j->code_bits < 1) return 0; // ran out of bits from stream, return 0s intead of continuing
   k = j->code_buffer;
   j->code_buffer <<= 1;
   --j->code_bits;
   return k & 0x80000000;
}

// given a value that's at position X in the zigzag stream,
// where does it appear in the 8x8 matrix coded as row-major?
static const stbi_uc stbi__jpeg_dezigzag[64+15] =
{
    0,  1,  8, 16,  9,  2,  3, 10,
   17, 24, 32, 25, 18, 11,  4,  5,
   12, 19, 26, 33, 40, 48, 41, 34,
   27, 20, 13,  6,  7, 14, 21, 28,
   35, 42, 49, 56, 57, 50, 43, 36,
   29, 22, 15, 23, 30, 37, 44, 51,
   58, 59, 52, 45, 38, 31, 39, 46,
   53, 60, 61, 54, 47, 55, 62, 63,
   // let corrupt input sample past end
   63, 63, 63, 63, 63, 63, 63, 63,
   63, 63, 63, 63, 63, 63, 63
};

// decode one 64-entry block--
static int stbi__jpeg_decode_block(stbi__jpeg *j, short data[64], stbi__huffman *hdc, stbi__huffman *hac, stbi__int16 *fac, int b, stbi__uint16 *dequant)
{
   int diff,dc,k;
   int t;

   if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
   t = stbi__jpeg_huff_decode(j, hdc);
   if (t < 0 || t > 15) return stbi__err("bad huffman code","Corrupt JPEG");

   // 0 all the ac values now so we can do it 32-bits at a time
   memset(data,0,64*sizeof(data[0]));

   diff = t ? stbi__extend_receive(j, t) : 0;
   if (!stbi__addints_valid(j->img_comp[b].dc_pred, diff)) return stbi__err("bad delta","Corrupt JPEG");
   dc = j->img_comp[b].dc_pred + diff;
   j->img_comp[b].dc_pred = dc;
   if (!stbi__mul2shorts_valid(dc, dequant[0])) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
   data[0] = (short) (dc * dequant[0]);

   // decode AC components, see JPEG spec
   k = 1;
   do {
      unsigned int zig;
      int c,r,s;
      if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
      c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS)-1);
      r = fac[c];
      if (r) { // fast-AC path
         k += (r >> 4) & 15; // run
         s = r & 15; // combined length
         if (s > j->code_bits) return stbi__err("bad huffman code", "Combined length longer than code bits available");
         j->code_buffer <<= s;
         j->code_bits -= s;
         // decode into unzigzag'd location
         zig = stbi__jpeg_dezigzag[k++];
         data[zig] = (short) ((r >> 8) * dequant[zig]);
      } else {
         int rs = stbi__jpeg_huff_decode(j, hac);
         if (rs < 0) return stbi__err("bad huffman code","Corrupt JPEG");
         s = rs & 15;
         r = rs >> 4;
         if (s == 0) {
            if (rs != 0xf0) break; // end block
            k += 16;
         } else {
            k += r;
            // decode into unzigzag'd location
            zig = stbi__jpeg_dezigzag[k++];
            data[zig] = (short) (stbi__extend_receive(j,s) * dequant[zig]);
         }
      }
   } while (k < 64);
   return 1;
}

static int stbi__jpeg_decode_block_prog_dc(stbi__jpeg *j, short data[64], stbi__huffman *hdc, int b)
{
   int diff,dc;
   int t;
   if (j->spec_end != 0) return stbi__err("can't merge dc and ac", "Corrupt JPEG");

   if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);

   if (j->succ_high == 0) {
      // first scan for DC coefficient, must be first
      memset(data,0,64*sizeof(data[0])); // 0 all the ac values now
      t = stbi__jpeg_huff_decode(j, hdc);
      if (t < 0 || t > 15) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
      diff = t ? stbi__extend_receive(j, t) : 0;

      if (!stbi__addints_valid(j->img_comp[b].dc_pred, diff)) return stbi__err("bad delta", "Corrupt JPEG");
      dc = j->img_comp[b].dc_pred + diff;
      j->img_comp[b].dc_pred = dc;
      if (!stbi__mul2shorts_valid(dc, 1 << j->succ_low)) return stbi__err("can't merge dc and ac", "Corrupt JPEG");
      data[0] = (short) (dc * (1 << j->succ_low));
   } else {
      // refinement scan for DC coefficient
      if (stbi__jpeg_get_bit(j))
         data[0] += (short) (1 << j->succ_low);
   }
   return 1;
}

// @OPTIMIZE: store non-zigzagged during the decode passes,
// and only de-zigzag when dequantizing
static int stbi__jpeg_decode_block_prog_ac(stbi__jpeg *j, short data[64], stbi__huffman *hac, stbi__int16 *fac)
{
   int k;
   if (j->spec_start == 0) return stbi__err("can't merge dc and ac", "Corrupt JPEG");

   if (j->succ_high == 0) {
      int shift = j->succ_low;

      if (j->eob_run) {
         --j->eob_run;
         return 1;
      }

      k = j->spec_start;
      do {
         unsigned int zig;
         int c,r,s;
         if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
         c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS)-1);
         r = fac[c];
         if (r) { // fast-AC path
            k += (r >> 4) & 15; // run
            s = r & 15; // combined length
            if (s > j->code_bits) return stbi__err("bad huffman code", "Combined length longer than code bits available");
            j->code_buffer <<= s;
            j->code_bits -= s;
            zig = stbi__jpeg_dezigzag[k++];
            data[zig] = (short) ((r >> 8) * (1 << shift));
         } else {
            int rs = stbi__jpeg_huff_decode(j, hac);
            if (rs < 0) return stbi__err("bad huffman code","Corrupt JPEG");
            s = rs & 15;
            r = rs >> 4;
            if (s == 0) {
               if (r < 15) {
                  j->eob_run = (1 << r);
                  if (r)
                     j->eob_run += stbi__jpeg_get_bits(j, r);
                  --j->eob_run;
                  break;
               }
               k += 16;
            } else {
               k += r;
               zig = stbi__jpeg_dezigzag[k++];
               data[zig] = (short) (stbi__extend_receive(j,s) * (1 << shift));
            }
         }
      } while (k <= j->spec_end);
   } else {
      // refinement scan for these AC coefficients

      short bit = (short) (1 << j->succ_low);

      if (j->eob_run) {
         --j->eob_run;
         for (k = j->spec_start; k <= j->spec_end; ++k) {
            short *p = &data[stbi__jpeg_dezigzag[k]];
            if (*p != 0)
               if (stbi__jpeg_get_bit(j))
                  if ((*p & bit)==0) {
                     if (*p > 0)
                        *p += bit;
                     else
                        *p -= bit;
                  }
         }
      } else {
         k = j->spec_start;
         do {
            int r,s;
            int rs = stbi__jpeg_huff_decode(j, hac); // @OPTIMIZE see if we can use the fast path here, advance-by-r is so slow, eh
            if (rs < 0) return stbi__err("bad huffman code","Corrupt JPEG");
            s = rs & 15;
            r = rs >> 4;
            if (s == 0) {
               if (r < 15) {
                  j->eob_run = (1 << r) - 1;
                  if (r)
                     j->eob_run += stbi__jpeg_get_bits(j, r);
                  r = 64; // force end of block
               } else {
                  // r=15 s=0 should write 16 0s, so we just do
                  // a run of 15 0s and then write s (which is 0),
                  // so we don't have to do anything special here
               }
            } else {
               if (s != 1) return stbi__err("bad huffman code", "Corrupt JPEG");
               // sign bit
               if (stbi__jpeg_get_bit(j))
                  s = bit;
               else
                  s = -bit;
            }

            // advance by r
            while (k <= j->spec_end) {
               short *p = &data[stbi__jpeg_dezigzag[k++]];
               if (*p != 0) {
                  if (stbi__jpeg_get_bit(j))
                     if ((*p & bit)==0) {
                        if (*p > 0)
                           *p += bit;
                        else
                           *p -= bit;
                     }
               } else {
                  if (r == 0) {
                     *p = (short) s;
                     break;
                  }
                  --r;
               }
            }
         } while (k <= j->spec_end);
      }
   }
   return 1;
}

// take a -128..127 value and stbi__clamp it and convert to 0..255
stbi_inline static stbi_uc stbi__clamp(int x)
{
   // trick to use a single test to catch both cases
   if ((unsigned int) x > 255) {
      if (x < 0) return 0;
      if (x > 255) return 255;
   }
   return (stbi_uc) x;
}

#define stbi__f2f(x)  ((int) (((x) * 4096 + 0.5)))
#define stbi__fsh(x)  ((x) * 4096)

// derived from jidctint -- DCT_ISLOW
#define STBI__IDCT_1D(s0,s1,s2,s3,s4,s5,s6,s7) \
   int t0,t1,t2,t3,p1,p2,p3,p4,p5,x0,x1,x2,x3; \
   p2 = s2;                                    \
   p3 = s6;                                    \
   p1 = (p2+p3) * stbi__f2f(0.5411961f);       \
   t2 = p1 + p3*stbi__f2f(-1.847759065f);      \
   t3 = p1 + p2*stbi__f2f( 0.765366865f);      \
   p2 = s0;                                    \
   p3 = s4;                                    \
   t0 = stbi__fsh(p2+p3);                      \
   t1 = stbi__fsh(p2-p3);                      \
   x0 = t0+t3;                                 \
   x3 = t0-t3;                                 \
   x1 = t1+t2;                                 \
   x2 = t1-t2;                                 \
   t0 = s7;                                    \
   t1 = s5;                                    \
   t2 = s3;                                    \
   t3 = s1;                                    \
   p3 = t0+t2;                                 \
   p4 = t1+t3;                                 \
   p1 = t0+t3;                                 \
   p2 = t1+t2;                                 \
   p5 = (p3+p4)*stbi__f2f( 1.175875602f);      \
   t0 = t0*stbi__f2f( 0.298631336f);           \
   t1 = t1*stbi__f2f( 2.053119869f);           \
   t2 = t2*stbi__f2f( 3.072711026f);           \
   t3 = t3*stbi__f2f( 1.501321110f);           \
   p1 = p5 + p1*stbi__f2f(-0.899976223f);      \
   p2 = p5 + p2*stbi__f2f(-2.562915447f);      \
   p3 = p3*stbi__f2f(-1.961570560f);           \
   p4 = p4*stbi__f2f(-0.390180644f);           \
   t3 += p1+p4;                                \
   t2 += p2+p3;                                \
   t1 += p2+p4;                                \
   t0 += p1+p3;

static void stbi__idct_block(stbi_uc *out, int out_stride, short data[64])
{
   int i,val[64],*v=val;
   stbi_uc *o;
   short *d = data;

   // columns
   for (i=0; i < 8; ++i,++d, ++v) {
      // if all zeroes, shortcut -- this avoids dequantizing 0s and IDCTing
      if (d[ 8]==0 && d[16]==0 && d[24]==0 && d[32]==0
           && d[40]==0 && d[48]==0 && d[56]==0) {
         //    no shortcut                 0     seconds
         //    (1|2|3|4|5|6|7)==0          0     seconds
         //    all separate               -0.047 seconds
         //    1 && 2|3 && 4|5 && 6|7:    -0.047 seconds
         int dcterm = d[0]*4;
         v[0] = v[8] = v[16] = v[24] = v[32] = v[40] = v[48] = v[56] = dcterm;
      } else {
         STBI__IDCT_1D(d[ 0],d[ 8],d[16],d[24],d[32],d[40],d[48],d[56])
         // constants scaled things up by 1<<12; let's bring them back
         // down, but keep 2 extra bits of precision
         x0 += 512; x1 += 512; x2 += 512; x3 += 512;
         v[ 0] = (x0+t3) >> 10;
         v[56] = (x0-t3) >> 10;
         v[ 8] = (x1+t2) >> 10;
         v[48] = (x1-t2) >> 10;
         v[16] = (x2+t1) >> 10;
         v[40] = (x2-t1) >> 10;
         v[24] = (x3+t0) >> 10;
         v[32] = (x3-t0) >> 10;
      }
   }

   for (i=0, v=val, o=out; i < 8; ++i,v+=8,o+=out_stride) {
      // no fast case since the first 1D IDCT spread components out
      STBI__IDCT_1D(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7])
      // constants scaled things up by 1<<12, plus we had 1<<2 from first
      // loop, plus horizontal and vertical each scale by sqrt(8) so together
      // we've got an extra 1<<3, so 1<<17 total we need to remove.
      // so we want to round that, which means adding 0.5 * 1<<17,
      // aka 65536. Also, we'll end up with -128 to 127 that we want
      // to encode as 0..255 by adding 128, so we'll add that before the shift
      x0 += 65536 + (128<<17);
      x1 += 65536 + (128<<17);
      x2 += 65536 + (128<<17);
      x3 += 65536 + (128<<17);
      // tried computing the shifts into temps, or'ing the temps to see
      // if any were out of range, but that was slower
      o[0] = stbi__clamp((x0+t3) >> 17);
      o[7] = stbi__clamp((x0-t3) >> 17);
      o[1] = stbi__clamp((x1+t2) >> 17);
      o[6] = stbi__clamp((x1-t2) >> 17);
      o[2] = stbi__clamp((x2+t1) >> 17);
      o[5] = stbi__clamp((x2-t1) >> 17);
      o[3] = stbi__clamp((x3+t0) >> 17);
      o[4] = stbi__clamp((x3-t0) >> 17);
   }
}

#ifdef STBI_SSE2
// sse2 integer IDCT. not the fastest possible implementation but it
// produces bit-identical results to the generic C version so it's
// fully "transparent".
static void stbi__idct_simd(stbi_uc *out, int out_stride, short data[64])
{
   // This is constructed to match our regular (generic) integer IDCT exactly.
   __m128i row0, row1, row2, row3, row4, row5, row6, row7;
   __m128i tmp;

   // dot product constant: even elems=x, odd elems=y
   #define dct_const(x,y)  _mm_setr_epi16((x),(y),(x),(y),(x),(y),(x),(y))

   // out(0) = c0[even]*x + c0[odd]*y   (c0, x, y 16-bit, out 32-bit)
   // out(1) = c1[even]*x + c1[odd]*y
   #define dct_rot(out0,out1, x,y,c0,c1) \
      __m128i c0##lo = _mm_unpacklo_epi16((x),(y)); \
      __m128i c0##hi = _mm_unpackhi_epi16((x),(y)); \
      __m128i out0##_l = _mm_madd_epi16(c0##lo, c0); \
      __m128i out0##_h = _mm_madd_epi16(c0##hi, c0); \
      __m128i out1##_l = _mm_madd_epi16(c0##lo, c1); \
      __m128i out1##_h = _mm_madd_epi16(c0##hi, c1)

   // out = in << 12  (in 16-bit, out 32-bit)
   #define dct_widen(out, in) \
      __m128i out##_l = _mm_srai_epi32(_mm_unpacklo_epi16(_mm_setzero_si128(), (in)), 4); \
      __m128i out##_h = _mm_srai_epi32(_mm_unpackhi_epi16(_mm_setzero_si128(), (in)), 4)

   // wide add
   #define dct_wadd(out, a, b) \
      __m128i out##_l = _mm_add_epi32(a##_l, b##_l); \
      __m128i out##_h = _mm_add_epi32(a##_h, b##_h)

   // wide sub
   #define dct_wsub(out, a, b) \
      __m128i out##_l = _mm_sub_epi32(a##_l, b##_l); \
      __m128i out##_h = _mm_sub_epi32(a##_h, b##_h)

   // butterfly a/b, add bias, then shift by "s" and pack
   #define dct_bfly32o(out0, out1, a,b,bias,s) \
      { \
         __m128i abiased_l = _mm_add_epi32(a##_l, bias); \
         __m128i abiased_h = _mm_add_epi32(a##_h, bias); \
         dct_wadd(sum, abiased, b); \
         dct_wsub(dif, abiased, b); \
         out0 = _mm_packs_epi32(_mm_srai_epi32(sum_l, s), _mm_srai_epi32(sum_h, s)); \
         out1 = _mm_packs_epi32(_mm_srai_epi32(dif_l, s), _mm_srai_epi32(dif_h, s)); \
      }

   // 8-bit interleave step (for transposes)
   #define dct_interleave8(a, b) \
      tmp = a; \
      a = _mm_unpacklo_epi8(a, b); \
      b = _mm_unpackhi_epi8(tmp, b)

   // 16-bit interleave step (for transposes)
   #define dct_interleave16(a, b) \
      tmp = a; \
      a = _mm_unpacklo_epi16(a, b); \
      b = _mm_unpackhi_epi16(tmp, b)

   #define dct_pass(bias,shift) \
      { \
         /* even part */ \
         dct_rot(t2e,t3e, row2,row6, rot0_0,rot0_1); \
         __m128i sum04 = _mm_add_epi16(row0, row4); \
         __m128i dif04 = _mm_sub_epi16(row0, row4); \
         dct_widen(t0e, sum04); \
         dct_widen(t1e, dif04); \
         dct_wadd(x0, t0e, t3e); \
         dct_wsub(x3, t0e, t3e); \
         dct_wadd(x1, t1e, t2e); \
         dct_wsub(x2, t1e, t2e); \
         /* odd part */ \
         dct_rot(y0o,y2o, row7,row3, rot2_0,rot2_1); \
         dct_rot(y1o,y3o, row5,row1, rot3_0,rot3_1); \
         __m128i sum17 = _mm_add_epi16(row1, row7); \
         __m128i sum35 = _mm_add_epi16(row3, row5); \
         dct_rot(y4o,y5o, sum17,sum35, rot1_0,rot1_1); \
         dct_wadd(x4, y0o, y4o); \
         dct_wadd(x5, y1o, y5o); \
         dct_wadd(x6, y2o, y5o); \
         dct_wadd(x7, y3o, y4o); \
         dct_bfly32o(row0,row7, x0,x7,bias,shift); \
         dct_bfly32o(row1,row6, x1,x6,bias,shift); \
         dct_bfly32o(row2,row5, x2,x5,bias,shift); \
         dct_bfly32o(row3,row4, x3,x4,bias,shift); \
      }

   __m128i rot0_0 = dct_const(stbi__f2f(0.5411961f), stbi__f2f(0.5411961f) + stbi__f2f(-1.847759065f));
   __m128i rot0_1 = dct_const(stbi__f2f(0.5411961f) + stbi__f2f( 0.765366865f), stbi__f2f(0.5411961f));
   __m128i rot1_0 = dct_const(stbi__f2f(1.175875602f) + stbi__f2f(-0.899976223f), stbi__f2f(1.175875602f));
   __m128i rot1_1 = dct_const(stbi__f2f(1.175875602f), stbi__f2f(1.175875602f) + stbi__f2f(-2.562915447f));
   __m128i rot2_0 = dct_const(stbi__f2f(-1.961570560f) + stbi__f2f( 0.298631336f), stbi__f2f(-1.961570560f));
   __m128i rot2_1 = dct_const(stbi__f2f(-1.961570560f), stbi__f2f(-1.961570560f) + stbi__f2f( 3.072711026f));
   __m128i rot3_0 = dct_const(stbi__f2f(-0.390180644f) + stbi__f2f( 2.053119869f), stbi__f2f(-0.390180644f));
   __m128i rot3_1 = dct_const(stbi__f2f(-0.390180644f), stbi__f2f(-0.390180644f) + stbi__f2f( 1.501321110f));

   // rounding biases in column/row passes, see stbi__idct_block for explanation.
   __m128i bias_0 = _mm_set1_epi32(512);
   __m128i bias_1 = _mm_set1_epi32(65536 + (128<<17));

   // load
   row0 = _mm_load_si128((const __m128i *) (data + 0*8));
   row1 = _mm_load_si128((const __m128i *) (data + 1*8));
   row2 = _mm_load_si128((const __m128i *) (data + 2*8));
   row3 = _mm_load_si128((const __m128i *) (data + 3*8));
   row4 = _mm_load_si128((const __m128i *) (data + 4*8));
   row5 = _mm_load_si128((const __m128i *) (data + 5*8));
   row6 = _mm_load_si128((const __m128i *) (data + 6*8));
   row7 = _mm_load_si128((const __m128i *) (data + 7*8));

   // column pass
   dct_pass(bias_0, 10);

   {
      // 16bit 8x8 transpose pass 1
      dct_interleave16(row0, row4);
      dct_interleave16(row1, row5);
      dct_interleave16(row2, row6);
      dct_interleave16(row3, row7);

      // transpose pass 2
      dct_interleave16(row0, row2);
      dct_interleave16(row1, row3);
      dct_interleave16(row4, row6);
      dct_interleave16(row5, row7);

      // transpose pass 3
      dct_interleave16(row0, row1);
      dct_interleave16(row2, row3);
      dct_interleave16(row4, row5);
      dct_interleave16(row6, row7);
   }

   // row pass
   dct_pass(bias_1, 17);

   {
      // pack
      __m128i p0 = _mm_packus_epi16(row0, row1); // a0a1a2a3...a7b0b1b2b3...b7
      __m128i p1 = _mm_packus_epi16(row2, row3);
      __m128i p2 = _mm_packus_epi16(row4, row5);
      __m128i p3 = _mm_packus_epi16(row6, row7);

      // 8bit 8x8 transpose pass 1
      dct_interleave8(p0, p2); // a0e0a1e1...
      dct_interleave8(p1, p3); // c0g0c1g1...

      // transpose pass 2
      dct_interleave8(p0, p1); // a0c0e0g0...
      dct_interleave8(p2, p3); // b0d0f0h0...

      // transpose pass 3
      dct_interleave8(p0, p2); // a0b0c0d0...
      dct_interleave8(p1, p3); // a4b4c4d4...

      // store
      _mm_storel_epi64((__m128i *) out, p0); out += out_stride;
      _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p0, 0x4e)); out += out_stride;
      _mm_storel_epi64((__m128i *) out, p2); out += out_stride;
      _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p2, 0x4e)); out += out_stride;
      _mm_storel_epi64((__m128i *) out, p1); out += out_stride;
      _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p1, 0x4e)); out += out_stride;
      _mm_storel_epi64((__m128i *) out, p3); out += out_stride;
      _mm_storel_epi64((__m128i *) out, _mm_shuffle_epi32(p3, 0x4e));
   }

#undef dct_const
#undef dct_rot
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_interleave8
#undef dct_interleave16
#undef dct_pass
}

#endif // STBI_SSE2

#ifdef STBI_NEON

// NEON integer IDCT. should produce bit-identical
// results to the generic C version.
static void stbi__idct_simd(stbi_uc *out, int out_stride, short data[64])
{
   int16x8_t row0, row1, row2, row3, row4, row5, row6, row7;

   int16x4_t rot0_0 = vdup_n_s16(stbi__f2f(0.5411961f));
   int16x4_t rot0_1 = vdup_n_s16(stbi__f2f(-1.847759065f));
   int16x4_t rot0_2 = vdup_n_s16(stbi__f2f( 0.765366865f));
   int16x4_t rot1_0 = vdup_n_s16(stbi__f2f( 1.175875602f));
   int16x4_t rot1_1 = vdup_n_s16(stbi__f2f(-0.899976223f));
   int16x4_t rot1_2 = vdup_n_s16(stbi__f2f(-2.562915447f));
   int16x4_t rot2_0 = vdup_n_s16(stbi__f2f(-1.961570560f));
   int16x4_t rot2_1 = vdup_n_s16(stbi__f2f(-0.390180644f));
   int16x4_t rot3_0 = vdup_n_s16(stbi__f2f( 0.298631336f));
   int16x4_t rot3_1 = vdup_n_s16(stbi__f2f( 2.053119869f));
   int16x4_t rot3_2 = vdup_n_s16(stbi__f2f( 3.072711026f));
   int16x4_t rot3_3 = vdup_n_s16(stbi__f2f( 1.501321110f));

#define dct_long_mul(out, inq, coeff) \
   int32x4_t out##_l = vmull_s16(vget_low_s16(inq), coeff); \
   int32x4_t out##_h = vmull_s16(vget_high_s16(inq), coeff)

#define dct_long_mac(out, acc, inq, coeff) \
   int32x4_t out##_l = vmlal_s16(acc##_l, vget_low_s16(inq), coeff); \
   int32x4_t out##_h = vmlal_s16(acc##_h, vget_high_s16(inq), coeff)

#define dct_widen(out, inq) \
   int32x4_t out##_l = vshll_n_s16(vget_low_s16(inq), 12); \
   int32x4_t out##_h = vshll_n_s16(vget_high_s16(inq), 12)

// wide add
#define dct_wadd(out, a, b) \
   int32x4_t out##_l = vaddq_s32(a##_l, b##_l); \
   int32x4_t out##_h = vaddq_s32(a##_h, b##_h)

// wide sub
#define dct_wsub(out, a, b) \
   int32x4_t out##_l = vsubq_s32(a##_l, b##_l); \
   int32x4_t out##_h = vsubq_s32(a##_h, b##_h)

// butterfly a/b, then shift using "shiftop" by "s" and pack
#define dct_bfly32o(out0,out1, a,b,shiftop,s) \
   { \
      dct_wadd(sum, a, b); \
      dct_wsub(dif, a, b); \
      out0 = vcombine_s16(shiftop(sum_l, s), shiftop(sum_h, s)); \
      out1 = vcombine_s16(shiftop(dif_l, s), shiftop(dif_h, s)); \
   }

#define dct_pass(shiftop, shift) \
   { \
      /* even part */ \
      int16x8_t sum26 = vaddq_s16(row2, row6); \
      dct_long_mul(p1e, sum26, rot0_0); \
      dct_long_mac(t2e, p1e, row6, rot0_1); \
      dct_long_mac(t3e, p1e, row2, rot0_2); \
      int16x8_t sum04 = vaddq_s16(row0, row4); \
      int16x8_t dif04 = vsubq_s16(row0, row4); \
      dct_widen(t0e, sum04); \
      dct_widen(t1e, dif04); \
      dct_wadd(x0, t0e, t3e); \
      dct_wsub(x3, t0e, t3e); \
      dct_wadd(x1, t1e, t2e); \
      dct_wsub(x2, t1e, t2e); \
      /* odd part */ \
      int16x8_t sum15 = vaddq_s16(row1, row5); \
      int16x8_t sum17 = vaddq_s16(row1, row7); \
      int16x8_t sum35 = vaddq_s16(row3, row5); \
      int16x8_t sum37 = vaddq_s16(row3, row7); \
      int16x8_t sumodd = vaddq_s16(sum17, sum35); \
      dct_long_mul(p5o, sumodd, rot1_0); \
      dct_long_mac(p1o, p5o, sum17, rot1_1); \
      dct_long_mac(p2o, p5o, sum35, rot1_2); \
      dct_long_mul(p3o, sum37, rot2_0); \
      dct_long_mul(p4o, sum15, rot2_1); \
      dct_wadd(sump13o, p1o, p3o); \
      dct_wadd(sump24o, p2o, p4o); \
      dct_wadd(sump23o, p2o, p3o); \
      dct_wadd(sump14o, p1o, p4o); \
      dct_long_mac(x4, sump13o, row7, rot3_0); \
      dct_long_mac(x5, sump24o, row5, rot3_1); \
      dct_long_mac(x6, sump23o, row3, rot3_2); \
      dct_long_mac(x7, sump14o, row1, rot3_3); \
      dct_bfly32o(row0,row7, x0,x7,shiftop,shift); \
      dct_bfly32o(row1,row6, x1,x6,shiftop,shift); \
      dct_bfly32o(row2,row5, x2,x5,shiftop,shift); \
      dct_bfly32o(row3,row4, x3,x4,shiftop,shift); \
   }

   // load
   row0 = vld1q_s16(data + 0*8);
   row1 = vld1q_s16(data + 1*8);
   row2 = vld1q_s16(data + 2*8);
   row3 = vld1q_s16(data + 3*8);
   row4 = vld1q_s16(data + 4*8);
   row5 = vld1q_s16(data + 5*8);
   row6 = vld1q_s16(data + 6*8);
   row7 = vld1q_s16(data + 7*8);

   // add DC bias
   row0 = vaddq_s16(row0, vsetq_lane_s16(1024, vdupq_n_s16(0), 0));

   // column pass
   dct_pass(vrshrn_n_s32, 10);

   // 16bit 8x8 transpose
   {
// these three map to a single VTRN.16, VTRN.32, and VSWP, respectively.
// whether compilers actually get this is another story, sadly.
#define dct_trn16(x, y) { int16x8x2_t t = vtrnq_s16(x, y); x = t.val[0]; y = t.val[1]; }
#define dct_trn32(x, y) { int32x4x2_t t = vtrnq_s32(vreinterpretq_s32_s16(x), vreinterpretq_s32_s16(y)); x = vreinterpretq_s16_s32(t.val[0]); y = vreinterpretq_s16_s32(t.val[1]); }
#define dct_trn64(x, y) { int16x8_t x0 = x; int16x8_t y0 = y; x = vcombine_s16(vget_low_s16(x0), vget_low_s16(y0)); y = vcombine_s16(vget_high_s16(x0), vget_high_s16(y0)); }

      // pass 1
      dct_trn16(row0, row1); // a0b0a2b2a4b4a6b6
      dct_trn16(row2, row3);
      dct_trn16(row4, row5);
      dct_trn16(row6, row7);

      // pass 2
      dct_trn32(row0, row2); // a0b0c0d0a4b4c4d4
      dct_trn32(row1, row3);
      dct_trn32(row4, row6);
      dct_trn32(row5, row7);

      // pass 3
      dct_trn64(row0, row4); // a0b0c0d0e0f0g0h0
      dct_trn64(row1, row5);
      dct_trn64(row2, row6);
      dct_trn64(row3, row7);

#undef dct_trn16
#undef dct_trn32
#undef dct_trn64
   }

   // row pass
   // vrshrn_n_s32 only supports shifts up to 16, we need
   // 17. so do a non-rounding shift of 16 first then follow
   // up with a rounding shift by 1.
   dct_pass(vshrn_n_s32, 16);

   {
      // pack and round
      uint8x8_t p0 = vqrshrun_n_s16(row0, 1);
      uint8x8_t p1 = vqrshrun_n_s16(row1, 1);
      uint8x8_t p2 = vqrshrun_n_s16(row2, 1);
      uint8x8_t p3 = vqrshrun_n_s16(row3, 1);
      uint8x8_t p4 = vqrshrun_n_s16(row4, 1);
      uint8x8_t p5 = vqrshrun_n_s16(row5, 1);
      uint8x8_t p6 = vqrshrun_n_s16(row6, 1);
      uint8x8_t p7 = vqrshrun_n_s16(row7, 1);

      // again, these can translate into one instruction, but often don't.
#define dct_trn8_8(x, y) { uint8x8x2_t t = vtrn_u8(x, y); x = t.val[0]; y = t.val[1]; }
#define dct_trn8_16(x, y) { uint16x4x2_t t = vtrn_u16(vreinterpret_u16_u8(x), vreinterpret_u16_u8(y)); x = vreinterpret_u8_u16(t.val[0]); y = vreinterpret_u8_u16(t.val[1]); }
#define dct_trn8_32(x, y) { uint32x2x2_t t = vtrn_u32(vreinterpret_u32_u8(x), vreinterpret_u32_u8(y)); x = vreinterpret_u8_u32(t.val[0]); y = vreinterpret_u8_u32(t.val[1]); }

      // sadly can't use interleaved stores here since we only write
      // 8 bytes to each scan line!

      // 8x8 8-bit transpose pass 1
      dct_trn8_8(p0, p1);
      dct_trn8_8(p2, p3);
      dct_trn8_8(p4, p5);
      dct_trn8_8(p6, p7);

      // pass 2
      dct_trn8_16(p0, p2);
      dct_trn8_16(p1, p3);
      dct_trn8_16(p4, p6);
      dct_trn8_16(p5, p7);

      // pass 3
      dct_trn8_32(p0, p4);
      dct_trn8_32(p1, p5);
      dct_trn8_32(p2, p6);
      dct_trn8_32(p3, p7);

      // store
      vst1_u8(out, p0); out += out_stride;
      vst1_u8(out, p1); out += out_stride;
      vst1_u8(out, p2); out += out_stride;
      vst1_u8(out, p3); out += out_stride;
      vst1_u8(out, p4); out += out_stride;
      vst1_u8(out, p5); out += out_stride;
      vst1_u8(out, p6); out += out_stride;
      vst1_u8(out, p7);

#undef dct_trn8_8
#undef dct_trn8_16
#undef dct_trn8_32
   }

#undef dct_long_mul
#undef dct_long_mac
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_pass
}

#endif // STBI_NEON

#define STBI__MARKER_none  0xff
// if there's a pending marker from the entropy stream, return that
// otherwise, fetch from the stream and get a marker. if there's no
// marker, return 0xff, which is never a valid marker value
static stbi_uc stbi__get_marker(stbi__jpeg *j)
{
   stbi_uc x;
   if (j->marker != STBI__MARKER_none) { x = j->marker; j->marker = STBI__MARKER_none; return x; }
   x = stbi__get8(j->s);
   if (x != 0xff) return STBI__MARKER_none;
   while (x == 0xff)
      x = stbi__get8(j->s); // consume repeated 0xff fill bytes
   return x;
}

// in each scan, we'll have scan_n components, and the order
// of the components is specified by order[]
#define STBI__RESTART(x)     ((x) >= 0xd0 && (x) <= 0xd7)

// after a restart interval, stbi__jpeg_reset the entropy decoder and
// the dc prediction
static void stbi__jpeg_reset(stbi__jpeg *j)
{
   j->code_bits = 0;
   j->code_buffer = 0;
   j->nomore = 0;
   j->img_comp[0].dc_pred = j->img_comp[1].dc_pred = j->img_comp[2].dc_pred = j->img_comp[3].dc_pred = 0;
   j->marker = STBI__MARKER_none;
   j->todo = j->restart_interval ? j->restart_interval : 0x7fffffff;
   j->eob_run = 0;
   // no more than 1<<31 MCUs if no restart_interal? that's plenty safe,
   // since we don't even allow 1<<30 pixels
}

static int stbi__parse_entropy_coded_data(stbi__jpeg *z)
{
   stbi__jpeg_reset(z);
   if (!z->progressive) {
      if (z->scan_n == 1) {
         int i,j;
         STBI_SIMD_ALIGN(short, data[64]);
         int n = z->order[0];
         // non-interleaved data, we just need to process one block at a time,
         // in trivial scanline order
         // number of blocks to do just depends on how many actual "pixels" this
         // component has, independent of interleaved MCU blocking and such
         int w = (z->img_comp[n].x+7) >> 3;
         int h = (z->img_comp[n].y+7) >> 3;
         for (j=0; j < h; ++j) {
            for (i=0; i < w; ++i) {
               int ha = z->img_comp[n].ha;
               if (!stbi__jpeg_decode_block(z, data, z->huff_dc+z->img_comp[n].hd, z->huff_ac+ha, z->fast_ac[ha], n, z->dequant[z->img_comp[n].tq])) return 0;
               z->idct_block_kernel(z->img_comp[n].data+z->img_comp[n].w2*j*8+i*8, z->img_comp[n].w2, data);
               // every data block is an MCU, so countdown the restart interval
               if (--z->todo <= 0) {
                  if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                  // if it's NOT a restart, then just bail, so we get corrupt data
                  // rather than no data
                  if (!STBI__RESTART(z->marker)) return 1;
                  stbi__jpeg_reset(z);
               }
            }
         }
         return 1;
      } else { // interleaved
         int i,j,k,x,y;
         STBI_SIMD_ALIGN(short, data[64]);
         for (j=0; j < z->img_mcu_y; ++j) {
            for (i=0; i < z->img_mcu_x; ++i) {
               // scan an interleaved mcu... process scan_n components in order
               for (k=0; k < z->scan_n; ++k) {
                  int n = z->order[k];
                  // scan out an mcu's worth of this component; that's just determined
                  // by the basic H and V specified for the component
                  for (y=0; y < z->img_comp[n].v; ++y) {
                     for (x=0; x < z->img_comp[n].h; ++x) {
                        int x2 = (i*z->img_comp[n].h + x)*8;
                        int y2 = (j*z->img_comp[n].v + y)*8;
                        int ha = z->img_comp[n].ha;
                        if (!stbi__jpeg_decode_block(z, data, z->huff_dc+z->img_comp[n].hd, z->huff_ac+ha, z->fast_ac[ha], n, z->dequant[z->img_comp[n].tq])) return 0;
                        z->idct_block_kernel(z->img_comp[n].data+z->img_comp[n].w2*y2+x2, z->img_comp[n].w2, data);
                     }
                  }
               }
               // after all interleaved components, that's an interleaved MCU,
               // so now count down the restart interval
               if (--z->todo <= 0) {
                  if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                  if (!STBI__RESTART(z->marker)) return 1;
                  stbi__jpeg_reset(z);
               }
            }
         }
         return 1;
      }
   } else {
      if (z->scan_n == 1) {
         int i,j;
         int n = z->order[0];
         // non-interleaved data, we just need to process one block at a time,
         // in trivial scanline order
         // number of blocks to do just depends on how many actual "pixels" this
         // component has, independent of interleaved MCU blocking and such
         int w = (z->img_comp[n].x+7) >> 3;
         int h = (z->img_comp[n].y+7) >> 3;
         for (j=0; j < h; ++j) {
            for (i=0; i < w; ++i) {
               short *data = z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
               if (z->spec_start == 0) {
                  if (!stbi__jpeg_decode_block_prog_dc(z, data, &z->huff_dc[z->img_comp[n].hd], n))
                     return 0;
               } else {
                  int ha = z->img_comp[n].ha;
                  if (!stbi__jpeg_decode_block_prog_ac(z, data, &z->huff_ac[ha], z->fast_ac[ha]))
                     return 0;
               }
               // every data block is an MCU, so countdown the restart interval
               if (--z->todo <= 0) {
                  if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                  if (!STBI__RESTART(z->marker)) return 1;
                  stbi__jpeg_reset(z);
               }
            }
         }
         return 1;
      } else { // interleaved
         int i,j,k,x,y;
         for (j=0; j < z->img_mcu_y; ++j) {
            for (i=0; i < z->img_mcu_x; ++i) {
               // scan an interleaved mcu... process scan_n components in order
               for (k=0; k < z->scan_n; ++k) {
                  int n = z->order[k];
                  // scan out an mcu's worth of this component; that's just determined
                  // by the basic H and V specified for the component
                  for (y=0; y < z->img_comp[n].v; ++y) {
                     for (x=0; x < z->img_comp[n].h; ++x) {
                        int x2 = (i*z->img_comp[n].h + x);
                        int y2 = (j*z->img_comp[n].v + y);
                        short *data = z->img_comp[n].coeff + 64 * (x2 + y2 * z->img_comp[n].coeff_w);
                        if (!stbi__jpeg_decode_block_prog_dc(z, data, &z->huff_dc[z->img_comp[n].hd], n))
                           return 0;
                     }
                  }
               }
               // after all interleaved components, that's an interleaved MCU,
               // so now count down the restart interval
               if (--z->todo <= 0) {
                  if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
                  if (!STBI__RESTART(z->marker)) return 1;
                  stbi__jpeg_reset(z);
               }
            }
         }
         return 1;
      }
   }
}

static void stbi__jpeg_dequantize(short *data, stbi__uint16 *dequant)
{
   int i;
   for (i=0; i < 64; ++i)
      data[i] *= dequant[i];
}

static void stbi__jpeg_finish(stbi__jpeg *z)
{
   if (z->progressive) {
      // dequantize and idct the data
      int i,j,n;
      for (n=0; n < z->s->img_n; ++n) {
         int w = (z->img_comp[n].x+7) >> 3;
         int h = (z->img_comp[n].y+7) >> 3;
         for (j=0; j < h; ++j) {
            for (i=0; i < w; ++i) {
               short *data = z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
               stbi__jpeg_dequantize(data, z->dequant[z->img_comp[n].tq]);
               z->idct_block_kernel(z->img_comp[n].data+z->img_comp[n].w2*j*8+i*8, z->img_comp[n].w2, data);
            }
         }
      }
   }
}

static int stbi__process_marker(stbi__jpeg *z, int m)
{
   int L;
   switch (m) {
      case STBI__MARKER_none: // no marker found
         return stbi__err("expected marker","Corrupt JPEG");

      case 0xDD: // DRI - specify restart interval
         if (stbi__get16be(z->s) != 4) return stbi__err("bad DRI len","Corrupt JPEG");
         z->restart_interval = stbi__get16be(z->s);
         return 1;

      case 0xDB: // DQT - define quantization table
         L = stbi__get16be(z->s)-2;
         while (L > 0) {
            int q = stbi__get8(z->s);
            int p = q >> 4, sixteen = (p != 0);
            int t = q & 15,i;
            if (p != 0 && p != 1) return stbi__err("bad DQT type","Corrupt JPEG");
            if (t > 3) return stbi__err("bad DQT table","Corrupt JPEG");

            for (i=0; i < 64; ++i)
               z->dequant[t][stbi__jpeg_dezigzag[i]] = (stbi__uint16)(sixteen ? stbi__get16be(z->s) : stbi__get8(z->s));
            L -= (sixteen ? 129 : 65);
         }
         return L==0;

      case 0xC4: // DHT - define huffman table
         L = stbi__get16be(z->s)-2;
         while (L > 0) {
            stbi_uc *v;
            int sizes[16],i,n=0;
            int q = stbi__get8(z->s);
            int tc = q >> 4;
            int th = q & 15;
            if (tc > 1 || th > 3) return stbi__err("bad DHT header","Corrupt JPEG");
            for (i=0; i < 16; ++i) {
               sizes[i] = stbi__get8(z->s);
               n += sizes[i];
            }
            if(n > 256) return stbi__err("bad DHT header","Corrupt JPEG"); // Loop over i < n would write past end of values!
            L -= 17;
            if (tc == 0) {
               if (!stbi__build_huffman(z->huff_dc+th, sizes)) return 0;
               v = z->huff_dc[th].values;
            } else {
               if (!stbi__build_huffman(z->huff_ac+th, sizes)) return 0;
               v = z->huff_ac[th].values;
            }
            for (i=0; i < n; ++i)
               v[i] = stbi__get8(z->s);
            if (tc != 0)
               stbi__build_fast_ac(z->fast_ac[th], z->huff_ac + th);
            L -= n;
         }
         return L==0;
   }

   // check for comment block or APP blocks
   if ((m >= 0xE0 && m <= 0xEF) || m == 0xFE) {
      L = stbi__get16be(z->s);
      if (L < 2) {
         if (m == 0xFE)
            return stbi__err("bad COM len","Corrupt JPEG");
         else
            return stbi__err("bad APP len","Corrupt JPEG");
      }
      L -= 2;

      if (m == 0xE0 && L >= 5) { // JFIF APP0 segment
         static const unsigned char tag[5] = {'J','F','I','F','\0'};
         int ok = 1;
         int i;
         for (i=0; i < 5; ++i)
            if (stbi__get8(z->s) != tag[i])
               ok = 0;
         L -= 5;
         if (ok)
            z->jfif = 1;
      } else if (m == 0xEE && L >= 12) { // Adobe APP14 segment
         static const unsigned char tag[6] = {'A','d','o','b','e','\0'};
         int ok = 1;
         int i;
         for (i=0; i < 6; ++i)
            if (stbi__get8(z->s) != tag[i])
               ok = 0;
         L -= 6;
         if (ok) {
            stbi__get8(z->s); // version
            stbi__get16be(z->s); // flags0
            stbi__get16be(z->s); // flags1
            z->app14_color_transform = stbi__get8(z->s); // color transform
            L -= 6;
         }
      }

      stbi__skip(z->s, L);
      return 1;
   }

   return stbi__err("unknown marker","Corrupt JPEG");
}

// after we see SOS
static int stbi__process_scan_header(stbi__jpeg *z)
{
   int i;
   int Ls = stbi__get16be(z->s);
   z->scan_n = stbi__get8(z->s);
   if (z->scan_n < 1 || z->scan_n > 4 || z->scan_n > (int) z->s->img_n) return stbi__err("bad SOS component count","Corrupt JPEG");
   if (Ls != 6+2*z->scan_n) return stbi__err("bad SOS len","Corrupt JPEG");
   for (i=0; i < z->scan_n; ++i) {
      int id = stbi__get8(z->s), which;
      int q = stbi__get8(z->s);
      for (which = 0; which < z->s->img_n; ++which)
         if (z->img_comp[which].id == id)
            break;
      if (which == z->s->img_n) return 0; // no match
      z->img_comp[which].hd = q >> 4;   if (z->img_comp[which].hd > 3) return stbi__err("bad DC huff","Corrupt JPEG");
      z->img_comp[which].ha = q & 15;   if (z->img_comp[which].ha > 3) return stbi__err("bad AC huff","Corrupt JPEG");
      z->order[i] = which;
   }

   {
      int aa;
      z->spec_start = stbi__get8(z->s);
      z->spec_end   = stbi__get8(z->s); // should be 63, but might be 0
      aa = stbi__get8(z->s);
      z->succ_high = (aa >> 4);
      z->succ_low  = (aa & 15);
      if (z->progressive) {
         if (z->spec_start > 63 || z->spec_end > 63  || z->spec_start > z->spec_end || z->succ_high > 13 || z->succ_low > 13)
            return stbi__err("bad SOS", "Corrupt JPEG");
      } else {
         if (z->spec_start != 0) return stbi__err("bad SOS","Corrupt JPEG");
         if (z->succ_high != 0 || z->succ_low != 0) return stbi__err("bad SOS","Corrupt JPEG");
         z->spec_end = 63;
      }
   }

   return 1;
}

static int stbi__free_jpeg_components(stbi__jpeg *z, int ncomp, int why)
{
   int i;
   for (i=0; i < ncomp; ++i) {
      if (z->img_comp[i].raw_data) {
         STBI_FREE(z->img_comp[i].raw_data);
         z->img_comp[i].raw_data = NULL;
         z->img_comp[i].data = NULL;
      }
      if (z->img_comp[i].raw_coeff) {
         STBI_FREE(z->img_comp[i].raw_coeff);
         z->img_comp[i].raw_coeff = 0;
         z->img_comp[i].coeff = 0;
      }
      if (z->img_comp[i].linebuf) {
         STBI_FREE(z->img_comp[i].linebuf);
         z->img_comp[i].linebuf = NULL;
      }
   }
   return why;
}

static int stbi__process_frame_header(stbi__jpeg *z, int scan)
{
   stbi__context *s = z->s;
   int Lf,p,i,q, h_max=1,v_max=1,c;
   Lf = stbi__get16be(s);         if (Lf < 11) return stbi__err("bad SOF len","Corrupt JPEG"); // JPEG
   p  = stbi__get8(s);            if (p != 8) return stbi__err("only 8-bit","JPEG format not supported: 8-bit only"); // JPEG baseline
   s->img_y = stbi__get16be(s);   if (s->img_y == 0) return stbi__err("no header height", "JPEG format not supported: delayed height"); // Legal, but we don't handle it--but neither does IJG
   s->img_x = stbi__get16be(s);   if (s->img_x == 0) return stbi__err("0 width","Corrupt JPEG"); // JPEG requires
   if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
   if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
   c = stbi__get8(s);
   if (c != 3 && c != 1 && c != 4) return stbi__err("bad component count","Corrupt JPEG");
   s->img_n = c;
   for (i=0; i < c; ++i) {
      z->img_comp[i].data = NULL;
      z->img_comp[i].linebuf = NULL;
   }

   if (Lf != 8+3*s->img_n) return stbi__err("bad SOF len","Corrupt JPEG");

   z->rgb = 0;
   for (i=0; i < s->img_n; ++i) {
      static const unsigned char rgb[3] = { 'R', 'G', 'B' };
      z->img_comp[i].id = stbi__get8(s);
      if (s->img_n == 3 && z->img_comp[i].id == rgb[i])
         ++z->rgb;
      q = stbi__get8(s);
      z->img_comp[i].h = (q >> 4);  if (!z->img_comp[i].h || z->img_comp[i].h > 4) return stbi__err("bad H","Corrupt JPEG");
      z->img_comp[i].v = q & 15;    if (!z->img_comp[i].v || z->img_comp[i].v > 4) return stbi__err("bad V","Corrupt JPEG");
      z->img_comp[i].tq = stbi__get8(s);  if (z->img_comp[i].tq > 3) return stbi__err("bad TQ","Corrupt JPEG");
   }

   if (scan != STBI__SCAN_load) return 1;

   if (!stbi__mad3sizes_valid(s->img_x, s->img_y, s->img_n, 0)) return stbi__err("too large", "Image too large to decode");

   for (i=0; i < s->img_n; ++i) {
      if (z->img_comp[i].h > h_max) h_max = z->img_comp[i].h;
      if (z->img_comp[i].v > v_max) v_max = z->img_comp[i].v;
   }

   // check that plane subsampling factors are integer ratios; our resamplers can't deal with fractional ratios
   // and I've never seen a non-corrupted JPEG file actually use them
   for (i=0; i < s->img_n; ++i) {
      if (h_max % z->img_comp[i].h != 0) return stbi__err("bad H","Corrupt JPEG");
      if (v_max % z->img_comp[i].v != 0) return stbi__err("bad V","Corrupt JPEG");
   }

   // compute interleaved mcu info
   z->img_h_max = h_max;
   z->img_v_max = v_max;
   z->img_mcu_w = h_max * 8;
   z->img_mcu_h = v_max * 8;
   // these sizes can't be more than 17 bits
   z->img_mcu_x = (s->img_x + z->img_mcu_w-1) / z->img_mcu_w;
   z->img_mcu_y = (s->img_y + z->img_mcu_h-1) / z->img_mcu_h;

   for (i=0; i < s->img_n; ++i) {
      // number of effective pixels (e.g. for non-interleaved MCU)
      z->img_comp[i].x = (s->img_x * z->img_comp[i].h + h_max-1) / h_max;
      z->img_comp[i].y = (s->img_y * z->img_comp[i].v + v_max-1) / v_max;
      // to simplify generation, we'll allocate enough memory to decode
      // the bogus oversized data from using interleaved MCUs and their
      // big blocks (e.g. a 16x16 iMCU on an image of width 33); we won't
      // discard the extra data until colorspace conversion
      //
      // img_mcu_x, img_mcu_y: <=17 bits; comp[i].h and .v are <=4 (checked earlier)
      // so these muls can't overflow with 32-bit ints (which we require)
      z->img_comp[i].w2 = z->img_mcu_x * z->img_comp[i].h * 8;
      z->img_comp[i].h2 = z->img_mcu_y * z->img_comp[i].v * 8;
      z->img_comp[i].coeff = 0;
      z->img_comp[i].raw_coeff = 0;
      z->img_comp[i].linebuf = NULL;
      z->img_comp[i].raw_data = stbi__malloc_mad2(z->img_comp[i].w2, z->img_comp[i].h2, 15);
      if (z->img_comp[i].raw_data == NULL)
         return stbi__free_jpeg_components(z, i+1, stbi__err("outofmem", "Out of memory"));
      // align blocks for idct using mmx/sse
      z->img_comp[i].data = (stbi_uc*) (((size_t) z->img_comp[i].raw_data + 15) & ~15);
      if (z->progressive) {
         // w2, h2 are multiples of 8 (see above)
         z->img_comp[i].coeff_w = z->img_comp[i].w2 / 8;
         z->img_comp[i].coeff_h = z->img_comp[i].h2 / 8;
         z->img_comp[i].raw_coeff = stbi__malloc_mad3(z->img_comp[i].w2, z->img_comp[i].h2, sizeof(short), 15);
         if (z->img_comp[i].raw_coeff == NULL)
            return stbi__free_jpeg_components(z, i+1, stbi__err("outofmem", "Out of memory"));
         z->img_comp[i].coeff = (short*) (((size_t) z->img_comp[i].raw_coeff + 15) & ~15);
      }
   }

   return 1;
}

// use comparisons since in some cases we handle more than one case (e.g. SOF)
#define stbi__DNL(x)         ((x) == 0xdc)
#define stbi__SOI(x)         ((x) == 0xd8)
#define stbi__EOI(x)         ((x) == 0xd9)
#define stbi__SOF(x)         ((x) == 0xc0 || (x) == 0xc1 || (x) == 0xc2)
#define stbi__SOS(x)         ((x) == 0xda)

#define stbi__SOF_progressive(x)   ((x) == 0xc2)

static int stbi__decode_jpeg_header(stbi__jpeg *z, int scan)
{
   int m;
   z->jfif = 0;
   z->app14_color_transform = -1; // valid values are 0,1,2
   z->marker = STBI__MARKER_none; // initialize cached marker to empty
   m = stbi__get_marker(z);
   if (!stbi__SOI(m)) return stbi__err("no SOI","Corrupt JPEG");
   if (scan == STBI__SCAN_type) return 1;
   m = stbi__get_marker(z);
   while (!stbi__SOF(m)) {
      if (!stbi__process_marker(z,m)) return 0;
      m = stbi__get_marker(z);
      while (m == STBI__MARKER_none) {
         // some files have extra padding after their blocks, so ok, we'll scan
         if (stbi__at_eof(z->s)) return stbi__err("no SOF", "Corrupt JPEG");
         m = stbi__get_marker(z);
      }
   }
   z->progressive = stbi__SOF_progressive(m);
   if (!stbi__process_frame_header(z, scan)) return 0;
   return 1;
}

static int stbi__skip_jpeg_junk_at_end(stbi__jpeg *j)
{
   // some JPEGs have junk at end, skip over it but if we find what looks
   // like a valid marker, resume there
   while (!stbi__at_eof(j->s)) {
      int x = stbi__get8(j->s);
      while (x == 255) { // might be a marker
         if (stbi__at_eof(j->s)) return STBI__MARKER_none;
         x = stbi__get8(j->s);
         if (x != 0x00 && x != 0xff) {
            // not a stuffed zero or lead-in to another marker, looks
            // like an actual marker, return it
            return x;
         }
         // stuffed zero has x=0 now which ends the loop, meaning we go
         // back to regular scan loop.
         // repeated 0xff keeps trying to read the next byte of the marker.
      }
   }
   return STBI__MARKER_none;
}

// decode image to YCbCr format
static int stbi__decode_jpeg_image(stbi__jpeg *j)
{
   int m;
   for (m = 0; m < 4; m++) {
      j->img_comp[m].raw_data = NULL;
      j->img_comp[m].raw_coeff = NULL;
   }
   j->restart_interval = 0;
   if (!stbi__decode_jpeg_header(j, STBI__SCAN_load)) return 0;
   m = stbi__get_marker(j);
   while (!stbi__EOI(m)) {
      if (stbi__SOS(m)) {
         if (!stbi__process_scan_header(j)) return 0;
         if (!stbi__parse_entropy_coded_data(j)) return 0;
         if (j->marker == STBI__MARKER_none ) {
         j->marker = stbi__skip_jpeg_junk_at_end(j);
            // if we reach eof without hitting a marker, stbi__get_marker() below will fail and we'll eventually return 0
         }
         m = stbi__get_marker(j);
         if (STBI__RESTART(m))
            m = stbi__get_marker(j);
      } else if (stbi__DNL(m)) {
         int Ld = stbi__get16be(j->s);
         stbi__uint32 NL = stbi__get16be(j->s);
         if (Ld != 4) return stbi__err("bad DNL len", "Corrupt JPEG");
         if (NL != j->s->img_y) return stbi__err("bad DNL height", "Corrupt JPEG");
         m = stbi__get_marker(j);
      } else {
         if (!stbi__process_marker(j, m)) return 1;
         m = stbi__get_marker(j);
      }
   }
   if (j->progressive)
      stbi__jpeg_finish(j);
   return 1;
}

// static jfif-centered resampling (across block boundaries)

typedef stbi_uc *(*resample_row_func)(stbi_uc *out, stbi_uc *in0, stbi_uc *in1,
                                    int w, int hs);

#define stbi__div4(x) ((stbi_uc) ((x) >> 2))

static stbi_uc *resample_row_1(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   STBI_NOTUSED(out);
   STBI_NOTUSED(in_far);
   STBI_NOTUSED(w);
   STBI_NOTUSED(hs);
   return in_near;
}

static stbi_uc* stbi__resample_row_v_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // need to generate two samples vertically for every one in input
   int i;
   STBI_NOTUSED(hs);
   for (i=0; i < w; ++i)
      out[i] = stbi__div4(3*in_near[i] + in_far[i] + 2);
   return out;
}

static stbi_uc*  stbi__resample_row_h_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // need to generate two samples horizontally for every one in input
   int i;
   stbi_uc *input = in_near;

   if (w == 1) {
      // if only one sample, can't do any interpolation
      out[0] = out[1] = input[0];
      return out;
   }

   out[0] = input[0];
   out[1] = stbi__div4(input[0]*3 + input[1] + 2);
   for (i=1; i < w-1; ++i) {
      int n = 3*input[i]+2;
      out[i*2+0] = stbi__div4(n+input[i-1]);
      out[i*2+1] = stbi__div4(n+input[i+1]);
   }
   out[i*2+0] = stbi__div4(input[w-2]*3 + input[w-1] + 2);
   out[i*2+1] = input[w-1];

   STBI_NOTUSED(in_far);
   STBI_NOTUSED(hs);

   return out;
}

#define stbi__div16(x) ((stbi_uc) ((x) >> 4))

static stbi_uc *stbi__resample_row_hv_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // need to generate 2x2 samples for every one in input
   int i,t0,t1;
   if (w == 1) {
      out[0] = out[1] = stbi__div4(3*in_near[0] + in_far[0] + 2);
      return out;
   }

   t1 = 3*in_near[0] + in_far[0];
   out[0] = stbi__div4(t1+2);
   for (i=1; i < w; ++i) {
      t0 = t1;
      t1 = 3*in_near[i]+in_far[i];
      out[i*2-1] = stbi__div16(3*t0 + t1 + 8);
      out[i*2  ] = stbi__div16(3*t1 + t0 + 8);
   }
   out[w*2-1] = stbi__div4(t1+2);

   STBI_NOTUSED(hs);

   return out;
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static stbi_uc *stbi__resample_row_hv_2_simd(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // need to generate 2x2 samples for every one in input
   int i=0,t0,t1;

   if (w == 1) {
      out[0] = out[1] = stbi__div4(3*in_near[0] + in_far[0] + 2);
      return out;
   }

   t1 = 3*in_near[0] + in_far[0];
   // process groups of 8 pixels for as long as we can.
   // note we can't handle the last pixel in a row in this loop
   // because we need to handle the filter boundary conditions.
   for (; i < ((w-1) & ~7); i += 8) {
#if defined(STBI_SSE2)
      // load and perform the vertical filtering pass
      // this uses 3*x + y = 4*x + (y - x)
      __m128i zero  = _mm_setzero_si128();
      __m128i farb  = _mm_loadl_epi64((__m128i *) (in_far + i));
      __m128i nearb = _mm_loadl_epi64((__m128i *) (in_near + i));
      __m128i farw  = _mm_unpacklo_epi8(farb, zero);
      __m128i nearw = _mm_unpacklo_epi8(nearb, zero);
      __m128i diff  = _mm_sub_epi16(farw, nearw);
      __m128i nears = _mm_slli_epi16(nearw, 2);
      __m128i curr  = _mm_add_epi16(nears, diff); // current row

      // horizontal filter works the same based on shifted vers of current
      // row. "prev" is current row shifted right by 1 pixel; we need to
      // insert the previous pixel value (from t1).
      // "next" is current row shifted left by 1 pixel, with first pixel
      // of next block of 8 pixels added in.
      __m128i prv0 = _mm_slli_si128(curr, 2);
      __m128i nxt0 = _mm_srli_si128(curr, 2);
      __m128i prev = _mm_insert_epi16(prv0, t1, 0);
      __m128i next = _mm_insert_epi16(nxt0, 3*in_near[i+8] + in_far[i+8], 7);

      // horizontal filter, polyphase implementation since it's convenient:
      // even pixels = 3*cur + prev = cur*4 + (prev - cur)
      // odd  pixels = 3*cur + next = cur*4 + (next - cur)
      // note the shared term.
      __m128i bias  = _mm_set1_epi16(8);
      __m128i curs = _mm_slli_epi16(curr, 2);
      __m128i prvd = _mm_sub_epi16(prev, curr);
      __m128i nxtd = _mm_sub_epi16(next, curr);
      __m128i curb = _mm_add_epi16(curs, bias);
      __m128i even = _mm_add_epi16(prvd, curb);
      __m128i odd  = _mm_add_epi16(nxtd, curb);

      // interleave even and odd pixels, then undo scaling.
      __m128i int0 = _mm_unpacklo_epi16(even, odd);
      __m128i int1 = _mm_unpackhi_epi16(even, odd);
      __m128i de0  = _mm_srli_epi16(int0, 4);
      __m128i de1  = _mm_srli_epi16(int1, 4);

      // pack and write output
      __m128i outv = _mm_packus_epi16(de0, de1);
      _mm_storeu_si128((__m128i *) (out + i*2), outv);
#elif defined(STBI_NEON)
      // load and perform the vertical filtering pass
      // this uses 3*x + y = 4*x + (y - x)
      uint8x8_t farb  = vld1_u8(in_far + i);
      uint8x8_t nearb = vld1_u8(in_near + i);
      int16x8_t diff  = vreinterpretq_s16_u16(vsubl_u8(farb, nearb));
      int16x8_t nears = vreinterpretq_s16_u16(vshll_n_u8(nearb, 2));
      int16x8_t curr  = vaddq_s16(nears, diff); // current row

      // horizontal filter works the same based on shifted vers of current
      // row. "prev" is current row shifted right by 1 pixel; we need to
      // insert the previous pixel value (from t1).
      // "next" is current row shifted left by 1 pixel, with first pixel
      // of next block of 8 pixels added in.
      int16x8_t prv0 = vextq_s16(curr, curr, 7);
      int16x8_t nxt0 = vextq_s16(curr, curr, 1);
      int16x8_t prev = vsetq_lane_s16(t1, prv0, 0);
      int16x8_t next = vsetq_lane_s16(3*in_near[i+8] + in_far[i+8], nxt0, 7);

      // horizontal filter, polyphase implementation since it's convenient:
      // even pixels = 3*cur + prev = cur*4 + (prev - cur)
      // odd  pixels = 3*cur + next = cur*4 + (next - cur)
      // note the shared term.
      int16x8_t curs = vshlq_n_s16(curr, 2);
      int16x8_t prvd = vsubq_s16(prev, curr);
      int16x8_t nxtd = vsubq_s16(next, curr);
      int16x8_t even = vaddq_s16(curs, prvd);
      int16x8_t odd  = vaddq_s16(curs, nxtd);

      // undo scaling and round, then store with even/odd phases interleaved
      uint8x8x2_t o;
      o.val[0] = vqrshrun_n_s16(even, 4);
      o.val[1] = vqrshrun_n_s16(odd,  4);
      vst2_u8(out + i*2, o);
#endif

      // "previous" value for next iter
      t1 = 3*in_near[i+7] + in_far[i+7];
   }

   t0 = t1;
   t1 = 3*in_near[i] + in_far[i];
   out[i*2] = stbi__div16(3*t1 + t0 + 8);

   for (++i; i < w; ++i) {
      t0 = t1;
      t1 = 3*in_near[i]+in_far[i];
      out[i*2-1] = stbi__div16(3*t0 + t1 + 8);
      out[i*2  ] = stbi__div16(3*t1 + t0 + 8);
   }
   out[w*2-1] = stbi__div4(t1+2);

   STBI_NOTUSED(hs);

   return out;
}
#endif

static stbi_uc *stbi__resample_row_generic(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs)
{
   // resample with nearest-neighbor
   int i,j;
   STBI_NOTUSED(in_far);
   for (i=0; i < w; ++i)
      for (j=0; j < hs; ++j)
         out[i*hs+j] = in_near[i];
   return out;
}

// this is a reduced-precision calculation of YCbCr-to-RGB introduced
// to make sure the code produces the same results in both SIMD and scalar
#define stbi__float2fixed(x)  (((int) ((x) * 4096.0f + 0.5f)) << 8)
static void stbi__YCbCr_to_RGB_row(stbi_uc *out, const stbi_uc *y, const stbi_uc *pcb, const stbi_uc *pcr, int count, int step)
{
   int i;
   for (i=0; i < count; ++i) {
      int y_fixed = (y[i] << 20) + (1<<19); // rounding
      int r,g,b;
      int cr = pcr[i] - 128;
      int cb = pcb[i] - 128;
      r = y_fixed +  cr* stbi__float2fixed(1.40200f);
      g = y_fixed + (cr*-stbi__float2fixed(0.71414f)) + ((cb*-stbi__float2fixed(0.34414f)) & 0xffff0000);
      b = y_fixed                                     +   cb* stbi__float2fixed(1.77200f);
      r >>= 20;
      g >>= 20;
      b >>= 20;
      if ((unsigned) r > 255) { if (r < 0) r = 0; else r = 255; }
      if ((unsigned) g > 255) { if (g < 0) g = 0; else g = 255; }
      if ((unsigned) b > 255) { if (b < 0) b = 0; else b = 255; }
      out[0] = (stbi_uc)r;
      out[1] = (stbi_uc)g;
      out[2] = (stbi_uc)b;
      out[3] = 255;
      out += step;
   }
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static void stbi__YCbCr_to_RGB_simd(stbi_uc *out, stbi_uc const *y, stbi_uc const *pcb, stbi_uc const *pcr, int count, int step)
{
   int i = 0;

#ifdef STBI_SSE2
   // step == 3 is pretty ugly on the final interleave, and i'm not convinced
   // it's useful in practice (you wouldn't use it for textures, for example).
   // so just accelerate step == 4 case.
   if (step == 4) {
      // this is a fairly straightforward implementation and not super-optimized.
      __m128i signflip  = _mm_set1_epi8(-0x80);
      __m128i cr_const0 = _mm_set1_epi16(   (short) ( 1.40200f*4096.0f+0.5f));
      __m128i cr_const1 = _mm_set1_epi16( - (short) ( 0.71414f*4096.0f+0.5f));
      __m128i cb_const0 = _mm_set1_epi16( - (short) ( 0.34414f*4096.0f+0.5f));
      __m128i cb_const1 = _mm_set1_epi16(   (short) ( 1.77200f*4096.0f+0.5f));
      __m128i y_bias = _mm_set1_epi8((char) (unsigned char) 128);
      __m128i xw = _mm_set1_epi16(255); // alpha channel

      for (; i+7 < count; i += 8) {
         // load
         __m128i y_bytes = _mm_loadl_epi64((__m128i *) (y+i));
         __m128i cr_bytes = _mm_loadl_epi64((__m128i *) (pcr+i));
         __m128i cb_bytes = _mm_loadl_epi64((__m128i *) (pcb+i));
         __m128i cr_biased = _mm_xor_si128(cr_bytes, signflip); // -128
         __m128i cb_biased = _mm_xor_si128(cb_bytes, signflip); // -128

         // unpack to short (and left-shift cr, cb by 8)
         __m128i yw  = _mm_unpacklo_epi8(y_bias, y_bytes);
         __m128i crw = _mm_unpacklo_epi8(_mm_setzero_si128(), cr_biased);
         __m128i cbw = _mm_unpacklo_epi8(_mm_setzero_si128(), cb_biased);

         // color transform
         __m128i yws = _mm_srli_epi16(yw, 4);
         __m128i cr0 = _mm_mulhi_epi16(cr_const0, crw);
         __m128i cb0 = _mm_mulhi_epi16(cb_const0, cbw);
         __m128i cb1 = _mm_mulhi_epi16(cbw, cb_const1);
         __m128i cr1 = _mm_mulhi_epi16(crw, cr_const1);
         __m128i rws = _mm_add_epi16(cr0, yws);
         __m128i gwt = _mm_add_epi16(cb0, yws);
         __m128i bws = _mm_add_epi16(yws, cb1);
         __m128i gws = _mm_add_epi16(gwt, cr1);

         // descale
         __m128i rw = _mm_srai_epi16(rws, 4);
         __m128i bw = _mm_srai_epi16(bws, 4);
         __m128i gw = _mm_srai_epi16(gws, 4);

         // back to byte, set up for transpose
         __m128i brb = _mm_packus_epi16(rw, bw);
         __m128i gxb = _mm_packus_epi16(gw, xw);

         // transpose to interleave channels
         __m128i t0 = _mm_unpacklo_epi8(brb, gxb);
         __m128i t1 = _mm_unpackhi_epi8(brb, gxb);
         __m128i o0 = _mm_unpacklo_epi16(t0, t1);
         __m128i o1 = _mm_unpackhi_epi16(t0, t1);

         // store
         _mm_storeu_si128((__m128i *) (out + 0), o0);
         _mm_storeu_si128((__m128i *) (out + 16), o1);
         out += 32;
      }
   }
#endif

#ifdef STBI_NEON
   // in this version, step=3 support would be easy to add. but is there demand?
   if (step == 4) {
      // this is a fairly straightforward implementation and not super-optimized.
      uint8x8_t signflip = vdup_n_u8(0x80);
      int16x8_t cr_const0 = vdupq_n_s16(   (short) ( 1.40200f*4096.0f+0.5f));
      int16x8_t cr_const1 = vdupq_n_s16( - (short) ( 0.71414f*4096.0f+0.5f));
      int16x8_t cb_const0 = vdupq_n_s16( - (short) ( 0.34414f*4096.0f+0.5f));
      int16x8_t cb_const1 = vdupq_n_s16(   (short) ( 1.77200f*4096.0f+0.5f));

      for (; i+7 < count; i += 8) {
         // load
         uint8x8_t y_bytes  = vld1_u8(y + i);
         uint8x8_t cr_bytes = vld1_u8(pcr + i);
         uint8x8_t cb_bytes = vld1_u8(pcb + i);
         int8x8_t cr_biased = vreinterpret_s8_u8(vsub_u8(cr_bytes, signflip));
         int8x8_t cb_biased = vreinterpret_s8_u8(vsub_u8(cb_bytes, signflip));

         // expand to s16
         int16x8_t yws = vreinterpretq_s16_u16(vshll_n_u8(y_bytes, 4));
         int16x8_t crw = vshll_n_s8(cr_biased, 7);
         int16x8_t cbw = vshll_n_s8(cb_biased, 7);

         // color transform
         int16x8_t cr0 = vqdmulhq_s16(crw, cr_const0);
         int16x8_t cb0 = vqdmulhq_s16(cbw, cb_const0);
         int16x8_t cr1 = vqdmulhq_s16(crw, cr_const1);
         int16x8_t cb1 = vqdmulhq_s16(cbw, cb_const1);
         int16x8_t rws = vaddq_s16(yws, cr0);
         int16x8_t gws = vaddq_s16(vaddq_s16(yws, cb0), cr1);
         int16x8_t bws = vaddq_s16(yws, cb1);

         // undo scaling, round, convert to byte
         uint8x8x4_t o;
         o.val[0] = vqrshrun_n_s16(rws, 4);
         o.val[1] = vqrshrun_n_s16(gws, 4);
         o.val[2] = vqrshrun_n_s16(bws, 4);
         o.val[3] = vdup_n_u8(255);

         // store, interleaving r/g/b/a
         vst4_u8(out, o);
         out += 8*4;
      }
   }
#endif

   for (; i < count; ++i) {
      int y_fixed = (y[i] << 20) + (1<<19); // rounding
      int r,g,b;
      int cr = pcr[i] - 128;
      int cb = pcb[i] - 128;
      r = y_fixed + cr* stbi__float2fixed(1.40200f);
      g = y_fixed + cr*-stbi__float2fixed(0.71414f) + ((cb*-stbi__float2fixed(0.34414f)) & 0xffff0000);
      b = y_fixed                                   +   cb* stbi__float2fixed(1.77200f);
      r >>= 20;
      g >>= 20;
      b >>= 20;
      if ((unsigned) r > 255) { if (r < 0) r = 0; else r = 255; }
      if ((unsigned) g > 255) { if (g < 0) g = 0; else g = 255; }
      if ((unsigned) b > 255) { if (b < 0) b = 0; else b = 255; }
      out[0] = (stbi_uc)r;
      out[1] = (stbi_uc)g;
      out[2] = (stbi_uc)b;
      out[3] = 255;
      out += step;
   }
}
#endif

// set up the kernels
static void stbi__setup_jpeg(stbi__jpeg *j)
{
   j->idct_block_kernel = stbi__idct_block;
   j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_row;
   j->resample_row_hv_2_kernel = stbi__resample_row_hv_2;

#ifdef STBI_SSE2
   if (stbi__sse2_available()) {
      j->idct_block_kernel = stbi__idct_simd;
      j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_simd;
      j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
   }
#endif

#ifdef STBI_NEON
   j->idct_block_kernel = stbi__idct_simd;
   j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_simd;
   j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
#endif
}

// clean up the temporary component buffers
static void stbi__cleanup_jpeg(stbi__jpeg *j)
{
   stbi__free_jpeg_components(j, j->s->img_n, 0);
}

typedef struct
{
   resample_row_func resample;
   stbi_uc *line0,*line1;
   int hs,vs;   // expansion factor in each axis
   int w_lores; // horizontal pixels pre-expansion
   int ystep;   // how far through vertical expansion we are
   int ypos;    // which pre-expansion row we're on
} stbi__resample;

// fast 0..255 * 0..255 => 0..255 rounded multiplication
static stbi_uc stbi__blinn_8x8(stbi_uc x, stbi_uc y)
{
   unsigned int t = x*y + 128;
   return (stbi_uc) ((t + (t >>8)) >> 8);
}

static stbi_uc *load_jpeg_image(stbi__jpeg *z, int *out_x, int *out_y, int *comp, int req_comp)
{
   int n, decode_n, is_rgb;
   z->s->img_n = 0; // make stbi__cleanup_jpeg safe

   // validate req_comp
   if (req_comp < 0 || req_comp > 4) return stbi__errpuc("bad req_comp", "Internal error");

   // load a jpeg image from whichever source, but leave in YCbCr format
   if (!stbi__decode_jpeg_image(z)) { stbi__cleanup_jpeg(z); return NULL; }

   // determine actual number of components to generate
   n = req_comp ? req_comp : z->s->img_n >= 3 ? 3 : 1;

   is_rgb = z->s->img_n == 3 && (z->rgb == 3 || (z->app14_color_transform == 0 && !z->jfif));

   if (z->s->img_n == 3 && n < 3 && !is_rgb)
      decode_n = 1;
   else
      decode_n = z->s->img_n;

   // nothing to do if no components requested; check this now to avoid
   // accessing uninitialized coutput[0] later
   if (decode_n <= 0) { stbi__cleanup_jpeg(z); return NULL; }

   // resample and color-convert
   {
      int k;
      unsigned int i,j;
      stbi_uc *output;
      stbi_uc *coutput[4] = { NULL, NULL, NULL, NULL };

      stbi__resample res_comp[4];

      for (k=0; k < decode_n; ++k) {
         stbi__resample *r = &res_comp[k];

         // allocate line buffer big enough for upsampling off the edges
         // with upsample factor of 4
         z->img_comp[k].linebuf = (stbi_uc *) stbi__malloc(z->s->img_x + 3);
         if (!z->img_comp[k].linebuf) { stbi__cleanup_jpeg(z); return stbi__errpuc("outofmem", "Out of memory"); }

         r->hs      = z->img_h_max / z->img_comp[k].h;
         r->vs      = z->img_v_max / z->img_comp[k].v;
         r->ystep   = r->vs >> 1;
         r->w_lores = (z->s->img_x + r->hs-1) / r->hs;
         r->ypos    = 0;
         r->line0   = r->line1 = z->img_comp[k].data;

         if      (r->hs == 1 && r->vs == 1) r->resample = resample_row_1;
         else if (r->hs == 1 && r->vs == 2) r->resample = stbi__resample_row_v_2;
         else if (r->hs == 2 && r->vs == 1) r->resample = stbi__resample_row_h_2;
         else if (r->hs == 2 && r->vs == 2) r->resample = z->resample_row_hv_2_kernel;
         else                               r->resample = stbi__resample_row_generic;
      }

      // can't error after this so, this is safe
      output = (stbi_uc *) stbi__malloc_mad3(n, z->s->img_x, z->s->img_y, 1);
      if (!output) { stbi__cleanup_jpeg(z); return stbi__errpuc("outofmem", "Out of memory"); }

      // now go ahead and resample
      for (j=0; j < z->s->img_y; ++j) {
         stbi_uc *out = output + n * z->s->img_x * j;
         for (k=0; k < decode_n; ++k) {
            stbi__resample *r = &res_comp[k];
            int y_bot = r->ystep >= (r->vs >> 1);
            coutput[k] = r->resample(z->img_comp[k].linebuf,
                                     y_bot ? r->line1 : r->line0,
                                     y_bot ? r->line0 : r->line1,
                                     r->w_lores, r->hs);
            if (++r->ystep >= r->vs) {
               r->ystep = 0;
               r->line0 = r->line1;
               if (++r->ypos < z->img_comp[k].y)
                  r->line1 += z->img_comp[k].w2;
            }
         }
         if (n >= 3) {
            stbi_uc *y = coutput[0];
            if (z->s->img_n == 3) {
               if (is_rgb) {
                  for (i=0; i < z->s->img_x; ++i) {
                     out[0] = y[i];
                     out[1] = coutput[1][i];
                     out[2] = coutput[2][i];
                     out[3] = 255;
                     out += n;
                  }
               } else {
                  z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
               }
            } else if (z->s->img_n == 4) {
               if (z->app14_color_transform == 0) { // CMYK
                  for (i=0; i < z->s->img_x; ++i) {
                     stbi_uc m = coutput[3][i];
                     out[0] = stbi__blinn_8x8(coutput[0][i], m);
                     out[1] = stbi__blinn_8x8(coutput[1][i], m);
                     out[2] = stbi__blinn_8x8(coutput[2][i], m);
                     out[3] = 255;
                     out += n;
                  }
               } else if (z->app14_color_transform == 2) { // YCCK
                  z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
                  for (i=0; i < z->s->img_x; ++i) {
                     stbi_uc m = coutput[3][i];
                     out[0] = stbi__blinn_8x8(255 - out[0], m);
                     out[1] = stbi__blinn_8x8(255 - out[1], m);
                     out[2] = stbi__blinn_8x8(255 - out[2], m);
                     out += n;
                  }
               } else { // YCbCr + alpha?  Ignore the fourth channel for now
                  z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x, n);
               }
            } else
               for (i=0; i < z->s->img_x; ++i) {
                  out[0] = out[1] = out[2] = y[i];
                  out[3] = 255; // not used if n==3
                  out += n;
               }
         } else {
            if (is_rgb) {
               if (n == 1)
                  for (i=0; i < z->s->img_x; ++i)
                     *out++ = stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
               else {
                  for (i=0; i < z->s->img_x; ++i, out += 2) {
                     out[0] = stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
                     out[1] = 255;
                  }
               }
            } else if (z->s->img_n == 4 && z->app14_color_transform == 0) {
               for (i=0; i < z->s->img_x; ++i) {
                  stbi_uc m = coutput[3][i];
                  stbi_uc r = stbi__blinn_8x8(coutput[0][i], m);
                  stbi_uc g = stbi__blinn_8x8(coutput[1][i], m);
                  stbi_uc b = stbi__blinn_8x8(coutput[2][i], m);
                  out[0] = stbi__compute_y(r, g, b);
                  out[1] = 255;
                  out += n;
               }
            } else if (z->s->img_n == 4 && z->app14_color_transform == 2) {
               for (i=0; i < z->s->img_x; ++i) {
                  out[0] = stbi__blinn_8x8(255 - coutput[0][i], coutput[3][i]);
                  out[1] = 255;
                  out += n;
               }
            } else {
               stbi_uc *y = coutput[0];
               if (n == 1)
                  for (i=0; i < z->s->img_x; ++i) out[i] = y[i];
               else
                  for (i=0; i < z->s->img_x; ++i) { *out++ = y[i]; *out++ = 255; }
            }
         }
      }
      stbi__cleanup_jpeg(z);
      *out_x = z->s->img_x;
      *out_y = z->s->img_y;
      if (comp) *comp = z->s->img_n >= 3 ? 3 : 1; // report original components, not output
      return output;
   }
}

static void *stbi__jpeg_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   unsigned char* result;
   stbi__jpeg* j = (stbi__jpeg*) stbi__malloc(sizeof(stbi__jpeg));
   if (!j) return stbi__errpuc("outofmem", "Out of memory");
   memset(j, 0, sizeof(stbi__jpeg));
   STBI_NOTUSED(ri);
   j->s = s;
   stbi__setup_jpeg(j);
   result = load_jpeg_image(j, x,y,comp,req_comp);
   STBI_FREE(j);
   return result;
}

static int stbi__jpeg_test(stbi__context *s)
{
   int r;
   stbi__jpeg* j = (stbi__jpeg*)stbi__malloc(sizeof(stbi__jpeg));
   if (!j) return stbi__err("outofmem", "Out of memory");
   memset(j, 0, sizeof(stbi__jpeg));
   j->s = s;
   stbi__setup_jpeg(j);
   r = stbi__decode_jpeg_header(j, STBI__SCAN_type);
   stbi__rewind(s);
   STBI_FREE(j);
   return r;
}

static int stbi__jpeg_info_raw(stbi__jpeg *j, int *x, int *y, int *comp)
{
   if (!stbi__decode_jpeg_header(j, STBI__SCAN_header)) {
      stbi__rewind( j->s );
      return 0;
   }
   if (x) *x = j->s->img_x;
   if (y) *y = j->s->img_y;
   if (comp) *comp = j->s->img_n >= 3 ? 3 : 1;
   return 1;
}

static int stbi__jpeg_info(stbi__context *s, int *x, int *y, int *comp)
{
   int result;
   stbi__jpeg* j = (stbi__jpeg*) (stbi__malloc(sizeof(stbi__jpeg)));
   if (!j) return stbi__err("outofmem", "Out of memory");
   memset(j, 0, sizeof(stbi__jpeg));
   j->s = s;
   result = stbi__jpeg_info_raw(j, x, y, comp);
   STBI_FREE(j);
   return result;
}
#endif

// public domain zlib decode    v0.2  Sean Barrett 2006-11-18
//    simple implementation
//      - all input must be provided in an upfront buffer
//      - all output is written to a single output buffer (can malloc/realloc)
//    performance
//      - fast huffman

#ifndef STBI_NO_ZLIB

// fast-way is faster to check than jpeg huffman, but slow way is slower
#define STBI__ZFAST_BITS  9 // accelerate all cases in default tables
#define STBI__ZFAST_MASK  ((1 << STBI__ZFAST_BITS) - 1)
#define STBI__ZNSYMS 288 // number of symbols in literal/length alphabet

// zlib-style huffman encoding
// (jpegs packs from left, zlib from right, so can't share code)
typedef struct
{
   stbi__uint16 fast[1 << STBI__ZFAST_BITS];
   stbi__uint16 firstcode[16];
   int maxcode[17];
   stbi__uint16 firstsymbol[16];
   stbi_uc  size[STBI__ZNSYMS];
   stbi__uint16 value[STBI__ZNSYMS];
} stbi__zhuffman;

stbi_inline static int stbi__bitreverse16(int n)
{
  n = ((n & 0xAAAA) >>  1) | ((n & 0x5555) << 1);
  n = ((n & 0xCCCC) >>  2) | ((n & 0x3333) << 2);
  n = ((n & 0xF0F0) >>  4) | ((n & 0x0F0F) << 4);
  n = ((n & 0xFF00) >>  8) | ((n & 0x00FF) << 8);
  return n;
}

stbi_inline static int stbi__bit_reverse(int v, int bits)
{
   STBI_ASSERT(bits <= 16);
   // to bit reverse n bits, reverse 16 and shift
   // e.g. 11 bits, bit reverse and shift away 5
   return stbi__bitreverse16(v) >> (16-bits);
}

static int stbi__zbuild_huffman(stbi__zhuffman *z, const stbi_uc *sizelist, int num)
{
   int i,k=0;
   int code, next_code[16], sizes[17];

   // DEFLATE spec for generating codes
   memset(sizes, 0, sizeof(sizes));
   memset(z->fast, 0, sizeof(z->fast));
   for (i=0; i < num; ++i)
      ++sizes[sizelist[i]];
   sizes[0] = 0;
   for (i=1; i < 16; ++i)
      if (sizes[i] > (1 << i))
         return stbi__err("bad sizes", "Corrupt PNG");
   code = 0;
   for (i=1; i < 16; ++i) {
      next_code[i] = code;
      z->firstcode[i] = (stbi__uint16) code;
      z->firstsymbol[i] = (stbi__uint16) k;
      code = (code + sizes[i]);
      if (sizes[i])
         if (code-1 >= (1 << i)) return stbi__err("bad codelengths","Corrupt PNG");
      z->maxcode[i] = code << (16-i); // preshift for inner loop
      code <<= 1;
      k += sizes[i];
   }
   z->maxcode[16] = 0x10000; // sentinel
   for (i=0; i < num; ++i) {
      int s = sizelist[i];
      if (s) {
         int c = next_code[s] - z->firstcode[s] + z->firstsymbol[s];
         stbi__uint16 fastv = (stbi__uint16) ((s << 9) | i);
         z->size [c] = (stbi_uc     ) s;
         z->value[c] = (stbi__uint16) i;
         if (s <= STBI__ZFAST_BITS) {
            int j = stbi__bit_reverse(next_code[s],s);
            while (j < (1 << STBI__ZFAST_BITS)) {
               z->fast[j] = fastv;
               j += (1 << s);
            }
         }
         ++next_code[s];
      }
   }
   return 1;
}

// zlib-from-memory implementation for PNG reading
//    because PNG allows splitting the zlib stream arbitrarily,
//    and it's annoying structurally to have PNG call ZLIB call PNG,
//    we require PNG read all the IDATs and combine them into a single
//    memory buffer

typedef struct
{
   stbi_uc *zbuffer, *zbuffer_end;
   int num_bits;
   stbi__uint32 code_buffer;

   char *zout;
   char *zout_start;
   char *zout_end;
   int   z_expandable;

   stbi__zhuffman z_length, z_distance;
} stbi__zbuf;

stbi_inline static int stbi__zeof(stbi__zbuf *z)
{
   return (z->zbuffer >= z->zbuffer_end);
}

stbi_inline static stbi_uc stbi__zget8(stbi__zbuf *z)
{
   return stbi__zeof(z) ? 0 : *z->zbuffer++;
}

static void stbi__fill_bits(stbi__zbuf *z)
{
   do {
      if (z->code_buffer >= (1U << z->num_bits)) {
        z->zbuffer = z->zbuffer_end;  /* treat this as EOF so we fail. */
        return;
      }
      z->code_buffer |= (unsigned int) stbi__zget8(z) << z->num_bits;
      z->num_bits += 8;
   } while (z->num_bits <= 24);
}

stbi_inline static unsigned int stbi__zreceive(stbi__zbuf *z, int n)
{
   unsigned int k;
   if (z->num_bits < n) stbi__fill_bits(z);
   k = z->code_buffer & ((1 << n) - 1);
   z->code_buffer >>= n;
   z->num_bits -= n;
   return k;
}

static int stbi__zhuffman_decode_slowpath(stbi__zbuf *a, stbi__zhuffman *z)
{
   int b,s,k;
   // not resolved by fast table, so compute it the slow way
   // use jpeg approach, which requires MSbits at top
   k = stbi__bit_reverse(a->code_buffer, 16);
   for (s=STBI__ZFAST_BITS+1; ; ++s)
      if (k < z->maxcode[s])
         break;
   if (s >= 16) return -1; // invalid code!
   // code size is s, so:
   b = (k >> (16-s)) - z->firstcode[s] + z->firstsymbol[s];
   if (b >= STBI__ZNSYMS) return -1; // some data was corrupt somewhere!
   if (z->size[b] != s) return -1;  // was originally an assert, but report failure instead.
   a->code_buffer >>= s;
   a->num_bits -= s;
   return z->value[b];
}

stbi_inline static int stbi__zhuffman_decode(stbi__zbuf *a, stbi__zhuffman *z)
{
   int b,s;
   if (a->num_bits < 16) {
      if (stbi__zeof(a)) {
         return -1;   /* report error for unexpected end of data. */
      }
      stbi__fill_bits(a);
   }
   b = z->fast[a->code_buffer & STBI__ZFAST_MASK];
   if (b) {
      s = b >> 9;
      a->code_buffer >>= s;
      a->num_bits -= s;
      return b & 511;
   }
   return stbi__zhuffman_decode_slowpath(a, z);
}

static int stbi__zexpand(stbi__zbuf *z, char *zout, int n)  // need to make room for n bytes
{
   char *q;
   unsigned int cur, limit, old_limit;
   z->zout = zout;
   if (!z->z_expandable) return stbi__err("output buffer limit","Corrupt PNG");
   cur   = (unsigned int) (z->zout - z->zout_start);
   limit = old_limit = (unsigned) (z->zout_end - z->zout_start);
   if (UINT_MAX - cur < (unsigned) n) return stbi__err("outofmem", "Out of memory");
   while (cur + n > limit) {
      if(limit > UINT_MAX / 2) return stbi__err("outofmem", "Out of memory");
      limit *= 2;
   }
   q = (char *) STBI_REALLOC_SIZED(z->zout_start, old_limit, limit);
   STBI_NOTUSED(old_limit);
   if (q == NULL) return stbi__err("outofmem", "Out of memory");
   z->zout_start = q;
   z->zout       = q + cur;
   z->zout_end   = q + limit;
   return 1;
}

static const int stbi__zlength_base[31] = {
   3,4,5,6,7,8,9,10,11,13,
   15,17,19,23,27,31,35,43,51,59,
   67,83,99,115,131,163,195,227,258,0,0 };

static const int stbi__zlength_extra[31]=
{ 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };

static const int stbi__zdist_base[32] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};

static const int stbi__zdist_extra[32] =
{ 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

static int stbi__parse_huffman_block(stbi__zbuf *a)
{
   char *zout = a->zout;
   for(;;) {
      int z = stbi__zhuffman_decode(a, &a->z_length);
      if (z < 256) {
         if (z < 0) return stbi__err("bad huffman code","Corrupt PNG"); // error in huffman codes
         if (zout >= a->zout_end) {
            if (!stbi__zexpand(a, zout, 1)) return 0;
            zout = a->zout;
         }
         *zout++ = (char) z;
      } else {
         stbi_uc *p;
         int len,dist;
         if (z == 256) {
            a->zout = zout;
            return 1;
         }
         if (z >= 286) return stbi__err("bad huffman code","Corrupt PNG"); // per DEFLATE, length codes 286 and 287 must not appear in compressed data
         z -= 257;
         len = stbi__zlength_base[z];
         if (stbi__zlength_extra[z]) len += stbi__zreceive(a, stbi__zlength_extra[z]);
         z = stbi__zhuffman_decode(a, &a->z_distance);
         if (z < 0 || z >= 30) return stbi__err("bad huffman code","Corrupt PNG"); // per DEFLATE, distance codes 30 and 31 must not appear in compressed data
         dist = stbi__zdist_base[z];
         if (stbi__zdist_extra[z]) dist += stbi__zreceive(a, stbi__zdist_extra[z]);
         if (zout - a->zout_start < dist) return stbi__err("bad dist","Corrupt PNG");
         if (zout + len > a->zout_end) {
            if (!stbi__zexpand(a, zout, len)) return 0;
            zout = a->zout;
         }
         p = (stbi_uc *) (zout - dist);
         if (dist == 1) { // run of one byte; common in images.
            stbi_uc v = *p;
            if (len) { do *zout++ = v; while (--len); }
         } else {
            if (len) { do *zout++ = *p++; while (--len); }
         }
      }
   }
}

static int stbi__compute_huffman_codes(stbi__zbuf *a)
{
   static const stbi_uc length_dezigzag[19] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
   stbi__zhuffman z_codelength;
   stbi_uc lencodes[286+32+137];//padding for maximum single op
   stbi_uc codelength_sizes[19];
   int i,n;

   int hlit  = stbi__zreceive(a,5) + 257;
   int hdist = stbi__zreceive(a,5) + 1;
   int hclen = stbi__zreceive(a,4) + 4;
   int ntot  = hlit + hdist;

   memset(codelength_sizes, 0, sizeof(codelength_sizes));
   for (i=0; i < hclen; ++i) {
      int s = stbi__zreceive(a,3);
      codelength_sizes[length_dezigzag[i]] = (stbi_uc) s;
   }
   if (!stbi__zbuild_huffman(&z_codelength, codelength_sizes, 19)) return 0;

   n = 0;
   while (n < ntot) {
      int c = stbi__zhuffman_decode(a, &z_codelength);
      if (c < 0 || c >= 19) return stbi__err("bad codelengths", "Corrupt PNG");
      if (c < 16)
         lencodes[n++] = (stbi_uc) c;
      else {
         stbi_uc fill = 0;
         if (c == 16) {
            c = stbi__zreceive(a,2)+3;
            if (n == 0) return stbi__err("bad codelengths", "Corrupt PNG");
            fill = lencodes[n-1];
         } else if (c == 17) {
            c = stbi__zreceive(a,3)+3;
         } else if (c == 18) {
            c = stbi__zreceive(a,7)+11;
         } else {
            return stbi__err("bad codelengths", "Corrupt PNG");
         }
         if (ntot - n < c) return stbi__err("bad codelengths", "Corrupt PNG");
         memset(lencodes+n, fill, c);
         n += c;
      }
   }
   if (n != ntot) return stbi__err("bad codelengths","Corrupt PNG");
   if (!stbi__zbuild_huffman(&a->z_length, lencodes, hlit)) return 0;
   if (!stbi__zbuild_huffman(&a->z_distance, lencodes+hlit, hdist)) return 0;
   return 1;
}

static int stbi__parse_uncompressed_block(stbi__zbuf *a)
{
   stbi_uc header[4];
   int len,nlen,k;
   if (a->num_bits & 7)
      stbi__zreceive(a, a->num_bits & 7); // discard
   // drain the bit-packed data into header
   k = 0;
   while (a->num_bits > 0) {
      header[k++] = (stbi_uc) (a->code_buffer & 255); // suppress MSVC run-time check
      a->code_buffer >>= 8;
      a->num_bits -= 8;
   }
   if (a->num_bits < 0) return stbi__err("zlib corrupt","Corrupt PNG");
   // now fill header the normal way
   while (k < 4)
      header[k++] = stbi__zget8(a);
   len  = header[1] * 256 + header[0];
   nlen = header[3] * 256 + header[2];
   if (nlen != (len ^ 0xffff)) return stbi__err("zlib corrupt","Corrupt PNG");
   if (a->zbuffer + len > a->zbuffer_end) return stbi__err("read past buffer","Corrupt PNG");
   if (a->zout + len > a->zout_end)
      if (!stbi__zexpand(a, a->zout, len)) return 0;
   memcpy(a->zout, a->zbuffer, len);
   a->zbuffer += len;
   a->zout += len;
   return 1;
}

static int stbi__parse_zlib_header(stbi__zbuf *a)
{
   int cmf   = stbi__zget8(a);
   int cm    = cmf & 15;
   /* int cinfo = cmf >> 4; */
   int flg   = stbi__zget8(a);
   if (stbi__zeof(a)) return stbi__err("bad zlib header","Corrupt PNG"); // zlib spec
   if ((cmf*256+flg) % 31 != 0) return stbi__err("bad zlib header","Corrupt PNG"); // zlib spec
   if (flg & 32) return stbi__err("no preset dict","Corrupt PNG"); // preset dictionary not allowed in png
   if (cm != 8) return stbi__err("bad compression","Corrupt PNG"); // DEFLATE required for png
   // window = 1 << (8 + cinfo)... but who cares, we fully buffer output
   return 1;
}

static const stbi_uc stbi__zdefault_length[STBI__ZNSYMS] =
{
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8
};
static const stbi_uc stbi__zdefault_distance[32] =
{
   5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
};
/*
Init algorithm:
{
   int i;   // use <= to match clearly with spec
   for (i=0; i <= 143; ++i)     stbi__zdefault_length[i]   = 8;
   for (   ; i <= 255; ++i)     stbi__zdefault_length[i]   = 9;
   for (   ; i <= 279; ++i)     stbi__zdefault_length[i]   = 7;
   for (   ; i <= 287; ++i)     stbi__zdefault_length[i]   = 8;

   for (i=0; i <=  31; ++i)     stbi__zdefault_distance[i] = 5;
}
*/

static int stbi__parse_zlib(stbi__zbuf *a, int parse_header)
{
   int final, type;
   if (parse_header)
      if (!stbi__parse_zlib_header(a)) return 0;
   a->num_bits = 0;
   a->code_buffer = 0;
   do {
      final = stbi__zreceive(a,1);
      type = stbi__zreceive(a,2);
      if (type == 0) {
         if (!stbi__parse_uncompressed_block(a)) return 0;
      } else if (type == 3) {
         return 0;
      } else {
         if (type == 1) {
            // use fixed code lengths
            if (!stbi__zbuild_huffman(&a->z_length  , stbi__zdefault_length  , STBI__ZNSYMS)) return 0;
            if (!stbi__zbuild_huffman(&a->z_distance, stbi__zdefault_distance,  32)) return 0;
         } else {
            if (!stbi__compute_huffman_codes(a)) return 0;
         }
         if (!stbi__parse_huffman_block(a)) return 0;
      }
   } while (!final);
   return 1;
}

static int stbi__do_zlib(stbi__zbuf *a, char *obuf, int olen, int exp, int parse_header)
{
   a->zout_start = obuf;
   a->zout       = obuf;
   a->zout_end   = obuf + olen;
   a->z_expandable = exp;

   return stbi__parse_zlib(a, parse_header);
}

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen)
{
   stbi__zbuf a;
   char *p = (char *) stbi__malloc(initial_size);
   if (p == NULL) return NULL;
   a.zbuffer = (stbi_uc *) buffer;
   a.zbuffer_end = (stbi_uc *) buffer + len;
   if (stbi__do_zlib(&a, p, initial_size, 1, 1)) {
      if (outlen) *outlen = (int) (a.zout - a.zout_start);
      return a.zout_start;
   } else {
      STBI_FREE(a.zout_start);
      return NULL;
   }
}

STBIDEF char *stbi_zlib_decode_malloc(char const *buffer, int len, int *outlen)
{
   return stbi_zlib_decode_malloc_guesssize(buffer, len, 16384, outlen);
}

STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen, int parse_header)
{
   stbi__zbuf a;
   char *p = (char *) stbi__malloc(initial_size);
   if (p == NULL) return NULL;
   a.zbuffer = (stbi_uc *) buffer;
   a.zbuffer_end = (stbi_uc *) buffer + len;
   if (stbi__do_zlib(&a, p, initial_size, 1, parse_header)) {
      if (outlen) *outlen = (int) (a.zout - a.zout_start);
      return a.zout_start;
   } else {
      STBI_FREE(a.zout_start);
      return NULL;
   }
}

STBIDEF int stbi_zlib_decode_buffer(char *obuffer, int olen, char const *ibuffer, int ilen)
{
   stbi__zbuf a;
   a.zbuffer = (stbi_uc *) ibuffer;
   a.zbuffer_end = (stbi_uc *) ibuffer + ilen;
   if (stbi__do_zlib(&a, obuffer, olen, 0, 1))
      return (int) (a.zout - a.zout_start);
   else
      return -1;
}

STBIDEF char *stbi_zlib_decode_noheader_malloc(char const *buffer, int len, int *outlen)
{
   stbi__zbuf a;
   char *p = (char *) stbi__malloc(16384);
   if (p == NULL) return NULL;
   a.zbuffer = (stbi_uc *) buffer;
   a.zbuffer_end = (stbi_uc *) buffer+len;
   if (stbi__do_zlib(&a, p, 16384, 1, 0)) {
      if (outlen) *outlen = (int) (a.zout - a.zout_start);
      return a.zout_start;
   } else {
      STBI_FREE(a.zout_start);
      return NULL;
   }
}

STBIDEF int stbi_zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen)
{
   stbi__zbuf a;
   a.zbuffer = (stbi_uc *) ibuffer;
   a.zbuffer_end = (stbi_uc *) ibuffer + ilen;
   if (stbi__do_zlib(&a, obuffer, olen, 0, 0))
      return (int) (a.zout - a.zout_start);
   else
      return -1;
}
#endif

// public domain "baseline" PNG decoder   v0.10  Sean Barrett 2006-11-18
//    simple implementation
//      - only 8-bit samples
//      - no CRC checking
//      - allocates lots of intermediate memory
//        - avoids problem of streaming data between subsystems
//        - avoids explicit window management
//    performance
//      - uses stb_zlib, a PD zlib implementation with fast huffman decoding

#ifndef STBI_NO_PNG
typedef struct
{
   stbi__uint32 length;
   stbi__uint32 type;
} stbi__pngchunk;

static stbi__pngchunk stbi__get_chunk_header(stbi__context *s)
{
   stbi__pngchunk c;
   c.length = stbi__get32be(s);
   c.type   = stbi__get32be(s);
   return c;
}

static int stbi__check_png_header(stbi__context *s)
{
   static const stbi_uc png_sig[8] = { 137,80,78,71,13,10,26,10 };
   int i;
   for (i=0; i < 8; ++i)
      if (stbi__get8(s) != png_sig[i]) return stbi__err("bad png sig","Not a PNG");
   return 1;
}

typedef struct
{
   stbi__context *s;
   stbi_uc *idata, *expanded, *out;
   int depth;
} stbi__png;


enum {
   STBI__F_none=0,
   STBI__F_sub=1,
   STBI__F_up=2,
   STBI__F_avg=3,
   STBI__F_paeth=4,
   // synthetic filters used for first scanline to avoid needing a dummy row of 0s
   STBI__F_avg_first,
   STBI__F_paeth_first
};

static stbi_uc first_row_filter[5] =
{
   STBI__F_none,
   STBI__F_sub,
   STBI__F_none,
   STBI__F_avg_first,
   STBI__F_paeth_first
};

static int stbi__paeth(int a, int b, int c)
{
   int p = a + b - c;
   int pa = abs(p-a);
   int pb = abs(p-b);
   int pc = abs(p-c);
   if (pa <= pb && pa <= pc) return a;
   if (pb <= pc) return b;
   return c;
}

static const stbi_uc stbi__depth_scale_table[9] = { 0, 0xff, 0x55, 0, 0x11, 0,0,0, 0x01 };

// create the png data from post-deflated data
static int stbi__create_png_image_raw(stbi__png *a, stbi_uc *raw, stbi__uint32 raw_len, int out_n, stbi__uint32 x, stbi__uint32 y, int depth, int color)
{
   int bytes = (depth == 16? 2 : 1);
   stbi__context *s = a->s;
   stbi__uint32 i,j,stride = x*out_n*bytes;
   stbi__uint32 img_len, img_width_bytes;
   int k;
   int img_n = s->img_n; // copy it into a local for later

   int output_bytes = out_n*bytes;
   int filter_bytes = img_n*bytes;
   int width = x;

   STBI_ASSERT(out_n == s->img_n || out_n == s->img_n+1);
   a->out = (stbi_uc *) stbi__malloc_mad3(x, y, output_bytes, 0); // extra bytes to write off the end into
   if (!a->out) return stbi__err("outofmem", "Out of memory");

   if (!stbi__mad3sizes_valid(img_n, x, depth, 7)) return stbi__err("too large", "Corrupt PNG");
   img_width_bytes = (((img_n * x * depth) + 7) >> 3);
   img_len = (img_width_bytes + 1) * y;

   // we used to check for exact match between raw_len and img_len on non-interlaced PNGs,
   // but issue #276 reported a PNG in the wild that had extra data at the end (all zeros),
   // so just check for raw_len < img_len always.
   if (raw_len < img_len) return stbi__err("not enough pixels","Corrupt PNG");

   for (j=0; j < y; ++j) {
      stbi_uc *cur = a->out + stride*j;
      stbi_uc *prior;
      int filter = *raw++;

      if (filter > 4)
         return stbi__err("invalid filter","Corrupt PNG");

      if (depth < 8) {
         if (img_width_bytes > x) return stbi__err("invalid width","Corrupt PNG");
         cur += x*out_n - img_width_bytes; // store output to the rightmost img_len bytes, so we can decode in place
         filter_bytes = 1;
         width = img_width_bytes;
      }
      prior = cur - stride; // bugfix: need to compute this after 'cur +=' computation above

      // if first row, use special filter that doesn't sample previous row
      if (j == 0) filter = first_row_filter[filter];

      // handle first byte explicitly
      for (k=0; k < filter_bytes; ++k) {
         switch (filter) {
            case STBI__F_none       : cur[k] = raw[k]; break;
            case STBI__F_sub        : cur[k] = raw[k]; break;
            case STBI__F_up         : cur[k] = STBI__BYTECAST(raw[k] + prior[k]); break;
            case STBI__F_avg        : cur[k] = STBI__BYTECAST(raw[k] + (prior[k]>>1)); break;
            case STBI__F_paeth      : cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(0,prior[k],0)); break;
            case STBI__F_avg_first  : cur[k] = raw[k]; break;
            case STBI__F_paeth_first: cur[k] = raw[k]; break;
         }
      }

      if (depth == 8) {
         if (img_n != out_n)
            cur[img_n] = 255; // first pixel
         raw += img_n;
         cur += out_n;
         prior += out_n;
      } else if (depth == 16) {
         if (img_n != out_n) {
            cur[filter_bytes]   = 255; // first pixel top byte
            cur[filter_bytes+1] = 255; // first pixel bottom byte
         }
         raw += filter_bytes;
         cur += output_bytes;
         prior += output_bytes;
      } else {
         raw += 1;
         cur += 1;
         prior += 1;
      }

      // this is a little gross, so that we don't switch per-pixel or per-component
      if (depth < 8 || img_n == out_n) {
         int nk = (width - 1)*filter_bytes;
         #define STBI__CASE(f) \
             case f:     \
                for (k=0; k < nk; ++k)
         switch (filter) {
            // "none" filter turns into a memcpy here; make that explicit.
            case STBI__F_none:         memcpy(cur, raw, nk); break;
            STBI__CASE(STBI__F_sub)          { cur[k] = STBI__BYTECAST(raw[k] + cur[k-filter_bytes]); } break;
            STBI__CASE(STBI__F_up)           { cur[k] = STBI__BYTECAST(raw[k] + prior[k]); } break;
            STBI__CASE(STBI__F_avg)          { cur[k] = STBI__BYTECAST(raw[k] + ((prior[k] + cur[k-filter_bytes])>>1)); } break;
            STBI__CASE(STBI__F_paeth)        { cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k-filter_bytes],prior[k],prior[k-filter_bytes])); } break;
            STBI__CASE(STBI__F_avg_first)    { cur[k] = STBI__BYTECAST(raw[k] + (cur[k-filter_bytes] >> 1)); } break;
            STBI__CASE(STBI__F_paeth_first)  { cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k-filter_bytes],0,0)); } break;
         }
         #undef STBI__CASE
         raw += nk;
      } else {
         STBI_ASSERT(img_n+1 == out_n);
         #define STBI__CASE(f) \
             case f:     \
                for (i=x-1; i >= 1; --i, cur[filter_bytes]=255,raw+=filter_bytes,cur+=output_bytes,prior+=output_bytes) \
                   for (k=0; k < filter_bytes; ++k)
         switch (filter) {
            STBI__CASE(STBI__F_none)         { cur[k] = raw[k]; } break;
            STBI__CASE(STBI__F_sub)          { cur[k] = STBI__BYTECAST(raw[k] + cur[k- output_bytes]); } break;
            STBI__CASE(STBI__F_up)           { cur[k] = STBI__BYTECAST(raw[k] + prior[k]); } break;
            STBI__CASE(STBI__F_avg)          { cur[k] = STBI__BYTECAST(raw[k] + ((prior[k] + cur[k- output_bytes])>>1)); } break;
            STBI__CASE(STBI__F_paeth)        { cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k- output_bytes],prior[k],prior[k- output_bytes])); } break;
            STBI__CASE(STBI__F_avg_first)    { cur[k] = STBI__BYTECAST(raw[k] + (cur[k- output_bytes] >> 1)); } break;
            STBI__CASE(STBI__F_paeth_first)  { cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k- output_bytes],0,0)); } break;
         }
         #undef STBI__CASE

         // the loop above sets the high byte of the pixels' alpha, but for
         // 16 bit png files we also need the low byte set. we'll do that here.
         if (depth == 16) {
            cur = a->out + stride*j; // start at the beginning of the row again
            for (i=0; i < x; ++i,cur+=output_bytes) {
               cur[filter_bytes+1] = 255;
            }
         }
      }
   }

   // we make a separate pass to expand bits to pixels; for performance,
   // this could run two scanlines behind the above code, so it won't
   // intefere with filtering but will still be in the cache.
   if (depth < 8) {
      for (j=0; j < y; ++j) {
         stbi_uc *cur = a->out + stride*j;
         stbi_uc *in  = a->out + stride*j + x*out_n - img_width_bytes;
         // unpack 1/2/4-bit into a 8-bit buffer. allows us to keep the common 8-bit path optimal at minimal cost for 1/2/4-bit
         // png guarante byte alignment, if width is not multiple of 8/4/2 we'll decode dummy trailing data that will be skipped in the later loop
         stbi_uc scale = (color == 0) ? stbi__depth_scale_table[depth] : 1; // scale grayscale values to 0..255 range

         // note that the final byte might overshoot and write more data than desired.
         // we can allocate enough data that this never writes out of memory, but it
         // could also overwrite the next scanline. can it overwrite non-empty data
         // on the next scanline? yes, consider 1-pixel-wide scanlines with 1-bit-per-pixel.
         // so we need to explicitly clamp the final ones

         if (depth == 4) {
            for (k=x*img_n; k >= 2; k-=2, ++in) {
               *cur++ = scale * ((*in >> 4)       );
               *cur++ = scale * ((*in     ) & 0x0f);
            }
            if (k > 0) *cur++ = scale * ((*in >> 4)       );
         } else if (depth == 2) {
            for (k=x*img_n; k >= 4; k-=4, ++in) {
               *cur++ = scale * ((*in >> 6)       );
               *cur++ = scale * ((*in >> 4) & 0x03);
               *cur++ = scale * ((*in >> 2) & 0x03);
               *cur++ = scale * ((*in     ) & 0x03);
            }
            if (k > 0) *cur++ = scale * ((*in >> 6)       );
            if (k > 1) *cur++ = scale * ((*in >> 4) & 0x03);
            if (k > 2) *cur++ = scale * ((*in >> 2) & 0x03);
         } else if (depth == 1) {
            for (k=x*img_n; k >= 8; k-=8, ++in) {
               *cur++ = scale * ((*in >> 7)       );
               *cur++ = scale * ((*in >> 6) & 0x01);
               *cur++ = scale * ((*in >> 5) & 0x01);
               *cur++ = scale * ((*in >> 4) & 0x01);
               *cur++ = scale * ((*in >> 3) & 0x01);
               *cur++ = scale * ((*in >> 2) & 0x01);
               *cur++ = scale * ((*in >> 1) & 0x01);
               *cur++ = scale * ((*in     ) & 0x01);
            }
            if (k > 0) *cur++ = scale * ((*in >> 7)       );
            if (k > 1) *cur++ = scale * ((*in >> 6) & 0x01);
            if (k > 2) *cur++ = scale * ((*in >> 5) & 0x01);
            if (k > 3) *cur++ = scale * ((*in >> 4) & 0x01);
            if (k > 4) *cur++ = scale * ((*in >> 3) & 0x01);
            if (k > 5) *cur++ = scale * ((*in >> 2) & 0x01);
            if (k > 6) *cur++ = scale * ((*in >> 1) & 0x01);
         }
         if (img_n != out_n) {
            int q;
            // insert alpha = 255
            cur = a->out + stride*j;
            if (img_n == 1) {
               for (q=x-1; q >= 0; --q) {
                  cur[q*2+1] = 255;
                  cur[q*2+0] = cur[q];
               }
            } else {
               STBI_ASSERT(img_n == 3);
               for (q=x-1; q >= 0; --q) {
                  cur[q*4+3] = 255;
                  cur[q*4+2] = cur[q*3+2];
                  cur[q*4+1] = cur[q*3+1];
                  cur[q*4+0] = cur[q*3+0];
               }
            }
         }
      }
   } else if (depth == 16) {
      // force the image data from big-endian to platform-native.
      // this is done in a separate pass due to the decoding relying
      // on the data being untouched, but could probably be done
      // per-line during decode if care is taken.
      stbi_uc *cur = a->out;
      stbi__uint16 *cur16 = (stbi__uint16*)cur;

      for(i=0; i < x*y*out_n; ++i,cur16++,cur+=2) {
         *cur16 = (cur[0] << 8) | cur[1];
      }
   }

   return 1;
}

static int stbi__create_png_image(stbi__png *a, stbi_uc *image_data, stbi__uint32 image_data_len, int out_n, int depth, int color, int interlaced)
{
   int bytes = (depth == 16 ? 2 : 1);
   int out_bytes = out_n * bytes;
   stbi_uc *final;
   int p;
   if (!interlaced)
      return stbi__create_png_image_raw(a, image_data, image_data_len, out_n, a->s->img_x, a->s->img_y, depth, color);

   // de-interlacing
   final = (stbi_uc *) stbi__malloc_mad3(a->s->img_x, a->s->img_y, out_bytes, 0);
   if (!final) return stbi__err("outofmem", "Out of memory");
   for (p=0; p < 7; ++p) {
      int xorig[] = { 0,4,0,2,0,1,0 };
      int yorig[] = { 0,0,4,0,2,0,1 };
      int xspc[]  = { 8,8,4,4,2,2,1 };
      int yspc[]  = { 8,8,8,4,4,2,2 };
      int i,j,x,y;
      // pass1_x[4] = 0, pass1_x[5] = 1, pass1_x[12] = 1
      x = (a->s->img_x - xorig[p] + xspc[p]-1) / xspc[p];
      y = (a->s->img_y - yorig[p] + yspc[p]-1) / yspc[p];
      if (x && y) {
         stbi__uint32 img_len = ((((a->s->img_n * x * depth) + 7) >> 3) + 1) * y;
         if (!stbi__create_png_image_raw(a, image_data, image_data_len, out_n, x, y, depth, color)) {
            STBI_FREE(final);
            return 0;
         }
         for (j=0; j < y; ++j) {
            for (i=0; i < x; ++i) {
               int out_y = j*yspc[p]+yorig[p];
               int out_x = i*xspc[p]+xorig[p];
               memcpy(final + out_y*a->s->img_x*out_bytes + out_x*out_bytes,
                      a->out + (j*x+i)*out_bytes, out_bytes);
            }
         }
         STBI_FREE(a->out);
         image_data += img_len;
         image_data_len -= img_len;
      }
   }
   a->out = final;

   return 1;
}

static int stbi__compute_transparency(stbi__png *z, stbi_uc tc[3], int out_n)
{
   stbi__context *s = z->s;
   stbi__uint32 i, pixel_count = s->img_x * s->img_y;
   stbi_uc *p = z->out;

   // compute color-based transparency, assuming we've
   // already got 255 as the alpha value in the output
   STBI_ASSERT(out_n == 2 || out_n == 4);

   if (out_n == 2) {
      for (i=0; i < pixel_count; ++i) {
         p[1] = (p[0] == tc[0] ? 0 : 255);
         p += 2;
      }
   } else {
      for (i=0; i < pixel_count; ++i) {
         if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
            p[3] = 0;
         p += 4;
      }
   }
   return 1;
}

static int stbi__compute_transparency16(stbi__png *z, stbi__uint16 tc[3], int out_n)
{
   stbi__context *s = z->s;
   stbi__uint32 i, pixel_count = s->img_x * s->img_y;
   stbi__uint16 *p = (stbi__uint16*) z->out;

   // compute color-based transparency, assuming we've
   // already got 65535 as the alpha value in the output
   STBI_ASSERT(out_n == 2 || out_n == 4);

   if (out_n == 2) {
      for (i = 0; i < pixel_count; ++i) {
         p[1] = (p[0] == tc[0] ? 0 : 65535);
         p += 2;
      }
   } else {
      for (i = 0; i < pixel_count; ++i) {
         if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
            p[3] = 0;
         p += 4;
      }
   }
   return 1;
}

static int stbi__expand_png_palette(stbi__png *a, stbi_uc *palette, int len, int pal_img_n)
{
   stbi__uint32 i, pixel_count = a->s->img_x * a->s->img_y;
   stbi_uc *p, *temp_out, *orig = a->out;

   p = (stbi_uc *) stbi__malloc_mad2(pixel_count, pal_img_n, 0);
   if (p == NULL) return stbi__err("outofmem", "Out of memory");

   // between here and free(out) below, exitting would leak
   temp_out = p;

   if (pal_img_n == 3) {
      for (i=0; i < pixel_count; ++i) {
         int n = orig[i]*4;
         p[0] = palette[n  ];
         p[1] = palette[n+1];
         p[2] = palette[n+2];
         p += 3;
      }
   } else {
      for (i=0; i < pixel_count; ++i) {
         int n = orig[i]*4;
         p[0] = palette[n  ];
         p[1] = palette[n+1];
         p[2] = palette[n+2];
         p[3] = palette[n+3];
         p += 4;
      }
   }
   STBI_FREE(a->out);
   a->out = temp_out;

   STBI_NOTUSED(len);

   return 1;
}

static int stbi__unpremultiply_on_load_global = 0;
static int stbi__de_iphone_flag_global = 0;

STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply)
{
   stbi__unpremultiply_on_load_global = flag_true_if_should_unpremultiply;
}

STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert)
{
   stbi__de_iphone_flag_global = flag_true_if_should_convert;
}

#ifndef STBI_THREAD_LOCAL
#define stbi__unpremultiply_on_load  stbi__unpremultiply_on_load_global
#define stbi__de_iphone_flag  stbi__de_iphone_flag_global
#else
static STBI_THREAD_LOCAL int stbi__unpremultiply_on_load_local, stbi__unpremultiply_on_load_set;
static STBI_THREAD_LOCAL int stbi__de_iphone_flag_local, stbi__de_iphone_flag_set;

STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply)
{
   stbi__unpremultiply_on_load_local = flag_true_if_should_unpremultiply;
   stbi__unpremultiply_on_load_set = 1;
}

STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert)
{
   stbi__de_iphone_flag_local = flag_true_if_should_convert;
   stbi__de_iphone_flag_set = 1;
}

#define stbi__unpremultiply_on_load  (stbi__unpremultiply_on_load_set           \
                                       ? stbi__unpremultiply_on_load_local      \
                                       : stbi__unpremultiply_on_load_global)
#define stbi__de_iphone_flag  (stbi__de_iphone_flag_set                         \
                                ? stbi__de_iphone_flag_local                    \
                                : stbi__de_iphone_flag_global)
#endif // STBI_THREAD_LOCAL

static void stbi__de_iphone(stbi__png *z)
{
   stbi__context *s = z->s;
   stbi__uint32 i, pixel_count = s->img_x * s->img_y;
   stbi_uc *p = z->out;

   if (s->img_out_n == 3) {  // convert bgr to rgb
      for (i=0; i < pixel_count; ++i) {
         stbi_uc t = p[0];
         p[0] = p[2];
         p[2] = t;
         p += 3;
      }
   } else {
      STBI_ASSERT(s->img_out_n == 4);
      if (stbi__unpremultiply_on_load) {
         // convert bgr to rgb and unpremultiply
         for (i=0; i < pixel_count; ++i) {
            stbi_uc a = p[3];
            stbi_uc t = p[0];
            if (a) {
               stbi_uc half = a / 2;
               p[0] = (p[2] * 255 + half) / a;
               p[1] = (p[1] * 255 + half) / a;
               p[2] = ( t   * 255 + half) / a;
            } else {
               p[0] = p[2];
               p[2] = t;
            }
            p += 4;
         }
      } else {
         // convert bgr to rgb
         for (i=0; i < pixel_count; ++i) {
            stbi_uc t = p[0];
            p[0] = p[2];
            p[2] = t;
            p += 4;
         }
      }
   }
}

#define STBI__PNG_TYPE(a,b,c,d)  (((unsigned) (a) << 24) + ((unsigned) (b) << 16) + ((unsigned) (c) << 8) + (unsigned) (d))

static int stbi__parse_png_file(stbi__png *z, int scan, int req_comp)
{
   stbi_uc palette[1024], pal_img_n=0;
   stbi_uc has_trans=0, tc[3]={0};
   stbi__uint16 tc16[3];
   stbi__uint32 ioff=0, idata_limit=0, i, pal_len=0;
   int first=1,k,interlace=0, color=0, is_iphone=0;
   stbi__context *s = z->s;

   z->expanded = NULL;
   z->idata = NULL;
   z->out = NULL;

   if (!stbi__check_png_header(s)) return 0;

   if (scan == STBI__SCAN_type) return 1;

   for (;;) {
      stbi__pngchunk c = stbi__get_chunk_header(s);
      switch (c.type) {
         case STBI__PNG_TYPE('C','g','B','I'):
            is_iphone = 1;
            stbi__skip(s, c.length);
            break;
         case STBI__PNG_TYPE('I','H','D','R'): {
            int comp,filter;
            if (!first) return stbi__err("multiple IHDR","Corrupt PNG");
            first = 0;
            if (c.length != 13) return stbi__err("bad IHDR len","Corrupt PNG");
            s->img_x = stbi__get32be(s);
            s->img_y = stbi__get32be(s);
            if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
            if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
            z->depth = stbi__get8(s);  if (z->depth != 1 && z->depth != 2 && z->depth != 4 && z->depth != 8 && z->depth != 16)  return stbi__err("1/2/4/8/16-bit only","PNG not supported: 1/2/4/8/16-bit only");
            color = stbi__get8(s);  if (color > 6)         return stbi__err("bad ctype","Corrupt PNG");
            if (color == 3 && z->depth == 16)                  return stbi__err("bad ctype","Corrupt PNG");
            if (color == 3) pal_img_n = 3; else if (color & 1) return stbi__err("bad ctype","Corrupt PNG");
            comp  = stbi__get8(s);  if (comp) return stbi__err("bad comp method","Corrupt PNG");
            filter= stbi__get8(s);  if (filter) return stbi__err("bad filter method","Corrupt PNG");
            interlace = stbi__get8(s); if (interlace>1) return stbi__err("bad interlace method","Corrupt PNG");
            if (!s->img_x || !s->img_y) return stbi__err("0-pixel image","Corrupt PNG");
            if (!pal_img_n) {
               s->img_n = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
               if ((1 << 30) / s->img_x / s->img_n < s->img_y) return stbi__err("too large", "Image too large to decode");
            } else {
               // if paletted, then pal_n is our final components, and
               // img_n is # components to decompress/filter.
               s->img_n = 1;
               if ((1 << 30) / s->img_x / 4 < s->img_y) return stbi__err("too large","Corrupt PNG");
            }
            // even with SCAN_header, have to scan to see if we have a tRNS
            break;
         }

         case STBI__PNG_TYPE('P','L','T','E'):  {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (c.length > 256*3) return stbi__err("invalid PLTE","Corrupt PNG");
            pal_len = c.length / 3;
            if (pal_len * 3 != c.length) return stbi__err("invalid PLTE","Corrupt PNG");
            for (i=0; i < pal_len; ++i) {
               palette[i*4+0] = stbi__get8(s);
               palette[i*4+1] = stbi__get8(s);
               palette[i*4+2] = stbi__get8(s);
               palette[i*4+3] = 255;
            }
            break;
         }

         case STBI__PNG_TYPE('t','R','N','S'): {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (z->idata) return stbi__err("tRNS after IDAT","Corrupt PNG");
            if (pal_img_n) {
               if (scan == STBI__SCAN_header) { s->img_n = 4; return 1; }
               if (pal_len == 0) return stbi__err("tRNS before PLTE","Corrupt PNG");
               if (c.length > pal_len) return stbi__err("bad tRNS len","Corrupt PNG");
               pal_img_n = 4;
               for (i=0; i < c.length; ++i)
                  palette[i*4+3] = stbi__get8(s);
            } else {
               if (!(s->img_n & 1)) return stbi__err("tRNS with alpha","Corrupt PNG");
               if (c.length != (stbi__uint32) s->img_n*2) return stbi__err("bad tRNS len","Corrupt PNG");
               has_trans = 1;
               // non-paletted with tRNS = constant alpha. if header-scanning, we can stop now.
               if (scan == STBI__SCAN_header) { ++s->img_n; return 1; }
               if (z->depth == 16) {
                  for (k = 0; k < s->img_n; ++k) tc16[k] = (stbi__uint16)stbi__get16be(s); // copy the values as-is
               } else {
                  for (k = 0; k < s->img_n; ++k) tc[k] = (stbi_uc)(stbi__get16be(s) & 255) * stbi__depth_scale_table[z->depth]; // non 8-bit images will be larger
               }
            }
            break;
         }

         case STBI__PNG_TYPE('I','D','A','T'): {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (pal_img_n && !pal_len) return stbi__err("no PLTE","Corrupt PNG");
            if (scan == STBI__SCAN_header) {
               // header scan definitely stops at first IDAT
               if (pal_img_n)
                  s->img_n = pal_img_n;
               return 1;
            }
            if (c.length > (1u << 30)) return stbi__err("IDAT size limit", "IDAT section larger than 2^30 bytes");
            if ((int)(ioff + c.length) < (int)ioff) return 0;
            if (ioff + c.length > idata_limit) {
               stbi__uint32 idata_limit_old = idata_limit;
               stbi_uc *p;
               if (idata_limit == 0) idata_limit = c.length > 4096 ? c.length : 4096;
               while (ioff + c.length > idata_limit)
                  idata_limit *= 2;
               STBI_NOTUSED(idata_limit_old);
               p = (stbi_uc *) STBI_REALLOC_SIZED(z->idata, idata_limit_old, idata_limit); if (p == NULL) return stbi__err("outofmem", "Out of memory");
               z->idata = p;
            }
            if (!stbi__getn(s, z->idata+ioff,c.length)) return stbi__err("outofdata","Corrupt PNG");
            ioff += c.length;
            break;
         }

         case STBI__PNG_TYPE('I','E','N','D'): {
            stbi__uint32 raw_len, bpl;
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (scan != STBI__SCAN_load) return 1;
            if (z->idata == NULL) return stbi__err("no IDAT","Corrupt PNG");
            // initial guess for decoded data size to avoid unnecessary reallocs
            bpl = (s->img_x * z->depth + 7) / 8; // bytes per line, per component
            raw_len = bpl * s->img_y * s->img_n /* pixels */ + s->img_y /* filter mode per row */;
            z->expanded = (stbi_uc *) stbi_zlib_decode_malloc_guesssize_headerflag((char *) z->idata, ioff, raw_len, (int *) &raw_len, !is_iphone);
            if (z->expanded == NULL) return 0; // zlib should set error
            STBI_FREE(z->idata); z->idata = NULL;
            if ((req_comp == s->img_n+1 && req_comp != 3 && !pal_img_n) || has_trans)
               s->img_out_n = s->img_n+1;
            else
               s->img_out_n = s->img_n;
            if (!stbi__create_png_image(z, z->expanded, raw_len, s->img_out_n, z->depth, color, interlace)) return 0;
            if (has_trans) {
               if (z->depth == 16) {
                  if (!stbi__compute_transparency16(z, tc16, s->img_out_n)) return 0;
               } else {
                  if (!stbi__compute_transparency(z, tc, s->img_out_n)) return 0;
               }
            }
            if (is_iphone && stbi__de_iphone_flag && s->img_out_n > 2)
               stbi__de_iphone(z);
            if (pal_img_n) {
               // pal_img_n == 3 or 4
               s->img_n = pal_img_n; // record the actual colors we had
               s->img_out_n = pal_img_n;
               if (req_comp >= 3) s->img_out_n = req_comp;
               if (!stbi__expand_png_palette(z, palette, pal_len, s->img_out_n))
                  return 0;
            } else if (has_trans) {
               // non-paletted image with tRNS -> source image has (constant) alpha
               ++s->img_n;
            }
            STBI_FREE(z->expanded); z->expanded = NULL;
            // end of PNG chunk, read and skip CRC
            stbi__get32be(s);
            return 1;
         }

         default:
            // if critical, fail
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if ((c.type & (1 << 29)) == 0) {
               #ifndef STBI_NO_FAILURE_STRINGS
               // not threadsafe
               static char invalid_chunk[] = "XXXX PNG chunk not known";
               invalid_chunk[0] = STBI__BYTECAST(c.type >> 24);
               invalid_chunk[1] = STBI__BYTECAST(c.type >> 16);
               invalid_chunk[2] = STBI__BYTECAST(c.type >>  8);
               invalid_chunk[3] = STBI__BYTECAST(c.type >>  0);
               #endif
               return stbi__err(invalid_chunk, "PNG not supported: unknown PNG chunk type");
            }
            stbi__skip(s, c.length);
            break;
      }
      // end of PNG chunk, read and skip CRC
      stbi__get32be(s);
   }
}

static void *stbi__do_png(stbi__png *p, int *x, int *y, int *n, int req_comp, stbi__result_info *ri)
{
   void *result=NULL;
   if (req_comp < 0 || req_comp > 4) return stbi__errpuc("bad req_comp", "Internal error");
   if (stbi__parse_png_file(p, STBI__SCAN_load, req_comp)) {
      if (p->depth <= 8)
         ri->bits_per_channel = 8;
      else if (p->depth == 16)
         ri->bits_per_channel = 16;
      else
         return stbi__errpuc("bad bits_per_channel", "PNG not supported: unsupported color depth");
      result = p->out;
      p->out = NULL;
      if (req_comp && req_comp != p->s->img_out_n) {
         if (ri->bits_per_channel == 8)
            result = stbi__convert_format((unsigned char *) result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
         else
            result = stbi__convert_format16((stbi__uint16 *) result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
         p->s->img_out_n = req_comp;
         if (result == NULL) return result;
      }
      *x = p->s->img_x;
      *y = p->s->img_y;
      if (n) *n = p->s->img_n;
   }
   STBI_FREE(p->out);      p->out      = NULL;
   STBI_FREE(p->expanded); p->expanded = NULL;
   STBI_FREE(p->idata);    p->idata    = NULL;

   return result;
}

static void *stbi__png_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   stbi__png p;
   p.s = s;
   return stbi__do_png(&p, x,y,comp,req_comp, ri);
}

static int stbi__png_test(stbi__context *s)
{
   int r;
   r = stbi__check_png_header(s);
   stbi__rewind(s);
   return r;
}

static int stbi__png_info_raw(stbi__png *p, int *x, int *y, int *comp)
{
   if (!stbi__parse_png_file(p, STBI__SCAN_header, 0)) {
      stbi__rewind( p->s );
      return 0;
   }
   if (x) *x = p->s->img_x;
   if (y) *y = p->s->img_y;
   if (comp) *comp = p->s->img_n;
   return 1;
}

static int stbi__png_info(stbi__context *s, int *x, int *y, int *comp)
{
   stbi__png p;
   p.s = s;
   return stbi__png_info_raw(&p, x, y, comp);
}

static int stbi__png_is16(stbi__context *s)
{
   stbi__png p;
   p.s = s;
   if (!stbi__png_info_raw(&p, NULL, NULL, NULL))
	   return 0;
   if (p.depth != 16) {
      stbi__rewind(p.s);
      return 0;
   }
   return 1;
}
#endif

// Microsoft/Windows BMP image

#ifndef STBI_NO_BMP
static int stbi__bmp_test_raw(stbi__context *s)
{
   int r;
   int sz;
   if (stbi__get8(s) != 'B') return 0;
   if (stbi__get8(s) != 'M') return 0;
   stbi__get32le(s); // discard filesize
   stbi__get16le(s); // discard reserved
   stbi__get16le(s); // discard reserved
   stbi__get32le(s); // discard data offset
   sz = stbi__get32le(s);
   r = (sz == 12 || sz == 40 || sz == 56 || sz == 108 || sz == 124);
   return r;
}

static int stbi__bmp_test(stbi__context *s)
{
   int r = stbi__bmp_test_raw(s);
   stbi__rewind(s);
   return r;
}


// returns 0..31 for the highest set bit
static int stbi__high_bit(unsigned int z)
{
   int n=0;
   if (z == 0) return -1;
   if (z >= 0x10000) { n += 16; z >>= 16; }
   if (z >= 0x00100) { n +=  8; z >>=  8; }
   if (z >= 0x00010) { n +=  4; z >>=  4; }
   if (z >= 0x00004) { n +=  2; z >>=  2; }
   if (z >= 0x00002) { n +=  1;/* >>=  1;*/ }
   return n;
}

static int stbi__bitcount(unsigned int a)
{
   a = (a & 0x55555555) + ((a >>  1) & 0x55555555); // max 2
   a = (a & 0x33333333) + ((a >>  2) & 0x33333333); // max 4
   a = (a + (a >> 4)) & 0x0f0f0f0f; // max 8 per 4, now 8 bits
   a = (a + (a >> 8)); // max 16 per 8 bits
   a = (a + (a >> 16)); // max 32 per 8 bits
   return a & 0xff;
}

// extract an arbitrarily-aligned N-bit value (N=bits)
// from v, and then make it 8-bits long and fractionally
// extend it to full full range.
static int stbi__shiftsigned(unsigned int v, int shift, int bits)
{
   static unsigned int mul_table[9] = {
      0,
      0xff/*0b11111111*/, 0x55/*0b01010101*/, 0x49/*0b01001001*/, 0x11/*0b00010001*/,
      0x21/*0b00100001*/, 0x41/*0b01000001*/, 0x81/*0b10000001*/, 0x01/*0b00000001*/,
   };
   static unsigned int shift_table[9] = {
      0, 0,0,1,0,2,4,6,0,
   };
   if (shift < 0)
      v <<= -shift;
   else
      v >>= shift;
   STBI_ASSERT(v < 256);
   v >>= (8-bits);
   STBI_ASSERT(bits >= 0 && bits <= 8);
   return (int) ((unsigned) v * mul_table[bits]) >> shift_table[bits];
}

typedef struct
{
   int bpp, offset, hsz;
   unsigned int mr,mg,mb,ma, all_a;
   int extra_read;
} stbi__bmp_data;

static int stbi__bmp_set_mask_defaults(stbi__bmp_data *info, int compress)
{
   // BI_BITFIELDS specifies masks explicitly, don't override
   if (compress == 3)
      return 1;

   if (compress == 0) {
      if (info->bpp == 16) {
         info->mr = 31u << 10;
         info->mg = 31u <<  5;
         info->mb = 31u <<  0;
      } else if (info->bpp == 32) {
         info->mr = 0xffu << 16;
         info->mg = 0xffu <<  8;
         info->mb = 0xffu <<  0;
         info->ma = 0xffu << 24;
         info->all_a = 0; // if all_a is 0 at end, then we loaded alpha channel but it was all 0
      } else {
         // otherwise, use defaults, which is all-0
         info->mr = info->mg = info->mb = info->ma = 0;
      }
      return 1;
   }
   return 0; // error
}

static void *stbi__bmp_parse_header(stbi__context *s, stbi__bmp_data *info)
{
   int hsz;
   if (stbi__get8(s) != 'B' || stbi__get8(s) != 'M') return stbi__errpuc("not BMP", "Corrupt BMP");
   stbi__get32le(s); // discard filesize
   stbi__get16le(s); // discard reserved
   stbi__get16le(s); // discard reserved
   info->offset = stbi__get32le(s);
   info->hsz = hsz = stbi__get32le(s);
   info->mr = info->mg = info->mb = info->ma = 0;
   info->extra_read = 14;

   if (info->offset < 0) return stbi__errpuc("bad BMP", "bad BMP");

   if (hsz != 12 && hsz != 40 && hsz != 56 && hsz != 108 && hsz != 124) return stbi__errpuc("unknown BMP", "BMP type not supported: unknown");
   if (hsz == 12) {
      s->img_x = stbi__get16le(s);
      s->img_y = stbi__get16le(s);
   } else {
      s->img_x = stbi__get32le(s);
      s->img_y = stbi__get32le(s);
   }
   if (stbi__get16le(s) != 1) return stbi__errpuc("bad BMP", "bad BMP");
   info->bpp = stbi__get16le(s);
   if (hsz != 12) {
      int compress = stbi__get32le(s);
      if (compress == 1 || compress == 2) return stbi__errpuc("BMP RLE", "BMP type not supported: RLE");
      if (compress >= 4) return stbi__errpuc("BMP JPEG/PNG", "BMP type not supported: unsupported compression"); // this includes PNG/JPEG modes
      if (compress == 3 && info->bpp != 16 && info->bpp != 32) return stbi__errpuc("bad BMP", "bad BMP"); // bitfields requires 16 or 32 bits/pixel
      stbi__get32le(s); // discard sizeof
      stbi__get32le(s); // discard hres
      stbi__get32le(s); // discard vres
      stbi__get32le(s); // discard colorsused
      stbi__get32le(s); // discard max important
      if (hsz == 40 || hsz == 56) {
         if (hsz == 56) {
            stbi__get32le(s);
            stbi__get32le(s);
            stbi__get32le(s);
            stbi__get32le(s);
         }
         if (info->bpp == 16 || info->bpp == 32) {
            if (compress == 0) {
               stbi__bmp_set_mask_defaults(info, compress);
            } else if (compress == 3) {
               info->mr = stbi__get32le(s);
               info->mg = stbi__get32le(s);
               info->mb = stbi__get32le(s);
               info->extra_read += 12;
               // not documented, but generated by photoshop and handled by mspaint
               if (info->mr == info->mg && info->mg == info->mb) {
                  // ?!?!?
                  return stbi__errpuc("bad BMP", "bad BMP");
               }
            } else
               return stbi__errpuc("bad BMP", "bad BMP");
         }
      } else {
         // V4/V5 header
         int i;
         if (hsz != 108 && hsz != 124)
            return stbi__errpuc("bad BMP", "bad BMP");
         info->mr = stbi__get32le(s);
         info->mg = stbi__get32le(s);
         info->mb = stbi__get32le(s);
         info->ma = stbi__get32le(s);
         if (compress != 3) // override mr/mg/mb unless in BI_BITFIELDS mode, as per docs
            stbi__bmp_set_mask_defaults(info, compress);
         stbi__get32le(s); // discard color space
         for (i=0; i < 12; ++i)
            stbi__get32le(s); // discard color space parameters
         if (hsz == 124) {
            stbi__get32le(s); // discard rendering intent
            stbi__get32le(s); // discard offset of profile data
            stbi__get32le(s); // discard size of profile data
            stbi__get32le(s); // discard reserved
         }
      }
   }
   return (void *) 1;
}


static void *stbi__bmp_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   stbi_uc *out;
   unsigned int mr=0,mg=0,mb=0,ma=0, all_a;
   stbi_uc pal[256][4];
   int psize=0,i,j,width;
   int flip_vertically, pad, target;
   stbi__bmp_data info;
   STBI_NOTUSED(ri);

   info.all_a = 255;
   if (stbi__bmp_parse_header(s, &info) == NULL)
      return NULL; // error code already set

   flip_vertically = ((int) s->img_y) > 0;
   s->img_y = abs((int) s->img_y);

   if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   mr = info.mr;
   mg = info.mg;
   mb = info.mb;
   ma = info.ma;
   all_a = info.all_a;

   if (info.hsz == 12) {
      if (info.bpp < 24)
         psize = (info.offset - info.extra_read - 24) / 3;
   } else {
      if (info.bpp < 16)
         psize = (info.offset - info.extra_read - info.hsz) >> 2;
   }
   if (psize == 0) {
      // accept some number of extra bytes after the header, but if the offset points either to before
      // the header ends or implies a large amount of extra data, reject the file as malformed
      int bytes_read_so_far = s->callback_already_read + (int)(s->img_buffer - s->img_buffer_original);
      int header_limit = 1024; // max we actually read is below 256 bytes currently.
      int extra_data_limit = 256*4; // what ordinarily goes here is a palette; 256 entries*4 bytes is its max size.
      if (bytes_read_so_far <= 0 || bytes_read_so_far > header_limit) {
         return stbi__errpuc("bad header", "Corrupt BMP");
      }
      // we established that bytes_read_so_far is positive and sensible.
      // the first half of this test rejects offsets that are either too small positives, or
      // negative, and guarantees that info.offset >= bytes_read_so_far > 0. this in turn
      // ensures the number computed in the second half of the test can't overflow.
      if (info.offset < bytes_read_so_far || info.offset - bytes_read_so_far > extra_data_limit) {
         return stbi__errpuc("bad offset", "Corrupt BMP");
      } else {
         stbi__skip(s, info.offset - bytes_read_so_far);
      }
   }

   if (info.bpp == 24 && ma == 0xff000000)
      s->img_n = 3;
   else
      s->img_n = ma ? 4 : 3;
   if (req_comp && req_comp >= 3) // we can directly decode 3 or 4
      target = req_comp;
   else
      target = s->img_n; // if they want monochrome, we'll post-convert

   // sanity-check size
   if (!stbi__mad3sizes_valid(target, s->img_x, s->img_y, 0))
      return stbi__errpuc("too large", "Corrupt BMP");

   out = (stbi_uc *) stbi__malloc_mad3(target, s->img_x, s->img_y, 0);
   if (!out) return stbi__errpuc("outofmem", "Out of memory");
   if (info.bpp < 16) {
      int z=0;
      if (psize == 0 || psize > 256) { STBI_FREE(out); return stbi__errpuc("invalid", "Corrupt BMP"); }
      for (i=0; i < psize; ++i) {
         pal[i][2] = stbi__get8(s);
         pal[i][1] = stbi__get8(s);
         pal[i][0] = stbi__get8(s);
         if (info.hsz != 12) stbi__get8(s);
         pal[i][3] = 255;
      }
      stbi__skip(s, info.offset - info.extra_read - info.hsz - psize * (info.hsz == 12 ? 3 : 4));
      if (info.bpp == 1) width = (s->img_x + 7) >> 3;
      else if (info.bpp == 4) width = (s->img_x + 1) >> 1;
      else if (info.bpp == 8) width = s->img_x;
      else { STBI_FREE(out); return stbi__errpuc("bad bpp", "Corrupt BMP"); }
      pad = (-width)&3;
      if (info.bpp == 1) {
         for (j=0; j < (int) s->img_y; ++j) {
            int bit_offset = 7, v = stbi__get8(s);
            for (i=0; i < (int) s->img_x; ++i) {
               int color = (v>>bit_offset)&0x1;
               out[z++] = pal[color][0];
               out[z++] = pal[color][1];
               out[z++] = pal[color][2];
               if (target == 4) out[z++] = 255;
               if (i+1 == (int) s->img_x) break;
               if((--bit_offset) < 0) {
                  bit_offset = 7;
                  v = stbi__get8(s);
               }
            }
            stbi__skip(s, pad);
         }
      } else {
         for (j=0; j < (int) s->img_y; ++j) {
            for (i=0; i < (int) s->img_x; i += 2) {
               int v=stbi__get8(s),v2=0;
               if (info.bpp == 4) {
                  v2 = v & 15;
                  v >>= 4;
               }
               out[z++] = pal[v][0];
               out[z++] = pal[v][1];
               out[z++] = pal[v][2];
               if (target == 4) out[z++] = 255;
               if (i+1 == (int) s->img_x) break;
               v = (info.bpp == 8) ? stbi__get8(s) : v2;
               out[z++] = pal[v][0];
               out[z++] = pal[v][1];
               out[z++] = pal[v][2];
               if (target == 4) out[z++] = 255;
            }
            stbi__skip(s, pad);
         }
      }
   } else {
      int rshift=0,gshift=0,bshift=0,ashift=0,rcount=0,gcount=0,bcount=0,acount=0;
      int z = 0;
      int easy=0;
      stbi__skip(s, info.offset - info.extra_read - info.hsz);
      if (info.bpp == 24) width = 3 * s->img_x;
      else if (info.bpp == 16) width = 2*s->img_x;
      else /* bpp = 32 and pad = 0 */ width=0;
      pad = (-width) & 3;
      if (info.bpp == 24) {
         easy = 1;
      } else if (info.bpp == 32) {
         if (mb == 0xff && mg == 0xff00 && mr == 0x00ff0000 && ma == 0xff000000)
            easy = 2;
      }
      if (!easy) {
         if (!mr || !mg || !mb) { STBI_FREE(out); return stbi__errpuc("bad masks", "Corrupt BMP"); }
         // right shift amt to put high bit in position #7
         rshift = stbi__high_bit(mr)-7; rcount = stbi__bitcount(mr);
         gshift = stbi__high_bit(mg)-7; gcount = stbi__bitcount(mg);
         bshift = stbi__high_bit(mb)-7; bcount = stbi__bitcount(mb);
         ashift = stbi__high_bit(ma)-7; acount = stbi__bitcount(ma);
         if (rcount > 8 || gcount > 8 || bcount > 8 || acount > 8) { STBI_FREE(out); return stbi__errpuc("bad masks", "Corrupt BMP"); }
      }
      for (j=0; j < (int) s->img_y; ++j) {
         if (easy) {
            for (i=0; i < (int) s->img_x; ++i) {
               unsigned char a;
               out[z+2] = stbi__get8(s);
               out[z+1] = stbi__get8(s);
               out[z+0] = stbi__get8(s);
               z += 3;
               a = (easy == 2 ? stbi__get8(s) : 255);
               all_a |= a;
               if (target == 4) out[z++] = a;
            }
         } else {
            int bpp = info.bpp;
            for (i=0; i < (int) s->img_x; ++i) {
               stbi__uint32 v = (bpp == 16 ? (stbi__uint32) stbi__get16le(s) : stbi__get32le(s));
               unsigned int a;
               out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mr, rshift, rcount));
               out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mg, gshift, gcount));
               out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mb, bshift, bcount));
               a = (ma ? stbi__shiftsigned(v & ma, ashift, acount) : 255);
               all_a |= a;
               if (target == 4) out[z++] = STBI__BYTECAST(a);
            }
         }
         stbi__skip(s, pad);
      }
   }

   // if alpha channel is all 0s, replace with all 255s
   if (target == 4 && all_a == 0)
      for (i=4*s->img_x*s->img_y-1; i >= 0; i -= 4)
         out[i] = 255;

   if (flip_vertically) {
      stbi_uc t;
      for (j=0; j < (int) s->img_y>>1; ++j) {
         stbi_uc *p1 = out +      j     *s->img_x*target;
         stbi_uc *p2 = out + (s->img_y-1-j)*s->img_x*target;
         for (i=0; i < (int) s->img_x*target; ++i) {
            t = p1[i]; p1[i] = p2[i]; p2[i] = t;
         }
      }
   }

   if (req_comp && req_comp != target) {
      out = stbi__convert_format(out, target, req_comp, s->img_x, s->img_y);
      if (out == NULL) return out; // stbi__convert_format frees input on failure
   }

   *x = s->img_x;
   *y = s->img_y;
   if (comp) *comp = s->img_n;
   return out;
}
#endif

// Targa Truevision - TGA
// by Jonathan Dummer
#ifndef STBI_NO_TGA
// returns STBI_rgb or whatever, 0 on error
static int stbi__tga_get_comp(int bits_per_pixel, int is_grey, int* is_rgb16)
{
   // only RGB or RGBA (incl. 16bit) or grey allowed
   if (is_rgb16) *is_rgb16 = 0;
   switch(bits_per_pixel) {
      case 8:  return STBI_grey;
      case 16: if(is_grey) return STBI_grey_alpha;
               // fallthrough
      case 15: if(is_rgb16) *is_rgb16 = 1;
               return STBI_rgb;
      case 24: // fallthrough
      case 32: return bits_per_pixel/8;
      default: return 0;
   }
}

static int stbi__tga_info(stbi__context *s, int *x, int *y, int *comp)
{
    int tga_w, tga_h, tga_comp, tga_image_type, tga_bits_per_pixel, tga_colormap_bpp;
    int sz, tga_colormap_type;
    stbi__get8(s);                   // discard Offset
    tga_colormap_type = stbi__get8(s); // colormap type
    if( tga_colormap_type > 1 ) {
        stbi__rewind(s);
        return 0;      // only RGB or indexed allowed
    }
    tga_image_type = stbi__get8(s); // image type
    if ( tga_colormap_type == 1 ) { // colormapped (paletted) image
        if (tga_image_type != 1 && tga_image_type != 9) {
            stbi__rewind(s);
            return 0;
        }
        stbi__skip(s,4);       // skip index of first colormap entry and number of entries
        sz = stbi__get8(s);    //   check bits per palette color entry
        if ( (sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32) ) {
            stbi__rewind(s);
            return 0;
        }
        stbi__skip(s,4);       // skip image x and y origin
        tga_colormap_bpp = sz;
    } else { // "normal" image w/o colormap - only RGB or grey allowed, +/- RLE
        if ( (tga_image_type != 2) && (tga_image_type != 3) && (tga_image_type != 10) && (tga_image_type != 11) ) {
            stbi__rewind(s);
            return 0; // only RGB or grey allowed, +/- RLE
        }
        stbi__skip(s,9); // skip colormap specification and image x/y origin
        tga_colormap_bpp = 0;
    }
    tga_w = stbi__get16le(s);
    if( tga_w < 1 ) {
        stbi__rewind(s);
        return 0;   // test width
    }
    tga_h = stbi__get16le(s);
    if( tga_h < 1 ) {
        stbi__rewind(s);
        return 0;   // test height
    }
    tga_bits_per_pixel = stbi__get8(s); // bits per pixel
    stbi__get8(s); // ignore alpha bits
    if (tga_colormap_bpp != 0) {
        if((tga_bits_per_pixel != 8) && (tga_bits_per_pixel != 16)) {
            // when using a colormap, tga_bits_per_pixel is the size of the indexes
            // I don't think anything but 8 or 16bit indexes makes sense
            stbi__rewind(s);
            return 0;
        }
        tga_comp = stbi__tga_get_comp(tga_colormap_bpp, 0, NULL);
    } else {
        tga_comp = stbi__tga_get_comp(tga_bits_per_pixel, (tga_image_type == 3) || (tga_image_type == 11), NULL);
    }
    if(!tga_comp) {
      stbi__rewind(s);
      return 0;
    }
    if (x) *x = tga_w;
    if (y) *y = tga_h;
    if (comp) *comp = tga_comp;
    return 1;                   // seems to have passed everything
}

static int stbi__tga_test(stbi__context *s)
{
   int res = 0;
   int sz, tga_color_type;
   stbi__get8(s);      //   discard Offset
   tga_color_type = stbi__get8(s);   //   color type
   if ( tga_color_type > 1 ) goto errorEnd;   //   only RGB or indexed allowed
   sz = stbi__get8(s);   //   image type
   if ( tga_color_type == 1 ) { // colormapped (paletted) image
      if (sz != 1 && sz != 9) goto errorEnd; // colortype 1 demands image type 1 or 9
      stbi__skip(s,4);       // skip index of first colormap entry and number of entries
      sz = stbi__get8(s);    //   check bits per palette color entry
      if ( (sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32) ) goto errorEnd;
      stbi__skip(s,4);       // skip image x and y origin
   } else { // "normal" image w/o colormap
      if ( (sz != 2) && (sz != 3) && (sz != 10) && (sz != 11) ) goto errorEnd; // only RGB or grey allowed, +/- RLE
      stbi__skip(s,9); // skip colormap specification and image x/y origin
   }
   if ( stbi__get16le(s) < 1 ) goto errorEnd;      //   test width
   if ( stbi__get16le(s) < 1 ) goto errorEnd;      //   test height
   sz = stbi__get8(s);   //   bits per pixel
   if ( (tga_color_type == 1) && (sz != 8) && (sz != 16) ) goto errorEnd; // for colormapped images, bpp is size of an index
   if ( (sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32) ) goto errorEnd;

   res = 1; // if we got this far, everything's good and we can return 1 instead of 0

errorEnd:
   stbi__rewind(s);
   return res;
}

// read 16bit value and convert to 24bit RGB
static void stbi__tga_read_rgb16(stbi__context *s, stbi_uc* out)
{
   stbi__uint16 px = (stbi__uint16)stbi__get16le(s);
   stbi__uint16 fiveBitMask = 31;
   // we have 3 channels with 5bits each
   int r = (px >> 10) & fiveBitMask;
   int g = (px >> 5) & fiveBitMask;
   int b = px & fiveBitMask;
   // Note that this saves the data in RGB(A) order, so it doesn't need to be swapped later
   out[0] = (stbi_uc)((r * 255)/31);
   out[1] = (stbi_uc)((g * 255)/31);
   out[2] = (stbi_uc)((b * 255)/31);

   // some people claim that the most significant bit might be used for alpha
   // (possibly if an alpha-bit is set in the "image descriptor byte")
   // but that only made 16bit test images completely translucent..
   // so let's treat all 15 and 16bit TGAs as RGB with no alpha.
}

static void *stbi__tga_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   //   read in the TGA header stuff
   int tga_offset = stbi__get8(s);
   int tga_indexed = stbi__get8(s);
   int tga_image_type = stbi__get8(s);
   int tga_is_RLE = 0;
   int tga_palette_start = stbi__get16le(s);
   int tga_palette_len = stbi__get16le(s);
   int tga_palette_bits = stbi__get8(s);
   int tga_x_origin = stbi__get16le(s);
   int tga_y_origin = stbi__get16le(s);
   int tga_width = stbi__get16le(s);
   int tga_height = stbi__get16le(s);
   int tga_bits_per_pixel = stbi__get8(s);
   int tga_comp, tga_rgb16=0;
   int tga_inverted = stbi__get8(s);
   // int tga_alpha_bits = tga_inverted & 15; // the 4 lowest bits - unused (useless?)
   //   image data
   unsigned char *tga_data;
   unsigned char *tga_palette = NULL;
   int i, j;
   unsigned char raw_data[4] = {0};
   int RLE_count = 0;
   int RLE_repeating = 0;
   int read_next_pixel = 1;
   STBI_NOTUSED(ri);
   STBI_NOTUSED(tga_x_origin); // @TODO
   STBI_NOTUSED(tga_y_origin); // @TODO

   if (tga_height > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (tga_width > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   //   do a tiny bit of precessing
   if ( tga_image_type >= 8 )
   {
      tga_image_type -= 8;
      tga_is_RLE = 1;
   }
   tga_inverted = 1 - ((tga_inverted >> 5) & 1);

   //   If I'm paletted, then I'll use the number of bits from the palette
   if ( tga_indexed ) tga_comp = stbi__tga_get_comp(tga_palette_bits, 0, &tga_rgb16);
   else tga_comp = stbi__tga_get_comp(tga_bits_per_pixel, (tga_image_type == 3), &tga_rgb16);

   if(!tga_comp) // shouldn't really happen, stbi__tga_test() should have ensured basic consistency
      return stbi__errpuc("bad format", "Can't find out TGA pixelformat");

   //   tga info
   *x = tga_width;
   *y = tga_height;
   if (comp) *comp = tga_comp;

   if (!stbi__mad3sizes_valid(tga_width, tga_height, tga_comp, 0))
      return stbi__errpuc("too large", "Corrupt TGA");

   tga_data = (unsigned char*)stbi__malloc_mad3(tga_width, tga_height, tga_comp, 0);
   if (!tga_data) return stbi__errpuc("outofmem", "Out of memory");

   // skip to the data's starting position (offset usually = 0)
   stbi__skip(s, tga_offset );

   if ( !tga_indexed && !tga_is_RLE && !tga_rgb16 ) {
      for (i=0; i < tga_height; ++i) {
         int row = tga_inverted ? tga_height -i - 1 : i;
         stbi_uc *tga_row = tga_data + row*tga_width*tga_comp;
         stbi__getn(s, tga_row, tga_width * tga_comp);
      }
   } else  {
      //   do I need to load a palette?
      if ( tga_indexed)
      {
         if (tga_palette_len == 0) {  /* you have to have at least one entry! */
            STBI_FREE(tga_data);
            return stbi__errpuc("bad palette", "Corrupt TGA");
         }

         //   any data to skip? (offset usually = 0)
         stbi__skip(s, tga_palette_start );
         //   load the palette
         tga_palette = (unsigned char*)stbi__malloc_mad2(tga_palette_len, tga_comp, 0);
         if (!tga_palette) {
            STBI_FREE(tga_data);
            return stbi__errpuc("outofmem", "Out of memory");
         }
         if (tga_rgb16) {
            stbi_uc *pal_entry = tga_palette;
            STBI_ASSERT(tga_comp == STBI_rgb);
            for (i=0; i < tga_palette_len; ++i) {
               stbi__tga_read_rgb16(s, pal_entry);
               pal_entry += tga_comp;
            }
         } else if (!stbi__getn(s, tga_palette, tga_palette_len * tga_comp)) {
               STBI_FREE(tga_data);
               STBI_FREE(tga_palette);
               return stbi__errpuc("bad palette", "Corrupt TGA");
         }
      }
      //   load the data
      for (i=0; i < tga_width * tga_height; ++i)
      {
         //   if I'm in RLE mode, do I need to get a RLE stbi__pngchunk?
         if ( tga_is_RLE )
         {
            if ( RLE_count == 0 )
            {
               //   yep, get the next byte as a RLE command
               int RLE_cmd = stbi__get8(s);
               RLE_count = 1 + (RLE_cmd & 127);
               RLE_repeating = RLE_cmd >> 7;
               read_next_pixel = 1;
            } else if ( !RLE_repeating )
            {
               read_next_pixel = 1;
            }
         } else
         {
            read_next_pixel = 1;
         }
         //   OK, if I need to read a pixel, do it now
         if ( read_next_pixel )
         {
            //   load however much data we did have
            if ( tga_indexed )
            {
               // read in index, then perform the lookup
               int pal_idx = (tga_bits_per_pixel == 8) ? stbi__get8(s) : stbi__get16le(s);
               if ( pal_idx >= tga_palette_len ) {
                  // invalid index
                  pal_idx = 0;
               }
               pal_idx *= tga_comp;
               for (j = 0; j < tga_comp; ++j) {
                  raw_data[j] = tga_palette[pal_idx+j];
               }
            } else if(tga_rgb16) {
               STBI_ASSERT(tga_comp == STBI_rgb);
               stbi__tga_read_rgb16(s, raw_data);
            } else {
               //   read in the data raw
               for (j = 0; j < tga_comp; ++j) {
                  raw_data[j] = stbi__get8(s);
               }
            }
            //   clear the reading flag for the next pixel
            read_next_pixel = 0;
         } // end of reading a pixel

         // copy data
         for (j = 0; j < tga_comp; ++j)
           tga_data[i*tga_comp+j] = raw_data[j];

         //   in case we're in RLE mode, keep counting down
         --RLE_count;
      }
      //   do I need to invert the image?
      if ( tga_inverted )
      {
         for (j = 0; j*2 < tga_height; ++j)
         {
            int index1 = j * tga_width * tga_comp;
            int index2 = (tga_height - 1 - j) * tga_width * tga_comp;
            for (i = tga_width * tga_comp; i > 0; --i)
            {
               unsigned char temp = tga_data[index1];
               tga_data[index1] = tga_data[index2];
               tga_data[index2] = temp;
               ++index1;
               ++index2;
            }
         }
      }
      //   clear my palette, if I had one
      if ( tga_palette != NULL )
      {
         STBI_FREE( tga_palette );
      }
   }

   // swap RGB - if the source data was RGB16, it already is in the right order
   if (tga_comp >= 3 && !tga_rgb16)
   {
      unsigned char* tga_pixel = tga_data;
      for (i=0; i < tga_width * tga_height; ++i)
      {
         unsigned char temp = tga_pixel[0];
         tga_pixel[0] = tga_pixel[2];
         tga_pixel[2] = temp;
         tga_pixel += tga_comp;
      }
   }

   // convert to target component count
   if (req_comp && req_comp != tga_comp)
      tga_data = stbi__convert_format(tga_data, tga_comp, req_comp, tga_width, tga_height);

   //   the things I do to get rid of an error message, and yet keep
   //   Microsoft's C compilers happy... [8^(
   tga_palette_start = tga_palette_len = tga_palette_bits =
         tga_x_origin = tga_y_origin = 0;
   STBI_NOTUSED(tga_palette_start);
   //   OK, done
   return tga_data;
}
#endif

// *************************************************************************************************
// Photoshop PSD loader -- PD by Thatcher Ulrich, integration by Nicolas Schulz, tweaked by STB

#ifndef STBI_NO_PSD
static int stbi__psd_test(stbi__context *s)
{
   int r = (stbi__get32be(s) == 0x38425053);
   stbi__rewind(s);
   return r;
}

static int stbi__psd_decode_rle(stbi__context *s, stbi_uc *p, int pixelCount)
{
   int count, nleft, len;

   count = 0;
   while ((nleft = pixelCount - count) > 0) {
      len = stbi__get8(s);
      if (len == 128) {
         // No-op.
      } else if (len < 128) {
         // Copy next len+1 bytes literally.
         len++;
         if (len > nleft) return 0; // corrupt data
         count += len;
         while (len) {
            *p = stbi__get8(s);
            p += 4;
            len--;
         }
      } else if (len > 128) {
         stbi_uc   val;
         // Next -len+1 bytes in the dest are replicated from next source byte.
         // (Interpret len as a negative 8-bit int.)
         len = 257 - len;
         if (len > nleft) return 0; // corrupt data
         val = stbi__get8(s);
         count += len;
         while (len) {
            *p = val;
            p += 4;
            len--;
         }
      }
   }

   return 1;
}

static void *stbi__psd_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri, int bpc)
{
   int pixelCount;
   int channelCount, compression;
   int channel, i;
   int bitdepth;
   int w,h;
   stbi_uc *out;
   STBI_NOTUSED(ri);

   // Check identifier
   if (stbi__get32be(s) != 0x38425053)   // "8BPS"
      return stbi__errpuc("not PSD", "Corrupt PSD image");

   // Check file type version.
   if (stbi__get16be(s) != 1)
      return stbi__errpuc("wrong version", "Unsupported version of PSD image");

   // Skip 6 reserved bytes.
   stbi__skip(s, 6 );

   // Read the number of channels (R, G, B, A, etc).
   channelCount = stbi__get16be(s);
   if (channelCount < 0 || channelCount > 16)
      return stbi__errpuc("wrong channel count", "Unsupported number of channels in PSD image");

   // Read the rows and columns of the image.
   h = stbi__get32be(s);
   w = stbi__get32be(s);

   if (h > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (w > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   // Make sure the depth is 8 bits.
   bitdepth = stbi__get16be(s);
   if (bitdepth != 8 && bitdepth != 16)
      return stbi__errpuc("unsupported bit depth", "PSD bit depth is not 8 or 16 bit");

   // Make sure the color mode is RGB.
   // Valid options are:
   //   0: Bitmap
   //   1: Grayscale
   //   2: Indexed color
   //   3: RGB color
   //   4: CMYK color
   //   7: Multichannel
   //   8: Duotone
   //   9: Lab color
   if (stbi__get16be(s) != 3)
      return stbi__errpuc("wrong color format", "PSD is not in RGB color format");

   // Skip the Mode Data.  (It's the palette for indexed color; other info for other modes.)
   stbi__skip(s,stbi__get32be(s) );

   // Skip the image resources.  (resolution, pen tool paths, etc)
   stbi__skip(s, stbi__get32be(s) );

   // Skip the reserved data.
   stbi__skip(s, stbi__get32be(s) );

   // Find out if the data is compressed.
   // Known values:
   //   0: no compression
   //   1: RLE compressed
   compression = stbi__get16be(s);
   if (compression > 1)
      return stbi__errpuc("bad compression", "PSD has an unknown compression format");

   // Check size
   if (!stbi__mad3sizes_valid(4, w, h, 0))
      return stbi__errpuc("too large", "Corrupt PSD");

   // Create the destination image.

   if (!compression && bitdepth == 16 && bpc == 16) {
      out = (stbi_uc *) stbi__malloc_mad3(8, w, h, 0);
      ri->bits_per_channel = 16;
   } else
      out = (stbi_uc *) stbi__malloc(4 * w*h);

   if (!out) return stbi__errpuc("outofmem", "Out of memory");
   pixelCount = w*h;

   // Initialize the data to zero.
   //memset( out, 0, pixelCount * 4 );

   // Finally, the image data.
   if (compression) {
      // RLE as used by .PSD and .TIFF
      // Loop until you get the number of unpacked bytes you are expecting:
      //     Read the next source byte into n.
      //     If n is between 0 and 127 inclusive, copy the next n+1 bytes literally.
      //     Else if n is between -127 and -1 inclusive, copy the next byte -n+1 times.
      //     Else if n is 128, noop.
      // Endloop

      // The RLE-compressed data is preceded by a 2-byte data count for each row in the data,
      // which we're going to just skip.
      stbi__skip(s, h * channelCount * 2 );

      // Read the RLE data by channel.
      for (channel = 0; channel < 4; channel++) {
         stbi_uc *p;

         p = out+channel;
         if (channel >= channelCount) {
            // Fill this channel with default data.
            for (i = 0; i < pixelCount; i++, p += 4)
               *p = (channel == 3 ? 255 : 0);
         } else {
            // Read the RLE data.
            if (!stbi__psd_decode_rle(s, p, pixelCount)) {
               STBI_FREE(out);
               return stbi__errpuc("corrupt", "bad RLE data");
            }
         }
      }

   } else {
      // We're at the raw image data.  It's each channel in order (Red, Green, Blue, Alpha, ...)
      // where each channel consists of an 8-bit (or 16-bit) value for each pixel in the image.

      // Read the data by channel.
      for (channel = 0; channel < 4; channel++) {
         if (channel >= channelCount) {
            // Fill this channel with default data.
            if (bitdepth == 16 && bpc == 16) {
               stbi__uint16 *q = ((stbi__uint16 *) out) + channel;
               stbi__uint16 val = channel == 3 ? 65535 : 0;
               for (i = 0; i < pixelCount; i++, q += 4)
                  *q = val;
            } else {
               stbi_uc *p = out+channel;
               stbi_uc val = channel == 3 ? 255 : 0;
               for (i = 0; i < pixelCount; i++, p += 4)
                  *p = val;
            }
         } else {
            if (ri->bits_per_channel == 16) {    // output bpc
               stbi__uint16 *q = ((stbi__uint16 *) out) + channel;
               for (i = 0; i < pixelCount; i++, q += 4)
                  *q = (stbi__uint16) stbi__get16be(s);
            } else {
               stbi_uc *p = out+channel;
               if (bitdepth == 16) {  // input bpc
                  for (i = 0; i < pixelCount; i++, p += 4)
                     *p = (stbi_uc) (stbi__get16be(s) >> 8);
               } else {
                  for (i = 0; i < pixelCount; i++, p += 4)
                     *p = stbi__get8(s);
               }
            }
         }
      }
   }

   // remove weird white matte from PSD
   if (channelCount >= 4) {
      if (ri->bits_per_channel == 16) {
         for (i=0; i < w*h; ++i) {
            stbi__uint16 *pixel = (stbi__uint16 *) out + 4*i;
            if (pixel[3] != 0 && pixel[3] != 65535) {
               float a = pixel[3] / 65535.0f;
               float ra = 1.0f / a;
               float inv_a = 65535.0f * (1 - ra);
               pixel[0] = (stbi__uint16) (pixel[0]*ra + inv_a);
               pixel[1] = (stbi__uint16) (pixel[1]*ra + inv_a);
               pixel[2] = (stbi__uint16) (pixel[2]*ra + inv_a);
            }
         }
      } else {
         for (i=0; i < w*h; ++i) {
            unsigned char *pixel = out + 4*i;
            if (pixel[3] != 0 && pixel[3] != 255) {
               float a = pixel[3] / 255.0f;
               float ra = 1.0f / a;
               float inv_a = 255.0f * (1 - ra);
               pixel[0] = (unsigned char) (pixel[0]*ra + inv_a);
               pixel[1] = (unsigned char) (pixel[1]*ra + inv_a);
               pixel[2] = (unsigned char) (pixel[2]*ra + inv_a);
            }
         }
      }
   }

   // convert to desired output format
   if (req_comp && req_comp != 4) {
      if (ri->bits_per_channel == 16)
         out = (stbi_uc *) stbi__convert_format16((stbi__uint16 *) out, 4, req_comp, w, h);
      else
         out = stbi__convert_format(out, 4, req_comp, w, h);
      if (out == NULL) return out; // stbi__convert_format frees input on failure
   }

   if (comp) *comp = 4;
   *y = h;
   *x = w;

   return out;
}
#endif

// *************************************************************************************************
// Softimage PIC loader
// by Tom Seddon
//
// See http://softimage.wiki.softimage.com/index.php/INFO:_PIC_file_format
// See http://ozviz.wasp.uwa.edu.au/~pbourke/dataformats/softimagepic/

#ifndef STBI_NO_PIC
static int stbi__pic_is4(stbi__context *s,const char *str)
{
   int i;
   for (i=0; i<4; ++i)
      if (stbi__get8(s) != (stbi_uc)str[i])
         return 0;

   return 1;
}

static int stbi__pic_test_core(stbi__context *s)
{
   int i;

   if (!stbi__pic_is4(s,"\x53\x80\xF6\x34"))
      return 0;

   for(i=0;i<84;++i)
      stbi__get8(s);

   if (!stbi__pic_is4(s,"PICT"))
      return 0;

   return 1;
}

typedef struct
{
   stbi_uc size,type,channel;
} stbi__pic_packet;

static stbi_uc *stbi__readval(stbi__context *s, int channel, stbi_uc *dest)
{
   int mask=0x80, i;

   for (i=0; i<4; ++i, mask>>=1) {
      if (channel & mask) {
         if (stbi__at_eof(s)) return stbi__errpuc("bad file","PIC file too short");
         dest[i]=stbi__get8(s);
      }
   }

   return dest;
}

static void stbi__copyval(int channel,stbi_uc *dest,const stbi_uc *src)
{
   int mask=0x80,i;

   for (i=0;i<4; ++i, mask>>=1)
      if (channel&mask)
         dest[i]=src[i];
}

static stbi_uc *stbi__pic_load_core(stbi__context *s,int width,int height,int *comp, stbi_uc *result)
{
   int act_comp=0,num_packets=0,y,chained;
   stbi__pic_packet packets[10];

   // this will (should...) cater for even some bizarre stuff like having data
    // for the same channel in multiple packets.
   do {
      stbi__pic_packet *packet;

      if (num_packets==sizeof(packets)/sizeof(packets[0]))
         return stbi__errpuc("bad format","too many packets");

      packet = &packets[num_packets++];

      chained = stbi__get8(s);
      packet->size    = stbi__get8(s);
      packet->type    = stbi__get8(s);
      packet->channel = stbi__get8(s);

      act_comp |= packet->channel;

      if (stbi__at_eof(s))          return stbi__errpuc("bad file","file too short (reading packets)");
      if (packet->size != 8)  return stbi__errpuc("bad format","packet isn't 8bpp");
   } while (chained);

   *comp = (act_comp & 0x10 ? 4 : 3); // has alpha channel?

   for(y=0; y<height; ++y) {
      int packet_idx;

      for(packet_idx=0; packet_idx < num_packets; ++packet_idx) {
         stbi__pic_packet *packet = &packets[packet_idx];
         stbi_uc *dest = result+y*width*4;

         switch (packet->type) {
            default:
               return stbi__errpuc("bad format","packet has bad compression type");

            case 0: {//uncompressed
               int x;

               for(x=0;x<width;++x, dest+=4)
                  if (!stbi__readval(s,packet->channel,dest))
                     return 0;
               break;
            }

            case 1://Pure RLE
               {
                  int left=width, i;

                  while (left>0) {
                     stbi_uc count,value[4];

                     count=stbi__get8(s);
                     if (stbi__at_eof(s))   return stbi__errpuc("bad file","file too short (pure read count)");

                     if (count > left)
                        count = (stbi_uc) left;

                     if (!stbi__readval(s,packet->channel,value))  return 0;

                     for(i=0; i<count; ++i,dest+=4)
                        stbi__copyval(packet->channel,dest,value);
                     left -= count;
                  }
               }
               break;

            case 2: {//Mixed RLE
               int left=width;
               while (left>0) {
                  int count = stbi__get8(s), i;
                  if (stbi__at_eof(s))  return stbi__errpuc("bad file","file too short (mixed read count)");

                  if (count >= 128) { // Repeated
                     stbi_uc value[4];

                     if (count==128)
                        count = stbi__get16be(s);
                     else
                        count -= 127;
                     if (count > left)
                        return stbi__errpuc("bad file","scanline overrun");

                     if (!stbi__readval(s,packet->channel,value))
                        return 0;

                     for(i=0;i<count;++i, dest += 4)
                        stbi__copyval(packet->channel,dest,value);
                  } else { // Raw
                     ++count;
                     if (count>left) return stbi__errpuc("bad file","scanline overrun");

                     for(i=0;i<count;++i, dest+=4)
                        if (!stbi__readval(s,packet->channel,dest))
                           return 0;
                  }
                  left-=count;
               }
               break;
            }
         }
      }
   }

   return result;
}

static void *stbi__pic_load(stbi__context *s,int *px,int *py,int *comp,int req_comp, stbi__result_info *ri)
{
   stbi_uc *result;
   int i, x,y, internal_comp;
   STBI_NOTUSED(ri);

   if (!comp) comp = &internal_comp;

   for (i=0; i<92; ++i)
      stbi__get8(s);

   x = stbi__get16be(s);
   y = stbi__get16be(s);

   if (y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   if (stbi__at_eof(s))  return stbi__errpuc("bad file","file too short (pic header)");
   if (!stbi__mad3sizes_valid(x, y, 4, 0)) return stbi__errpuc("too large", "PIC image too large to decode");

   stbi__get32be(s); //skip `ratio'
   stbi__get16be(s); //skip `fields'
   stbi__get16be(s); //skip `pad'

   // intermediate buffer is RGBA
   result = (stbi_uc *) stbi__malloc_mad3(x, y, 4, 0);
   if (!result) return stbi__errpuc("outofmem", "Out of memory");
   memset(result, 0xff, x*y*4);

   if (!stbi__pic_load_core(s,x,y,comp, result)) {
      STBI_FREE(result);
      result=0;
   }
   *px = x;
   *py = y;
   if (req_comp == 0) req_comp = *comp;
   result=stbi__convert_format(result,4,req_comp,x,y);

   return result;
}

static int stbi__pic_test(stbi__context *s)
{
   int r = stbi__pic_test_core(s);
   stbi__rewind(s);
   return r;
}
#endif

// *************************************************************************************************
// GIF loader -- public domain by Jean-Marc Lienher -- simplified/shrunk by stb

#ifndef STBI_NO_GIF
typedef struct
{
   stbi__int16 prefix;
   stbi_uc first;
   stbi_uc suffix;
} stbi__gif_lzw;

typedef struct
{
   int w,h;
   stbi_uc *out;                 // output buffer (always 4 components)
   stbi_uc *background;          // The current "background" as far as a gif is concerned
   stbi_uc *history;
   int flags, bgindex, ratio, transparent, eflags;
   stbi_uc  pal[256][4];
   stbi_uc lpal[256][4];
   stbi__gif_lzw codes[8192];
   stbi_uc *color_table;
   int parse, step;
   int lflags;
   int start_x, start_y;
   int max_x, max_y;
   int cur_x, cur_y;
   int line_size;
   int delay;
} stbi__gif;

static int stbi__gif_test_raw(stbi__context *s)
{
   int sz;
   if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8') return 0;
   sz = stbi__get8(s);
   if (sz != '9' && sz != '7') return 0;
   if (stbi__get8(s) != 'a') return 0;
   return 1;
}

static int stbi__gif_test(stbi__context *s)
{
   int r = stbi__gif_test_raw(s);
   stbi__rewind(s);
   return r;
}

static void stbi__gif_parse_colortable(stbi__context *s, stbi_uc pal[256][4], int num_entries, int transp)
{
   int i;
   for (i=0; i < num_entries; ++i) {
      pal[i][2] = stbi__get8(s);
      pal[i][1] = stbi__get8(s);
      pal[i][0] = stbi__get8(s);
      pal[i][3] = transp == i ? 0 : 255;
   }
}

static int stbi__gif_header(stbi__context *s, stbi__gif *g, int *comp, int is_info)
{
   stbi_uc version;
   if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' || stbi__get8(s) != '8')
      return stbi__err("not GIF", "Corrupt GIF");

   version = stbi__get8(s);
   if (version != '7' && version != '9')    return stbi__err("not GIF", "Corrupt GIF");
   if (stbi__get8(s) != 'a')                return stbi__err("not GIF", "Corrupt GIF");

   stbi__g_failure_reason = "";
   g->w = stbi__get16le(s);
   g->h = stbi__get16le(s);
   g->flags = stbi__get8(s);
   g->bgindex = stbi__get8(s);
   g->ratio = stbi__get8(s);
   g->transparent = -1;

   if (g->w > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");
   if (g->h > STBI_MAX_DIMENSIONS) return stbi__err("too large","Very large image (corrupt?)");

   if (comp != 0) *comp = 4;  // can't actually tell whether it's 3 or 4 until we parse the comments

   if (is_info) return 1;

   if (g->flags & 0x80)
      stbi__gif_parse_colortable(s,g->pal, 2 << (g->flags & 7), -1);

   return 1;
}

static int stbi__gif_info_raw(stbi__context *s, int *x, int *y, int *comp)
{
   stbi__gif* g = (stbi__gif*) stbi__malloc(sizeof(stbi__gif));
   if (!g) return stbi__err("outofmem", "Out of memory");
   if (!stbi__gif_header(s, g, comp, 1)) {
      STBI_FREE(g);
      stbi__rewind( s );
      return 0;
   }
   if (x) *x = g->w;
   if (y) *y = g->h;
   STBI_FREE(g);
   return 1;
}

static void stbi__out_gif_code(stbi__gif *g, stbi__uint16 code)
{
   stbi_uc *p, *c;
   int idx;

   // recurse to decode the prefixes, since the linked-list is backwards,
   // and working backwards through an interleaved image would be nasty
   if (g->codes[code].prefix >= 0)
      stbi__out_gif_code(g, g->codes[code].prefix);

   if (g->cur_y >= g->max_y) return;

   idx = g->cur_x + g->cur_y;
   p = &g->out[idx];
   g->history[idx / 4] = 1;

   c = &g->color_table[g->codes[code].suffix * 4];
   if (c[3] > 128) { // don't render transparent pixels;
      p[0] = c[2];
      p[1] = c[1];
      p[2] = c[0];
      p[3] = c[3];
   }
   g->cur_x += 4;

   if (g->cur_x >= g->max_x) {
      g->cur_x = g->start_x;
      g->cur_y += g->step;

      while (g->cur_y >= g->max_y && g->parse > 0) {
         g->step = (1 << g->parse) * g->line_size;
         g->cur_y = g->start_y + (g->step >> 1);
         --g->parse;
      }
   }
}

static stbi_uc *stbi__process_gif_raster(stbi__context *s, stbi__gif *g)
{
   stbi_uc lzw_cs;
   stbi__int32 len, init_code;
   stbi__uint32 first;
   stbi__int32 codesize, codemask, avail, oldcode, bits, valid_bits, clear;
   stbi__gif_lzw *p;

   lzw_cs = stbi__get8(s);
   if (lzw_cs > 12) return NULL;
   clear = 1 << lzw_cs;
   first = 1;
   codesize = lzw_cs + 1;
   codemask = (1 << codesize) - 1;
   bits = 0;
   valid_bits = 0;
   for (init_code = 0; init_code < clear; init_code++) {
      g->codes[init_code].prefix = -1;
      g->codes[init_code].first = (stbi_uc) init_code;
      g->codes[init_code].suffix = (stbi_uc) init_code;
   }

   // support no starting clear code
   avail = clear+2;
   oldcode = -1;

   len = 0;
   for(;;) {
      if (valid_bits < codesize) {
         if (len == 0) {
            len = stbi__get8(s); // start new block
            if (len == 0)
               return g->out;
         }
         --len;
         bits |= (stbi__int32) stbi__get8(s) << valid_bits;
         valid_bits += 8;
      } else {
         stbi__int32 code = bits & codemask;
         bits >>= codesize;
         valid_bits -= codesize;
         // @OPTIMIZE: is there some way we can accelerate the non-clear path?
         if (code == clear) {  // clear code
            codesize = lzw_cs + 1;
            codemask = (1 << codesize) - 1;
            avail = clear + 2;
            oldcode = -1;
            first = 0;
         } else if (code == clear + 1) { // end of stream code
            stbi__skip(s, len);
            while ((len = stbi__get8(s)) > 0)
               stbi__skip(s,len);
            return g->out;
         } else if (code <= avail) {
            if (first) {
               return stbi__errpuc("no clear code", "Corrupt GIF");
            }

            if (oldcode >= 0) {
               p = &g->codes[avail++];
               if (avail > 8192) {
                  return stbi__errpuc("too many codes", "Corrupt GIF");
               }

               p->prefix = (stbi__int16) oldcode;
               p->first = g->codes[oldcode].first;
               p->suffix = (code == avail) ? p->first : g->codes[code].first;
            } else if (code == avail)
               return stbi__errpuc("illegal code in raster", "Corrupt GIF");

            stbi__out_gif_code(g, (stbi__uint16) code);

            if ((avail & codemask) == 0 && avail <= 0x0FFF) {
               codesize++;
               codemask = (1 << codesize) - 1;
            }

            oldcode = code;
         } else {
            return stbi__errpuc("illegal code in raster", "Corrupt GIF");
         }
      }
   }
}

// this function is designed to support animated gifs, although stb_image doesn't support it
// two back is the image from two frames ago, used for a very specific disposal format
static stbi_uc *stbi__gif_load_next(stbi__context *s, stbi__gif *g, int *comp, int req_comp, stbi_uc *two_back)
{
   int dispose;
   int first_frame;
   int pi;
   int pcount;
   STBI_NOTUSED(req_comp);

   // on first frame, any non-written pixels get the background colour (non-transparent)
   first_frame = 0;
   if (g->out == 0) {
      if (!stbi__gif_header(s, g, comp,0)) return 0; // stbi__g_failure_reason set by stbi__gif_header
      if (!stbi__mad3sizes_valid(4, g->w, g->h, 0))
         return stbi__errpuc("too large", "GIF image is too large");
      pcount = g->w * g->h;
      g->out = (stbi_uc *) stbi__malloc(4 * pcount);
      g->background = (stbi_uc *) stbi__malloc(4 * pcount);
      g->history = (stbi_uc *) stbi__malloc(pcount);
      if (!g->out || !g->background || !g->history)
         return stbi__errpuc("outofmem", "Out of memory");

      // image is treated as "transparent" at the start - ie, nothing overwrites the current background;
      // background colour is only used for pixels that are not rendered first frame, after that "background"
      // color refers to the color that was there the previous frame.
      memset(g->out, 0x00, 4 * pcount);
      memset(g->background, 0x00, 4 * pcount); // state of the background (starts transparent)
      memset(g->history, 0x00, pcount);        // pixels that were affected previous frame
      first_frame = 1;
   } else {
      // second frame - how do we dispose of the previous one?
      dispose = (g->eflags & 0x1C) >> 2;
      pcount = g->w * g->h;

      if ((dispose == 3) && (two_back == 0)) {
         dispose = 2; // if I don't have an image to revert back to, default to the old background
      }

      if (dispose == 3) { // use previous graphic
         for (pi = 0; pi < pcount; ++pi) {
            if (g->history[pi]) {
               memcpy( &g->out[pi * 4], &two_back[pi * 4], 4 );
            }
         }
      } else if (dispose == 2) {
         // restore what was changed last frame to background before that frame;
         for (pi = 0; pi < pcount; ++pi) {
            if (g->history[pi]) {
               memcpy( &g->out[pi * 4], &g->background[pi * 4], 4 );
            }
         }
      } else {
         // This is a non-disposal case eithe way, so just
         // leave the pixels as is, and they will become the new background
         // 1: do not dispose
         // 0:  not specified.
      }

      // background is what out is after the undoing of the previou frame;
      memcpy( g->background, g->out, 4 * g->w * g->h );
   }

   // clear my history;
   memset( g->history, 0x00, g->w * g->h );        // pixels that were affected previous frame

   for (;;) {
      int tag = stbi__get8(s);
      switch (tag) {
         case 0x2C: /* Image Descriptor */
         {
            stbi__int32 x, y, w, h;
            stbi_uc *o;

            x = stbi__get16le(s);
            y = stbi__get16le(s);
            w = stbi__get16le(s);
            h = stbi__get16le(s);
            if (((x + w) > (g->w)) || ((y + h) > (g->h)))
               return stbi__errpuc("bad Image Descriptor", "Corrupt GIF");

            g->line_size = g->w * 4;
            g->start_x = x * 4;
            g->start_y = y * g->line_size;
            g->max_x   = g->start_x + w * 4;
            g->max_y   = g->start_y + h * g->line_size;
            g->cur_x   = g->start_x;
            g->cur_y   = g->start_y;

            // if the width of the specified rectangle is 0, that means
            // we may not see *any* pixels or the image is malformed;
            // to make sure this is caught, move the current y down to
            // max_y (which is what out_gif_code checks).
            if (w == 0)
               g->cur_y = g->max_y;

            g->lflags = stbi__get8(s);

            if (g->lflags & 0x40) {
               g->step = 8 * g->line_size; // first interlaced spacing
               g->parse = 3;
            } else {
               g->step = g->line_size;
               g->parse = 0;
            }

            if (g->lflags & 0x80) {
               stbi__gif_parse_colortable(s,g->lpal, 2 << (g->lflags & 7), g->eflags & 0x01 ? g->transparent : -1);
               g->color_table = (stbi_uc *) g->lpal;
            } else if (g->flags & 0x80) {
               g->color_table = (stbi_uc *) g->pal;
            } else
               return stbi__errpuc("missing color table", "Corrupt GIF");

            o = stbi__process_gif_raster(s, g);
            if (!o) return NULL;

            // if this was the first frame,
            pcount = g->w * g->h;
            if (first_frame && (g->bgindex > 0)) {
               // if first frame, any pixel not drawn to gets the background color
               for (pi = 0; pi < pcount; ++pi) {
                  if (g->history[pi] == 0) {
                     g->pal[g->bgindex][3] = 255; // just in case it was made transparent, undo that; It will be reset next frame if need be;
                     memcpy( &g->out[pi * 4], &g->pal[g->bgindex], 4 );
                  }
               }
            }

            return o;
         }

         case 0x21: // Comment Extension.
         {
            int len;
            int ext = stbi__get8(s);
            if (ext == 0xF9) { // Graphic Control Extension.
               len = stbi__get8(s);
               if (len == 4) {
                  g->eflags = stbi__get8(s);
                  g->delay = 10 * stbi__get16le(s); // delay - 1/100th of a second, saving as 1/1000ths.

                  // unset old transparent
                  if (g->transparent >= 0) {
                     g->pal[g->transparent][3] = 255;
                  }
                  if (g->eflags & 0x01) {
                     g->transparent = stbi__get8(s);
                     if (g->transparent >= 0) {
                        g->pal[g->transparent][3] = 0;
                     }
                  } else {
                     // don't need transparent
                     stbi__skip(s, 1);
                     g->transparent = -1;
                  }
               } else {
                  stbi__skip(s, len);
                  break;
               }
            }
            while ((len = stbi__get8(s)) != 0) {
               stbi__skip(s, len);
            }
            break;
         }

         case 0x3B: // gif stream termination code
            return (stbi_uc *) s; // using '1' causes warning on some compilers

         default:
            return stbi__errpuc("unknown code", "Corrupt GIF");
      }
   }
}

static void *stbi__load_gif_main_outofmem(stbi__gif *g, stbi_uc *out, int **delays)
{
   STBI_FREE(g->out);
   STBI_FREE(g->history);
   STBI_FREE(g->background);

   if (out) STBI_FREE(out);
   if (delays && *delays) STBI_FREE(*delays);
   return stbi__errpuc("outofmem", "Out of memory");
}

static void *stbi__load_gif_main(stbi__context *s, int **delays, int *x, int *y, int *z, int *comp, int req_comp)
{
   if (stbi__gif_test(s)) {
      int layers = 0;
      stbi_uc *u = 0;
      stbi_uc *out = 0;
      stbi_uc *two_back = 0;
      stbi__gif g;
      int stride;
      int out_size = 0;
      int delays_size = 0;

      STBI_NOTUSED(out_size);
      STBI_NOTUSED(delays_size);

      memset(&g, 0, sizeof(g));
      if (delays) {
         *delays = 0;
      }

      do {
         u = stbi__gif_load_next(s, &g, comp, req_comp, two_back);
         if (u == (stbi_uc *) s) u = 0;  // end of animated gif marker

         if (u) {
            *x = g.w;
            *y = g.h;
            ++layers;
            stride = g.w * g.h * 4;

            if (out) {
               void *tmp = (stbi_uc*) STBI_REALLOC_SIZED( out, out_size, layers * stride );
               if (!tmp)
                  return stbi__load_gif_main_outofmem(&g, out, delays);
               else {
                   out = (stbi_uc*) tmp;
                   out_size = layers * stride;
               }

               if (delays) {
                  int *new_delays = (int*) STBI_REALLOC_SIZED( *delays, delays_size, sizeof(int) * layers );
                  if (!new_delays)
                     return stbi__load_gif_main_outofmem(&g, out, delays);
                  *delays = new_delays;
                  delays_size = layers * sizeof(int);
               }
            } else {
               out = (stbi_uc*)stbi__malloc( layers * stride );
               if (!out)
                  return stbi__load_gif_main_outofmem(&g, out, delays);
               out_size = layers * stride;
               if (delays) {
                  *delays = (int*) stbi__malloc( layers * sizeof(int) );
                  if (!*delays)
                     return stbi__load_gif_main_outofmem(&g, out, delays);
                  delays_size = layers * sizeof(int);
               }
            }
            memcpy( out + ((layers - 1) * stride), u, stride );
            if (layers >= 2) {
               two_back = out - 2 * stride;
            }

            if (delays) {
               (*delays)[layers - 1U] = g.delay;
            }
         }
      } while (u != 0);

      // free temp buffer;
      STBI_FREE(g.out);
      STBI_FREE(g.history);
      STBI_FREE(g.background);

      // do the final conversion after loading everything;
      if (req_comp && req_comp != 4)
         out = stbi__convert_format(out, 4, req_comp, layers * g.w, g.h);

      *z = layers;
      return out;
   } else {
      return stbi__errpuc("not GIF", "Image was not as a gif type.");
   }
}

static void *stbi__gif_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   stbi_uc *u = 0;
   stbi__gif g;
   memset(&g, 0, sizeof(g));
   STBI_NOTUSED(ri);

   u = stbi__gif_load_next(s, &g, comp, req_comp, 0);
   if (u == (stbi_uc *) s) u = 0;  // end of animated gif marker
   if (u) {
      *x = g.w;
      *y = g.h;

      // moved conversion to after successful load so that the same
      // can be done for multiple frames.
      if (req_comp && req_comp != 4)
         u = stbi__convert_format(u, 4, req_comp, g.w, g.h);
   } else if (g.out) {
      // if there was an error and we allocated an image buffer, free it!
      STBI_FREE(g.out);
   }

   // free buffers needed for multiple frame loading;
   STBI_FREE(g.history);
   STBI_FREE(g.background);

   return u;
}

static int stbi__gif_info(stbi__context *s, int *x, int *y, int *comp)
{
   return stbi__gif_info_raw(s,x,y,comp);
}
#endif

// *************************************************************************************************
// Radiance RGBE HDR loader
// originally by Nicolas Schulz
#ifndef STBI_NO_HDR
static int stbi__hdr_test_core(stbi__context *s, const char *signature)
{
   int i;
   for (i=0; signature[i]; ++i)
      if (stbi__get8(s) != signature[i])
          return 0;
   stbi__rewind(s);
   return 1;
}

static int stbi__hdr_test(stbi__context* s)
{
   int r = stbi__hdr_test_core(s, "#?RADIANCE\n");
   stbi__rewind(s);
   if(!r) {
       r = stbi__hdr_test_core(s, "#?RGBE\n");
       stbi__rewind(s);
   }
   return r;
}

#define STBI__HDR_BUFLEN  1024
static char *stbi__hdr_gettoken(stbi__context *z, char *buffer)
{
   int len=0;
   char c = '\0';

   c = (char) stbi__get8(z);

   while (!stbi__at_eof(z) && c != '\n') {
      buffer[len++] = c;
      if (len == STBI__HDR_BUFLEN-1) {
         // flush to end of line
         while (!stbi__at_eof(z) && stbi__get8(z) != '\n')
            ;
         break;
      }
      c = (char) stbi__get8(z);
   }

   buffer[len] = 0;
   return buffer;
}

static void stbi__hdr_convert(float *output, stbi_uc *input, int req_comp)
{
   if ( input[3] != 0 ) {
      float f1;
      // Exponent
      f1 = (float) ldexp(1.0f, input[3] - (int)(128 + 8));
      if (req_comp <= 2)
         output[0] = (input[0] + input[1] + input[2]) * f1 / 3;
      else {
         output[0] = input[0] * f1;
         output[1] = input[1] * f1;
         output[2] = input[2] * f1;
      }
      if (req_comp == 2) output[1] = 1;
      if (req_comp == 4) output[3] = 1;
   } else {
      switch (req_comp) {
         case 4: output[3] = 1; /* fallthrough */
         case 3: output[0] = output[1] = output[2] = 0;
                 break;
         case 2: output[1] = 1; /* fallthrough */
         case 1: output[0] = 0;
                 break;
      }
   }
}

static float *stbi__hdr_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   char buffer[STBI__HDR_BUFLEN];
   char *token;
   int valid = 0;
   int width, height;
   stbi_uc *scanline;
   float *hdr_data;
   int len;
   unsigned char count, value;
   int i, j, k, c1,c2, z;
   const char *headerToken;
   STBI_NOTUSED(ri);

   // Check identifier
   headerToken = stbi__hdr_gettoken(s,buffer);
   if (strcmp(headerToken, "#?RADIANCE") != 0 && strcmp(headerToken, "#?RGBE") != 0)
      return stbi__errpf("not HDR", "Corrupt HDR image");

   // Parse header
   for(;;) {
      token = stbi__hdr_gettoken(s,buffer);
      if (token[0] == 0) break;
      if (strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0) valid = 1;
   }

   if (!valid)    return stbi__errpf("unsupported format", "Unsupported HDR format");

   // Parse width and height
   // can't use sscanf() if we're not using stdio!
   token = stbi__hdr_gettoken(s,buffer);
   if (strncmp(token, "-Y ", 3))  return stbi__errpf("unsupported data layout", "Unsupported HDR format");
   token += 3;
   height = (int) strtol(token, &token, 10);
   while (*token == ' ') ++token;
   if (strncmp(token, "+X ", 3))  return stbi__errpf("unsupported data layout", "Unsupported HDR format");
   token += 3;
   width = (int) strtol(token, NULL, 10);

   if (height > STBI_MAX_DIMENSIONS) return stbi__errpf("too large","Very large image (corrupt?)");
   if (width > STBI_MAX_DIMENSIONS) return stbi__errpf("too large","Very large image (corrupt?)");

   *x = width;
   *y = height;

   if (comp) *comp = 3;
   if (req_comp == 0) req_comp = 3;

   if (!stbi__mad4sizes_valid(width, height, req_comp, sizeof(float), 0))
      return stbi__errpf("too large", "HDR image is too large");

   // Read data
   hdr_data = (float *) stbi__malloc_mad4(width, height, req_comp, sizeof(float), 0);
   if (!hdr_data)
      return stbi__errpf("outofmem", "Out of memory");

   // Load image data
   // image data is stored as some number of sca
   if ( width < 8 || width >= 32768) {
      // Read flat data
      for (j=0; j < height; ++j) {
         for (i=0; i < width; ++i) {
            stbi_uc rgbe[4];
           main_decode_loop:
            stbi__getn(s, rgbe, 4);
            stbi__hdr_convert(hdr_data + j * width * req_comp + i * req_comp, rgbe, req_comp);
         }
      }
   } else {
      // Read RLE-encoded data
      scanline = NULL;

      for (j = 0; j < height; ++j) {
         c1 = stbi__get8(s);
         c2 = stbi__get8(s);
         len = stbi__get8(s);
         if (c1 != 2 || c2 != 2 || (len & 0x80)) {
            // not run-length encoded, so we have to actually use THIS data as a decoded
            // pixel (note this can't be a valid pixel--one of RGB must be >= 128)
            stbi_uc rgbe[4];
            rgbe[0] = (stbi_uc) c1;
            rgbe[1] = (stbi_uc) c2;
            rgbe[2] = (stbi_uc) len;
            rgbe[3] = (stbi_uc) stbi__get8(s);
            stbi__hdr_convert(hdr_data, rgbe, req_comp);
            i = 1;
            j = 0;
            STBI_FREE(scanline);
            goto main_decode_loop; // yes, this makes no sense
         }
         len <<= 8;
         len |= stbi__get8(s);
         if (len != width) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("invalid decoded scanline length", "corrupt HDR"); }
         if (scanline == NULL) {
            scanline = (stbi_uc *) stbi__malloc_mad2(width, 4, 0);
            if (!scanline) {
               STBI_FREE(hdr_data);
               return stbi__errpf("outofmem", "Out of memory");
            }
         }

         for (k = 0; k < 4; ++k) {
            int nleft;
            i = 0;
            while ((nleft = width - i) > 0) {
               count = stbi__get8(s);
               if (count > 128) {
                  // Run
                  value = stbi__get8(s);
                  count -= 128;
                  if ((count == 0) || (count > nleft)) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("corrupt", "bad RLE data in HDR"); }
                  for (z = 0; z < count; ++z)
                     scanline[i++ * 4 + k] = value;
               } else {
                  // Dump
                  if ((count == 0) || (count > nleft)) { STBI_FREE(hdr_data); STBI_FREE(scanline); return stbi__errpf("corrupt", "bad RLE data in HDR"); }
                  for (z = 0; z < count; ++z)
                     scanline[i++ * 4 + k] = stbi__get8(s);
               }
            }
         }
         for (i=0; i < width; ++i)
            stbi__hdr_convert(hdr_data+(j*width + i)*req_comp, scanline + i*4, req_comp);
      }
      if (scanline)
         STBI_FREE(scanline);
   }

   return hdr_data;
}

static int stbi__hdr_info(stbi__context *s, int *x, int *y, int *comp)
{
   char buffer[STBI__HDR_BUFLEN];
   char *token;
   int valid = 0;
   int dummy;

   if (!x) x = &dummy;
   if (!y) y = &dummy;
   if (!comp) comp = &dummy;

   if (stbi__hdr_test(s) == 0) {
       stbi__rewind( s );
       return 0;
   }

   for(;;) {
      token = stbi__hdr_gettoken(s,buffer);
      if (token[0] == 0) break;
      if (strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0) valid = 1;
   }

   if (!valid) {
       stbi__rewind( s );
       return 0;
   }
   token = stbi__hdr_gettoken(s,buffer);
   if (strncmp(token, "-Y ", 3)) {
       stbi__rewind( s );
       return 0;
   }
   token += 3;
   *y = (int) strtol(token, &token, 10);
   while (*token == ' ') ++token;
   if (strncmp(token, "+X ", 3)) {
       stbi__rewind( s );
       return 0;
   }
   token += 3;
   *x = (int) strtol(token, NULL, 10);
   *comp = 3;
   return 1;
}
#endif // STBI_NO_HDR

#ifndef STBI_NO_BMP
static int stbi__bmp_info(stbi__context *s, int *x, int *y, int *comp)
{
   void *p;
   stbi__bmp_data info;

   info.all_a = 255;
   p = stbi__bmp_parse_header(s, &info);
   if (p == NULL) {
      stbi__rewind( s );
      return 0;
   }
   if (x) *x = s->img_x;
   if (y) *y = s->img_y;
   if (comp) {
      if (info.bpp == 24 && info.ma == 0xff000000)
         *comp = 3;
      else
         *comp = info.ma ? 4 : 3;
   }
   return 1;
}
#endif

#ifndef STBI_NO_PSD
static int stbi__psd_info(stbi__context *s, int *x, int *y, int *comp)
{
   int channelCount, dummy, depth;
   if (!x) x = &dummy;
   if (!y) y = &dummy;
   if (!comp) comp = &dummy;
   if (stbi__get32be(s) != 0x38425053) {
       stbi__rewind( s );
       return 0;
   }
   if (stbi__get16be(s) != 1) {
       stbi__rewind( s );
       return 0;
   }
   stbi__skip(s, 6);
   channelCount = stbi__get16be(s);
   if (channelCount < 0 || channelCount > 16) {
       stbi__rewind( s );
       return 0;
   }
   *y = stbi__get32be(s);
   *x = stbi__get32be(s);
   depth = stbi__get16be(s);
   if (depth != 8 && depth != 16) {
       stbi__rewind( s );
       return 0;
   }
   if (stbi__get16be(s) != 3) {
       stbi__rewind( s );
       return 0;
   }
   *comp = 4;
   return 1;
}

static int stbi__psd_is16(stbi__context *s)
{
   int channelCount, depth;
   if (stbi__get32be(s) != 0x38425053) {
       stbi__rewind( s );
       return 0;
   }
   if (stbi__get16be(s) != 1) {
       stbi__rewind( s );
       return 0;
   }
   stbi__skip(s, 6);
   channelCount = stbi__get16be(s);
   if (channelCount < 0 || channelCount > 16) {
       stbi__rewind( s );
       return 0;
   }
   STBI_NOTUSED(stbi__get32be(s));
   STBI_NOTUSED(stbi__get32be(s));
   depth = stbi__get16be(s);
   if (depth != 16) {
       stbi__rewind( s );
       return 0;
   }
   return 1;
}
#endif

#ifndef STBI_NO_PIC
static int stbi__pic_info(stbi__context *s, int *x, int *y, int *comp)
{
   int act_comp=0,num_packets=0,chained,dummy;
   stbi__pic_packet packets[10];

   if (!x) x = &dummy;
   if (!y) y = &dummy;
   if (!comp) comp = &dummy;

   if (!stbi__pic_is4(s,"\x53\x80\xF6\x34")) {
      stbi__rewind(s);
      return 0;
   }

   stbi__skip(s, 88);

   *x = stbi__get16be(s);
   *y = stbi__get16be(s);
   if (stbi__at_eof(s)) {
      stbi__rewind( s);
      return 0;
   }
   if ( (*x) != 0 && (1 << 28) / (*x) < (*y)) {
      stbi__rewind( s );
      return 0;
   }

   stbi__skip(s, 8);

   do {
      stbi__pic_packet *packet;

      if (num_packets==sizeof(packets)/sizeof(packets[0]))
         return 0;

      packet = &packets[num_packets++];
      chained = stbi__get8(s);
      packet->size    = stbi__get8(s);
      packet->type    = stbi__get8(s);
      packet->channel = stbi__get8(s);
      act_comp |= packet->channel;

      if (stbi__at_eof(s)) {
          stbi__rewind( s );
          return 0;
      }
      if (packet->size != 8) {
          stbi__rewind( s );
          return 0;
      }
   } while (chained);

   *comp = (act_comp & 0x10 ? 4 : 3);

   return 1;
}
#endif

// *************************************************************************************************
// Portable Gray Map and Portable Pixel Map loader
// by Ken Miller
//
// PGM: http://netpbm.sourceforge.net/doc/pgm.html
// PPM: http://netpbm.sourceforge.net/doc/ppm.html
//
// Known limitations:
//    Does not support comments in the header section
//    Does not support ASCII image data (formats P2 and P3)

#ifndef STBI_NO_PNM

static int      stbi__pnm_test(stbi__context *s)
{
   char p, t;
   p = (char) stbi__get8(s);
   t = (char) stbi__get8(s);
   if (p != 'P' || (t != '5' && t != '6')) {
       stbi__rewind( s );
       return 0;
   }
   return 1;
}

static void *stbi__pnm_load(stbi__context *s, int *x, int *y, int *comp, int req_comp, stbi__result_info *ri)
{
   stbi_uc *out;
   STBI_NOTUSED(ri);

   ri->bits_per_channel = stbi__pnm_info(s, (int *)&s->img_x, (int *)&s->img_y, (int *)&s->img_n);
   if (ri->bits_per_channel == 0)
      return 0;

   if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");
   if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__errpuc("too large","Very large image (corrupt?)");

   *x = s->img_x;
   *y = s->img_y;
   if (comp) *comp = s->img_n;

   if (!stbi__mad4sizes_valid(s->img_n, s->img_x, s->img_y, ri->bits_per_channel / 8, 0))
      return stbi__errpuc("too large", "PNM too large");

   out = (stbi_uc *) stbi__malloc_mad4(s->img_n, s->img_x, s->img_y, ri->bits_per_channel / 8, 0);
   if (!out) return stbi__errpuc("outofmem", "Out of memory");
   if (!stbi__getn(s, out, s->img_n * s->img_x * s->img_y * (ri->bits_per_channel / 8))) {
      STBI_FREE(out);
      return stbi__errpuc("bad PNM", "PNM file truncated");
   }

   if (req_comp && req_comp != s->img_n) {
      if (ri->bits_per_channel == 16) {
         out = (stbi_uc *) stbi__convert_format16((stbi__uint16 *) out, s->img_n, req_comp, s->img_x, s->img_y);
      } else {
         out = stbi__convert_format(out, s->img_n, req_comp, s->img_x, s->img_y);
      }
      if (out == NULL) return out; // stbi__convert_format frees input on failure
   }
   return out;
}

static int      stbi__pnm_isspace(char c)
{
   return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

static void     stbi__pnm_skip_whitespace(stbi__context *s, char *c)
{
   for (;;) {
      while (!stbi__at_eof(s) && stbi__pnm_isspace(*c))
         *c = (char) stbi__get8(s);

      if (stbi__at_eof(s) || *c != '#')
         break;

      while (!stbi__at_eof(s) && *c != '\n' && *c != '\r' )
         *c = (char) stbi__get8(s);
   }
}

static int      stbi__pnm_isdigit(char c)
{
   return c >= '0' && c <= '9';
}

static int      stbi__pnm_getinteger(stbi__context *s, char *c)
{
   int value = 0;

   while (!stbi__at_eof(s) && stbi__pnm_isdigit(*c)) {
      value = value*10 + (*c - '0');
      *c = (char) stbi__get8(s);
      if((value > 214748364) || (value == 214748364 && *c > '7'))
          return stbi__err("integer parse overflow", "Parsing an integer in the PPM header overflowed a 32-bit int");
   }

   return value;
}

static int      stbi__pnm_info(stbi__context *s, int *x, int *y, int *comp)
{
   int maxv, dummy;
   char c, p, t;

   if (!x) x = &dummy;
   if (!y) y = &dummy;
   if (!comp) comp = &dummy;

   stbi__rewind(s);

   // Get identifier
   p = (char) stbi__get8(s);
   t = (char) stbi__get8(s);
   if (p != 'P' || (t != '5' && t != '6')) {
       stbi__rewind(s);
       return 0;
   }

   *comp = (t == '6') ? 3 : 1;  // '5' is 1-component .pgm; '6' is 3-component .ppm

   c = (char) stbi__get8(s);
   stbi__pnm_skip_whitespace(s, &c);

   *x = stbi__pnm_getinteger(s, &c); // read width
   if(*x == 0)
       return stbi__err("invalid width", "PPM image header had zero or overflowing width");
   stbi__pnm_skip_whitespace(s, &c);

   *y = stbi__pnm_getinteger(s, &c); // read height
   if (*y == 0)
       return stbi__err("invalid width", "PPM image header had zero or overflowing width");
   stbi__pnm_skip_whitespace(s, &c);

   maxv = stbi__pnm_getinteger(s, &c);  // read max value
   if (maxv > 65535)
      return stbi__err("max value > 65535", "PPM image supports only 8-bit and 16-bit images");
   else if (maxv > 255)
      return 16;
   else
      return 8;
}

static int stbi__pnm_is16(stbi__context *s)
{
   if (stbi__pnm_info(s, NULL, NULL, NULL) == 16)
	   return 1;
   return 0;
}
#endif

static int stbi__info_main(stbi__context *s, int *x, int *y, int *comp)
{
   #ifndef STBI_NO_JPEG
   if (stbi__jpeg_info(s, x, y, comp)) return 1;
   #endif

   #ifndef STBI_NO_PNG
   if (stbi__png_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_GIF
   if (stbi__gif_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_BMP
   if (stbi__bmp_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_PSD
   if (stbi__psd_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_PIC
   if (stbi__pic_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_PNM
   if (stbi__pnm_info(s, x, y, comp))  return 1;
   #endif

   #ifndef STBI_NO_HDR
   if (stbi__hdr_info(s, x, y, comp))  return 1;
   #endif

   // test tga last because it's a crappy test!
   #ifndef STBI_NO_TGA
   if (stbi__tga_info(s, x, y, comp))
       return 1;
   #endif
   return stbi__err("unknown image type", "Image not of any known type, or corrupt");
}

static int stbi__is_16_main(stbi__context *s)
{
   #ifndef STBI_NO_PNG
   if (stbi__png_is16(s))  return 1;
   #endif

   #ifndef STBI_NO_PSD
   if (stbi__psd_is16(s))  return 1;
   #endif

   #ifndef STBI_NO_PNM
   if (stbi__pnm_is16(s))  return 1;
   #endif
   return 0;
}

#ifndef STBI_NO_STDIO
STBIDEF int stbi_info(char const *filename, int *x, int *y, int *comp)
{
    FILE *f = stbi__fopen(filename, "rb");
    int result;
    if (!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_info_from_file(f, x, y, comp);
    fclose(f);
    return result;
}

STBIDEF int stbi_info_from_file(FILE *f, int *x, int *y, int *comp)
{
   int r;
   stbi__context s;
   long pos = ftell(f);
   stbi__start_file(&s, f);
   r = stbi__info_main(&s,x,y,comp);
   fseek(f,pos,SEEK_SET);
   return r;
}

STBIDEF int stbi_is_16_bit(char const *filename)
{
    FILE *f = stbi__fopen(filename, "rb");
    int result;
    if (!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_is_16_bit_from_file(f);
    fclose(f);
    return result;
}

STBIDEF int stbi_is_16_bit_from_file(FILE *f)
{
   int r;
   stbi__context s;
   long pos = ftell(f);
   stbi__start_file(&s, f);
   r = stbi__is_16_main(&s);
   fseek(f,pos,SEEK_SET);
   return r;
}
#endif // !STBI_NO_STDIO

STBIDEF int stbi_info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__info_main(&s,x,y,comp);
}

STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const *c, void *user, int *x, int *y, int *comp)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) c, user);
   return stbi__info_main(&s,x,y,comp);
}

STBIDEF int stbi_is_16_bit_from_memory(stbi_uc const *buffer, int len)
{
   stbi__context s;
   stbi__start_mem(&s,buffer,len);
   return stbi__is_16_main(&s);
}

STBIDEF int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const *c, void *user)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) c, user);
   return stbi__is_16_main(&s);
}

#endif // STB_IMAGE_IMPLEMENTATION

/*
   revision history:
      2.20  (2019-02-07) support utf8 filenames in Windows; fix warnings and platform ifdefs
      2.19  (2018-02-11) fix warning
      2.18  (2018-01-30) fix warnings
      2.17  (2018-01-29) change sbti__shiftsigned to avoid clang -O2 bug
                         1-bit BMP
                         *_is_16_bit api
                         avoid warnings
      2.16  (2017-07-23) all functions have 16-bit variants;
                         STBI_NO_STDIO works again;
                         compilation fixes;
                         fix rounding in unpremultiply;
                         optimize vertical flip;
                         disable raw_len validation;
                         documentation fixes
      2.15  (2017-03-18) fix png-1,2,4 bug; now all Imagenet JPGs decode;
                         warning fixes; disable run-time SSE detection on gcc;
                         uniform handling of optional "return" values;
                         thread-safe initialization of zlib tables
      2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs
      2.13  (2016-11-29) add 16-bit API, only supported for PNG right now
      2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
      2.11  (2016-04-02) allocate large structures on the stack
                         remove white matting for transparent PSD
                         fix reported channel count for PNG & BMP
                         re-enable SSE2 in non-gcc 64-bit
                         support RGB-formatted JPEG
                         read 16-bit PNGs (only as 8-bit)
      2.10  (2016-01-22) avoid warning introduced in 2.09 by STBI_REALLOC_SIZED
      2.09  (2016-01-16) allow comments in PNM files
                         16-bit-per-pixel TGA (not bit-per-component)
                         info() for TGA could break due to .hdr handling
                         info() for BMP to shares code instead of sloppy parse
                         can use STBI_REALLOC_SIZED if allocator doesn't support realloc
                         code cleanup
      2.08  (2015-09-13) fix to 2.07 cleanup, reading RGB PSD as RGBA
      2.07  (2015-09-13) fix compiler warnings
                         partial animated GIF support
                         limited 16-bpc PSD support
                         #ifdef unused functions
                         bug with < 92 byte PIC,PNM,HDR,TGA
      2.06  (2015-04-19) fix bug where PSD returns wrong '*comp' value
      2.05  (2015-04-19) fix bug in progressive JPEG handling, fix warning
      2.04  (2015-04-15) try to re-enable SIMD on MinGW 64-bit
      2.03  (2015-04-12) extra corruption checking (mmozeiko)
                         stbi_set_flip_vertically_on_load (nguillemot)
                         fix NEON support; fix mingw support
      2.02  (2015-01-19) fix incorrect assert, fix warning
      2.01  (2015-01-17) fix various warnings; suppress SIMD on gcc 32-bit without -msse2
      2.00b (2014-12-25) fix STBI_MALLOC in progressive JPEG
      2.00  (2014-12-25) optimize JPG, including x86 SSE2 & NEON SIMD (ryg)
                         progressive JPEG (stb)
                         PGM/PPM support (Ken Miller)
                         STBI_MALLOC,STBI_REALLOC,STBI_FREE
                         GIF bugfix -- seemingly never worked
                         STBI_NO_*, STBI_ONLY_*
      1.48  (2014-12-14) fix incorrectly-named assert()
      1.47  (2014-12-14) 1/2/4-bit PNG support, both direct and paletted (Omar Cornut & stb)
                         optimize PNG (ryg)
                         fix bug in interlaced PNG with user-specified channel count (stb)
      1.46  (2014-08-26)
              fix broken tRNS chunk (colorkey-style transparency) in non-paletted PNG
      1.45  (2014-08-16)
              fix MSVC-ARM internal compiler error by wrapping malloc
      1.44  (2014-08-07)
              various warning fixes from Ronny Chevalier
      1.43  (2014-07-15)
              fix MSVC-only compiler problem in code changed in 1.42
      1.42  (2014-07-09)
              don't define _CRT_SECURE_NO_WARNINGS (affects user code)
              fixes to stbi__cleanup_jpeg path
              added STBI_ASSERT to avoid requiring assert.h
      1.41  (2014-06-25)
              fix search&replace from 1.36 that messed up comments/error messages
      1.40  (2014-06-22)
              fix gcc struct-initialization warning
      1.39  (2014-06-15)
              fix to TGA optimization when req_comp != number of components in TGA;
              fix to GIF loading because BMP wasn't rewinding (whoops, no GIFs in my test suite)
              add support for BMP version 5 (more ignored fields)
      1.38  (2014-06-06)
              suppress MSVC warnings on integer casts truncating values
              fix accidental rename of 'skip' field of I/O
      1.37  (2014-06-04)
              remove duplicate typedef
      1.36  (2014-06-03)
              convert to header file single-file library
              if de-iphone isn't set, load iphone images color-swapped instead of returning NULL
      1.35  (2014-05-27)
              various warnings
              fix broken STBI_SIMD path
              fix bug where stbi_load_from_file no longer left file pointer in correct place
              fix broken non-easy path for 32-bit BMP (possibly never used)
              TGA optimization by Arseny Kapoulkine
      1.34  (unknown)
              use STBI_NOTUSED in stbi__resample_row_generic(), fix one more leak in tga failure case
      1.33  (2011-07-14)
              make stbi_is_hdr work in STBI_NO_HDR (as specified), minor compiler-friendly improvements
      1.32  (2011-07-13)
              support for "info" function for all supported filetypes (SpartanJ)
      1.31  (2011-06-20)
              a few more leak fixes, bug in PNG handling (SpartanJ)
      1.30  (2011-06-11)
              added ability to load files via callbacks to accomidate custom input streams (Ben Wenger)
              removed deprecated format-specific test/load functions
              removed support for installable file formats (stbi_loader) -- would have been broken for IO callbacks anyway
              error cases in bmp and tga give messages and don't leak (Raymond Barbiero, grisha)
              fix inefficiency in decoding 32-bit BMP (David Woo)
      1.29  (2010-08-16)
              various warning fixes from Aurelien Pocheville
      1.28  (2010-08-01)
              fix bug in GIF palette transparency (SpartanJ)
      1.27  (2010-08-01)
              cast-to-stbi_uc to fix warnings
      1.26  (2010-07-24)
              fix bug in file buffering for PNG reported by SpartanJ
      1.25  (2010-07-17)
              refix trans_data warning (Won Chun)
      1.24  (2010-07-12)
              perf improvements reading from files on platforms with lock-heavy fgetc()
              minor perf improvements for jpeg
              deprecated type-specific functions so we'll get feedback if they're needed
              attempt to fix trans_data warning (Won Chun)
      1.23    fixed bug in iPhone support
      1.22  (2010-07-10)
              removed image *writing* support
              stbi_info support from Jetro Lauha
              GIF support from Jean-Marc Lienher
              iPhone PNG-extensions from James Brown
              warning-fixes from Nicolas Schulz and Janez Zemva (i.stbi__err. Janez (U+017D)emva)
      1.21    fix use of 'stbi_uc' in header (reported by jon blow)
      1.20    added support for Softimage PIC, by Tom Seddon
      1.19    bug in interlaced PNG corruption check (found by ryg)
      1.18  (2008-08-02)
              fix a threading bug (local mutable static)
      1.17    support interlaced PNG
      1.16    major bugfix - stbi__convert_format converted one too many pixels
      1.15    initialize some fields for thread safety
      1.14    fix threadsafe conversion bug
              header-file-only version (#define STBI_HEADER_FILE_ONLY before including)
      1.13    threadsafe
      1.12    const qualifiers in the API
      1.11    Support installable IDCT, colorspace conversion routines
      1.10    Fixes for 64-bit (don't use "unsigned long")
              optimized upsampling by Fabian "ryg" Giesen
      1.09    Fix format-conversion for PSD code (bad global variables!)
      1.08    Thatcher Ulrich's PSD code integrated by Nicolas Schulz
      1.07    attempt to fix C++ warning/errors again
      1.06    attempt to fix C++ warning/errors again
      1.05    fix TGA loading to return correct *comp and use good luminance calc
      1.04    default float alpha is 1, not 255; use 'void *' for stbi_image_free
      1.03    bugfixes to STBI_NO_STDIO, STBI_NO_HDR
      1.02    support for (subset of) HDR files, float interface for preferred access to them
      1.01    fix bug: possible bug in handling right-side up bmps... not sure
              fix bug: the stbi__bmp_load() and stbi__tga_load() functions didn't work at all
      1.00    interface to zlib that skips zlib header
      0.99    correct handling of alpha in palette
      0.98    TGA loader by lonesock; dynamically add loaders (untested)
      0.97    jpeg errors on too large a file; also catch another malloc failure
      0.96    fix detection of invalid v value - particleman@mollyrocket forum
      0.95    during header scan, seek to markers in case of padding
      0.94    STBI_NO_STDIO to disable stdio usage; rename all #defines the same
      0.93    handle jpegtran output; verbose errors
      0.92    read 4,8,16,24,32-bit BMP files of several formats
      0.91    output 24-bit Windows 3.0 BMP files
      0.90    fix a few more warnings; bump version number to approach 1.0
      0.61    bugfixes due to Marc LeBlanc, Christopher Lloyd
      0.60    fix compiling as c++
      0.59    fix warnings: merge Dave Moore's -Wall fixes
      0.58    fix bug: zlib uncompressed mode len/nlen was wrong endian
      0.57    fix bug: jpg last huffman symbol before marker was >9 bits but less than 16 available
      0.56    fix bug: zlib uncompressed mode len vs. nlen
      0.55    fix bug: restart_interval not initialized to 0
      0.54    allow NULL for 'int *comp'
      0.53    fix bug in png 3->4; speedup png decoding
      0.52    png handles req_comp=3,4 directly; minor cleanup; jpeg comments
      0.51    obey req_comp requests, 1-component jpegs return as 1-component,
              on 'test' only check type, not whether we support this variant
      0.50  (2006-11-19)
              first released version
*/


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/


// stb_truetype.h - v1.26 - public domain
// authored from 2009-2021 by Sean Barrett / RAD Game Tools
//
// =======================================================================
//
//    NO SECURITY GUARANTEE -- DO NOT USE THIS ON UNTRUSTED FONT FILES
//
// This library does no range checking of the offsets found in the file,
// meaning an attacker can use it to read arbitrary memory.
//
// =======================================================================
//
//   This library processes TrueType files:
//        parse files
//        extract glyph metrics
//        extract glyph shapes
//        render glyphs to one-channel bitmaps with antialiasing (box filter)
//        render glyphs to one-channel SDF bitmaps (signed-distance field/function)
//
//   Todo:
//        non-MS cmaps
//        crashproof on bad data
//        hinting? (no longer patented)
//        cleartype-style AA?
//        optimize: use simple memory allocator for intermediates
//        optimize: build edge-list directly from curves
//        optimize: rasterize directly from curves?
//
// ADDITIONAL CONTRIBUTORS
//
//   Mikko Mononen: compound shape support, more cmap formats
//   Tor Andersson: kerning, subpixel rendering
//   Dougall Johnson: OpenType / Type 2 font handling
//   Daniel Ribeiro Maciel: basic GPOS-based kerning
//
//   Misc other:
//       Ryan Gordon
//       Simon Glass
//       github:IntellectualKitty
//       Imanol Celaya
//       Daniel Ribeiro Maciel
//
//   Bug/warning reports/fixes:
//       "Zer" on mollyrocket       Fabian "ryg" Giesen   github:NiLuJe
//       Cass Everitt               Martins Mozeiko       github:aloucks
//       stoiko (Haemimont Games)   Cap Petschulat        github:oyvindjam
//       Brian Hook                 Omar Cornut           github:vassvik
//       Walter van Niftrik         Ryan Griege
//       David Gow                  Peter LaValle
//       David Given                Sergey Popov
//       Ivan-Assen Ivanov          Giumo X. Clanjor
//       Anthony Pesch              Higor Euripedes
//       Johan Duparc               Thomas Fields
//       Hou Qiming                 Derek Vinyard
//       Rob Loach                  Cort Stratton
//       Kenney Phillis Jr.         Brian Costabile
//       Ken Voskuil (kaesve)
//
// VERSION HISTORY
//
//   1.26 (2021-08-28) fix broken rasterizer
//   1.25 (2021-07-11) many fixes
//   1.24 (2020-02-05) fix warning
//   1.23 (2020-02-02) query SVG data for glyphs; query whole kerning table (but only kern not GPOS)
//   1.22 (2019-08-11) minimize missing-glyph duplication; fix kerning if both 'GPOS' and 'kern' are defined
//   1.21 (2019-02-25) fix warning
//   1.20 (2019-02-07) PackFontRange skips missing codepoints; GetScaleFontVMetrics()
//   1.19 (2018-02-11) GPOS kerning, STBTT_fmod
//   1.18 (2018-01-29) add missing function
//   1.17 (2017-07-23) make more arguments const; doc fix
//   1.16 (2017-07-12) SDF support
//   1.15 (2017-03-03) make more arguments const
//   1.14 (2017-01-16) num-fonts-in-TTC function
//   1.13 (2017-01-02) support OpenType fonts, certain Apple fonts
//   1.12 (2016-10-25) suppress warnings about casting away const with -Wcast-qual
//   1.11 (2016-04-02) fix unused-variable warning
//   1.10 (2016-04-02) user-defined fabs(); rare memory leak; remove duplicate typedef
//   1.09 (2016-01-16) warning fix; avoid crash on outofmem; use allocation userdata properly
//   1.08 (2015-09-13) document stbtt_Rasterize(); fixes for vertical & horizontal edges
//   1.07 (2015-08-01) allow PackFontRanges to accept arrays of sparse codepoints;
//                     variant PackFontRanges to pack and render in separate phases;
//                     fix stbtt_GetFontOFfsetForIndex (never worked for non-0 input?);
//                     fixed an assert() bug in the new rasterizer
//                     replace assert() with STBTT_assert() in new rasterizer
//
//   Full history can be found at the end of this file.
//
// LICENSE
//
//   See end of file for license information.
//
// USAGE
//
//   Include this file in whatever places need to refer to it. In ONE C/C++
//   file, write:
//      #define STB_TRUETYPE_IMPLEMENTATION
//   before the #include of this file. This expands out the actual
//   implementation into that C/C++ file.
//
//   To make the implementation private to the file that generates the implementation,
//      #define STBTT_STATIC
//
//   Simple 3D API (don't ship this, but it's fine for tools and quick start)
//           stbtt_BakeFontBitmap()               -- bake a font to a bitmap for use as texture
//           stbtt_GetBakedQuad()                 -- compute quad to draw for a given char
//
//   Improved 3D API (more shippable):
//           #include "stb_rect_pack.h"           -- optional, but you really want it
//           stbtt_PackBegin()
//           stbtt_PackSetOversampling()          -- for improved quality on small fonts
//           stbtt_PackFontRanges()               -- pack and renders
//           stbtt_PackEnd()
//           stbtt_GetPackedQuad()
//
//   "Load" a font file from a memory buffer (you have to keep the buffer loaded)
//           stbtt_InitFont()
//           stbtt_GetFontOffsetForIndex()        -- indexing for TTC font collections
//           stbtt_GetNumberOfFonts()             -- number of fonts for TTC font collections
//
//   Render a unicode codepoint to a bitmap
//           stbtt_GetCodepointBitmap()           -- allocates and returns a bitmap
//           stbtt_MakeCodepointBitmap()          -- renders into bitmap you provide
//           stbtt_GetCodepointBitmapBox()        -- how big the bitmap must be
//
//   Character advance/positioning
//           stbtt_GetCodepointHMetrics()
//           stbtt_GetFontVMetrics()
//           stbtt_GetFontVMetricsOS2()
//           stbtt_GetCodepointKernAdvance()
//
//   Starting with version 1.06, the rasterizer was replaced with a new,
//   faster and generally-more-precise rasterizer. The new rasterizer more
//   accurately measures pixel coverage for anti-aliasing, except in the case
//   where multiple shapes overlap, in which case it overestimates the AA pixel
//   coverage. Thus, anti-aliasing of intersecting shapes may look wrong. If
//   this turns out to be a problem, you can re-enable the old rasterizer with
//        #define STBTT_RASTERIZER_VERSION 1
//   which will incur about a 15% speed hit.
//
// ADDITIONAL DOCUMENTATION
//
//   Immediately after this block comment are a series of sample programs.
//
//   After the sample programs is the "header file" section. This section
//   includes documentation for each API function.
//
//   Some important concepts to understand to use this library:
//
//      Codepoint
//         Characters are defined by unicode codepoints, e.g. 65 is
//         uppercase A, 231 is lowercase c with a cedilla, 0x7e30 is
//         the hiragana for "ma".
//
//      Glyph
//         A visual character shape (every codepoint is rendered as
//         some glyph)
//
//      Glyph index
//         A font-specific integer ID representing a glyph
//
//      Baseline
//         Glyph shapes are defined relative to a baseline, which is the
//         bottom of uppercase characters. Characters extend both above
//         and below the baseline.
//
//      Current Point
//         As you draw text to the screen, you keep track of a "current point"
//         which is the origin of each character. The current point's vertical
//         position is the baseline. Even "baked fonts" use this model.
//
//      Vertical Font Metrics
//         The vertical qualities of the font, used to vertically position
//         and space the characters. See docs for stbtt_GetFontVMetrics.
//
//      Font Size in Pixels or Points
//         The preferred interface for specifying font sizes in stb_truetype
//         is to specify how tall the font's vertical extent should be in pixels.
//         If that sounds good enough, skip the next paragraph.
//
//         Most font APIs instead use "points", which are a common typographic
//         measurement for describing font size, defined as 72 points per inch.
//         stb_truetype provides a point API for compatibility. However, true
//         "per inch" conventions don't make much sense on computer displays
//         since different monitors have different number of pixels per
//         inch. For example, Windows traditionally uses a convention that
//         there are 96 pixels per inch, thus making 'inch' measurements have
//         nothing to do with inches, and thus effectively defining a point to
//         be 1.333 pixels. Additionally, the TrueType font data provides
//         an explicit scale factor to scale a given font's glyphs to points,
//         but the author has observed that this scale factor is often wrong
//         for non-commercial fonts, thus making fonts scaled in points
//         according to the TrueType spec incoherently sized in practice.
//
// DETAILED USAGE:
//
//  Scale:
//    Select how high you want the font to be, in points or pixels.
//    Call ScaleForPixelHeight or ScaleForMappingEmToPixels to compute
//    a scale factor SF that will be used by all other functions.
//
//  Baseline:
//    You need to select a y-coordinate that is the baseline of where
//    your text will appear. Call GetFontBoundingBox to get the baseline-relative
//    bounding box for all characters. SF*-y0 will be the distance in pixels
//    that the worst-case character could extend above the baseline, so if
//    you want the top edge of characters to appear at the top of the
//    screen where y=0, then you would set the baseline to SF*-y0.
//
//  Current point:
//    Set the current point where the first character will appear. The
//    first character could extend left of the current point; this is font
//    dependent. You can either choose a current point that is the leftmost
//    point and hope, or add some padding, or check the bounding box or
//    left-side-bearing of the first character to be displayed and set
//    the current point based on that.
//
//  Displaying a character:
//    Compute the bounding box of the character. It will contain signed values
//    relative to <current_point, baseline>. I.e. if it returns x0,y0,x1,y1,
//    then the character should be displayed in the rectangle from
//    <current_point+SF*x0, baseline+SF*y0> to <current_point+SF*x1,baseline+SF*y1).
//
//  Advancing for the next character:
//    Call GlyphHMetrics, and compute 'current_point += SF * advance'.
//
//
// ADVANCED USAGE
//
//   Quality:
//
//    - Use the functions with Subpixel at the end to allow your characters
//      to have subpixel positioning. Since the font is anti-aliased, not
//      hinted, this is very import for quality. (This is not possible with
//      baked fonts.)
//
//    - Kerning is now supported, and if you're supporting subpixel rendering
//      then kerning is worth using to give your text a polished look.
//
//   Performance:
//
//    - Convert Unicode codepoints to glyph indexes and operate on the glyphs;
//      if you don't do this, stb_truetype is forced to do the conversion on
//      every call.
//
//    - There are a lot of memory allocations. We should modify it to take
//      a temp buffer and allocate from the temp buffer (without freeing),
//      should help performance a lot.
//
// NOTES
//
//   The system uses the raw data found in the .ttf file without changing it
//   and without building auxiliary data structures. This is a bit inefficient
//   on little-endian systems (the data is big-endian), but assuming you're
//   caching the bitmaps or glyph shapes this shouldn't be a big deal.
//
//   It appears to be very hard to programmatically determine what font a
//   given file is in a general way. I provide an API for this, but I don't
//   recommend it.
//
//
// PERFORMANCE MEASUREMENTS FOR 1.06:
//
//                      32-bit     64-bit
//   Previous release:  8.83 s     7.68 s
//   Pool allocations:  7.72 s     6.34 s
//   Inline sort     :  6.54 s     5.65 s
//   New rasterizer  :  5.63 s     5.00 s

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////
////  SAMPLE PROGRAMS
////
//
//  Incomplete text-in-3d-api example, which draws quads properly aligned to be lossless.
//  See "tests/truetype_demo_win32.c" for a complete version.
#if 0
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

unsigned char ttf_buffer[1<<20];
unsigned char temp_bitmap[512*512];

stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
GLuint ftex;

void my_stbtt_initfont(void)
{
   fread(ttf_buffer, 1, 1<<20, fopen("c:/windows/fonts/times.ttf", "rb"));
   stbtt_BakeFontBitmap(ttf_buffer,0, 32.0, temp_bitmap,512,512, 32,96, cdata); // no guarantee this fits!
   // can free ttf_buffer at this point
   glGenTextures(1, &ftex);
   glBindTexture(GL_TEXTURE_2D, ftex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
   // can free temp_bitmap at this point
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void my_stbtt_print(float x, float y, char *text)
{
   // assume orthographic projection with units = screen pixels, origin at top left
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, ftex);
   glBegin(GL_QUADS);
   while (*text) {
      if (*text >= 32 && *text < 128) {
         stbtt_aligned_quad q;
         stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
         glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
         glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
         glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
         glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
      }
      ++text;
   }
   glEnd();
}
#endif
//
//
//////////////////////////////////////////////////////////////////////////////
//
// Complete program (this compiles): get a single bitmap, print as ASCII art
//
#if 0
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

char ttf_buffer[1<<25];

int main(int argc, char **argv)
{
   stbtt_fontinfo font;
   unsigned char *bitmap;
   int w,h,i,j,c = (argc > 1 ? atoi(argv[1]) : 'a'), s = (argc > 2 ? atoi(argv[2]) : 20);

   fread(ttf_buffer, 1, 1<<25, fopen(argc > 3 ? argv[3] : "c:/windows/fonts/arialbd.ttf", "rb"));

   stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
   bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, s), c, &w, &h, 0,0);

   for (j=0; j < h; ++j) {
      for (i=0; i < w; ++i)
         putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
      putchar('\n');
   }
   return 0;
}
#endif
//
// Output:
//
//     .ii.
//    @@@@@@.
//   V@Mio@@o
//   :i.  V@V
//     :oM@@M
//   :@@@MM@M
//   @@o  o@M
//  :@@.  M@M
//   @@@o@@@@
//   :M@@V:@@.
//
//////////////////////////////////////////////////////////////////////////////
//
// Complete program: print "Hello World!" banner, with bugs
//
#if 0
char buffer[24<<20];
unsigned char screen[20][79];

int main(int arg, char **argv)
{
   stbtt_fontinfo font;
   int i,j,ascent,baseline,ch=0;
   float scale, xpos=2; // leave a little padding in case the character extends left
   char *text = "Heljo World!"; // intentionally misspelled to show 'lj' brokenness

   fread(buffer, 1, 1000000, fopen("c:/windows/fonts/arialbd.ttf", "rb"));
   stbtt_InitFont(&font, buffer, 0);

   scale = stbtt_ScaleForPixelHeight(&font, 15);
   stbtt_GetFontVMetrics(&font, &ascent,0,0);
   baseline = (int) (ascent*scale);

   while (text[ch]) {
      int advance,lsb,x0,y0,x1,y1;
      float x_shift = xpos - (float) floor(xpos);
      stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);
      stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale,scale,x_shift,0, &x0,&y0,&x1,&y1);
      stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline + y0][(int) xpos + x0], x1-x0,y1-y0, 79, scale,scale,x_shift,0, text[ch]);
      // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
      // because this API is really for baking character bitmaps into textures. if you want to render
      // a sequence of characters, you really need to render each bitmap to a temp buffer, then
      // "alpha blend" that into the working buffer
      xpos += (advance * scale);
      if (text[ch+1])
         xpos += scale*stbtt_GetCodepointKernAdvance(&font, text[ch],text[ch+1]);
      ++ch;
   }

   for (j=0; j < 20; ++j) {
      for (i=0; i < 78; ++i)
         putchar(" .:ioVM@"[screen[j][i]>>5]);
      putchar('\n');
   }

   return 0;
}
#endif


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////
////   INTEGRATION WITH YOUR CODEBASE
////
////   The following sections allow you to supply alternate definitions
////   of C library functions used by stb_truetype, e.g. if you don't
////   link with the C runtime library.

#ifdef STB_TRUETYPE_IMPLEMENTATION
   // #define your own (u)stbtt_int8/16/32 before including to override this
   #ifndef stbtt_uint8
   typedef unsigned char   stbtt_uint8;
   typedef signed   char   stbtt_int8;
   typedef unsigned short  stbtt_uint16;
   typedef signed   short  stbtt_int16;
   typedef unsigned int    stbtt_uint32;
   typedef signed   int    stbtt_int32;
   #endif

   typedef char stbtt__check_size32[sizeof(stbtt_int32)==4 ? 1 : -1];
   typedef char stbtt__check_size16[sizeof(stbtt_int16)==2 ? 1 : -1];

   // e.g. #define your own STBTT_ifloor/STBTT_iceil() to avoid math.h
   #ifndef STBTT_ifloor
   #include <math.h>
   #define STBTT_ifloor(x)   ((int) floor(x))
   #define STBTT_iceil(x)    ((int) ceil(x))
   #endif

   #ifndef STBTT_sqrt
   #include <math.h>
   #define STBTT_sqrt(x)      sqrt(x)
   #define STBTT_pow(x,y)     pow(x,y)
   #endif

   #ifndef STBTT_fmod
   #include <math.h>
   #define STBTT_fmod(x,y)    fmod(x,y)
   #endif

   #ifndef STBTT_cos
   #include <math.h>
   #define STBTT_cos(x)       cos(x)
   #define STBTT_acos(x)      acos(x)
   #endif

   #ifndef STBTT_fabs
   #include <math.h>
   #define STBTT_fabs(x)      fabs(x)
   #endif

   // #define your own functions "STBTT_malloc" / "STBTT_free" to avoid malloc.h
   #ifndef STBTT_malloc
   #include <stdlib.h>
   #define STBTT_malloc(x,u)  ((void)(u),malloc(x))
   #define STBTT_free(x,u)    ((void)(u),free(x))
   #endif

   #ifndef STBTT_assert
   #include <assert.h>
   #define STBTT_assert(x)    assert(x)
   #endif

   #ifndef STBTT_strlen
   #include <string.h>
   #define STBTT_strlen(x)    strlen(x)
   #endif

   #ifndef STBTT_memcpy
   #include <string.h>
   #define STBTT_memcpy       memcpy
   #define STBTT_memset       memset
   #endif
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////   INTERFACE
////
////

#ifndef __STB_INCLUDE_STB_TRUETYPE_H__
#define __STB_INCLUDE_STB_TRUETYPE_H__

#ifdef STBTT_STATIC
#define STBTT_DEF static
#else
#define STBTT_DEF extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

// private structure
typedef struct
{
   unsigned char *data;
   int cursor;
   int size;
} stbtt__buf;

//////////////////////////////////////////////////////////////////////////////
//
// TEXTURE BAKING API
//
// If you use this API, you only have to call two functions ever.
//

typedef struct
{
   unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
   float xoff,yoff,xadvance;
} stbtt_bakedchar;

STBTT_DEF int stbtt_BakeFontBitmap(const unsigned char *data, int offset,  // font location (use offset=0 for plain .ttf)
                                float pixel_height,                     // height of font in pixels
                                unsigned char *pixels, int pw, int ph,  // bitmap to be filled in
                                int first_char, int num_chars,          // characters to bake
                                stbtt_bakedchar *chardata);             // you allocate this, it's num_chars long
// if return is positive, the first unused row of the bitmap
// if return is negative, returns the negative of the number of characters that fit
// if return is 0, no characters fit and no rows were used
// This uses a very poor packing.

typedef struct
{
   float x0,y0,s0,t0; // top-left
   float x1,y1,s1,t1; // bottom-right
} stbtt_aligned_quad;

STBTT_DEF void stbtt_GetBakedQuad(const stbtt_bakedchar *chardata, int pw, int ph,  // same data as above
                               int char_index,             // character to display
                               float *xpos, float *ypos,   // pointers to current position in screen pixel space
                               stbtt_aligned_quad *q,      // output: quad to draw
                               int opengl_fillrule);       // true if opengl fill rule; false if DX9 or earlier
// Call GetBakedQuad with char_index = 'character - first_char', and it
// creates the quad you need to draw and advances the current position.
//
// The coordinate system used assumes y increases downwards.
//
// Characters will extend both above and below the current position;
// see discussion of "BASELINE" above.
//
// It's inefficient; you might want to c&p it and optimize it.

STBTT_DEF void stbtt_GetScaledFontVMetrics(const unsigned char *fontdata, int index, float size, float *ascent, float *descent, float *lineGap);
// Query the font vertical metrics without having to create a font first.


//////////////////////////////////////////////////////////////////////////////
//
// NEW TEXTURE BAKING API
//
// This provides options for packing multiple fonts into one atlas, not
// perfectly but better than nothing.

typedef struct
{
   unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
   float xoff,yoff,xadvance;
   float xoff2,yoff2;
} stbtt_packedchar;

typedef struct stbtt_pack_context stbtt_pack_context;
typedef struct stbtt_fontinfo stbtt_fontinfo;
#ifndef STB_RECT_PACK_VERSION
typedef struct stbrp_rect stbrp_rect;
#endif

STBTT_DEF int  stbtt_PackBegin(stbtt_pack_context *spc, unsigned char *pixels, int width, int height, int stride_in_bytes, int padding, void *alloc_context);
// Initializes a packing context stored in the passed-in stbtt_pack_context.
// Future calls using this context will pack characters into the bitmap passed
// in here: a 1-channel bitmap that is width * height. stride_in_bytes is
// the distance from one row to the next (or 0 to mean they are packed tightly
// together). "padding" is the amount of padding to leave between each
// character (normally you want '1' for bitmaps you'll use as textures with
// bilinear filtering).
//
// Returns 0 on failure, 1 on success.

STBTT_DEF void stbtt_PackEnd  (stbtt_pack_context *spc);
// Cleans up the packing context and frees all memory.

#define STBTT_POINT_SIZE(x)   (-(x))

STBTT_DEF int  stbtt_PackFontRange(stbtt_pack_context *spc, const unsigned char *fontdata, int font_index, float font_size,
                                int first_unicode_char_in_range, int num_chars_in_range, stbtt_packedchar *chardata_for_range);
// Creates character bitmaps from the font_index'th font found in fontdata (use
// font_index=0 if you don't know what that is). It creates num_chars_in_range
// bitmaps for characters with unicode values starting at first_unicode_char_in_range
// and increasing. Data for how to render them is stored in chardata_for_range;
// pass these to stbtt_GetPackedQuad to get back renderable quads.
//
// font_size is the full height of the character from ascender to descender,
// as computed by stbtt_ScaleForPixelHeight. To use a point size as computed
// by stbtt_ScaleForMappingEmToPixels, wrap the point size in STBTT_POINT_SIZE()
// and pass that result as 'font_size':
//       ...,                  20 , ... // font max minus min y is 20 pixels tall
//       ..., STBTT_POINT_SIZE(20), ... // 'M' is 20 pixels tall

typedef struct
{
   float font_size;
   int first_unicode_codepoint_in_range;  // if non-zero, then the chars are continuous, and this is the first codepoint
   int *array_of_unicode_codepoints;       // if non-zero, then this is an array of unicode codepoints
   int num_chars;
   stbtt_packedchar *chardata_for_range; // output
   unsigned char h_oversample, v_oversample; // don't set these, they're used internally
} stbtt_pack_range;

STBTT_DEF int  stbtt_PackFontRanges(stbtt_pack_context *spc, const unsigned char *fontdata, int font_index, stbtt_pack_range *ranges, int num_ranges);
// Creates character bitmaps from multiple ranges of characters stored in
// ranges. This will usually create a better-packed bitmap than multiple
// calls to stbtt_PackFontRange. Note that you can call this multiple
// times within a single PackBegin/PackEnd.

STBTT_DEF void stbtt_PackSetOversampling(stbtt_pack_context *spc, unsigned int h_oversample, unsigned int v_oversample);
// Oversampling a font increases the quality by allowing higher-quality subpixel
// positioning, and is especially valuable at smaller text sizes.
//
// This function sets the amount of oversampling for all following calls to
// stbtt_PackFontRange(s) or stbtt_PackFontRangesGatherRects for a given
// pack context. The default (no oversampling) is achieved by h_oversample=1
// and v_oversample=1. The total number of pixels required is
// h_oversample*v_oversample larger than the default; for example, 2x2
// oversampling requires 4x the storage of 1x1. For best results, render
// oversampled textures with bilinear filtering. Look at the readme in
// stb/tests/oversample for information about oversampled fonts
//
// To use with PackFontRangesGather etc., you must set it before calls
// call to PackFontRangesGatherRects.

STBTT_DEF void stbtt_PackSetSkipMissingCodepoints(stbtt_pack_context *spc, int skip);
// If skip != 0, this tells stb_truetype to skip any codepoints for which
// there is no corresponding glyph. If skip=0, which is the default, then
// codepoints without a glyph recived the font's "missing character" glyph,
// typically an empty box by convention.

STBTT_DEF void stbtt_GetPackedQuad(const stbtt_packedchar *chardata, int pw, int ph,  // same data as above
                               int char_index,             // character to display
                               float *xpos, float *ypos,   // pointers to current position in screen pixel space
                               stbtt_aligned_quad *q,      // output: quad to draw
                               int align_to_integer);

STBTT_DEF int  stbtt_PackFontRangesGatherRects(stbtt_pack_context *spc, const stbtt_fontinfo *info, stbtt_pack_range *ranges, int num_ranges, stbrp_rect *rects);
STBTT_DEF void stbtt_PackFontRangesPackRects(stbtt_pack_context *spc, stbrp_rect *rects, int num_rects);
STBTT_DEF int  stbtt_PackFontRangesRenderIntoRects(stbtt_pack_context *spc, const stbtt_fontinfo *info, stbtt_pack_range *ranges, int num_ranges, stbrp_rect *rects);
// Calling these functions in sequence is roughly equivalent to calling
// stbtt_PackFontRanges(). If you more control over the packing of multiple
// fonts, or if you want to pack custom data into a font texture, take a look
// at the source to of stbtt_PackFontRanges() and create a custom version
// using these functions, e.g. call GatherRects multiple times,
// building up a single array of rects, then call PackRects once,
// then call RenderIntoRects repeatedly. This may result in a
// better packing than calling PackFontRanges multiple times
// (or it may not).

// this is an opaque structure that you shouldn't mess with which holds
// all the context needed from PackBegin to PackEnd.
struct stbtt_pack_context {
   void *user_allocator_context;
   void *pack_info;
   int   width;
   int   height;
   int   stride_in_bytes;
   int   padding;
   int   skip_missing;
   unsigned int   h_oversample, v_oversample;
   unsigned char *pixels;
   void  *nodes;
};

//////////////////////////////////////////////////////////////////////////////
//
// FONT LOADING
//
//

STBTT_DEF int stbtt_GetNumberOfFonts(const unsigned char *data);
// This function will determine the number of fonts in a font file.  TrueType
// collection (.ttc) files may contain multiple fonts, while TrueType font
// (.ttf) files only contain one font. The number of fonts can be used for
// indexing with the previous function where the index is between zero and one
// less than the total fonts. If an error occurs, -1 is returned.

STBTT_DEF int stbtt_GetFontOffsetForIndex(const unsigned char *data, int index);
// Each .ttf/.ttc file may have more than one font. Each font has a sequential
// index number starting from 0. Call this function to get the font offset for
// a given index; it returns -1 if the index is out of range. A regular .ttf
// file will only define one font and it always be at offset 0, so it will
// return '0' for index 0, and -1 for all other indices.

// The following structure is defined publicly so you can declare one on
// the stack or as a global or etc, but you should treat it as opaque.
struct stbtt_fontinfo
{
   void           * userdata;
   unsigned char  * data;              // pointer to .ttf file
   int              fontstart;         // offset of start of font

   int numGlyphs;                     // number of glyphs, needed for range checking

   int loca,head,glyf,hhea,hmtx,kern,gpos,svg; // table locations as offset from start of .ttf
   int index_map;                     // a cmap mapping for our chosen character encoding
   int indexToLocFormat;              // format needed to map from glyph index to glyph

   stbtt__buf cff;                    // cff font data
   stbtt__buf charstrings;            // the charstring index
   stbtt__buf gsubrs;                 // global charstring subroutines index
   stbtt__buf subrs;                  // private charstring subroutines index
   stbtt__buf fontdicts;              // array of font dicts
   stbtt__buf fdselect;               // map from glyph to fontdict
};

STBTT_DEF int stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset);
// Given an offset into the file that defines a font, this function builds
// the necessary cached info for the rest of the system. You must allocate
// the stbtt_fontinfo yourself, and stbtt_InitFont will fill it out. You don't
// need to do anything special to free it, because the contents are pure
// value data with no additional data structures. Returns 0 on failure.


//////////////////////////////////////////////////////////////////////////////
//
// CHARACTER TO GLYPH-INDEX CONVERSIOn

STBTT_DEF int stbtt_FindGlyphIndex(const stbtt_fontinfo *info, int unicode_codepoint);
// If you're going to perform multiple operations on the same character
// and you want a speed-up, call this function with the character you're
// going to process, then use glyph-based functions instead of the
// codepoint-based functions.
// Returns 0 if the character codepoint is not defined in the font.


//////////////////////////////////////////////////////////////////////////////
//
// CHARACTER PROPERTIES
//

STBTT_DEF float stbtt_ScaleForPixelHeight(const stbtt_fontinfo *info, float pixels);
// computes a scale factor to produce a font whose "height" is 'pixels' tall.
// Height is measured as the distance from the highest ascender to the lowest
// descender; in other words, it's equivalent to calling stbtt_GetFontVMetrics
// and computing:
//       scale = pixels / (ascent - descent)
// so if you prefer to measure height by the ascent only, use a similar calculation.

STBTT_DEF float stbtt_ScaleForMappingEmToPixels(const stbtt_fontinfo *info, float pixels);
// computes a scale factor to produce a font whose EM size is mapped to
// 'pixels' tall. This is probably what traditional APIs compute, but
// I'm not positive.

STBTT_DEF void stbtt_GetFontVMetrics(const stbtt_fontinfo *info, int *ascent, int *descent, int *lineGap);
// ascent is the coordinate above the baseline the font extends; descent
// is the coordinate below the baseline the font extends (i.e. it is typically negative)
// lineGap is the spacing between one row's descent and the next row's ascent...
// so you should advance the vertical position by "*ascent - *descent + *lineGap"
//   these are expressed in unscaled coordinates, so you must multiply by
//   the scale factor for a given size

STBTT_DEF int  stbtt_GetFontVMetricsOS2(const stbtt_fontinfo *info, int *typoAscent, int *typoDescent, int *typoLineGap);
// analogous to GetFontVMetrics, but returns the "typographic" values from the OS/2
// table (specific to MS/Windows TTF files).
//
// Returns 1 on success (table present), 0 on failure.

STBTT_DEF void stbtt_GetFontBoundingBox(const stbtt_fontinfo *info, int *x0, int *y0, int *x1, int *y1);
// the bounding box around all possible characters

STBTT_DEF void stbtt_GetCodepointHMetrics(const stbtt_fontinfo *info, int codepoint, int *advanceWidth, int *leftSideBearing);
// leftSideBearing is the offset from the current horizontal position to the left edge of the character
// advanceWidth is the offset from the current horizontal position to the next horizontal position
//   these are expressed in unscaled coordinates

STBTT_DEF int  stbtt_GetCodepointKernAdvance(const stbtt_fontinfo *info, int ch1, int ch2);
// an additional amount to add to the 'advance' value between ch1 and ch2

STBTT_DEF int stbtt_GetCodepointBox(const stbtt_fontinfo *info, int codepoint, int *x0, int *y0, int *x1, int *y1);
// Gets the bounding box of the visible part of the glyph, in unscaled coordinates

STBTT_DEF void stbtt_GetGlyphHMetrics(const stbtt_fontinfo *info, int glyph_index, int *advanceWidth, int *leftSideBearing);
STBTT_DEF int  stbtt_GetGlyphKernAdvance(const stbtt_fontinfo *info, int glyph1, int glyph2);
STBTT_DEF int  stbtt_GetGlyphBox(const stbtt_fontinfo *info, int glyph_index, int *x0, int *y0, int *x1, int *y1);
// as above, but takes one or more glyph indices for greater efficiency

typedef struct stbtt_kerningentry
{
   int glyph1; // use stbtt_FindGlyphIndex
   int glyph2;
   int advance;
} stbtt_kerningentry;

STBTT_DEF int  stbtt_GetKerningTableLength(const stbtt_fontinfo *info);
STBTT_DEF int  stbtt_GetKerningTable(const stbtt_fontinfo *info, stbtt_kerningentry* table, int table_length);
// Retrieves a complete list of all of the kerning pairs provided by the font
// stbtt_GetKerningTable never writes more than table_length entries and returns how many entries it did write.
// The table will be sorted by (a.glyph1 == b.glyph1)?(a.glyph2 < b.glyph2):(a.glyph1 < b.glyph1)

//////////////////////////////////////////////////////////////////////////////
//
// GLYPH SHAPES (you probably don't need these, but they have to go before
// the bitmaps for C declaration-order reasons)
//

#ifndef STBTT_vmove // you can predefine these to use different values (but why?)
   enum {
      STBTT_vmove=1,
      STBTT_vline,
      STBTT_vcurve,
      STBTT_vcubic
   };
#endif

#ifndef stbtt_vertex // you can predefine this to use different values
                   // (we share this with other code at RAD)
   #define stbtt_vertex_type short // can't use stbtt_int16 because that's not visible in the header file
   typedef struct
   {
      stbtt_vertex_type x,y,cx,cy,cx1,cy1;
      unsigned char type,padding;
   } stbtt_vertex;
#endif

STBTT_DEF int stbtt_IsGlyphEmpty(const stbtt_fontinfo *info, int glyph_index);
// returns non-zero if nothing is drawn for this glyph

STBTT_DEF int stbtt_GetCodepointShape(const stbtt_fontinfo *info, int unicode_codepoint, stbtt_vertex **vertices);
STBTT_DEF int stbtt_GetGlyphShape(const stbtt_fontinfo *info, int glyph_index, stbtt_vertex **vertices);
// returns # of vertices and fills *vertices with the pointer to them
//   these are expressed in "unscaled" coordinates
//
// The shape is a series of contours. Each one starts with
// a STBTT_moveto, then consists of a series of mixed
// STBTT_lineto and STBTT_curveto segments. A lineto
// draws a line from previous endpoint to its x,y; a curveto
// draws a quadratic bezier from previous endpoint to
// its x,y, using cx,cy as the bezier control point.

STBTT_DEF void stbtt_FreeShape(const stbtt_fontinfo *info, stbtt_vertex *vertices);
// frees the data allocated above

STBTT_DEF unsigned char *stbtt_FindSVGDoc(const stbtt_fontinfo *info, int gl);
STBTT_DEF int stbtt_GetCodepointSVG(const stbtt_fontinfo *info, int unicode_codepoint, const char **svg);
STBTT_DEF int stbtt_GetGlyphSVG(const stbtt_fontinfo *info, int gl, const char **svg);
// fills svg with the character's SVG data.
// returns data size or 0 if SVG not found.

//////////////////////////////////////////////////////////////////////////////
//
// BITMAP RENDERING
//

STBTT_DEF void stbtt_FreeBitmap(unsigned char *bitmap, void *userdata);
// frees the bitmap allocated below

STBTT_DEF unsigned char *stbtt_GetCodepointBitmap(const stbtt_fontinfo *info, float scale_x, float scale_y, int codepoint, int *width, int *height, int *xoff, int *yoff);
// allocates a large-enough single-channel 8bpp bitmap and renders the
// specified character/glyph at the specified scale into it, with
// antialiasing. 0 is no coverage (transparent), 255 is fully covered (opaque).
// *width & *height are filled out with the width & height of the bitmap,
// which is stored left-to-right, top-to-bottom.
//
// xoff/yoff are the offset it pixel space from the glyph origin to the top-left of the bitmap

STBTT_DEF unsigned char *stbtt_GetCodepointBitmapSubpixel(const stbtt_fontinfo *info, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint, int *width, int *height, int *xoff, int *yoff);
// the same as stbtt_GetCodepoitnBitmap, but you can specify a subpixel
// shift for the character

STBTT_DEF void stbtt_MakeCodepointBitmap(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int codepoint);
// the same as stbtt_GetCodepointBitmap, but you pass in storage for the bitmap
// in the form of 'output', with row spacing of 'out_stride' bytes. the bitmap
// is clipped to out_w/out_h bytes. Call stbtt_GetCodepointBitmapBox to get the
// width and height and positioning info for it first.

STBTT_DEF void stbtt_MakeCodepointBitmapSubpixel(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint);
// same as stbtt_MakeCodepointBitmap, but you can specify a subpixel
// shift for the character

STBTT_DEF void stbtt_MakeCodepointBitmapSubpixelPrefilter(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x, int oversample_y, float *sub_x, float *sub_y, int codepoint);
// same as stbtt_MakeCodepointBitmapSubpixel, but prefiltering
// is performed (see stbtt_PackSetOversampling)

STBTT_DEF void stbtt_GetCodepointBitmapBox(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1);
// get the bbox of the bitmap centered around the glyph origin; so the
// bitmap width is ix1-ix0, height is iy1-iy0, and location to place
// the bitmap top left is (leftSideBearing*scale,iy0).
// (Note that the bitmap uses y-increases-down, but the shape uses
// y-increases-up, so CodepointBitmapBox and CodepointBox are inverted.)

STBTT_DEF void stbtt_GetCodepointBitmapBoxSubpixel(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);
// same as stbtt_GetCodepointBitmapBox, but you can specify a subpixel
// shift for the character

// the following functions are equivalent to the above functions, but operate
// on glyph indices instead of Unicode codepoints (for efficiency)
STBTT_DEF unsigned char *stbtt_GetGlyphBitmap(const stbtt_fontinfo *info, float scale_x, float scale_y, int glyph, int *width, int *height, int *xoff, int *yoff);
STBTT_DEF unsigned char *stbtt_GetGlyphBitmapSubpixel(const stbtt_fontinfo *info, float scale_x, float scale_y, float shift_x, float shift_y, int glyph, int *width, int *height, int *xoff, int *yoff);
STBTT_DEF void stbtt_MakeGlyphBitmap(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int glyph);
STBTT_DEF void stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph);
STBTT_DEF void stbtt_MakeGlyphBitmapSubpixelPrefilter(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x, int oversample_y, float *sub_x, float *sub_y, int glyph);
STBTT_DEF void stbtt_GetGlyphBitmapBox(const stbtt_fontinfo *font, int glyph, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1);
STBTT_DEF void stbtt_GetGlyphBitmapBoxSubpixel(const stbtt_fontinfo *font, int glyph, float scale_x, float scale_y,float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);


// @TODO: don't expose this structure
typedef struct
{
   int w,h,stride;
   unsigned char *pixels;
} stbtt__bitmap;

// rasterize a shape with quadratic beziers into a bitmap
STBTT_DEF void stbtt_Rasterize(stbtt__bitmap *result,        // 1-channel bitmap to draw into
                               float flatness_in_pixels,     // allowable error of curve in pixels
                               stbtt_vertex *vertices,       // array of vertices defining shape
                               int num_verts,                // number of vertices in above array
                               float scale_x, float scale_y, // scale applied to input vertices
                               float shift_x, float shift_y, // translation applied to input vertices
                               int x_off, int y_off,         // another translation applied to input
                               int invert,                   // if non-zero, vertically flip shape
                               void *userdata);              // context for to STBTT_MALLOC

//////////////////////////////////////////////////////////////////////////////
//
// Signed Distance Function (or Field) rendering

STBTT_DEF void stbtt_FreeSDF(unsigned char *bitmap, void *userdata);
// frees the SDF bitmap allocated below

STBTT_DEF unsigned char * stbtt_GetGlyphSDF(const stbtt_fontinfo *info, float scale, int glyph, int padding, unsigned char onedge_value, float pixel_dist_scale, int *width, int *height, int *xoff, int *yoff);
STBTT_DEF unsigned char * stbtt_GetCodepointSDF(const stbtt_fontinfo *info, float scale, int codepoint, int padding, unsigned char onedge_value, float pixel_dist_scale, int *width, int *height, int *xoff, int *yoff);
// These functions compute a discretized SDF field for a single character, suitable for storing
// in a single-channel texture, sampling with bilinear filtering, and testing against
// larger than some threshold to produce scalable fonts.
//        info              --  the font
//        scale             --  controls the size of the resulting SDF bitmap, same as it would be creating a regular bitmap
//        glyph/codepoint   --  the character to generate the SDF for
//        padding           --  extra "pixels" around the character which are filled with the distance to the character (not 0),
//                                 which allows effects like bit outlines
//        onedge_value      --  value 0-255 to test the SDF against to reconstruct the character (i.e. the isocontour of the character)
//        pixel_dist_scale  --  what value the SDF should increase by when moving one SDF "pixel" away from the edge (on the 0..255 scale)
//                                 if positive, > onedge_value is inside; if negative, < onedge_value is inside
//        width,height      --  output height & width of the SDF bitmap (including padding)
//        xoff,yoff         --  output origin of the character
//        return value      --  a 2D array of bytes 0..255, width*height in size
//
// pixel_dist_scale & onedge_value are a scale & bias that allows you to make
// optimal use of the limited 0..255 for your application, trading off precision
// and special effects. SDF values outside the range 0..255 are clamped to 0..255.
//
// Example:
//      scale = stbtt_ScaleForPixelHeight(22)
//      padding = 5
//      onedge_value = 180
//      pixel_dist_scale = 180/5.0 = 36.0
//
//      This will create an SDF bitmap in which the character is about 22 pixels
//      high but the whole bitmap is about 22+5+5=32 pixels high. To produce a filled
//      shape, sample the SDF at each pixel and fill the pixel if the SDF value
//      is greater than or equal to 180/255. (You'll actually want to antialias,
//      which is beyond the scope of this example.) Additionally, you can compute
//      offset outlines (e.g. to stroke the character border inside & outside,
//      or only outside). For example, to fill outside the character up to 3 SDF
//      pixels, you would compare against (180-36.0*3)/255 = 72/255. The above
//      choice of variables maps a range from 5 pixels outside the shape to
//      2 pixels inside the shape to 0..255; this is intended primarily for apply
//      outside effects only (the interior range is needed to allow proper
//      antialiasing of the font at *smaller* sizes)
//
// The function computes the SDF analytically at each SDF pixel, not by e.g.
// building a higher-res bitmap and approximating it. In theory the quality
// should be as high as possible for an SDF of this size & representation, but
// unclear if this is true in practice (perhaps building a higher-res bitmap
// and computing from that can allow drop-out prevention).
//
// The algorithm has not been optimized at all, so expect it to be slow
// if computing lots of characters or very large sizes.



//////////////////////////////////////////////////////////////////////////////
//
// Finding the right font...
//
// You should really just solve this offline, keep your own tables
// of what font is what, and don't try to get it out of the .ttf file.
// That's because getting it out of the .ttf file is really hard, because
// the names in the file can appear in many possible encodings, in many
// possible languages, and e.g. if you need a case-insensitive comparison,
// the details of that depend on the encoding & language in a complex way
// (actually underspecified in truetype, but also gigantic).
//
// But you can use the provided functions in two possible ways:
//     stbtt_FindMatchingFont() will use *case-sensitive* comparisons on
//             unicode-encoded names to try to find the font you want;
//             you can run this before calling stbtt_InitFont()
//
//     stbtt_GetFontNameString() lets you get any of the various strings
//             from the file yourself and do your own comparisons on them.
//             You have to have called stbtt_InitFont() first.


STBTT_DEF int stbtt_FindMatchingFont(const unsigned char *fontdata, const char *name, int flags);
// returns the offset (not index) of the font that matches, or -1 if none
//   if you use STBTT_MACSTYLE_DONTCARE, use a font name like "Arial Bold".
//   if you use any other flag, use a font name like "Arial"; this checks
//     the 'macStyle' header field; i don't know if fonts set this consistently
#define STBTT_MACSTYLE_DONTCARE     0
#define STBTT_MACSTYLE_BOLD         1
#define STBTT_MACSTYLE_ITALIC       2
#define STBTT_MACSTYLE_UNDERSCORE   4
#define STBTT_MACSTYLE_NONE         8   // <= not same as 0, this makes us check the bitfield is 0

STBTT_DEF int stbtt_CompareUTF8toUTF16_bigendian(const char *s1, int len1, const char *s2, int len2);
// returns 1/0 whether the first string interpreted as utf8 is identical to
// the second string interpreted as big-endian utf16... useful for strings from next func

STBTT_DEF const char *stbtt_GetFontNameString(const stbtt_fontinfo *font, int *length, int platformID, int encodingID, int languageID, int nameID);
// returns the string (which may be big-endian double byte, e.g. for unicode)
// and puts the length in bytes in *length.
//
// some of the values for the IDs are below; for more see the truetype spec:
//     http://developer.apple.com/textfonts/TTRefMan/RM06/Chap6name.html
//     http://www.microsoft.com/typography/otspec/name.htm

enum { // platformID
   STBTT_PLATFORM_ID_UNICODE   =0,
   STBTT_PLATFORM_ID_MAC       =1,
   STBTT_PLATFORM_ID_ISO       =2,
   STBTT_PLATFORM_ID_MICROSOFT =3
};

enum { // encodingID for STBTT_PLATFORM_ID_UNICODE
   STBTT_UNICODE_EID_UNICODE_1_0    =0,
   STBTT_UNICODE_EID_UNICODE_1_1    =1,
   STBTT_UNICODE_EID_ISO_10646      =2,
   STBTT_UNICODE_EID_UNICODE_2_0_BMP=3,
   STBTT_UNICODE_EID_UNICODE_2_0_FULL=4
};

enum { // encodingID for STBTT_PLATFORM_ID_MICROSOFT
   STBTT_MS_EID_SYMBOL        =0,
   STBTT_MS_EID_UNICODE_BMP   =1,
   STBTT_MS_EID_SHIFTJIS      =2,
   STBTT_MS_EID_UNICODE_FULL  =10
};

enum { // encodingID for STBTT_PLATFORM_ID_MAC; same as Script Manager codes
   STBTT_MAC_EID_ROMAN        =0,   STBTT_MAC_EID_ARABIC       =4,
   STBTT_MAC_EID_JAPANESE     =1,   STBTT_MAC_EID_HEBREW       =5,
   STBTT_MAC_EID_CHINESE_TRAD =2,   STBTT_MAC_EID_GREEK        =6,
   STBTT_MAC_EID_KOREAN       =3,   STBTT_MAC_EID_RUSSIAN      =7
};

enum { // languageID for STBTT_PLATFORM_ID_MICROSOFT; same as LCID...
       // problematic because there are e.g. 16 english LCIDs and 16 arabic LCIDs
   STBTT_MS_LANG_ENGLISH     =0x0409,   STBTT_MS_LANG_ITALIAN     =0x0410,
   STBTT_MS_LANG_CHINESE     =0x0804,   STBTT_MS_LANG_JAPANESE    =0x0411,
   STBTT_MS_LANG_DUTCH       =0x0413,   STBTT_MS_LANG_KOREAN      =0x0412,
   STBTT_MS_LANG_FRENCH      =0x040c,   STBTT_MS_LANG_RUSSIAN     =0x0419,
   STBTT_MS_LANG_GERMAN      =0x0407,   STBTT_MS_LANG_SPANISH     =0x0409,
   STBTT_MS_LANG_HEBREW      =0x040d,   STBTT_MS_LANG_SWEDISH     =0x041D
};

enum { // languageID for STBTT_PLATFORM_ID_MAC
   STBTT_MAC_LANG_ENGLISH      =0 ,   STBTT_MAC_LANG_JAPANESE     =11,
   STBTT_MAC_LANG_ARABIC       =12,   STBTT_MAC_LANG_KOREAN       =23,
   STBTT_MAC_LANG_DUTCH        =4 ,   STBTT_MAC_LANG_RUSSIAN      =32,
   STBTT_MAC_LANG_FRENCH       =1 ,   STBTT_MAC_LANG_SPANISH      =6 ,
   STBTT_MAC_LANG_GERMAN       =2 ,   STBTT_MAC_LANG_SWEDISH      =5 ,
   STBTT_MAC_LANG_HEBREW       =10,   STBTT_MAC_LANG_CHINESE_SIMPLIFIED =33,
   STBTT_MAC_LANG_ITALIAN      =3 ,   STBTT_MAC_LANG_CHINESE_TRAD =19
};

#ifdef __cplusplus
}
#endif

#endif // __STB_INCLUDE_STB_TRUETYPE_H__

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////   IMPLEMENTATION
////
////

#ifdef STB_TRUETYPE_IMPLEMENTATION

#ifndef STBTT_MAX_OVERSAMPLE
#define STBTT_MAX_OVERSAMPLE   8
#endif

#if STBTT_MAX_OVERSAMPLE > 255
#error "STBTT_MAX_OVERSAMPLE cannot be > 255"
#endif

typedef int stbtt__test_oversample_pow2[(STBTT_MAX_OVERSAMPLE & (STBTT_MAX_OVERSAMPLE-1)) == 0 ? 1 : -1];

#ifndef STBTT_RASTERIZER_VERSION
#define STBTT_RASTERIZER_VERSION 2
#endif

#ifdef _MSC_VER
#define STBTT__NOTUSED(v)  (void)(v)
#else
#define STBTT__NOTUSED(v)  (void)sizeof(v)
#endif

//////////////////////////////////////////////////////////////////////////
//
// stbtt__buf helpers to parse data from file
//

static stbtt_uint8 stbtt__buf_get8(stbtt__buf *b)
{
   if (b->cursor >= b->size)
      return 0;
   return b->data[b->cursor++];
}

static stbtt_uint8 stbtt__buf_peek8(stbtt__buf *b)
{
   if (b->cursor >= b->size)
      return 0;
   return b->data[b->cursor];
}

static void stbtt__buf_seek(stbtt__buf *b, int o)
{
   STBTT_assert(!(o > b->size || o < 0));
   b->cursor = (o > b->size || o < 0) ? b->size : o;
}

static void stbtt__buf_skip(stbtt__buf *b, int o)
{
   stbtt__buf_seek(b, b->cursor + o);
}

static stbtt_uint32 stbtt__buf_get(stbtt__buf *b, int n)
{
   stbtt_uint32 v = 0;
   int i;
   STBTT_assert(n >= 1 && n <= 4);
   for (i = 0; i < n; i++)
      v = (v << 8) | stbtt__buf_get8(b);
   return v;
}

static stbtt__buf stbtt__new_buf(const void *p, size_t size)
{
   stbtt__buf r;
   STBTT_assert(size < 0x40000000);
   r.data = (stbtt_uint8*) p;
   r.size = (int) size;
   r.cursor = 0;
   return r;
}

#define stbtt__buf_get16(b)  stbtt__buf_get((b), 2)
#define stbtt__buf_get32(b)  stbtt__buf_get((b), 4)

static stbtt__buf stbtt__buf_range(const stbtt__buf *b, int o, int s)
{
   stbtt__buf r = stbtt__new_buf(NULL, 0);
   if (o < 0 || s < 0 || o > b->size || s > b->size - o) return r;
   r.data = b->data + o;
   r.size = s;
   return r;
}

static stbtt__buf stbtt__cff_get_index(stbtt__buf *b)
{
   int count, start, offsize;
   start = b->cursor;
   count = stbtt__buf_get16(b);
   if (count) {
      offsize = stbtt__buf_get8(b);
      STBTT_assert(offsize >= 1 && offsize <= 4);
      stbtt__buf_skip(b, offsize * count);
      stbtt__buf_skip(b, stbtt__buf_get(b, offsize) - 1);
   }
   return stbtt__buf_range(b, start, b->cursor - start);
}

static stbtt_uint32 stbtt__cff_int(stbtt__buf *b)
{
   int b0 = stbtt__buf_get8(b);
   if (b0 >= 32 && b0 <= 246)       return b0 - 139;
   else if (b0 >= 247 && b0 <= 250) return (b0 - 247)*256 + stbtt__buf_get8(b) + 108;
   else if (b0 >= 251 && b0 <= 254) return -(b0 - 251)*256 - stbtt__buf_get8(b) - 108;
   else if (b0 == 28)               return stbtt__buf_get16(b);
   else if (b0 == 29)               return stbtt__buf_get32(b);
   STBTT_assert(0);
   return 0;
}

static void stbtt__cff_skip_operand(stbtt__buf *b) {
   int v, b0 = stbtt__buf_peek8(b);
   STBTT_assert(b0 >= 28);
   if (b0 == 30) {
      stbtt__buf_skip(b, 1);
      while (b->cursor < b->size) {
         v = stbtt__buf_get8(b);
         if ((v & 0xF) == 0xF || (v >> 4) == 0xF)
            break;
      }
   } else {
      stbtt__cff_int(b);
   }
}

static stbtt__buf stbtt__dict_get(stbtt__buf *b, int key)
{
   stbtt__buf_seek(b, 0);
   while (b->cursor < b->size) {
      int start = b->cursor, end, op;
      while (stbtt__buf_peek8(b) >= 28)
         stbtt__cff_skip_operand(b);
      end = b->cursor;
      op = stbtt__buf_get8(b);
      if (op == 12)  op = stbtt__buf_get8(b) | 0x100;
      if (op == key) return stbtt__buf_range(b, start, end-start);
   }
   return stbtt__buf_range(b, 0, 0);
}

static void stbtt__dict_get_ints(stbtt__buf *b, int key, int outcount, stbtt_uint32 *out)
{
   int i;
   stbtt__buf operands = stbtt__dict_get(b, key);
   for (i = 0; i < outcount && operands.cursor < operands.size; i++)
      out[i] = stbtt__cff_int(&operands);
}

static int stbtt__cff_index_count(stbtt__buf *b)
{
   stbtt__buf_seek(b, 0);
   return stbtt__buf_get16(b);
}

static stbtt__buf stbtt__cff_index_get(stbtt__buf b, int i)
{
   int count, offsize, start, end;
   stbtt__buf_seek(&b, 0);
   count = stbtt__buf_get16(&b);
   offsize = stbtt__buf_get8(&b);
   STBTT_assert(i >= 0 && i < count);
   STBTT_assert(offsize >= 1 && offsize <= 4);
   stbtt__buf_skip(&b, i*offsize);
   start = stbtt__buf_get(&b, offsize);
   end = stbtt__buf_get(&b, offsize);
   return stbtt__buf_range(&b, 2+(count+1)*offsize+start, end - start);
}

//////////////////////////////////////////////////////////////////////////
//
// accessors to parse data from file
//

// on platforms that don't allow misaligned reads, if we want to allow
// truetype fonts that aren't padded to alignment, define ALLOW_UNALIGNED_TRUETYPE

#define ttBYTE(p)     (* (stbtt_uint8 *) (p))
#define ttCHAR(p)     (* (stbtt_int8 *) (p))
#define ttFixed(p)    ttLONG(p)

static stbtt_uint16 ttUSHORT(stbtt_uint8 *p) { return p[0]*256 + p[1]; }
static stbtt_int16 ttSHORT(stbtt_uint8 *p)   { return p[0]*256 + p[1]; }
static stbtt_uint32 ttULONG(stbtt_uint8 *p)  { return (p[0]<<24) + (p[1]<<16) + (p[2]<<8) + p[3]; }
static stbtt_int32 ttLONG(stbtt_uint8 *p)    { return (p[0]<<24) + (p[1]<<16) + (p[2]<<8) + p[3]; }

#define stbtt_tag4(p,c0,c1,c2,c3) ((p)[0] == (c0) && (p)[1] == (c1) && (p)[2] == (c2) && (p)[3] == (c3))
#define stbtt_tag(p,str)           stbtt_tag4(p,str[0],str[1],str[2],str[3])

static int stbtt__isfont(stbtt_uint8 *font)
{
   // check the version number
   if (stbtt_tag4(font, '1',0,0,0))  return 1; // TrueType 1
   if (stbtt_tag(font, "typ1"))   return 1; // TrueType with type 1 font -- we don't support this!
   if (stbtt_tag(font, "OTTO"))   return 1; // OpenType with CFF
   if (stbtt_tag4(font, 0,1,0,0)) return 1; // OpenType 1.0
   if (stbtt_tag(font, "true"))   return 1; // Apple specification for TrueType fonts
   return 0;
}

// @OPTIMIZE: binary search
static stbtt_uint32 stbtt__find_table(stbtt_uint8 *data, stbtt_uint32 fontstart, const char *tag)
{
   stbtt_int32 num_tables = ttUSHORT(data+fontstart+4);
   stbtt_uint32 tabledir = fontstart + 12;
   stbtt_int32 i;
   for (i=0; i < num_tables; ++i) {
      stbtt_uint32 loc = tabledir + 16*i;
      if (stbtt_tag(data+loc+0, tag))
         return ttULONG(data+loc+8);
   }
   return 0;
}

static int stbtt_GetFontOffsetForIndex_internal(unsigned char *font_collection, int index)
{
   // if it's just a font, there's only one valid index
   if (stbtt__isfont(font_collection))
      return index == 0 ? 0 : -1;

   // check if it's a TTC
   if (stbtt_tag(font_collection, "ttcf")) {
      // version 1?
      if (ttULONG(font_collection+4) == 0x00010000 || ttULONG(font_collection+4) == 0x00020000) {
         stbtt_int32 n = ttLONG(font_collection+8);
         if (index >= n)
            return -1;
         return ttULONG(font_collection+12+index*4);
      }
   }
   return -1;
}

static int stbtt_GetNumberOfFonts_internal(unsigned char *font_collection)
{
   // if it's just a font, there's only one valid font
   if (stbtt__isfont(font_collection))
      return 1;

   // check if it's a TTC
   if (stbtt_tag(font_collection, "ttcf")) {
      // version 1?
      if (ttULONG(font_collection+4) == 0x00010000 || ttULONG(font_collection+4) == 0x00020000) {
         return ttLONG(font_collection+8);
      }
   }
   return 0;
}

static stbtt__buf stbtt__get_subrs(stbtt__buf cff, stbtt__buf fontdict)
{
   stbtt_uint32 subrsoff = 0, private_loc[2] = { 0, 0 };
   stbtt__buf pdict;
   stbtt__dict_get_ints(&fontdict, 18, 2, private_loc);
   if (!private_loc[1] || !private_loc[0]) return stbtt__new_buf(NULL, 0);
   pdict = stbtt__buf_range(&cff, private_loc[1], private_loc[0]);
   stbtt__dict_get_ints(&pdict, 19, 1, &subrsoff);
   if (!subrsoff) return stbtt__new_buf(NULL, 0);
   stbtt__buf_seek(&cff, private_loc[1]+subrsoff);
   return stbtt__cff_get_index(&cff);
}

// since most people won't use this, find this table the first time it's needed
static int stbtt__get_svg(stbtt_fontinfo *info)
{
   stbtt_uint32 t;
   if (info->svg < 0) {
      t = stbtt__find_table(info->data, info->fontstart, "SVG ");
      if (t) {
         stbtt_uint32 offset = ttULONG(info->data + t + 2);
         info->svg = t + offset;
      } else {
         info->svg = 0;
      }
   }
   return info->svg;
}

static int stbtt_InitFont_internal(stbtt_fontinfo *info, unsigned char *data, int fontstart)
{
   stbtt_uint32 cmap, t;
   stbtt_int32 i,numTables;

   info->data = data;
   info->fontstart = fontstart;
   info->cff = stbtt__new_buf(NULL, 0);

   cmap = stbtt__find_table(data, fontstart, "cmap");       // required
   info->loca = stbtt__find_table(data, fontstart, "loca"); // required
   info->head = stbtt__find_table(data, fontstart, "head"); // required
   info->glyf = stbtt__find_table(data, fontstart, "glyf"); // required
   info->hhea = stbtt__find_table(data, fontstart, "hhea"); // required
   info->hmtx = stbtt__find_table(data, fontstart, "hmtx"); // required
   info->kern = stbtt__find_table(data, fontstart, "kern"); // not required
   info->gpos = stbtt__find_table(data, fontstart, "GPOS"); // not required

   if (!cmap || !info->head || !info->hhea || !info->hmtx)
      return 0;
   if (info->glyf) {
      // required for truetype
      if (!info->loca) return 0;
   } else {
      // initialization for CFF / Type2 fonts (OTF)
      stbtt__buf b, topdict, topdictidx;
      stbtt_uint32 cstype = 2, charstrings = 0, fdarrayoff = 0, fdselectoff = 0;
      stbtt_uint32 cff;

      cff = stbtt__find_table(data, fontstart, "CFF ");
      if (!cff) return 0;

      info->fontdicts = stbtt__new_buf(NULL, 0);
      info->fdselect = stbtt__new_buf(NULL, 0);

      // @TODO this should use size from table (not 512MB)
      info->cff = stbtt__new_buf(data+cff, 512*1024*1024);
      b = info->cff;

      // read the header
      stbtt__buf_skip(&b, 2);
      stbtt__buf_seek(&b, stbtt__buf_get8(&b)); // hdrsize

      // @TODO the name INDEX could list multiple fonts,
      // but we just use the first one.
      stbtt__cff_get_index(&b);  // name INDEX
      topdictidx = stbtt__cff_get_index(&b);
      topdict = stbtt__cff_index_get(topdictidx, 0);
      stbtt__cff_get_index(&b);  // string INDEX
      info->gsubrs = stbtt__cff_get_index(&b);

      stbtt__dict_get_ints(&topdict, 17, 1, &charstrings);
      stbtt__dict_get_ints(&topdict, 0x100 | 6, 1, &cstype);
      stbtt__dict_get_ints(&topdict, 0x100 | 36, 1, &fdarrayoff);
      stbtt__dict_get_ints(&topdict, 0x100 | 37, 1, &fdselectoff);
      info->subrs = stbtt__get_subrs(b, topdict);

      // we only support Type 2 charstrings
      if (cstype != 2) return 0;
      if (charstrings == 0) return 0;

      if (fdarrayoff) {
         // looks like a CID font
         if (!fdselectoff) return 0;
         stbtt__buf_seek(&b, fdarrayoff);
         info->fontdicts = stbtt__cff_get_index(&b);
         info->fdselect = stbtt__buf_range(&b, fdselectoff, b.size-fdselectoff);
      }

      stbtt__buf_seek(&b, charstrings);
      info->charstrings = stbtt__cff_get_index(&b);
   }

   t = stbtt__find_table(data, fontstart, "maxp");
   if (t)
      info->numGlyphs = ttUSHORT(data+t+4);
   else
      info->numGlyphs = 0xffff;

   info->svg = -1;

   // find a cmap encoding table we understand *now* to avoid searching
   // later. (todo: could make this installable)
   // the same regardless of glyph.
   numTables = ttUSHORT(data + cmap + 2);
   info->index_map = 0;
   for (i=0; i < numTables; ++i) {
      stbtt_uint32 encoding_record = cmap + 4 + 8 * i;
      // find an encoding we understand:
      switch(ttUSHORT(data+encoding_record)) {
         case STBTT_PLATFORM_ID_MICROSOFT:
            switch (ttUSHORT(data+encoding_record+2)) {
               case STBTT_MS_EID_UNICODE_BMP:
               case STBTT_MS_EID_UNICODE_FULL:
                  // MS/Unicode
                  info->index_map = cmap + ttULONG(data+encoding_record+4);
                  break;
            }
            break;
        case STBTT_PLATFORM_ID_UNICODE:
            // Mac/iOS has these
            // all the encodingIDs are unicode, so we don't bother to check it
            info->index_map = cmap + ttULONG(data+encoding_record+4);
            break;
      }
   }
   if (info->index_map == 0)
      return 0;

   info->indexToLocFormat = ttUSHORT(data+info->head + 50);
   return 1;
}

STBTT_DEF int stbtt_FindGlyphIndex(const stbtt_fontinfo *info, int unicode_codepoint)
{
   stbtt_uint8 *data = info->data;
   stbtt_uint32 index_map = info->index_map;

   stbtt_uint16 format = ttUSHORT(data + index_map + 0);
   if (format == 0) { // apple byte encoding
      stbtt_int32 bytes = ttUSHORT(data + index_map + 2);
      if (unicode_codepoint < bytes-6)
         return ttBYTE(data + index_map + 6 + unicode_codepoint);
      return 0;
   } else if (format == 6) {
      stbtt_uint32 first = ttUSHORT(data + index_map + 6);
      stbtt_uint32 count = ttUSHORT(data + index_map + 8);
      if ((stbtt_uint32) unicode_codepoint >= first && (stbtt_uint32) unicode_codepoint < first+count)
         return ttUSHORT(data + index_map + 10 + (unicode_codepoint - first)*2);
      return 0;
   } else if (format == 2) {
      STBTT_assert(0); // @TODO: high-byte mapping for japanese/chinese/korean
      return 0;
   } else if (format == 4) { // standard mapping for windows fonts: binary search collection of ranges
      stbtt_uint16 segcount = ttUSHORT(data+index_map+6) >> 1;
      stbtt_uint16 searchRange = ttUSHORT(data+index_map+8) >> 1;
      stbtt_uint16 entrySelector = ttUSHORT(data+index_map+10);
      stbtt_uint16 rangeShift = ttUSHORT(data+index_map+12) >> 1;

      // do a binary search of the segments
      stbtt_uint32 endCount = index_map + 14;
      stbtt_uint32 search = endCount;

      if (unicode_codepoint > 0xffff)
         return 0;

      // they lie from endCount .. endCount + segCount
      // but searchRange is the nearest power of two, so...
      if (unicode_codepoint >= ttUSHORT(data + search + rangeShift*2))
         search += rangeShift*2;

      // now decrement to bias correctly to find smallest
      search -= 2;
      while (entrySelector) {
         stbtt_uint16 end;
         searchRange >>= 1;
         end = ttUSHORT(data + search + searchRange*2);
         if (unicode_codepoint > end)
            search += searchRange*2;
         --entrySelector;
      }
      search += 2;

      {
         stbtt_uint16 offset, start, last;
         stbtt_uint16 item = (stbtt_uint16) ((search - endCount) >> 1);

         start = ttUSHORT(data + index_map + 14 + segcount*2 + 2 + 2*item);
         last = ttUSHORT(data + endCount + 2*item);
         if (unicode_codepoint < start || unicode_codepoint > last)
            return 0;

         offset = ttUSHORT(data + index_map + 14 + segcount*6 + 2 + 2*item);
         if (offset == 0)
            return (stbtt_uint16) (unicode_codepoint + ttSHORT(data + index_map + 14 + segcount*4 + 2 + 2*item));

         return ttUSHORT(data + offset + (unicode_codepoint-start)*2 + index_map + 14 + segcount*6 + 2 + 2*item);
      }
   } else if (format == 12 || format == 13) {
      stbtt_uint32 ngroups = ttULONG(data+index_map+12);
      stbtt_int32 low,high;
      low = 0; high = (stbtt_int32)ngroups;
      // Binary search the right group.
      while (low < high) {
         stbtt_int32 mid = low + ((high-low) >> 1); // rounds down, so low <= mid < high
         stbtt_uint32 start_char = ttULONG(data+index_map+16+mid*12);
         stbtt_uint32 end_char = ttULONG(data+index_map+16+mid*12+4);
         if ((stbtt_uint32) unicode_codepoint < start_char)
            high = mid;
         else if ((stbtt_uint32) unicode_codepoint > end_char)
            low = mid+1;
         else {
            stbtt_uint32 start_glyph = ttULONG(data+index_map+16+mid*12+8);
            if (format == 12)
               return start_glyph + unicode_codepoint-start_char;
            else // format == 13
               return start_glyph;
         }
      }
      return 0; // not found
   }
   // @TODO
   STBTT_assert(0);
   return 0;
}

STBTT_DEF int stbtt_GetCodepointShape(const stbtt_fontinfo *info, int unicode_codepoint, stbtt_vertex **vertices)
{
   return stbtt_GetGlyphShape(info, stbtt_FindGlyphIndex(info, unicode_codepoint), vertices);
}

static void stbtt_setvertex(stbtt_vertex *v, stbtt_uint8 type, stbtt_int32 x, stbtt_int32 y, stbtt_int32 cx, stbtt_int32 cy)
{
   v->type = type;
   v->x = (stbtt_int16) x;
   v->y = (stbtt_int16) y;
   v->cx = (stbtt_int16) cx;
   v->cy = (stbtt_int16) cy;
}

static int stbtt__GetGlyfOffset(const stbtt_fontinfo *info, int glyph_index)
{
   int g1,g2;

   STBTT_assert(!info->cff.size);

   if (glyph_index >= info->numGlyphs) return -1; // glyph index out of range
   if (info->indexToLocFormat >= 2)    return -1; // unknown index->glyph map format

   if (info->indexToLocFormat == 0) {
      g1 = info->glyf + ttUSHORT(info->data + info->loca + glyph_index * 2) * 2;
      g2 = info->glyf + ttUSHORT(info->data + info->loca + glyph_index * 2 + 2) * 2;
   } else {
      g1 = info->glyf + ttULONG (info->data + info->loca + glyph_index * 4);
      g2 = info->glyf + ttULONG (info->data + info->loca + glyph_index * 4 + 4);
   }

   return g1==g2 ? -1 : g1; // if length is 0, return -1
}

static int stbtt__GetGlyphInfoT2(const stbtt_fontinfo *info, int glyph_index, int *x0, int *y0, int *x1, int *y1);

STBTT_DEF int stbtt_GetGlyphBox(const stbtt_fontinfo *info, int glyph_index, int *x0, int *y0, int *x1, int *y1)
{
   if (info->cff.size) {
      stbtt__GetGlyphInfoT2(info, glyph_index, x0, y0, x1, y1);
   } else {
      int g = stbtt__GetGlyfOffset(info, glyph_index);
      if (g < 0) return 0;

      if (x0) *x0 = ttSHORT(info->data + g + 2);
      if (y0) *y0 = ttSHORT(info->data + g + 4);
      if (x1) *x1 = ttSHORT(info->data + g + 6);
      if (y1) *y1 = ttSHORT(info->data + g + 8);
   }
   return 1;
}

STBTT_DEF int stbtt_GetCodepointBox(const stbtt_fontinfo *info, int codepoint, int *x0, int *y0, int *x1, int *y1)
{
   return stbtt_GetGlyphBox(info, stbtt_FindGlyphIndex(info,codepoint), x0,y0,x1,y1);
}

STBTT_DEF int stbtt_IsGlyphEmpty(const stbtt_fontinfo *info, int glyph_index)
{
   stbtt_int16 numberOfContours;
   int g;
   if (info->cff.size)
      return stbtt__GetGlyphInfoT2(info, glyph_index, NULL, NULL, NULL, NULL) == 0;
   g = stbtt__GetGlyfOffset(info, glyph_index);
   if (g < 0) return 1;
   numberOfContours = ttSHORT(info->data + g);
   return numberOfContours == 0;
}

static int stbtt__close_shape(stbtt_vertex *vertices, int num_vertices, int was_off, int start_off,
    stbtt_int32 sx, stbtt_int32 sy, stbtt_int32 scx, stbtt_int32 scy, stbtt_int32 cx, stbtt_int32 cy)
{
   if (start_off) {
      if (was_off)
         stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, (cx+scx)>>1, (cy+scy)>>1, cx,cy);
      stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, sx,sy,scx,scy);
   } else {
      if (was_off)
         stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve,sx,sy,cx,cy);
      else
         stbtt_setvertex(&vertices[num_vertices++], STBTT_vline,sx,sy,0,0);
   }
   return num_vertices;
}

static int stbtt__GetGlyphShapeTT(const stbtt_fontinfo *info, int glyph_index, stbtt_vertex **pvertices)
{
   stbtt_int16 numberOfContours;
   stbtt_uint8 *endPtsOfContours;
   stbtt_uint8 *data = info->data;
   stbtt_vertex *vertices=0;
   int num_vertices=0;
   int g = stbtt__GetGlyfOffset(info, glyph_index);

   *pvertices = NULL;

   if (g < 0) return 0;

   numberOfContours = ttSHORT(data + g);

   if (numberOfContours > 0) {
      stbtt_uint8 flags=0,flagcount;
      stbtt_int32 ins, i,j=0,m,n, next_move, was_off=0, off, start_off=0;
      stbtt_int32 x,y,cx,cy,sx,sy, scx,scy;
      stbtt_uint8 *points;
      endPtsOfContours = (data + g + 10);
      ins = ttUSHORT(data + g + 10 + numberOfContours * 2);
      points = data + g + 10 + numberOfContours * 2 + 2 + ins;

      n = 1+ttUSHORT(endPtsOfContours + numberOfContours*2-2);

      m = n + 2*numberOfContours;  // a loose bound on how many vertices we might need
      vertices = (stbtt_vertex *) STBTT_malloc(m * sizeof(vertices[0]), info->userdata);
      if (vertices == 0)
         return 0;

      next_move = 0;
      flagcount=0;

      // in first pass, we load uninterpreted data into the allocated array
      // above, shifted to the end of the array so we won't overwrite it when
      // we create our final data starting from the front

      off = m - n; // starting offset for uninterpreted data, regardless of how m ends up being calculated

      // first load flags

      for (i=0; i < n; ++i) {
         if (flagcount == 0) {
            flags = *points++;
            if (flags & 8)
               flagcount = *points++;
         } else
            --flagcount;
         vertices[off+i].type = flags;
      }

      // now load x coordinates
      x=0;
      for (i=0; i < n; ++i) {
         flags = vertices[off+i].type;
         if (flags & 2) {
            stbtt_int16 dx = *points++;
            x += (flags & 16) ? dx : -dx; // ???
         } else {
            if (!(flags & 16)) {
               x = x + (stbtt_int16) (points[0]*256 + points[1]);
               points += 2;
            }
         }
         vertices[off+i].x = (stbtt_int16) x;
      }

      // now load y coordinates
      y=0;
      for (i=0; i < n; ++i) {
         flags = vertices[off+i].type;
         if (flags & 4) {
            stbtt_int16 dy = *points++;
            y += (flags & 32) ? dy : -dy; // ???
         } else {
            if (!(flags & 32)) {
               y = y + (stbtt_int16) (points[0]*256 + points[1]);
               points += 2;
            }
         }
         vertices[off+i].y = (stbtt_int16) y;
      }

      // now convert them to our format
      num_vertices=0;
      sx = sy = cx = cy = scx = scy = 0;
      for (i=0; i < n; ++i) {
         flags = vertices[off+i].type;
         x     = (stbtt_int16) vertices[off+i].x;
         y     = (stbtt_int16) vertices[off+i].y;

         if (next_move == i) {
            if (i != 0)
               num_vertices = stbtt__close_shape(vertices, num_vertices, was_off, start_off, sx,sy,scx,scy,cx,cy);

            // now start the new one
            start_off = !(flags & 1);
            if (start_off) {
               // if we start off with an off-curve point, then when we need to find a point on the curve
               // where we can start, and we need to save some state for when we wraparound.
               scx = x;
               scy = y;
               if (!(vertices[off+i+1].type & 1)) {
                  // next point is also a curve point, so interpolate an on-point curve
                  sx = (x + (stbtt_int32) vertices[off+i+1].x) >> 1;
                  sy = (y + (stbtt_int32) vertices[off+i+1].y) >> 1;
               } else {
                  // otherwise just use the next point as our start point
                  sx = (stbtt_int32) vertices[off+i+1].x;
                  sy = (stbtt_int32) vertices[off+i+1].y;
                  ++i; // we're using point i+1 as the starting point, so skip it
               }
            } else {
               sx = x;
               sy = y;
            }
            stbtt_setvertex(&vertices[num_vertices++], STBTT_vmove,sx,sy,0,0);
            was_off = 0;
            next_move = 1 + ttUSHORT(endPtsOfContours+j*2);
            ++j;
         } else {
            if (!(flags & 1)) { // if it's a curve
               if (was_off) // two off-curve control points in a row means interpolate an on-curve midpoint
                  stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, (cx+x)>>1, (cy+y)>>1, cx, cy);
               cx = x;
               cy = y;
               was_off = 1;
            } else {
               if (was_off)
                  stbtt_setvertex(&vertices[num_vertices++], STBTT_vcurve, x,y, cx, cy);
               else
                  stbtt_setvertex(&vertices[num_vertices++], STBTT_vline, x,y,0,0);
               was_off = 0;
            }
         }
      }
      num_vertices = stbtt__close_shape(vertices, num_vertices, was_off, start_off, sx,sy,scx,scy,cx,cy);
   } else if (numberOfContours < 0) {
      // Compound shapes.
      int more = 1;
      stbtt_uint8 *comp = data + g + 10;
      num_vertices = 0;
      vertices = 0;
      while (more) {
         stbtt_uint16 flags, gidx;
         int comp_num_verts = 0, i;
         stbtt_vertex *comp_verts = 0, *tmp = 0;
         float mtx[6] = {1,0,0,1,0,0}, m, n;

         flags = ttSHORT(comp); comp+=2;
         gidx = ttSHORT(comp); comp+=2;

         if (flags & 2) { // XY values
            if (flags & 1) { // shorts
               mtx[4] = ttSHORT(comp); comp+=2;
               mtx[5] = ttSHORT(comp); comp+=2;
            } else {
               mtx[4] = ttCHAR(comp); comp+=1;
               mtx[5] = ttCHAR(comp); comp+=1;
            }
         }
         else {
            // @TODO handle matching point
            STBTT_assert(0);
         }
         if (flags & (1<<3)) { // WE_HAVE_A_SCALE
            mtx[0] = mtx[3] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[1] = mtx[2] = 0;
         } else if (flags & (1<<6)) { // WE_HAVE_AN_X_AND_YSCALE
            mtx[0] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[1] = mtx[2] = 0;
            mtx[3] = ttSHORT(comp)/16384.0f; comp+=2;
         } else if (flags & (1<<7)) { // WE_HAVE_A_TWO_BY_TWO
            mtx[0] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[1] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[2] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[3] = ttSHORT(comp)/16384.0f; comp+=2;
         }

         // Find transformation scales.
         m = (float) STBTT_sqrt(mtx[0]*mtx[0] + mtx[1]*mtx[1]);
         n = (float) STBTT_sqrt(mtx[2]*mtx[2] + mtx[3]*mtx[3]);

         // Get indexed glyph.
         comp_num_verts = stbtt_GetGlyphShape(info, gidx, &comp_verts);
         if (comp_num_verts > 0) {
            // Transform vertices.
            for (i = 0; i < comp_num_verts; ++i) {
               stbtt_vertex* v = &comp_verts[i];
               stbtt_vertex_type x,y;
               x=v->x; y=v->y;
               v->x = (stbtt_vertex_type)(m * (mtx[0]*x + mtx[2]*y + mtx[4]));
               v->y = (stbtt_vertex_type)(n * (mtx[1]*x + mtx[3]*y + mtx[5]));
               x=v->cx; y=v->cy;
               v->cx = (stbtt_vertex_type)(m * (mtx[0]*x + mtx[2]*y + mtx[4]));
               v->cy = (stbtt_vertex_type)(n * (mtx[1]*x + mtx[3]*y + mtx[5]));
            }
            // Append vertices.
            tmp = (stbtt_vertex*)STBTT_malloc((num_vertices+comp_num_verts)*sizeof(stbtt_vertex), info->userdata);
            if (!tmp) {
               if (vertices) STBTT_free(vertices, info->userdata);
               if (comp_verts) STBTT_free(comp_verts, info->userdata);
               return 0;
            }
            if (num_vertices > 0 && vertices) STBTT_memcpy(tmp, vertices, num_vertices*sizeof(stbtt_vertex));
            STBTT_memcpy(tmp+num_vertices, comp_verts, comp_num_verts*sizeof(stbtt_vertex));
            if (vertices) STBTT_free(vertices, info->userdata);
            vertices = tmp;
            STBTT_free(comp_verts, info->userdata);
            num_vertices += comp_num_verts;
         }
         // More components ?
         more = flags & (1<<5);
      }
   } else {
      // numberOfCounters == 0, do nothing
   }

   *pvertices = vertices;
   return num_vertices;
}

typedef struct
{
   int bounds;
   int started;
   float first_x, first_y;
   float x, y;
   stbtt_int32 min_x, max_x, min_y, max_y;

   stbtt_vertex *pvertices;
   int num_vertices;
} stbtt__csctx;

#define STBTT__CSCTX_INIT(bounds) {bounds,0, 0,0, 0,0, 0,0,0,0, NULL, 0}

static void stbtt__track_vertex(stbtt__csctx *c, stbtt_int32 x, stbtt_int32 y)
{
   if (x > c->max_x || !c->started) c->max_x = x;
   if (y > c->max_y || !c->started) c->max_y = y;
   if (x < c->min_x || !c->started) c->min_x = x;
   if (y < c->min_y || !c->started) c->min_y = y;
   c->started = 1;
}

static void stbtt__csctx_v(stbtt__csctx *c, stbtt_uint8 type, stbtt_int32 x, stbtt_int32 y, stbtt_int32 cx, stbtt_int32 cy, stbtt_int32 cx1, stbtt_int32 cy1)
{
   if (c->bounds) {
      stbtt__track_vertex(c, x, y);
      if (type == STBTT_vcubic) {
         stbtt__track_vertex(c, cx, cy);
         stbtt__track_vertex(c, cx1, cy1);
      }
   } else {
      stbtt_setvertex(&c->pvertices[c->num_vertices], type, x, y, cx, cy);
      c->pvertices[c->num_vertices].cx1 = (stbtt_int16) cx1;
      c->pvertices[c->num_vertices].cy1 = (stbtt_int16) cy1;
   }
   c->num_vertices++;
}

static void stbtt__csctx_close_shape(stbtt__csctx *ctx)
{
   if (ctx->first_x != ctx->x || ctx->first_y != ctx->y)
      stbtt__csctx_v(ctx, STBTT_vline, (int)ctx->first_x, (int)ctx->first_y, 0, 0, 0, 0);
}

static void stbtt__csctx_rmove_to(stbtt__csctx *ctx, float dx, float dy)
{
   stbtt__csctx_close_shape(ctx);
   ctx->first_x = ctx->x = ctx->x + dx;
   ctx->first_y = ctx->y = ctx->y + dy;
   stbtt__csctx_v(ctx, STBTT_vmove, (int)ctx->x, (int)ctx->y, 0, 0, 0, 0);
}

static void stbtt__csctx_rline_to(stbtt__csctx *ctx, float dx, float dy)
{
   ctx->x += dx;
   ctx->y += dy;
   stbtt__csctx_v(ctx, STBTT_vline, (int)ctx->x, (int)ctx->y, 0, 0, 0, 0);
}

static void stbtt__csctx_rccurve_to(stbtt__csctx *ctx, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3)
{
   float cx1 = ctx->x + dx1;
   float cy1 = ctx->y + dy1;
   float cx2 = cx1 + dx2;
   float cy2 = cy1 + dy2;
   ctx->x = cx2 + dx3;
   ctx->y = cy2 + dy3;
   stbtt__csctx_v(ctx, STBTT_vcubic, (int)ctx->x, (int)ctx->y, (int)cx1, (int)cy1, (int)cx2, (int)cy2);
}

static stbtt__buf stbtt__get_subr(stbtt__buf idx, int n)
{
   int count = stbtt__cff_index_count(&idx);
   int bias = 107;
   if (count >= 33900)
      bias = 32768;
   else if (count >= 1240)
      bias = 1131;
   n += bias;
   if (n < 0 || n >= count)
      return stbtt__new_buf(NULL, 0);
   return stbtt__cff_index_get(idx, n);
}

static stbtt__buf stbtt__cid_get_glyph_subrs(const stbtt_fontinfo *info, int glyph_index)
{
   stbtt__buf fdselect = info->fdselect;
   int nranges, start, end, v, fmt, fdselector = -1, i;

   stbtt__buf_seek(&fdselect, 0);
   fmt = stbtt__buf_get8(&fdselect);
   if (fmt == 0) {
      // untested
      stbtt__buf_skip(&fdselect, glyph_index);
      fdselector = stbtt__buf_get8(&fdselect);
   } else if (fmt == 3) {
      nranges = stbtt__buf_get16(&fdselect);
      start = stbtt__buf_get16(&fdselect);
      for (i = 0; i < nranges; i++) {
         v = stbtt__buf_get8(&fdselect);
         end = stbtt__buf_get16(&fdselect);
         if (glyph_index >= start && glyph_index < end) {
            fdselector = v;
            break;
         }
         start = end;
      }
   }
   if (fdselector == -1) stbtt__new_buf(NULL, 0);
   return stbtt__get_subrs(info->cff, stbtt__cff_index_get(info->fontdicts, fdselector));
}

static int stbtt__run_charstring(const stbtt_fontinfo *info, int glyph_index, stbtt__csctx *c)
{
   int in_header = 1, maskbits = 0, subr_stack_height = 0, sp = 0, v, i, b0;
   int has_subrs = 0, clear_stack;
   float s[48];
   stbtt__buf subr_stack[10], subrs = info->subrs, b;
   float f;

#define STBTT__CSERR(s) (0)

   // this currently ignores the initial width value, which isn't needed if we have hmtx
   b = stbtt__cff_index_get(info->charstrings, glyph_index);
   while (b.cursor < b.size) {
      i = 0;
      clear_stack = 1;
      b0 = stbtt__buf_get8(&b);
      switch (b0) {
      // @TODO implement hinting
      case 0x13: // hintmask
      case 0x14: // cntrmask
         if (in_header)
            maskbits += (sp / 2); // implicit "vstem"
         in_header = 0;
         stbtt__buf_skip(&b, (maskbits + 7) / 8);
         break;

      case 0x01: // hstem
      case 0x03: // vstem
      case 0x12: // hstemhm
      case 0x17: // vstemhm
         maskbits += (sp / 2);
         break;

      case 0x15: // rmoveto
         in_header = 0;
         if (sp < 2) return STBTT__CSERR("rmoveto stack");
         stbtt__csctx_rmove_to(c, s[sp-2], s[sp-1]);
         break;
      case 0x04: // vmoveto
         in_header = 0;
         if (sp < 1) return STBTT__CSERR("vmoveto stack");
         stbtt__csctx_rmove_to(c, 0, s[sp-1]);
         break;
      case 0x16: // hmoveto
         in_header = 0;
         if (sp < 1) return STBTT__CSERR("hmoveto stack");
         stbtt__csctx_rmove_to(c, s[sp-1], 0);
         break;

      case 0x05: // rlineto
         if (sp < 2) return STBTT__CSERR("rlineto stack");
         for (; i + 1 < sp; i += 2)
            stbtt__csctx_rline_to(c, s[i], s[i+1]);
         break;

      // hlineto/vlineto and vhcurveto/hvcurveto alternate horizontal and vertical
      // starting from a different place.

      case 0x07: // vlineto
         if (sp < 1) return STBTT__CSERR("vlineto stack");
         goto vlineto;
      case 0x06: // hlineto
         if (sp < 1) return STBTT__CSERR("hlineto stack");
         for (;;) {
            if (i >= sp) break;
            stbtt__csctx_rline_to(c, s[i], 0);
            i++;
      vlineto:
            if (i >= sp) break;
            stbtt__csctx_rline_to(c, 0, s[i]);
            i++;
         }
         break;

      case 0x1F: // hvcurveto
         if (sp < 4) return STBTT__CSERR("hvcurveto stack");
         goto hvcurveto;
      case 0x1E: // vhcurveto
         if (sp < 4) return STBTT__CSERR("vhcurveto stack");
         for (;;) {
            if (i + 3 >= sp) break;
            stbtt__csctx_rccurve_to(c, 0, s[i], s[i+1], s[i+2], s[i+3], (sp - i == 5) ? s[i + 4] : 0.0f);
            i += 4;
      hvcurveto:
            if (i + 3 >= sp) break;
            stbtt__csctx_rccurve_to(c, s[i], 0, s[i+1], s[i+2], (sp - i == 5) ? s[i+4] : 0.0f, s[i+3]);
            i += 4;
         }
         break;

      case 0x08: // rrcurveto
         if (sp < 6) return STBTT__CSERR("rcurveline stack");
         for (; i + 5 < sp; i += 6)
            stbtt__csctx_rccurve_to(c, s[i], s[i+1], s[i+2], s[i+3], s[i+4], s[i+5]);
         break;

      case 0x18: // rcurveline
         if (sp < 8) return STBTT__CSERR("rcurveline stack");
         for (; i + 5 < sp - 2; i += 6)
            stbtt__csctx_rccurve_to(c, s[i], s[i+1], s[i+2], s[i+3], s[i+4], s[i+5]);
         if (i + 1 >= sp) return STBTT__CSERR("rcurveline stack");
         stbtt__csctx_rline_to(c, s[i], s[i+1]);
         break;

      case 0x19: // rlinecurve
         if (sp < 8) return STBTT__CSERR("rlinecurve stack");
         for (; i + 1 < sp - 6; i += 2)
            stbtt__csctx_rline_to(c, s[i], s[i+1]);
         if (i + 5 >= sp) return STBTT__CSERR("rlinecurve stack");
         stbtt__csctx_rccurve_to(c, s[i], s[i+1], s[i+2], s[i+3], s[i+4], s[i+5]);
         break;

      case 0x1A: // vvcurveto
      case 0x1B: // hhcurveto
         if (sp < 4) return STBTT__CSERR("(vv|hh)curveto stack");
         f = 0.0;
         if (sp & 1) { f = s[i]; i++; }
         for (; i + 3 < sp; i += 4) {
            if (b0 == 0x1B)
               stbtt__csctx_rccurve_to(c, s[i], f, s[i+1], s[i+2], s[i+3], 0.0);
            else
               stbtt__csctx_rccurve_to(c, f, s[i], s[i+1], s[i+2], 0.0, s[i+3]);
            f = 0.0;
         }
         break;

      case 0x0A: // callsubr
         if (!has_subrs) {
            if (info->fdselect.size)
               subrs = stbtt__cid_get_glyph_subrs(info, glyph_index);
            has_subrs = 1;
         }
         // FALLTHROUGH
      case 0x1D: // callgsubr
         if (sp < 1) return STBTT__CSERR("call(g|)subr stack");
         v = (int) s[--sp];
         if (subr_stack_height >= 10) return STBTT__CSERR("recursion limit");
         subr_stack[subr_stack_height++] = b;
         b = stbtt__get_subr(b0 == 0x0A ? subrs : info->gsubrs, v);
         if (b.size == 0) return STBTT__CSERR("subr not found");
         b.cursor = 0;
         clear_stack = 0;
         break;

      case 0x0B: // return
         if (subr_stack_height <= 0) return STBTT__CSERR("return outside subr");
         b = subr_stack[--subr_stack_height];
         clear_stack = 0;
         break;

      case 0x0E: // endchar
         stbtt__csctx_close_shape(c);
         return 1;

      case 0x0C: { // two-byte escape
         float dx1, dx2, dx3, dx4, dx5, dx6, dy1, dy2, dy3, dy4, dy5, dy6;
         float dx, dy;
         int b1 = stbtt__buf_get8(&b);
         switch (b1) {
         // @TODO These "flex" implementations ignore the flex-depth and resolution,
         // and always draw beziers.
         case 0x22: // hflex
            if (sp < 7) return STBTT__CSERR("hflex stack");
            dx1 = s[0];
            dx2 = s[1];
            dy2 = s[2];
            dx3 = s[3];
            dx4 = s[4];
            dx5 = s[5];
            dx6 = s[6];
            stbtt__csctx_rccurve_to(c, dx1, 0, dx2, dy2, dx3, 0);
            stbtt__csctx_rccurve_to(c, dx4, 0, dx5, -dy2, dx6, 0);
            break;

         case 0x23: // flex
            if (sp < 13) return STBTT__CSERR("flex stack");
            dx1 = s[0];
            dy1 = s[1];
            dx2 = s[2];
            dy2 = s[3];
            dx3 = s[4];
            dy3 = s[5];
            dx4 = s[6];
            dy4 = s[7];
            dx5 = s[8];
            dy5 = s[9];
            dx6 = s[10];
            dy6 = s[11];
            //fd is s[12]
            stbtt__csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, dy3);
            stbtt__csctx_rccurve_to(c, dx4, dy4, dx5, dy5, dx6, dy6);
            break;

         case 0x24: // hflex1
            if (sp < 9) return STBTT__CSERR("hflex1 stack");
            dx1 = s[0];
            dy1 = s[1];
            dx2 = s[2];
            dy2 = s[3];
            dx3 = s[4];
            dx4 = s[5];
            dx5 = s[6];
            dy5 = s[7];
            dx6 = s[8];
            stbtt__csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, 0);
            stbtt__csctx_rccurve_to(c, dx4, 0, dx5, dy5, dx6, -(dy1+dy2+dy5));
            break;

         case 0x25: // flex1
            if (sp < 11) return STBTT__CSERR("flex1 stack");
            dx1 = s[0];
            dy1 = s[1];
            dx2 = s[2];
            dy2 = s[3];
            dx3 = s[4];
            dy3 = s[5];
            dx4 = s[6];
            dy4 = s[7];
            dx5 = s[8];
            dy5 = s[9];
            dx6 = dy6 = s[10];
            dx = dx1+dx2+dx3+dx4+dx5;
            dy = dy1+dy2+dy3+dy4+dy5;
            if (STBTT_fabs(dx) > STBTT_fabs(dy))
               dy6 = -dy;
            else
               dx6 = -dx;
            stbtt__csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, dy3);
            stbtt__csctx_rccurve_to(c, dx4, dy4, dx5, dy5, dx6, dy6);
            break;

         default:
            return STBTT__CSERR("unimplemented");
         }
      } break;

      default:
         if (b0 != 255 && b0 != 28 && b0 < 32)
            return STBTT__CSERR("reserved operator");

         // push immediate
         if (b0 == 255) {
            f = (float)(stbtt_int32)stbtt__buf_get32(&b) / 0x10000;
         } else {
            stbtt__buf_skip(&b, -1);
            f = (float)(stbtt_int16)stbtt__cff_int(&b);
         }
         if (sp >= 48) return STBTT__CSERR("push stack overflow");
         s[sp++] = f;
         clear_stack = 0;
         break;
      }
      if (clear_stack) sp = 0;
   }
   return STBTT__CSERR("no endchar");

#undef STBTT__CSERR
}

static int stbtt__GetGlyphShapeT2(const stbtt_fontinfo *info, int glyph_index, stbtt_vertex **pvertices)
{
   // runs the charstring twice, once to count and once to output (to avoid realloc)
   stbtt__csctx count_ctx = STBTT__CSCTX_INIT(1);
   stbtt__csctx output_ctx = STBTT__CSCTX_INIT(0);
   if (stbtt__run_charstring(info, glyph_index, &count_ctx)) {
      *pvertices = (stbtt_vertex*)STBTT_malloc(count_ctx.num_vertices*sizeof(stbtt_vertex), info->userdata);
      output_ctx.pvertices = *pvertices;
      if (stbtt__run_charstring(info, glyph_index, &output_ctx)) {
         STBTT_assert(output_ctx.num_vertices == count_ctx.num_vertices);
         return output_ctx.num_vertices;
      }
   }
   *pvertices = NULL;
   return 0;
}

static int stbtt__GetGlyphInfoT2(const stbtt_fontinfo *info, int glyph_index, int *x0, int *y0, int *x1, int *y1)
{
   stbtt__csctx c = STBTT__CSCTX_INIT(1);
   int r = stbtt__run_charstring(info, glyph_index, &c);
   if (x0)  *x0 = r ? c.min_x : 0;
   if (y0)  *y0 = r ? c.min_y : 0;
   if (x1)  *x1 = r ? c.max_x : 0;
   if (y1)  *y1 = r ? c.max_y : 0;
   return r ? c.num_vertices : 0;
}

STBTT_DEF int stbtt_GetGlyphShape(const stbtt_fontinfo *info, int glyph_index, stbtt_vertex **pvertices)
{
   if (!info->cff.size)
      return stbtt__GetGlyphShapeTT(info, glyph_index, pvertices);
   else
      return stbtt__GetGlyphShapeT2(info, glyph_index, pvertices);
}

STBTT_DEF void stbtt_GetGlyphHMetrics(const stbtt_fontinfo *info, int glyph_index, int *advanceWidth, int *leftSideBearing)
{
   stbtt_uint16 numOfLongHorMetrics = ttUSHORT(info->data+info->hhea + 34);
   if (glyph_index < numOfLongHorMetrics) {
      if (advanceWidth)     *advanceWidth    = ttSHORT(info->data + info->hmtx + 4*glyph_index);
      if (leftSideBearing)  *leftSideBearing = ttSHORT(info->data + info->hmtx + 4*glyph_index + 2);
   } else {
      if (advanceWidth)     *advanceWidth    = ttSHORT(info->data + info->hmtx + 4*(numOfLongHorMetrics-1));
      if (leftSideBearing)  *leftSideBearing = ttSHORT(info->data + info->hmtx + 4*numOfLongHorMetrics + 2*(glyph_index - numOfLongHorMetrics));
   }
}

STBTT_DEF int  stbtt_GetKerningTableLength(const stbtt_fontinfo *info)
{
   stbtt_uint8 *data = info->data + info->kern;

   // we only look at the first table. it must be 'horizontal' and format 0.
   if (!info->kern)
      return 0;
   if (ttUSHORT(data+2) < 1) // number of tables, need at least 1
      return 0;
   if (ttUSHORT(data+8) != 1) // horizontal flag must be set in format
      return 0;

   return ttUSHORT(data+10);
}

STBTT_DEF int stbtt_GetKerningTable(const stbtt_fontinfo *info, stbtt_kerningentry* table, int table_length)
{
   stbtt_uint8 *data = info->data + info->kern;
   int k, length;

   // we only look at the first table. it must be 'horizontal' and format 0.
   if (!info->kern)
      return 0;
   if (ttUSHORT(data+2) < 1) // number of tables, need at least 1
      return 0;
   if (ttUSHORT(data+8) != 1) // horizontal flag must be set in format
      return 0;

   length = ttUSHORT(data+10);
   if (table_length < length)
      length = table_length;

   for (k = 0; k < length; k++)
   {
      table[k].glyph1 = ttUSHORT(data+18+(k*6));
      table[k].glyph2 = ttUSHORT(data+20+(k*6));
      table[k].advance = ttSHORT(data+22+(k*6));
   }

   return length;
}

static int stbtt__GetGlyphKernInfoAdvance(const stbtt_fontinfo *info, int glyph1, int glyph2)
{
   stbtt_uint8 *data = info->data + info->kern;
   stbtt_uint32 needle, straw;
   int l, r, m;

   // we only look at the first table. it must be 'horizontal' and format 0.
   if (!info->kern)
      return 0;
   if (ttUSHORT(data+2) < 1) // number of tables, need at least 1
      return 0;
   if (ttUSHORT(data+8) != 1) // horizontal flag must be set in format
      return 0;

   l = 0;
   r = ttUSHORT(data+10) - 1;
   needle = glyph1 << 16 | glyph2;
   while (l <= r) {
      m = (l + r) >> 1;
      straw = ttULONG(data+18+(m*6)); // note: unaligned read
      if (needle < straw)
         r = m - 1;
      else if (needle > straw)
         l = m + 1;
      else
         return ttSHORT(data+22+(m*6));
   }
   return 0;
}

static stbtt_int32 stbtt__GetCoverageIndex(stbtt_uint8 *coverageTable, int glyph)
{
   stbtt_uint16 coverageFormat = ttUSHORT(coverageTable);
   switch (coverageFormat) {
      case 1: {
         stbtt_uint16 glyphCount = ttUSHORT(coverageTable + 2);

         // Binary search.
         stbtt_int32 l=0, r=glyphCount-1, m;
         int straw, needle=glyph;
         while (l <= r) {
            stbtt_uint8 *glyphArray = coverageTable + 4;
            stbtt_uint16 glyphID;
            m = (l + r) >> 1;
            glyphID = ttUSHORT(glyphArray + 2 * m);
            straw = glyphID;
            if (needle < straw)
               r = m - 1;
            else if (needle > straw)
               l = m + 1;
            else {
               return m;
            }
         }
         break;
      }

      case 2: {
         stbtt_uint16 rangeCount = ttUSHORT(coverageTable + 2);
         stbtt_uint8 *rangeArray = coverageTable + 4;

         // Binary search.
         stbtt_int32 l=0, r=rangeCount-1, m;
         int strawStart, strawEnd, needle=glyph;
         while (l <= r) {
            stbtt_uint8 *rangeRecord;
            m = (l + r) >> 1;
            rangeRecord = rangeArray + 6 * m;
            strawStart = ttUSHORT(rangeRecord);
            strawEnd = ttUSHORT(rangeRecord + 2);
            if (needle < strawStart)
               r = m - 1;
            else if (needle > strawEnd)
               l = m + 1;
            else {
               stbtt_uint16 startCoverageIndex = ttUSHORT(rangeRecord + 4);
               return startCoverageIndex + glyph - strawStart;
            }
         }
         break;
      }

      default: return -1; // unsupported
   }

   return -1;
}

static stbtt_int32  stbtt__GetGlyphClass(stbtt_uint8 *classDefTable, int glyph)
{
   stbtt_uint16 classDefFormat = ttUSHORT(classDefTable);
   switch (classDefFormat)
   {
      case 1: {
         stbtt_uint16 startGlyphID = ttUSHORT(classDefTable + 2);
         stbtt_uint16 glyphCount = ttUSHORT(classDefTable + 4);
         stbtt_uint8 *classDef1ValueArray = classDefTable + 6;

         if (glyph >= startGlyphID && glyph < startGlyphID + glyphCount)
            return (stbtt_int32)ttUSHORT(classDef1ValueArray + 2 * (glyph - startGlyphID));
         break;
      }

      case 2: {
         stbtt_uint16 classRangeCount = ttUSHORT(classDefTable + 2);
         stbtt_uint8 *classRangeRecords = classDefTable + 4;

         // Binary search.
         stbtt_int32 l=0, r=classRangeCount-1, m;
         int strawStart, strawEnd, needle=glyph;
         while (l <= r) {
            stbtt_uint8 *classRangeRecord;
            m = (l + r) >> 1;
            classRangeRecord = classRangeRecords + 6 * m;
            strawStart = ttUSHORT(classRangeRecord);
            strawEnd = ttUSHORT(classRangeRecord + 2);
            if (needle < strawStart)
               r = m - 1;
            else if (needle > strawEnd)
               l = m + 1;
            else
               return (stbtt_int32)ttUSHORT(classRangeRecord + 4);
         }
         break;
      }

      default:
         return -1; // Unsupported definition type, return an error.
   }

   // "All glyphs not assigned to a class fall into class 0". (OpenType spec)
   return 0;
}

// Define to STBTT_assert(x) if you want to break on unimplemented formats.
#define STBTT_GPOS_TODO_assert(x)

static stbtt_int32 stbtt__GetGlyphGPOSInfoAdvance(const stbtt_fontinfo *info, int glyph1, int glyph2)
{
   stbtt_uint16 lookupListOffset;
   stbtt_uint8 *lookupList;
   stbtt_uint16 lookupCount;
   stbtt_uint8 *data;
   stbtt_int32 i, sti;

   if (!info->gpos) return 0;

   data = info->data + info->gpos;

   if (ttUSHORT(data+0) != 1) return 0; // Major version 1
   if (ttUSHORT(data+2) != 0) return 0; // Minor version 0

   lookupListOffset = ttUSHORT(data+8);
   lookupList = data + lookupListOffset;
   lookupCount = ttUSHORT(lookupList);

   for (i=0; i<lookupCount; ++i) {
      stbtt_uint16 lookupOffset = ttUSHORT(lookupList + 2 + 2 * i);
      stbtt_uint8 *lookupTable = lookupList + lookupOffset;

      stbtt_uint16 lookupType = ttUSHORT(lookupTable);
      stbtt_uint16 subTableCount = ttUSHORT(lookupTable + 4);
      stbtt_uint8 *subTableOffsets = lookupTable + 6;
      if (lookupType != 2) // Pair Adjustment Positioning Subtable
         continue;

      for (sti=0; sti<subTableCount; sti++) {
         stbtt_uint16 subtableOffset = ttUSHORT(subTableOffsets + 2 * sti);
         stbtt_uint8 *table = lookupTable + subtableOffset;
         stbtt_uint16 posFormat = ttUSHORT(table);
         stbtt_uint16 coverageOffset = ttUSHORT(table + 2);
         stbtt_int32 coverageIndex = stbtt__GetCoverageIndex(table + coverageOffset, glyph1);
         if (coverageIndex == -1) continue;

         switch (posFormat) {
            case 1: {
               stbtt_int32 l, r, m;
               int straw, needle;
               stbtt_uint16 valueFormat1 = ttUSHORT(table + 4);
               stbtt_uint16 valueFormat2 = ttUSHORT(table + 6);
               if (valueFormat1 == 4 && valueFormat2 == 0) { // Support more formats?
                  stbtt_int32 valueRecordPairSizeInBytes = 2;
                  stbtt_uint16 pairSetCount = ttUSHORT(table + 8);
                  stbtt_uint16 pairPosOffset = ttUSHORT(table + 10 + 2 * coverageIndex);
                  stbtt_uint8 *pairValueTable = table + pairPosOffset;
                  stbtt_uint16 pairValueCount = ttUSHORT(pairValueTable);
                  stbtt_uint8 *pairValueArray = pairValueTable + 2;

                  if (coverageIndex >= pairSetCount) return 0;

                  needle=glyph2;
                  r=pairValueCount-1;
                  l=0;

                  // Binary search.
                  while (l <= r) {
                     stbtt_uint16 secondGlyph;
                     stbtt_uint8 *pairValue;
                     m = (l + r) >> 1;
                     pairValue = pairValueArray + (2 + valueRecordPairSizeInBytes) * m;
                     secondGlyph = ttUSHORT(pairValue);
                     straw = secondGlyph;
                     if (needle < straw)
                        r = m - 1;
                     else if (needle > straw)
                        l = m + 1;
                     else {
                        stbtt_int16 xAdvance = ttSHORT(pairValue + 2);
                        return xAdvance;
                     }
                  }
               } else
                  return 0;
               break;
            }

            case 2: {
               stbtt_uint16 valueFormat1 = ttUSHORT(table + 4);
               stbtt_uint16 valueFormat2 = ttUSHORT(table + 6);
               if (valueFormat1 == 4 && valueFormat2 == 0) { // Support more formats?
                  stbtt_uint16 classDef1Offset = ttUSHORT(table + 8);
                  stbtt_uint16 classDef2Offset = ttUSHORT(table + 10);
                  int glyph1class = stbtt__GetGlyphClass(table + classDef1Offset, glyph1);
                  int glyph2class = stbtt__GetGlyphClass(table + classDef2Offset, glyph2);

                  stbtt_uint16 class1Count = ttUSHORT(table + 12);
                  stbtt_uint16 class2Count = ttUSHORT(table + 14);
                  stbtt_uint8 *class1Records, *class2Records;
                  stbtt_int16 xAdvance;

                  if (glyph1class < 0 || glyph1class >= class1Count) return 0; // malformed
                  if (glyph2class < 0 || glyph2class >= class2Count) return 0; // malformed

                  class1Records = table + 16;
                  class2Records = class1Records + 2 * (glyph1class * class2Count);
                  xAdvance = ttSHORT(class2Records + 2 * glyph2class);
                  return xAdvance;
               } else
                  return 0;
               break;
            }

            default:
               return 0; // Unsupported position format
         }
      }
   }

   return 0;
}

STBTT_DEF int  stbtt_GetGlyphKernAdvance(const stbtt_fontinfo *info, int g1, int g2)
{
   int xAdvance = 0;

   if (info->gpos)
      xAdvance += stbtt__GetGlyphGPOSInfoAdvance(info, g1, g2);
   else if (info->kern)
      xAdvance += stbtt__GetGlyphKernInfoAdvance(info, g1, g2);

   return xAdvance;
}

STBTT_DEF int  stbtt_GetCodepointKernAdvance(const stbtt_fontinfo *info, int ch1, int ch2)
{
   if (!info->kern && !info->gpos) // if no kerning table, don't waste time looking up both codepoint->glyphs
      return 0;
   return stbtt_GetGlyphKernAdvance(info, stbtt_FindGlyphIndex(info,ch1), stbtt_FindGlyphIndex(info,ch2));
}

STBTT_DEF void stbtt_GetCodepointHMetrics(const stbtt_fontinfo *info, int codepoint, int *advanceWidth, int *leftSideBearing)
{
   stbtt_GetGlyphHMetrics(info, stbtt_FindGlyphIndex(info,codepoint), advanceWidth, leftSideBearing);
}

STBTT_DEF void stbtt_GetFontVMetrics(const stbtt_fontinfo *info, int *ascent, int *descent, int *lineGap)
{
   if (ascent ) *ascent  = ttSHORT(info->data+info->hhea + 4);
   if (descent) *descent = ttSHORT(info->data+info->hhea + 6);
   if (lineGap) *lineGap = ttSHORT(info->data+info->hhea + 8);
}

STBTT_DEF int  stbtt_GetFontVMetricsOS2(const stbtt_fontinfo *info, int *typoAscent, int *typoDescent, int *typoLineGap)
{
   int tab = stbtt__find_table(info->data, info->fontstart, "OS/2");
   if (!tab)
      return 0;
   if (typoAscent ) *typoAscent  = ttSHORT(info->data+tab + 68);
   if (typoDescent) *typoDescent = ttSHORT(info->data+tab + 70);
   if (typoLineGap) *typoLineGap = ttSHORT(info->data+tab + 72);
   return 1;
}

STBTT_DEF void stbtt_GetFontBoundingBox(const stbtt_fontinfo *info, int *x0, int *y0, int *x1, int *y1)
{
   *x0 = ttSHORT(info->data + info->head + 36);
   *y0 = ttSHORT(info->data + info->head + 38);
   *x1 = ttSHORT(info->data + info->head + 40);
   *y1 = ttSHORT(info->data + info->head + 42);
}

STBTT_DEF float stbtt_ScaleForPixelHeight(const stbtt_fontinfo *info, float height)
{
   int fheight = ttSHORT(info->data + info->hhea + 4) - ttSHORT(info->data + info->hhea + 6);
   return (float) height / fheight;
}

STBTT_DEF float stbtt_ScaleForMappingEmToPixels(const stbtt_fontinfo *info, float pixels)
{
   int unitsPerEm = ttUSHORT(info->data + info->head + 18);
   return pixels / unitsPerEm;
}

STBTT_DEF void stbtt_FreeShape(const stbtt_fontinfo *info, stbtt_vertex *v)
{
   STBTT_free(v, info->userdata);
}

STBTT_DEF stbtt_uint8 *stbtt_FindSVGDoc(const stbtt_fontinfo *info, int gl)
{
   int i;
   stbtt_uint8 *data = info->data;
   stbtt_uint8 *svg_doc_list = data + stbtt__get_svg((stbtt_fontinfo *) info);

   int numEntries = ttUSHORT(svg_doc_list);
   stbtt_uint8 *svg_docs = svg_doc_list + 2;

   for(i=0; i<numEntries; i++) {
      stbtt_uint8 *svg_doc = svg_docs + (12 * i);
      if ((gl >= ttUSHORT(svg_doc)) && (gl <= ttUSHORT(svg_doc + 2)))
         return svg_doc;
   }
   return 0;
}

STBTT_DEF int stbtt_GetGlyphSVG(const stbtt_fontinfo *info, int gl, const char **svg)
{
   stbtt_uint8 *data = info->data;
   stbtt_uint8 *svg_doc;

   if (info->svg == 0)
      return 0;

   svg_doc = stbtt_FindSVGDoc(info, gl);
   if (svg_doc != NULL) {
      *svg = (char *) data + info->svg + ttULONG(svg_doc + 4);
      return ttULONG(svg_doc + 8);
   } else {
      return 0;
   }
}

STBTT_DEF int stbtt_GetCodepointSVG(const stbtt_fontinfo *info, int unicode_codepoint, const char **svg)
{
   return stbtt_GetGlyphSVG(info, stbtt_FindGlyphIndex(info, unicode_codepoint), svg);
}

//////////////////////////////////////////////////////////////////////////////
//
// antialiasing software rasterizer
//

STBTT_DEF void stbtt_GetGlyphBitmapBoxSubpixel(const stbtt_fontinfo *font, int glyph, float scale_x, float scale_y,float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1)
{
   int x0=0,y0=0,x1,y1; // =0 suppresses compiler warning
   if (!stbtt_GetGlyphBox(font, glyph, &x0,&y0,&x1,&y1)) {
      // e.g. space character
      if (ix0) *ix0 = 0;
      if (iy0) *iy0 = 0;
      if (ix1) *ix1 = 0;
      if (iy1) *iy1 = 0;
   } else {
      // move to integral bboxes (treating pixels as little squares, what pixels get touched)?
      if (ix0) *ix0 = STBTT_ifloor( x0 * scale_x + shift_x);
      if (iy0) *iy0 = STBTT_ifloor(-y1 * scale_y + shift_y);
      if (ix1) *ix1 = STBTT_iceil ( x1 * scale_x + shift_x);
      if (iy1) *iy1 = STBTT_iceil (-y0 * scale_y + shift_y);
   }
}

STBTT_DEF void stbtt_GetGlyphBitmapBox(const stbtt_fontinfo *font, int glyph, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1)
{
   stbtt_GetGlyphBitmapBoxSubpixel(font, glyph, scale_x, scale_y,0.0f,0.0f, ix0, iy0, ix1, iy1);
}

STBTT_DEF void stbtt_GetCodepointBitmapBoxSubpixel(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1)
{
   stbtt_GetGlyphBitmapBoxSubpixel(font, stbtt_FindGlyphIndex(font,codepoint), scale_x, scale_y,shift_x,shift_y, ix0,iy0,ix1,iy1);
}

STBTT_DEF void stbtt_GetCodepointBitmapBox(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1)
{
   stbtt_GetCodepointBitmapBoxSubpixel(font, codepoint, scale_x, scale_y,0.0f,0.0f, ix0,iy0,ix1,iy1);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Rasterizer

typedef struct stbtt__hheap_chunk
{
   struct stbtt__hheap_chunk *next;
} stbtt__hheap_chunk;

typedef struct stbtt__hheap
{
   struct stbtt__hheap_chunk *head;
   void   *first_free;
   int    num_remaining_in_head_chunk;
} stbtt__hheap;

static void *stbtt__hheap_alloc(stbtt__hheap *hh, size_t size, void *userdata)
{
   if (hh->first_free) {
      void *p = hh->first_free;
      hh->first_free = * (void **) p;
      return p;
   } else {
      if (hh->num_remaining_in_head_chunk == 0) {
         int count = (size < 32 ? 2000 : size < 128 ? 800 : 100);
         stbtt__hheap_chunk *c = (stbtt__hheap_chunk *) STBTT_malloc(sizeof(stbtt__hheap_chunk) + size * count, userdata);
         if (c == NULL)
            return NULL;
         c->next = hh->head;
         hh->head = c;
         hh->num_remaining_in_head_chunk = count;
      }
      --hh->num_remaining_in_head_chunk;
      return (char *) (hh->head) + sizeof(stbtt__hheap_chunk) + size * hh->num_remaining_in_head_chunk;
   }
}

static void stbtt__hheap_free(stbtt__hheap *hh, void *p)
{
   *(void **) p = hh->first_free;
   hh->first_free = p;
}

static void stbtt__hheap_cleanup(stbtt__hheap *hh, void *userdata)
{
   stbtt__hheap_chunk *c = hh->head;
   while (c) {
      stbtt__hheap_chunk *n = c->next;
      STBTT_free(c, userdata);
      c = n;
   }
}

typedef struct stbtt__edge {
   float x0,y0, x1,y1;
   int invert;
} stbtt__edge;


typedef struct stbtt__active_edge
{
   struct stbtt__active_edge *next;
   #if STBTT_RASTERIZER_VERSION==1
   int x,dx;
   float ey;
   int direction;
   #elif STBTT_RASTERIZER_VERSION==2
   float fx,fdx,fdy;
   float direction;
   float sy;
   float ey;
   #else
   #error "Unrecognized value of STBTT_RASTERIZER_VERSION"
   #endif
} stbtt__active_edge;

#if STBTT_RASTERIZER_VERSION == 1
#define STBTT_FIXSHIFT   10
#define STBTT_FIX        (1 << STBTT_FIXSHIFT)
#define STBTT_FIXMASK    (STBTT_FIX-1)

static stbtt__active_edge *stbtt__new_active(stbtt__hheap *hh, stbtt__edge *e, int off_x, float start_point, void *userdata)
{
   stbtt__active_edge *z = (stbtt__active_edge *) stbtt__hheap_alloc(hh, sizeof(*z), userdata);
   float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
   STBTT_assert(z != NULL);
   if (!z) return z;

   // round dx down to avoid overshooting
   if (dxdy < 0)
      z->dx = -STBTT_ifloor(STBTT_FIX * -dxdy);
   else
      z->dx = STBTT_ifloor(STBTT_FIX * dxdy);

   z->x = STBTT_ifloor(STBTT_FIX * e->x0 + z->dx * (start_point - e->y0)); // use z->dx so when we offset later it's by the same amount
   z->x -= off_x * STBTT_FIX;

   z->ey = e->y1;
   z->next = 0;
   z->direction = e->invert ? 1 : -1;
   return z;
}
#elif STBTT_RASTERIZER_VERSION == 2
static stbtt__active_edge *stbtt__new_active(stbtt__hheap *hh, stbtt__edge *e, int off_x, float start_point, void *userdata)
{
   stbtt__active_edge *z = (stbtt__active_edge *) stbtt__hheap_alloc(hh, sizeof(*z), userdata);
   float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
   STBTT_assert(z != NULL);
   //STBTT_assert(e->y0 <= start_point);
   if (!z) return z;
   z->fdx = dxdy;
   z->fdy = dxdy != 0.0f ? (1.0f/dxdy) : 0.0f;
   z->fx = e->x0 + dxdy * (start_point - e->y0);
   z->fx -= off_x;
   z->direction = e->invert ? 1.0f : -1.0f;
   z->sy = e->y0;
   z->ey = e->y1;
   z->next = 0;
   return z;
}
#else
#error "Unrecognized value of STBTT_RASTERIZER_VERSION"
#endif

#if STBTT_RASTERIZER_VERSION == 1
// note: this routine clips fills that extend off the edges... ideally this
// wouldn't happen, but it could happen if the truetype glyph bounding boxes
// are wrong, or if the user supplies a too-small bitmap
static void stbtt__fill_active_edges(unsigned char *scanline, int len, stbtt__active_edge *e, int max_weight)
{
   // non-zero winding fill
   int x0=0, w=0;

   while (e) {
      if (w == 0) {
         // if we're currently at zero, we need to record the edge start point
         x0 = e->x; w += e->direction;
      } else {
         int x1 = e->x; w += e->direction;
         // if we went to zero, we need to draw
         if (w == 0) {
            int i = x0 >> STBTT_FIXSHIFT;
            int j = x1 >> STBTT_FIXSHIFT;

            if (i < len && j >= 0) {
               if (i == j) {
                  // x0,x1 are the same pixel, so compute combined coverage
                  scanline[i] = scanline[i] + (stbtt_uint8) ((x1 - x0) * max_weight >> STBTT_FIXSHIFT);
               } else {
                  if (i >= 0) // add antialiasing for x0
                     scanline[i] = scanline[i] + (stbtt_uint8) (((STBTT_FIX - (x0 & STBTT_FIXMASK)) * max_weight) >> STBTT_FIXSHIFT);
                  else
                     i = -1; // clip

                  if (j < len) // add antialiasing for x1
                     scanline[j] = scanline[j] + (stbtt_uint8) (((x1 & STBTT_FIXMASK) * max_weight) >> STBTT_FIXSHIFT);
                  else
                     j = len; // clip

                  for (++i; i < j; ++i) // fill pixels between x0 and x1
                     scanline[i] = scanline[i] + (stbtt_uint8) max_weight;
               }
            }
         }
      }

      e = e->next;
   }
}

static void stbtt__rasterize_sorted_edges(stbtt__bitmap *result, stbtt__edge *e, int n, int vsubsample, int off_x, int off_y, void *userdata)
{
   stbtt__hheap hh = { 0, 0, 0 };
   stbtt__active_edge *active = NULL;
   int y,j=0;
   int max_weight = (255 / vsubsample);  // weight per vertical scanline
   int s; // vertical subsample index
   unsigned char scanline_data[512], *scanline;

   if (result->w > 512)
      scanline = (unsigned char *) STBTT_malloc(result->w, userdata);
   else
      scanline = scanline_data;

   y = off_y * vsubsample;
   e[n].y0 = (off_y + result->h) * (float) vsubsample + 1;

   while (j < result->h) {
      STBTT_memset(scanline, 0, result->w);
      for (s=0; s < vsubsample; ++s) {
         // find center of pixel for this scanline
         float scan_y = y + 0.5f;
         stbtt__active_edge **step = &active;

         // update all active edges;
         // remove all active edges that terminate before the center of this scanline
         while (*step) {
            stbtt__active_edge * z = *step;
            if (z->ey <= scan_y) {
               *step = z->next; // delete from list
               STBTT_assert(z->direction);
               z->direction = 0;
               stbtt__hheap_free(&hh, z);
            } else {
               z->x += z->dx; // advance to position for current scanline
               step = &((*step)->next); // advance through list
            }
         }

         // resort the list if needed
         for(;;) {
            int changed=0;
            step = &active;
            while (*step && (*step)->next) {
               if ((*step)->x > (*step)->next->x) {
                  stbtt__active_edge *t = *step;
                  stbtt__active_edge *q = t->next;

                  t->next = q->next;
                  q->next = t;
                  *step = q;
                  changed = 1;
               }
               step = &(*step)->next;
            }
            if (!changed) break;
         }

         // insert all edges that start before the center of this scanline -- omit ones that also end on this scanline
         while (e->y0 <= scan_y) {
            if (e->y1 > scan_y) {
               stbtt__active_edge *z = stbtt__new_active(&hh, e, off_x, scan_y, userdata);
               if (z != NULL) {
                  // find insertion point
                  if (active == NULL)
                     active = z;
                  else if (z->x < active->x) {
                     // insert at front
                     z->next = active;
                     active = z;
                  } else {
                     // find thing to insert AFTER
                     stbtt__active_edge *p = active;
                     while (p->next && p->next->x < z->x)
                        p = p->next;
                     // at this point, p->next->x is NOT < z->x
                     z->next = p->next;
                     p->next = z;
                  }
               }
            }
            ++e;
         }

         // now process all active edges in XOR fashion
         if (active)
            stbtt__fill_active_edges(scanline, result->w, active, max_weight);

         ++y;
      }
      STBTT_memcpy(result->pixels + j * result->stride, scanline, result->w);
      ++j;
   }

   stbtt__hheap_cleanup(&hh, userdata);

   if (scanline != scanline_data)
      STBTT_free(scanline, userdata);
}

#elif STBTT_RASTERIZER_VERSION == 2

// the edge passed in here does not cross the vertical line at x or the vertical line at x+1
// (i.e. it has already been clipped to those)
static void stbtt__handle_clipped_edge(float *scanline, int x, stbtt__active_edge *e, float x0, float y0, float x1, float y1)
{
   if (y0 == y1) return;
   STBTT_assert(y0 < y1);
   STBTT_assert(e->sy <= e->ey);
   if (y0 > e->ey) return;
   if (y1 < e->sy) return;
   if (y0 < e->sy) {
      x0 += (x1-x0) * (e->sy - y0) / (y1-y0);
      y0 = e->sy;
   }
   if (y1 > e->ey) {
      x1 += (x1-x0) * (e->ey - y1) / (y1-y0);
      y1 = e->ey;
   }

   if (x0 == x)
      STBTT_assert(x1 <= x+1);
   else if (x0 == x+1)
      STBTT_assert(x1 >= x);
   else if (x0 <= x)
      STBTT_assert(x1 <= x);
   else if (x0 >= x+1)
      STBTT_assert(x1 >= x+1);
   else
      STBTT_assert(x1 >= x && x1 <= x+1);

   if (x0 <= x && x1 <= x)
      scanline[x] += e->direction * (y1-y0);
   else if (x0 >= x+1 && x1 >= x+1)
      ;
   else {
      STBTT_assert(x0 >= x && x0 <= x+1 && x1 >= x && x1 <= x+1);
      scanline[x] += e->direction * (y1-y0) * (1-((x0-x)+(x1-x))/2); // coverage = 1 - average x position
   }
}

static float stbtt__sized_trapezoid_area(float height, float top_width, float bottom_width)
{
   STBTT_assert(top_width >= 0);
   STBTT_assert(bottom_width >= 0);
   return (top_width + bottom_width) / 2.0f * height;
}

static float stbtt__position_trapezoid_area(float height, float tx0, float tx1, float bx0, float bx1)
{
   return stbtt__sized_trapezoid_area(height, tx1 - tx0, bx1 - bx0);
}

static float stbtt__sized_triangle_area(float height, float width)
{
   return height * width / 2;
}

static void stbtt__fill_active_edges_new(float *scanline, float *scanline_fill, int len, stbtt__active_edge *e, float y_top)
{
   float y_bottom = y_top+1;

   while (e) {
      // brute force every pixel

      // compute intersection points with top & bottom
      STBTT_assert(e->ey >= y_top);

      if (e->fdx == 0) {
         float x0 = e->fx;
         if (x0 < len) {
            if (x0 >= 0) {
               stbtt__handle_clipped_edge(scanline,(int) x0,e, x0,y_top, x0,y_bottom);
               stbtt__handle_clipped_edge(scanline_fill-1,(int) x0+1,e, x0,y_top, x0,y_bottom);
            } else {
               stbtt__handle_clipped_edge(scanline_fill-1,0,e, x0,y_top, x0,y_bottom);
            }
         }
      } else {
         float x0 = e->fx;
         float dx = e->fdx;
         float xb = x0 + dx;
         float x_top, x_bottom;
         float sy0,sy1;
         float dy = e->fdy;
         STBTT_assert(e->sy <= y_bottom && e->ey >= y_top);

         // compute endpoints of line segment clipped to this scanline (if the
         // line segment starts on this scanline. x0 is the intersection of the
         // line with y_top, but that may be off the line segment.
         if (e->sy > y_top) {
            x_top = x0 + dx * (e->sy - y_top);
            sy0 = e->sy;
         } else {
            x_top = x0;
            sy0 = y_top;
         }
         if (e->ey < y_bottom) {
            x_bottom = x0 + dx * (e->ey - y_top);
            sy1 = e->ey;
         } else {
            x_bottom = xb;
            sy1 = y_bottom;
         }

         if (x_top >= 0 && x_bottom >= 0 && x_top < len && x_bottom < len) {
            // from here on, we don't have to range check x values

            if ((int) x_top == (int) x_bottom) {
               float height;
               // simple case, only spans one pixel
               int x = (int) x_top;
               height = (sy1 - sy0) * e->direction;
               STBTT_assert(x >= 0 && x < len);
               scanline[x]      += stbtt__position_trapezoid_area(height, x_top, x+1.0f, x_bottom, x+1.0f);
               scanline_fill[x] += height; // everything right of this pixel is filled
            } else {
               int x,x1,x2;
               float y_crossing, y_final, step, sign, area;
               // covers 2+ pixels
               if (x_top > x_bottom) {
                  // flip scanline vertically; signed area is the same
                  float t;
                  sy0 = y_bottom - (sy0 - y_top);
                  sy1 = y_bottom - (sy1 - y_top);
                  t = sy0, sy0 = sy1, sy1 = t;
                  t = x_bottom, x_bottom = x_top, x_top = t;
                  dx = -dx;
                  dy = -dy;
                  t = x0, x0 = xb, xb = t;
               }
               STBTT_assert(dy >= 0);
               STBTT_assert(dx >= 0);

               x1 = (int) x_top;
               x2 = (int) x_bottom;
               // compute intersection with y axis at x1+1
               y_crossing = y_top + dy * (x1+1 - x0);

               // compute intersection with y axis at x2
               y_final = y_top + dy * (x2 - x0);

               //           x1    x_top                            x2    x_bottom
               //     y_top  +------|-----+------------+------------+--------|---+------------+
               //            |            |            |            |            |            |
               //            |            |            |            |            |            |
               //       sy0  |      Txxxxx|............|............|............|............|
               // y_crossing |            *xxxxx.......|............|............|............|
               //            |            |     xxxxx..|............|............|............|
               //            |            |     /-   xx*xxxx........|............|............|
               //            |            | dy <       |    xxxxxx..|............|............|
               //   y_final  |            |     \-     |          xx*xxx.........|............|
               //       sy1  |            |            |            |   xxxxxB...|............|
               //            |            |            |            |            |            |
               //            |            |            |            |            |            |
               //  y_bottom  +------------+------------+------------+------------+------------+
               //
               // goal is to measure the area covered by '.' in each pixel

               // if x2 is right at the right edge of x1, y_crossing can blow up, github #1057
               // @TODO: maybe test against sy1 rather than y_bottom?
               if (y_crossing > y_bottom)
                  y_crossing = y_bottom;

               sign = e->direction;

               // area of the rectangle covered from sy0..y_crossing
               area = sign * (y_crossing-sy0);

               // area of the triangle (x_top,sy0), (x1+1,sy0), (x1+1,y_crossing)
               scanline[x1] += stbtt__sized_triangle_area(area, x1+1 - x_top);

               // check if final y_crossing is blown up; no test case for this
               if (y_final > y_bottom) {
                  y_final = y_bottom;
                  dy = (y_final - y_crossing ) / (x2 - (x1+1)); // if denom=0, y_final = y_crossing, so y_final <= y_bottom
               }

               // in second pixel, area covered by line segment found in first pixel
               // is always a rectangle 1 wide * the height of that line segment; this
               // is exactly what the variable 'area' stores. it also gets a contribution
               // from the line segment within it. the THIRD pixel will get the first
               // pixel's rectangle contribution, the second pixel's rectangle contribution,
               // and its own contribution. the 'own contribution' is the same in every pixel except
               // the leftmost and rightmost, a trapezoid that slides down in each pixel.
               // the second pixel's contribution to the third pixel will be the
               // rectangle 1 wide times the height change in the second pixel, which is dy.

               step = sign * dy * 1; // dy is dy/dx, change in y for every 1 change in x,
               // which multiplied by 1-pixel-width is how much pixel area changes for each step in x
               // so the area advances by 'step' every time

               for (x = x1+1; x < x2; ++x) {
                  scanline[x] += area + step/2; // area of trapezoid is 1*step/2
                  area += step;
               }
               STBTT_assert(STBTT_fabs(area) <= 1.01f); // accumulated error from area += step unless we round step down
               STBTT_assert(sy1 > y_final-0.01f);

               // area covered in the last pixel is the rectangle from all the pixels to the left,
               // plus the trapezoid filled by the line segment in this pixel all the way to the right edge
               scanline[x2] += area + sign * stbtt__position_trapezoid_area(sy1-y_final, (float) x2, x2+1.0f, x_bottom, x2+1.0f);

               // the rest of the line is filled based on the total height of the line segment in this pixel
               scanline_fill[x2] += sign * (sy1-sy0);
            }
         } else {
            // if edge goes outside of box we're drawing, we require
            // clipping logic. since this does not match the intended use
            // of this library, we use a different, very slow brute
            // force implementation
            // note though that this does happen some of the time because
            // x_top and x_bottom can be extrapolated at the top & bottom of
            // the shape and actually lie outside the bounding box
            int x;
            for (x=0; x < len; ++x) {
               // cases:
               //
               // there can be up to two intersections with the pixel. any intersection
               // with left or right edges can be handled by splitting into two (or three)
               // regions. intersections with top & bottom do not necessitate case-wise logic.
               //
               // the old way of doing this found the intersections with the left & right edges,
               // then used some simple logic to produce up to three segments in sorted order
               // from top-to-bottom. however, this had a problem: if an x edge was epsilon
               // across the x border, then the corresponding y position might not be distinct
               // from the other y segment, and it might ignored as an empty segment. to avoid
               // that, we need to explicitly produce segments based on x positions.

               // rename variables to clearly-defined pairs
               float y0 = y_top;
               float x1 = (float) (x);
               float x2 = (float) (x+1);
               float x3 = xb;
               float y3 = y_bottom;

               // x = e->x + e->dx * (y-y_top)
               // (y-y_top) = (x - e->x) / e->dx
               // y = (x - e->x) / e->dx + y_top
               float y1 = (x - x0) / dx + y_top;
               float y2 = (x+1 - x0) / dx + y_top;

               if (x0 < x1 && x3 > x2) {         // three segments descending down-right
                  stbtt__handle_clipped_edge(scanline,x,e, x0,y0, x1,y1);
                  stbtt__handle_clipped_edge(scanline,x,e, x1,y1, x2,y2);
                  stbtt__handle_clipped_edge(scanline,x,e, x2,y2, x3,y3);
               } else if (x3 < x1 && x0 > x2) {  // three segments descending down-left
                  stbtt__handle_clipped_edge(scanline,x,e, x0,y0, x2,y2);
                  stbtt__handle_clipped_edge(scanline,x,e, x2,y2, x1,y1);
                  stbtt__handle_clipped_edge(scanline,x,e, x1,y1, x3,y3);
               } else if (x0 < x1 && x3 > x1) {  // two segments across x, down-right
                  stbtt__handle_clipped_edge(scanline,x,e, x0,y0, x1,y1);
                  stbtt__handle_clipped_edge(scanline,x,e, x1,y1, x3,y3);
               } else if (x3 < x1 && x0 > x1) {  // two segments across x, down-left
                  stbtt__handle_clipped_edge(scanline,x,e, x0,y0, x1,y1);
                  stbtt__handle_clipped_edge(scanline,x,e, x1,y1, x3,y3);
               } else if (x0 < x2 && x3 > x2) {  // two segments across x+1, down-right
                  stbtt__handle_clipped_edge(scanline,x,e, x0,y0, x2,y2);
                  stbtt__handle_clipped_edge(scanline,x,e, x2,y2, x3,y3);
               } else if (x3 < x2 && x0 > x2) {  // two segments across x+1, down-left
                  stbtt__handle_clipped_edge(scanline,x,e, x0,y0, x2,y2);
                  stbtt__handle_clipped_edge(scanline,x,e, x2,y2, x3,y3);
               } else {  // one segment
                  stbtt__handle_clipped_edge(scanline,x,e, x0,y0, x3,y3);
               }
            }
         }
      }
      e = e->next;
   }
}

// directly AA rasterize edges w/o supersampling
static void stbtt__rasterize_sorted_edges(stbtt__bitmap *result, stbtt__edge *e, int n, int vsubsample, int off_x, int off_y, void *userdata)
{
   stbtt__hheap hh = { 0, 0, 0 };
   stbtt__active_edge *active = NULL;
   int y,j=0, i;
   float scanline_data[129], *scanline, *scanline2;

   STBTT__NOTUSED(vsubsample);

   if (result->w > 64)
      scanline = (float *) STBTT_malloc((result->w*2+1) * sizeof(float), userdata);
   else
      scanline = scanline_data;

   scanline2 = scanline + result->w;

   y = off_y;
   e[n].y0 = (float) (off_y + result->h) + 1;

   while (j < result->h) {
      // find center of pixel for this scanline
      float scan_y_top    = y + 0.0f;
      float scan_y_bottom = y + 1.0f;
      stbtt__active_edge **step = &active;

      STBTT_memset(scanline , 0, result->w*sizeof(scanline[0]));
      STBTT_memset(scanline2, 0, (result->w+1)*sizeof(scanline[0]));

      // update all active edges;
      // remove all active edges that terminate before the top of this scanline
      while (*step) {
         stbtt__active_edge * z = *step;
         if (z->ey <= scan_y_top) {
            *step = z->next; // delete from list
            STBTT_assert(z->direction);
            z->direction = 0;
            stbtt__hheap_free(&hh, z);
         } else {
            step = &((*step)->next); // advance through list
         }
      }

      // insert all edges that start before the bottom of this scanline
      while (e->y0 <= scan_y_bottom) {
         if (e->y0 != e->y1) {
            stbtt__active_edge *z = stbtt__new_active(&hh, e, off_x, scan_y_top, userdata);
            if (z != NULL) {
               if (j == 0 && off_y != 0) {
                  if (z->ey < scan_y_top) {
                     // this can happen due to subpixel positioning and some kind of fp rounding error i think
                     z->ey = scan_y_top;
                  }
               }
               STBTT_assert(z->ey >= scan_y_top); // if we get really unlucky a tiny bit of an edge can be out of bounds
               // insert at front
               z->next = active;
               active = z;
            }
         }
         ++e;
      }

      // now process all active edges
      if (active)
         stbtt__fill_active_edges_new(scanline, scanline2+1, result->w, active, scan_y_top);

      {
         float sum = 0;
         for (i=0; i < result->w; ++i) {
            float k;
            int m;
            sum += scanline2[i];
            k = scanline[i] + sum;
            k = (float) STBTT_fabs(k)*255 + 0.5f;
            m = (int) k;
            if (m > 255) m = 255;
            result->pixels[j*result->stride + i] = (unsigned char) m;
         }
      }
      // advance all the edges
      step = &active;
      while (*step) {
         stbtt__active_edge *z = *step;
         z->fx += z->fdx; // advance to position for current scanline
         step = &((*step)->next); // advance through list
      }

      ++y;
      ++j;
   }

   stbtt__hheap_cleanup(&hh, userdata);

   if (scanline != scanline_data)
      STBTT_free(scanline, userdata);
}
#else
#error "Unrecognized value of STBTT_RASTERIZER_VERSION"
#endif

#define STBTT__COMPARE(a,b)  ((a)->y0 < (b)->y0)

static void stbtt__sort_edges_ins_sort(stbtt__edge *p, int n)
{
   int i,j;
   for (i=1; i < n; ++i) {
      stbtt__edge t = p[i], *a = &t;
      j = i;
      while (j > 0) {
         stbtt__edge *b = &p[j-1];
         int c = STBTT__COMPARE(a,b);
         if (!c) break;
         p[j] = p[j-1];
         --j;
      }
      if (i != j)
         p[j] = t;
   }
}

static void stbtt__sort_edges_quicksort(stbtt__edge *p, int n)
{
   /* threshold for transitioning to insertion sort */
   while (n > 12) {
      stbtt__edge t;
      int c01,c12,c,m,i,j;

      /* compute median of three */
      m = n >> 1;
      c01 = STBTT__COMPARE(&p[0],&p[m]);
      c12 = STBTT__COMPARE(&p[m],&p[n-1]);
      /* if 0 >= mid >= end, or 0 < mid < end, then use mid */
      if (c01 != c12) {
         /* otherwise, we'll need to swap something else to middle */
         int z;
         c = STBTT__COMPARE(&p[0],&p[n-1]);
         /* 0>mid && mid<n:  0>n => n; 0<n => 0 */
         /* 0<mid && mid>n:  0>n => 0; 0<n => n */
         z = (c == c12) ? 0 : n-1;
         t = p[z];
         p[z] = p[m];
         p[m] = t;
      }
      /* now p[m] is the median-of-three */
      /* swap it to the beginning so it won't move around */
      t = p[0];
      p[0] = p[m];
      p[m] = t;

      /* partition loop */
      i=1;
      j=n-1;
      for(;;) {
         /* handling of equality is crucial here */
         /* for sentinels & efficiency with duplicates */
         for (;;++i) {
            if (!STBTT__COMPARE(&p[i], &p[0])) break;
         }
         for (;;--j) {
            if (!STBTT__COMPARE(&p[0], &p[j])) break;
         }
         /* make sure we haven't crossed */
         if (i >= j) break;
         t = p[i];
         p[i] = p[j];
         p[j] = t;

         ++i;
         --j;
      }
      /* recurse on smaller side, iterate on larger */
      if (j < (n-i)) {
         stbtt__sort_edges_quicksort(p,j);
         p = p+i;
         n = n-i;
      } else {
         stbtt__sort_edges_quicksort(p+i, n-i);
         n = j;
      }
   }
}

static void stbtt__sort_edges(stbtt__edge *p, int n)
{
   stbtt__sort_edges_quicksort(p, n);
   stbtt__sort_edges_ins_sort(p, n);
}

typedef struct
{
   float x,y;
} stbtt__point;

static void stbtt__rasterize(stbtt__bitmap *result, stbtt__point *pts, int *wcount, int windings, float scale_x, float scale_y, float shift_x, float shift_y, int off_x, int off_y, int invert, void *userdata)
{
   float y_scale_inv = invert ? -scale_y : scale_y;
   stbtt__edge *e;
   int n,i,j,k,m;
#if STBTT_RASTERIZER_VERSION == 1
   int vsubsample = result->h < 8 ? 15 : 5;
#elif STBTT_RASTERIZER_VERSION == 2
   int vsubsample = 1;
#else
   #error "Unrecognized value of STBTT_RASTERIZER_VERSION"
#endif
   // vsubsample should divide 255 evenly; otherwise we won't reach full opacity

   // now we have to blow out the windings into explicit edge lists
   n = 0;
   for (i=0; i < windings; ++i)
      n += wcount[i];

   e = (stbtt__edge *) STBTT_malloc(sizeof(*e) * (n+1), userdata); // add an extra one as a sentinel
   if (e == 0) return;
   n = 0;

   m=0;
   for (i=0; i < windings; ++i) {
      stbtt__point *p = pts + m;
      m += wcount[i];
      j = wcount[i]-1;
      for (k=0; k < wcount[i]; j=k++) {
         int a=k,b=j;
         // skip the edge if horizontal
         if (p[j].y == p[k].y)
            continue;
         // add edge from j to k to the list
         e[n].invert = 0;
         if (invert ? p[j].y > p[k].y : p[j].y < p[k].y) {
            e[n].invert = 1;
            a=j,b=k;
         }
         e[n].x0 = p[a].x * scale_x + shift_x;
         e[n].y0 = (p[a].y * y_scale_inv + shift_y) * vsubsample;
         e[n].x1 = p[b].x * scale_x + shift_x;
         e[n].y1 = (p[b].y * y_scale_inv + shift_y) * vsubsample;
         ++n;
      }
   }

   // now sort the edges by their highest point (should snap to integer, and then by x)
   //STBTT_sort(e, n, sizeof(e[0]), stbtt__edge_compare);
   stbtt__sort_edges(e, n);

   // now, traverse the scanlines and find the intersections on each scanline, use xor winding rule
   stbtt__rasterize_sorted_edges(result, e, n, vsubsample, off_x, off_y, userdata);

   STBTT_free(e, userdata);
}

static void stbtt__add_point(stbtt__point *points, int n, float x, float y)
{
   if (!points) return; // during first pass, it's unallocated
   points[n].x = x;
   points[n].y = y;
}

// tessellate until threshold p is happy... @TODO warped to compensate for non-linear stretching
static int stbtt__tesselate_curve(stbtt__point *points, int *num_points, float x0, float y0, float x1, float y1, float x2, float y2, float objspace_flatness_squared, int n)
{
   // midpoint
   float mx = (x0 + 2*x1 + x2)/4;
   float my = (y0 + 2*y1 + y2)/4;
   // versus directly drawn line
   float dx = (x0+x2)/2 - mx;
   float dy = (y0+y2)/2 - my;
   if (n > 16) // 65536 segments on one curve better be enough!
      return 1;
   if (dx*dx+dy*dy > objspace_flatness_squared) { // half-pixel error allowed... need to be smaller if AA
      stbtt__tesselate_curve(points, num_points, x0,y0, (x0+x1)/2.0f,(y0+y1)/2.0f, mx,my, objspace_flatness_squared,n+1);
      stbtt__tesselate_curve(points, num_points, mx,my, (x1+x2)/2.0f,(y1+y2)/2.0f, x2,y2, objspace_flatness_squared,n+1);
   } else {
      stbtt__add_point(points, *num_points,x2,y2);
      *num_points = *num_points+1;
   }
   return 1;
}

static void stbtt__tesselate_cubic(stbtt__point *points, int *num_points, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float objspace_flatness_squared, int n)
{
   // @TODO this "flatness" calculation is just made-up nonsense that seems to work well enough
   float dx0 = x1-x0;
   float dy0 = y1-y0;
   float dx1 = x2-x1;
   float dy1 = y2-y1;
   float dx2 = x3-x2;
   float dy2 = y3-y2;
   float dx = x3-x0;
   float dy = y3-y0;
   float longlen = (float) (STBTT_sqrt(dx0*dx0+dy0*dy0)+STBTT_sqrt(dx1*dx1+dy1*dy1)+STBTT_sqrt(dx2*dx2+dy2*dy2));
   float shortlen = (float) STBTT_sqrt(dx*dx+dy*dy);
   float flatness_squared = longlen*longlen-shortlen*shortlen;

   if (n > 16) // 65536 segments on one curve better be enough!
      return;

   if (flatness_squared > objspace_flatness_squared) {
      float x01 = (x0+x1)/2;
      float y01 = (y0+y1)/2;
      float x12 = (x1+x2)/2;
      float y12 = (y1+y2)/2;
      float x23 = (x2+x3)/2;
      float y23 = (y2+y3)/2;

      float xa = (x01+x12)/2;
      float ya = (y01+y12)/2;
      float xb = (x12+x23)/2;
      float yb = (y12+y23)/2;

      float mx = (xa+xb)/2;
      float my = (ya+yb)/2;

      stbtt__tesselate_cubic(points, num_points, x0,y0, x01,y01, xa,ya, mx,my, objspace_flatness_squared,n+1);
      stbtt__tesselate_cubic(points, num_points, mx,my, xb,yb, x23,y23, x3,y3, objspace_flatness_squared,n+1);
   } else {
      stbtt__add_point(points, *num_points,x3,y3);
      *num_points = *num_points+1;
   }
}

// returns number of contours
static stbtt__point *stbtt_FlattenCurves(stbtt_vertex *vertices, int num_verts, float objspace_flatness, int **contour_lengths, int *num_contours, void *userdata)
{
   stbtt__point *points=0;
   int num_points=0;

   float objspace_flatness_squared = objspace_flatness * objspace_flatness;
   int i,n=0,start=0, pass;

   // count how many "moves" there are to get the contour count
   for (i=0; i < num_verts; ++i)
      if (vertices[i].type == STBTT_vmove)
         ++n;

   *num_contours = n;
   if (n == 0) return 0;

   *contour_lengths = (int *) STBTT_malloc(sizeof(**contour_lengths) * n, userdata);

   if (*contour_lengths == 0) {
      *num_contours = 0;
      return 0;
   }

   // make two passes through the points so we don't need to realloc
   for (pass=0; pass < 2; ++pass) {
      float x=0,y=0;
      if (pass == 1) {
         points = (stbtt__point *) STBTT_malloc(num_points * sizeof(points[0]), userdata);
         if (points == NULL) goto error;
      }
      num_points = 0;
      n= -1;
      for (i=0; i < num_verts; ++i) {
         switch (vertices[i].type) {
            case STBTT_vmove:
               // start the next contour
               if (n >= 0)
                  (*contour_lengths)[n] = num_points - start;
               ++n;
               start = num_points;

               x = vertices[i].x, y = vertices[i].y;
               stbtt__add_point(points, num_points++, x,y);
               break;
            case STBTT_vline:
               x = vertices[i].x, y = vertices[i].y;
               stbtt__add_point(points, num_points++, x, y);
               break;
            case STBTT_vcurve:
               stbtt__tesselate_curve(points, &num_points, x,y,
                                        vertices[i].cx, vertices[i].cy,
                                        vertices[i].x,  vertices[i].y,
                                        objspace_flatness_squared, 0);
               x = vertices[i].x, y = vertices[i].y;
               break;
            case STBTT_vcubic:
               stbtt__tesselate_cubic(points, &num_points, x,y,
                                        vertices[i].cx, vertices[i].cy,
                                        vertices[i].cx1, vertices[i].cy1,
                                        vertices[i].x,  vertices[i].y,
                                        objspace_flatness_squared, 0);
               x = vertices[i].x, y = vertices[i].y;
               break;
         }
      }
      (*contour_lengths)[n] = num_points - start;
   }

   return points;
error:
   STBTT_free(points, userdata);
   STBTT_free(*contour_lengths, userdata);
   *contour_lengths = 0;
   *num_contours = 0;
   return NULL;
}

STBTT_DEF void stbtt_Rasterize(stbtt__bitmap *result, float flatness_in_pixels, stbtt_vertex *vertices, int num_verts, float scale_x, float scale_y, float shift_x, float shift_y, int x_off, int y_off, int invert, void *userdata)
{
   float scale            = scale_x > scale_y ? scale_y : scale_x;
   int winding_count      = 0;
   int *winding_lengths   = NULL;
   stbtt__point *windings = stbtt_FlattenCurves(vertices, num_verts, flatness_in_pixels / scale, &winding_lengths, &winding_count, userdata);
   if (windings) {
      stbtt__rasterize(result, windings, winding_lengths, winding_count, scale_x, scale_y, shift_x, shift_y, x_off, y_off, invert, userdata);
      STBTT_free(winding_lengths, userdata);
      STBTT_free(windings, userdata);
   }
}

STBTT_DEF void stbtt_FreeBitmap(unsigned char *bitmap, void *userdata)
{
   STBTT_free(bitmap, userdata);
}

STBTT_DEF unsigned char *stbtt_GetGlyphBitmapSubpixel(const stbtt_fontinfo *info, float scale_x, float scale_y, float shift_x, float shift_y, int glyph, int *width, int *height, int *xoff, int *yoff)
{
   int ix0,iy0,ix1,iy1;
   stbtt__bitmap gbm;
   stbtt_vertex *vertices;
   int num_verts = stbtt_GetGlyphShape(info, glyph, &vertices);

   if (scale_x == 0) scale_x = scale_y;
   if (scale_y == 0) {
      if (scale_x == 0) {
         STBTT_free(vertices, info->userdata);
         return NULL;
      }
      scale_y = scale_x;
   }

   stbtt_GetGlyphBitmapBoxSubpixel(info, glyph, scale_x, scale_y, shift_x, shift_y, &ix0,&iy0,&ix1,&iy1);

   // now we get the size
   gbm.w = (ix1 - ix0);
   gbm.h = (iy1 - iy0);
   gbm.pixels = NULL; // in case we error

   if (width ) *width  = gbm.w;
   if (height) *height = gbm.h;
   if (xoff  ) *xoff   = ix0;
   if (yoff  ) *yoff   = iy0;

   if (gbm.w && gbm.h) {
      gbm.pixels = (unsigned char *) STBTT_malloc(gbm.w * gbm.h, info->userdata);
      if (gbm.pixels) {
         gbm.stride = gbm.w;

         stbtt_Rasterize(&gbm, 0.35f, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1, info->userdata);
      }
   }
   STBTT_free(vertices, info->userdata);
   return gbm.pixels;
}

STBTT_DEF unsigned char *stbtt_GetGlyphBitmap(const stbtt_fontinfo *info, float scale_x, float scale_y, int glyph, int *width, int *height, int *xoff, int *yoff)
{
   return stbtt_GetGlyphBitmapSubpixel(info, scale_x, scale_y, 0.0f, 0.0f, glyph, width, height, xoff, yoff);
}

STBTT_DEF void stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph)
{
   int ix0,iy0;
   stbtt_vertex *vertices;
   int num_verts = stbtt_GetGlyphShape(info, glyph, &vertices);
   stbtt__bitmap gbm;

   stbtt_GetGlyphBitmapBoxSubpixel(info, glyph, scale_x, scale_y, shift_x, shift_y, &ix0,&iy0,0,0);
   gbm.pixels = output;
   gbm.w = out_w;
   gbm.h = out_h;
   gbm.stride = out_stride;

   if (gbm.w && gbm.h)
      stbtt_Rasterize(&gbm, 0.35f, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0,iy0, 1, info->userdata);

   STBTT_free(vertices, info->userdata);
}

STBTT_DEF void stbtt_MakeGlyphBitmap(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int glyph)
{
   stbtt_MakeGlyphBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, 0.0f,0.0f, glyph);
}

STBTT_DEF unsigned char *stbtt_GetCodepointBitmapSubpixel(const stbtt_fontinfo *info, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint, int *width, int *height, int *xoff, int *yoff)
{
   return stbtt_GetGlyphBitmapSubpixel(info, scale_x, scale_y,shift_x,shift_y, stbtt_FindGlyphIndex(info,codepoint), width,height,xoff,yoff);
}

STBTT_DEF void stbtt_MakeCodepointBitmapSubpixelPrefilter(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x, int oversample_y, float *sub_x, float *sub_y, int codepoint)
{
   stbtt_MakeGlyphBitmapSubpixelPrefilter(info, output, out_w, out_h, out_stride, scale_x, scale_y, shift_x, shift_y, oversample_x, oversample_y, sub_x, sub_y, stbtt_FindGlyphIndex(info,codepoint));
}

STBTT_DEF void stbtt_MakeCodepointBitmapSubpixel(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint)
{
   stbtt_MakeGlyphBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, shift_x, shift_y, stbtt_FindGlyphIndex(info,codepoint));
}

STBTT_DEF unsigned char *stbtt_GetCodepointBitmap(const stbtt_fontinfo *info, float scale_x, float scale_y, int codepoint, int *width, int *height, int *xoff, int *yoff)
{
   return stbtt_GetCodepointBitmapSubpixel(info, scale_x, scale_y, 0.0f,0.0f, codepoint, width,height,xoff,yoff);
}

STBTT_DEF void stbtt_MakeCodepointBitmap(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int codepoint)
{
   stbtt_MakeCodepointBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, 0.0f,0.0f, codepoint);
}

//////////////////////////////////////////////////////////////////////////////
//
// bitmap baking
//
// This is SUPER-CRAPPY packing to keep source code small

static int stbtt_BakeFontBitmap_internal(unsigned char *data, int offset,  // font location (use offset=0 for plain .ttf)
                                float pixel_height,                     // height of font in pixels
                                unsigned char *pixels, int pw, int ph,  // bitmap to be filled in
                                int first_char, int num_chars,          // characters to bake
                                stbtt_bakedchar *chardata)
{
   float scale;
   int x,y,bottom_y, i;
   stbtt_fontinfo f;
   f.userdata = NULL;
   if (!stbtt_InitFont(&f, data, offset))
      return -1;
   STBTT_memset(pixels, 0, pw*ph); // background of 0 around pixels
   x=y=1;
   bottom_y = 1;

   scale = stbtt_ScaleForPixelHeight(&f, pixel_height);

   for (i=0; i < num_chars; ++i) {
      int advance, lsb, x0,y0,x1,y1,gw,gh;
      int g = stbtt_FindGlyphIndex(&f, first_char + i);
      stbtt_GetGlyphHMetrics(&f, g, &advance, &lsb);
      stbtt_GetGlyphBitmapBox(&f, g, scale,scale, &x0,&y0,&x1,&y1);
      gw = x1-x0;
      gh = y1-y0;
      if (x + gw + 1 >= pw)
         y = bottom_y, x = 1; // advance to next row
      if (y + gh + 1 >= ph) // check if it fits vertically AFTER potentially moving to next row
         return -i;
      STBTT_assert(x+gw < pw);
      STBTT_assert(y+gh < ph);
      stbtt_MakeGlyphBitmap(&f, pixels+x+y*pw, gw,gh,pw, scale,scale, g);
      chardata[i].x0 = (stbtt_int16) x;
      chardata[i].y0 = (stbtt_int16) y;
      chardata[i].x1 = (stbtt_int16) (x + gw);
      chardata[i].y1 = (stbtt_int16) (y + gh);
      chardata[i].xadvance = scale * advance;
      chardata[i].xoff     = (float) x0;
      chardata[i].yoff     = (float) y0;
      x = x + gw + 1;
      if (y+gh+1 > bottom_y)
         bottom_y = y+gh+1;
   }
   return bottom_y;
}

STBTT_DEF void stbtt_GetBakedQuad(const stbtt_bakedchar *chardata, int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q, int opengl_fillrule)
{
   float d3d_bias = opengl_fillrule ? 0 : -0.5f;
   float ipw = 1.0f / pw, iph = 1.0f / ph;
   const stbtt_bakedchar *b = chardata + char_index;
   int round_x = STBTT_ifloor((*xpos + b->xoff) + 0.5f);
   int round_y = STBTT_ifloor((*ypos + b->yoff) + 0.5f);

   q->x0 = round_x + d3d_bias;
   q->y0 = round_y + d3d_bias;
   q->x1 = round_x + b->x1 - b->x0 + d3d_bias;
   q->y1 = round_y + b->y1 - b->y0 + d3d_bias;

   q->s0 = b->x0 * ipw;
   q->t0 = b->y0 * iph;
   q->s1 = b->x1 * ipw;
   q->t1 = b->y1 * iph;

   *xpos += b->xadvance;
}

//////////////////////////////////////////////////////////////////////////////
//
// rectangle packing replacement routines if you don't have stb_rect_pack.h
//

#ifndef STB_RECT_PACK_VERSION

typedef int stbrp_coord;

////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//                                                                                //
// COMPILER WARNING ?!?!?                                                         //
//                                                                                //
//                                                                                //
// if you get a compile warning due to these symbols being defined more than      //
// once, move #include "stb_rect_pack.h" before #include "stb_truetype.h"         //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
   int width,height;
   int x,y,bottom_y;
} stbrp_context;

typedef struct
{
   unsigned char x;
} stbrp_node;

struct stbrp_rect
{
   stbrp_coord x,y;
   int id,w,h,was_packed;
};

static void stbrp_init_target(stbrp_context *con, int pw, int ph, stbrp_node *nodes, int num_nodes)
{
   con->width  = pw;
   con->height = ph;
   con->x = 0;
   con->y = 0;
   con->bottom_y = 0;
   STBTT__NOTUSED(nodes);
   STBTT__NOTUSED(num_nodes);
}

static void stbrp_pack_rects(stbrp_context *con, stbrp_rect *rects, int num_rects)
{
   int i;
   for (i=0; i < num_rects; ++i) {
      if (con->x + rects[i].w > con->width) {
         con->x = 0;
         con->y = con->bottom_y;
      }
      if (con->y + rects[i].h > con->height)
         break;
      rects[i].x = con->x;
      rects[i].y = con->y;
      rects[i].was_packed = 1;
      con->x += rects[i].w;
      if (con->y + rects[i].h > con->bottom_y)
         con->bottom_y = con->y + rects[i].h;
   }
   for (   ; i < num_rects; ++i)
      rects[i].was_packed = 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
// bitmap baking
//
// This is SUPER-AWESOME (tm Ryan Gordon) packing using stb_rect_pack.h. If
// stb_rect_pack.h isn't available, it uses the BakeFontBitmap strategy.

STBTT_DEF int stbtt_PackBegin(stbtt_pack_context *spc, unsigned char *pixels, int pw, int ph, int stride_in_bytes, int padding, void *alloc_context)
{
   stbrp_context *context = (stbrp_context *) STBTT_malloc(sizeof(*context)            ,alloc_context);
   int            num_nodes = pw - padding;
   stbrp_node    *nodes   = (stbrp_node    *) STBTT_malloc(sizeof(*nodes  ) * num_nodes,alloc_context);

   if (context == NULL || nodes == NULL) {
      if (context != NULL) STBTT_free(context, alloc_context);
      if (nodes   != NULL) STBTT_free(nodes  , alloc_context);
      return 0;
   }

   spc->user_allocator_context = alloc_context;
   spc->width = pw;
   spc->height = ph;
   spc->pixels = pixels;
   spc->pack_info = context;
   spc->nodes = nodes;
   spc->padding = padding;
   spc->stride_in_bytes = stride_in_bytes != 0 ? stride_in_bytes : pw;
   spc->h_oversample = 1;
   spc->v_oversample = 1;
   spc->skip_missing = 0;

   stbrp_init_target(context, pw-padding, ph-padding, nodes, num_nodes);

   if (pixels)
      STBTT_memset(pixels, 0, pw*ph); // background of 0 around pixels

   return 1;
}

STBTT_DEF void stbtt_PackEnd  (stbtt_pack_context *spc)
{
   STBTT_free(spc->nodes    , spc->user_allocator_context);
   STBTT_free(spc->pack_info, spc->user_allocator_context);
}

STBTT_DEF void stbtt_PackSetOversampling(stbtt_pack_context *spc, unsigned int h_oversample, unsigned int v_oversample)
{
   STBTT_assert(h_oversample <= STBTT_MAX_OVERSAMPLE);
   STBTT_assert(v_oversample <= STBTT_MAX_OVERSAMPLE);
   if (h_oversample <= STBTT_MAX_OVERSAMPLE)
      spc->h_oversample = h_oversample;
   if (v_oversample <= STBTT_MAX_OVERSAMPLE)
      spc->v_oversample = v_oversample;
}

STBTT_DEF void stbtt_PackSetSkipMissingCodepoints(stbtt_pack_context *spc, int skip)
{
   spc->skip_missing = skip;
}

#define STBTT__OVER_MASK  (STBTT_MAX_OVERSAMPLE-1)

static void stbtt__h_prefilter(unsigned char *pixels, int w, int h, int stride_in_bytes, unsigned int kernel_width)
{
   unsigned char buffer[STBTT_MAX_OVERSAMPLE];
   int safe_w = w - kernel_width;
   int j;
   STBTT_memset(buffer, 0, STBTT_MAX_OVERSAMPLE); // suppress bogus warning from VS2013 -analyze
   for (j=0; j < h; ++j) {
      int i;
      unsigned int total;
      STBTT_memset(buffer, 0, kernel_width);

      total = 0;

      // make kernel_width a constant in common cases so compiler can optimize out the divide
      switch (kernel_width) {
         case 2:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (unsigned char) (total / 2);
            }
            break;
         case 3:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (unsigned char) (total / 3);
            }
            break;
         case 4:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (unsigned char) (total / 4);
            }
            break;
         case 5:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (unsigned char) (total / 5);
            }
            break;
         default:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (unsigned char) (total / kernel_width);
            }
            break;
      }

      for (; i < w; ++i) {
         STBTT_assert(pixels[i] == 0);
         total -= buffer[i & STBTT__OVER_MASK];
         pixels[i] = (unsigned char) (total / kernel_width);
      }

      pixels += stride_in_bytes;
   }
}

static void stbtt__v_prefilter(unsigned char *pixels, int w, int h, int stride_in_bytes, unsigned int kernel_width)
{
   unsigned char buffer[STBTT_MAX_OVERSAMPLE];
   int safe_h = h - kernel_width;
   int j;
   STBTT_memset(buffer, 0, STBTT_MAX_OVERSAMPLE); // suppress bogus warning from VS2013 -analyze
   for (j=0; j < w; ++j) {
      int i;
      unsigned int total;
      STBTT_memset(buffer, 0, kernel_width);

      total = 0;

      // make kernel_width a constant in common cases so compiler can optimize out the divide
      switch (kernel_width) {
         case 2:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (unsigned char) (total / 2);
            }
            break;
         case 3:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (unsigned char) (total / 3);
            }
            break;
         case 4:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (unsigned char) (total / 4);
            }
            break;
         case 5:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (unsigned char) (total / 5);
            }
            break;
         default:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (unsigned char) (total / kernel_width);
            }
            break;
      }

      for (; i < h; ++i) {
         STBTT_assert(pixels[i*stride_in_bytes] == 0);
         total -= buffer[i & STBTT__OVER_MASK];
         pixels[i*stride_in_bytes] = (unsigned char) (total / kernel_width);
      }

      pixels += 1;
   }
}

static float stbtt__oversample_shift(int oversample)
{
   if (!oversample)
      return 0.0f;

   // The prefilter is a box filter of width "oversample",
   // which shifts phase by (oversample - 1)/2 pixels in
   // oversampled space. We want to shift in the opposite
   // direction to counter this.
   return (float)-(oversample - 1) / (2.0f * (float)oversample);
}

// rects array must be big enough to accommodate all characters in the given ranges
STBTT_DEF int stbtt_PackFontRangesGatherRects(stbtt_pack_context *spc, const stbtt_fontinfo *info, stbtt_pack_range *ranges, int num_ranges, stbrp_rect *rects)
{
   int i,j,k;
   int missing_glyph_added = 0;

   k=0;
   for (i=0; i < num_ranges; ++i) {
      float fh = ranges[i].font_size;
      float scale = fh > 0 ? stbtt_ScaleForPixelHeight(info, fh) : stbtt_ScaleForMappingEmToPixels(info, -fh);
      ranges[i].h_oversample = (unsigned char) spc->h_oversample;
      ranges[i].v_oversample = (unsigned char) spc->v_oversample;
      for (j=0; j < ranges[i].num_chars; ++j) {
         int x0,y0,x1,y1;
         int codepoint = ranges[i].array_of_unicode_codepoints == NULL ? ranges[i].first_unicode_codepoint_in_range + j : ranges[i].array_of_unicode_codepoints[j];
         int glyph = stbtt_FindGlyphIndex(info, codepoint);
         if (glyph == 0 && (spc->skip_missing || missing_glyph_added)) {
            rects[k].w = rects[k].h = 0;
         } else {
            stbtt_GetGlyphBitmapBoxSubpixel(info,glyph,
                                            scale * spc->h_oversample,
                                            scale * spc->v_oversample,
                                            0,0,
                                            &x0,&y0,&x1,&y1);
            rects[k].w = (stbrp_coord) (x1-x0 + spc->padding + spc->h_oversample-1);
            rects[k].h = (stbrp_coord) (y1-y0 + spc->padding + spc->v_oversample-1);
            if (glyph == 0)
               missing_glyph_added = 1;
         }
         ++k;
      }
   }

   return k;
}

STBTT_DEF void stbtt_MakeGlyphBitmapSubpixelPrefilter(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int prefilter_x, int prefilter_y, float *sub_x, float *sub_y, int glyph)
{
   stbtt_MakeGlyphBitmapSubpixel(info,
                                 output,
                                 out_w - (prefilter_x - 1),
                                 out_h - (prefilter_y - 1),
                                 out_stride,
                                 scale_x,
                                 scale_y,
                                 shift_x,
                                 shift_y,
                                 glyph);

   if (prefilter_x > 1)
      stbtt__h_prefilter(output, out_w, out_h, out_stride, prefilter_x);

   if (prefilter_y > 1)
      stbtt__v_prefilter(output, out_w, out_h, out_stride, prefilter_y);

   *sub_x = stbtt__oversample_shift(prefilter_x);
   *sub_y = stbtt__oversample_shift(prefilter_y);
}

// rects array must be big enough to accommodate all characters in the given ranges
STBTT_DEF int stbtt_PackFontRangesRenderIntoRects(stbtt_pack_context *spc, const stbtt_fontinfo *info, stbtt_pack_range *ranges, int num_ranges, stbrp_rect *rects)
{
   int i,j,k, missing_glyph = -1, return_value = 1;

   // save current values
   int old_h_over = spc->h_oversample;
   int old_v_over = spc->v_oversample;

   k = 0;
   for (i=0; i < num_ranges; ++i) {
      float fh = ranges[i].font_size;
      float scale = fh > 0 ? stbtt_ScaleForPixelHeight(info, fh) : stbtt_ScaleForMappingEmToPixels(info, -fh);
      float recip_h,recip_v,sub_x,sub_y;
      spc->h_oversample = ranges[i].h_oversample;
      spc->v_oversample = ranges[i].v_oversample;
      recip_h = 1.0f / spc->h_oversample;
      recip_v = 1.0f / spc->v_oversample;
      sub_x = stbtt__oversample_shift(spc->h_oversample);
      sub_y = stbtt__oversample_shift(spc->v_oversample);
      for (j=0; j < ranges[i].num_chars; ++j) {
         stbrp_rect *r = &rects[k];
         if (r->was_packed && r->w != 0 && r->h != 0) {
            stbtt_packedchar *bc = &ranges[i].chardata_for_range[j];
            int advance, lsb, x0,y0,x1,y1;
            int codepoint = ranges[i].array_of_unicode_codepoints == NULL ? ranges[i].first_unicode_codepoint_in_range + j : ranges[i].array_of_unicode_codepoints[j];
            int glyph = stbtt_FindGlyphIndex(info, codepoint);
            stbrp_coord pad = (stbrp_coord) spc->padding;

            // pad on left and top
            r->x += pad;
            r->y += pad;
            r->w -= pad;
            r->h -= pad;
            stbtt_GetGlyphHMetrics(info, glyph, &advance, &lsb);
            stbtt_GetGlyphBitmapBox(info, glyph,
                                    scale * spc->h_oversample,
                                    scale * spc->v_oversample,
                                    &x0,&y0,&x1,&y1);
            stbtt_MakeGlyphBitmapSubpixel(info,
                                          spc->pixels + r->x + r->y*spc->stride_in_bytes,
                                          r->w - spc->h_oversample+1,
                                          r->h - spc->v_oversample+1,
                                          spc->stride_in_bytes,
                                          scale * spc->h_oversample,
                                          scale * spc->v_oversample,
                                          0,0,
                                          glyph);

            if (spc->h_oversample > 1)
               stbtt__h_prefilter(spc->pixels + r->x + r->y*spc->stride_in_bytes,
                                  r->w, r->h, spc->stride_in_bytes,
                                  spc->h_oversample);

            if (spc->v_oversample > 1)
               stbtt__v_prefilter(spc->pixels + r->x + r->y*spc->stride_in_bytes,
                                  r->w, r->h, spc->stride_in_bytes,
                                  spc->v_oversample);

            bc->x0       = (stbtt_int16)  r->x;
            bc->y0       = (stbtt_int16)  r->y;
            bc->x1       = (stbtt_int16) (r->x + r->w);
            bc->y1       = (stbtt_int16) (r->y + r->h);
            bc->xadvance =                scale * advance;
            bc->xoff     =       (float)  x0 * recip_h + sub_x;
            bc->yoff     =       (float)  y0 * recip_v + sub_y;
            bc->xoff2    =                (x0 + r->w) * recip_h + sub_x;
            bc->yoff2    =                (y0 + r->h) * recip_v + sub_y;

            if (glyph == 0)
               missing_glyph = j;
         } else if (spc->skip_missing) {
            return_value = 0;
         } else if (r->was_packed && r->w == 0 && r->h == 0 && missing_glyph >= 0) {
            ranges[i].chardata_for_range[j] = ranges[i].chardata_for_range[missing_glyph];
         } else {
            return_value = 0; // if any fail, report failure
         }

         ++k;
      }
   }

   // restore original values
   spc->h_oversample = old_h_over;
   spc->v_oversample = old_v_over;

   return return_value;
}

STBTT_DEF void stbtt_PackFontRangesPackRects(stbtt_pack_context *spc, stbrp_rect *rects, int num_rects)
{
   stbrp_pack_rects((stbrp_context *) spc->pack_info, rects, num_rects);
}

STBTT_DEF int stbtt_PackFontRanges(stbtt_pack_context *spc, const unsigned char *fontdata, int font_index, stbtt_pack_range *ranges, int num_ranges)
{
   stbtt_fontinfo info;
   int i,j,n, return_value = 1;
   //stbrp_context *context = (stbrp_context *) spc->pack_info;
   stbrp_rect    *rects;

   // flag all characters as NOT packed
   for (i=0; i < num_ranges; ++i)
      for (j=0; j < ranges[i].num_chars; ++j)
         ranges[i].chardata_for_range[j].x0 =
         ranges[i].chardata_for_range[j].y0 =
         ranges[i].chardata_for_range[j].x1 =
         ranges[i].chardata_for_range[j].y1 = 0;

   n = 0;
   for (i=0; i < num_ranges; ++i)
      n += ranges[i].num_chars;

   rects = (stbrp_rect *) STBTT_malloc(sizeof(*rects) * n, spc->user_allocator_context);
   if (rects == NULL)
      return 0;

   info.userdata = spc->user_allocator_context;
   stbtt_InitFont(&info, fontdata, stbtt_GetFontOffsetForIndex(fontdata,font_index));

   n = stbtt_PackFontRangesGatherRects(spc, &info, ranges, num_ranges, rects);

   stbtt_PackFontRangesPackRects(spc, rects, n);

   return_value = stbtt_PackFontRangesRenderIntoRects(spc, &info, ranges, num_ranges, rects);

   STBTT_free(rects, spc->user_allocator_context);
   return return_value;
}

STBTT_DEF int stbtt_PackFontRange(stbtt_pack_context *spc, const unsigned char *fontdata, int font_index, float font_size,
            int first_unicode_codepoint_in_range, int num_chars_in_range, stbtt_packedchar *chardata_for_range)
{
   stbtt_pack_range range;
   range.first_unicode_codepoint_in_range = first_unicode_codepoint_in_range;
   range.array_of_unicode_codepoints = NULL;
   range.num_chars                   = num_chars_in_range;
   range.chardata_for_range          = chardata_for_range;
   range.font_size                   = font_size;
   return stbtt_PackFontRanges(spc, fontdata, font_index, &range, 1);
}

STBTT_DEF void stbtt_GetScaledFontVMetrics(const unsigned char *fontdata, int index, float size, float *ascent, float *descent, float *lineGap)
{
   int i_ascent, i_descent, i_lineGap;
   float scale;
   stbtt_fontinfo info;
   stbtt_InitFont(&info, fontdata, stbtt_GetFontOffsetForIndex(fontdata, index));
   scale = size > 0 ? stbtt_ScaleForPixelHeight(&info, size) : stbtt_ScaleForMappingEmToPixels(&info, -size);
   stbtt_GetFontVMetrics(&info, &i_ascent, &i_descent, &i_lineGap);
   *ascent  = (float) i_ascent  * scale;
   *descent = (float) i_descent * scale;
   *lineGap = (float) i_lineGap * scale;
}

STBTT_DEF void stbtt_GetPackedQuad(const stbtt_packedchar *chardata, int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q, int align_to_integer)
{
   float ipw = 1.0f / pw, iph = 1.0f / ph;
   const stbtt_packedchar *b = chardata + char_index;

   if (align_to_integer) {
      float x = (float) STBTT_ifloor((*xpos + b->xoff) + 0.5f);
      float y = (float) STBTT_ifloor((*ypos + b->yoff) + 0.5f);
      q->x0 = x;
      q->y0 = y;
      q->x1 = x + b->xoff2 - b->xoff;
      q->y1 = y + b->yoff2 - b->yoff;
   } else {
      q->x0 = *xpos + b->xoff;
      q->y0 = *ypos + b->yoff;
      q->x1 = *xpos + b->xoff2;
      q->y1 = *ypos + b->yoff2;
   }

   q->s0 = b->x0 * ipw;
   q->t0 = b->y0 * iph;
   q->s1 = b->x1 * ipw;
   q->t1 = b->y1 * iph;

   *xpos += b->xadvance;
}

//////////////////////////////////////////////////////////////////////////////
//
// sdf computation
//

#define STBTT_min(a,b)  ((a) < (b) ? (a) : (b))
#define STBTT_max(a,b)  ((a) < (b) ? (b) : (a))

static int stbtt__ray_intersect_bezier(float orig[2], float ray[2], float q0[2], float q1[2], float q2[2], float hits[2][2])
{
   float q0perp = q0[1]*ray[0] - q0[0]*ray[1];
   float q1perp = q1[1]*ray[0] - q1[0]*ray[1];
   float q2perp = q2[1]*ray[0] - q2[0]*ray[1];
   float roperp = orig[1]*ray[0] - orig[0]*ray[1];

   float a = q0perp - 2*q1perp + q2perp;
   float b = q1perp - q0perp;
   float c = q0perp - roperp;

   float s0 = 0., s1 = 0.;
   int num_s = 0;

   if (a != 0.0) {
      float discr = b*b - a*c;
      if (discr > 0.0) {
         float rcpna = -1 / a;
         float d = (float) STBTT_sqrt(discr);
         s0 = (b+d) * rcpna;
         s1 = (b-d) * rcpna;
         if (s0 >= 0.0 && s0 <= 1.0)
            num_s = 1;
         if (d > 0.0 && s1 >= 0.0 && s1 <= 1.0) {
            if (num_s == 0) s0 = s1;
            ++num_s;
         }
      }
   } else {
      // 2*b*s + c = 0
      // s = -c / (2*b)
      s0 = c / (-2 * b);
      if (s0 >= 0.0 && s0 <= 1.0)
         num_s = 1;
   }

   if (num_s == 0)
      return 0;
   else {
      float rcp_len2 = 1 / (ray[0]*ray[0] + ray[1]*ray[1]);
      float rayn_x = ray[0] * rcp_len2, rayn_y = ray[1] * rcp_len2;

      float q0d =   q0[0]*rayn_x +   q0[1]*rayn_y;
      float q1d =   q1[0]*rayn_x +   q1[1]*rayn_y;
      float q2d =   q2[0]*rayn_x +   q2[1]*rayn_y;
      float rod = orig[0]*rayn_x + orig[1]*rayn_y;

      float q10d = q1d - q0d;
      float q20d = q2d - q0d;
      float q0rd = q0d - rod;

      hits[0][0] = q0rd + s0*(2.0f - 2.0f*s0)*q10d + s0*s0*q20d;
      hits[0][1] = a*s0+b;

      if (num_s > 1) {
         hits[1][0] = q0rd + s1*(2.0f - 2.0f*s1)*q10d + s1*s1*q20d;
         hits[1][1] = a*s1+b;
         return 2;
      } else {
         return 1;
      }
   }
}

static int equal(float *a, float *b)
{
   return (a[0] == b[0] && a[1] == b[1]);
}

static int stbtt__compute_crossings_x(float x, float y, int nverts, stbtt_vertex *verts)
{
   int i;
   float orig[2], ray[2] = { 1, 0 };
   float y_frac;
   int winding = 0;

   // make sure y never passes through a vertex of the shape
   y_frac = (float) STBTT_fmod(y, 1.0f);
   if (y_frac < 0.01f)
      y += 0.01f;
   else if (y_frac > 0.99f)
      y -= 0.01f;

   orig[0] = x;
   orig[1] = y;

   // test a ray from (-infinity,y) to (x,y)
   for (i=0; i < nverts; ++i) {
      if (verts[i].type == STBTT_vline) {
         int x0 = (int) verts[i-1].x, y0 = (int) verts[i-1].y;
         int x1 = (int) verts[i  ].x, y1 = (int) verts[i  ].y;
         if (y > STBTT_min(y0,y1) && y < STBTT_max(y0,y1) && x > STBTT_min(x0,x1)) {
            float x_inter = (y - y0) / (y1 - y0) * (x1-x0) + x0;
            if (x_inter < x)
               winding += (y0 < y1) ? 1 : -1;
         }
      }
      if (verts[i].type == STBTT_vcurve) {
         int x0 = (int) verts[i-1].x , y0 = (int) verts[i-1].y ;
         int x1 = (int) verts[i  ].cx, y1 = (int) verts[i  ].cy;
         int x2 = (int) verts[i  ].x , y2 = (int) verts[i  ].y ;
         int ax = STBTT_min(x0,STBTT_min(x1,x2)), ay = STBTT_min(y0,STBTT_min(y1,y2));
         int by = STBTT_max(y0,STBTT_max(y1,y2));
         if (y > ay && y < by && x > ax) {
            float q0[2],q1[2],q2[2];
            float hits[2][2];
            q0[0] = (float)x0;
            q0[1] = (float)y0;
            q1[0] = (float)x1;
            q1[1] = (float)y1;
            q2[0] = (float)x2;
            q2[1] = (float)y2;
            if (equal(q0,q1) || equal(q1,q2)) {
               x0 = (int)verts[i-1].x;
               y0 = (int)verts[i-1].y;
               x1 = (int)verts[i  ].x;
               y1 = (int)verts[i  ].y;
               if (y > STBTT_min(y0,y1) && y < STBTT_max(y0,y1) && x > STBTT_min(x0,x1)) {
                  float x_inter = (y - y0) / (y1 - y0) * (x1-x0) + x0;
                  if (x_inter < x)
                     winding += (y0 < y1) ? 1 : -1;
               }
            } else {
               int num_hits = stbtt__ray_intersect_bezier(orig, ray, q0, q1, q2, hits);
               if (num_hits >= 1)
                  if (hits[0][0] < 0)
                     winding += (hits[0][1] < 0 ? -1 : 1);
               if (num_hits >= 2)
                  if (hits[1][0] < 0)
                     winding += (hits[1][1] < 0 ? -1 : 1);
            }
         }
      }
   }
   return winding;
}

static float stbtt__cuberoot( float x )
{
   if (x<0)
      return -(float) STBTT_pow(-x,1.0f/3.0f);
   else
      return  (float) STBTT_pow( x,1.0f/3.0f);
}

// x^3 + a*x^2 + b*x + c = 0
static int stbtt__solve_cubic(float a, float b, float c, float* r)
{
   float s = -a / 3;
   float p = b - a*a / 3;
   float q = a * (2*a*a - 9*b) / 27 + c;
   float p3 = p*p*p;
   float d = q*q + 4*p3 / 27;
   if (d >= 0) {
      float z = (float) STBTT_sqrt(d);
      float u = (-q + z) / 2;
      float v = (-q - z) / 2;
      u = stbtt__cuberoot(u);
      v = stbtt__cuberoot(v);
      r[0] = s + u + v;
      return 1;
   } else {
      float u = (float) STBTT_sqrt(-p/3);
      float v = (float) STBTT_acos(-STBTT_sqrt(-27/p3) * q / 2) / 3; // p3 must be negative, since d is negative
      float m = (float) STBTT_cos(v);
      float n = (float) STBTT_cos(v-3.141592/2)*1.732050808f;
      r[0] = s + u * 2 * m;
      r[1] = s - u * (m + n);
      r[2] = s - u * (m - n);

      //STBTT_assert( STBTT_fabs(((r[0]+a)*r[0]+b)*r[0]+c) < 0.05f);  // these asserts may not be safe at all scales, though they're in bezier t parameter units so maybe?
      //STBTT_assert( STBTT_fabs(((r[1]+a)*r[1]+b)*r[1]+c) < 0.05f);
      //STBTT_assert( STBTT_fabs(((r[2]+a)*r[2]+b)*r[2]+c) < 0.05f);
      return 3;
   }
}

STBTT_DEF unsigned char * stbtt_GetGlyphSDF(const stbtt_fontinfo *info, float scale, int glyph, int padding, unsigned char onedge_value, float pixel_dist_scale, int *width, int *height, int *xoff, int *yoff)
{
   float scale_x = scale, scale_y = scale;
   int ix0,iy0,ix1,iy1;
   int w,h;
   unsigned char *data;

   if (scale == 0) return NULL;

   stbtt_GetGlyphBitmapBoxSubpixel(info, glyph, scale, scale, 0.0f,0.0f, &ix0,&iy0,&ix1,&iy1);

   // if empty, return NULL
   if (ix0 == ix1 || iy0 == iy1)
      return NULL;

   ix0 -= padding;
   iy0 -= padding;
   ix1 += padding;
   iy1 += padding;

   w = (ix1 - ix0);
   h = (iy1 - iy0);

   if (width ) *width  = w;
   if (height) *height = h;
   if (xoff  ) *xoff   = ix0;
   if (yoff  ) *yoff   = iy0;

   // invert for y-downwards bitmaps
   scale_y = -scale_y;

   {
      int x,y,i,j;
      float *precompute;
      stbtt_vertex *verts;
      int num_verts = stbtt_GetGlyphShape(info, glyph, &verts);
      data = (unsigned char *) STBTT_malloc(w * h, info->userdata);
      precompute = (float *) STBTT_malloc(num_verts * sizeof(float), info->userdata);

      for (i=0,j=num_verts-1; i < num_verts; j=i++) {
         if (verts[i].type == STBTT_vline) {
            float x0 = verts[i].x*scale_x, y0 = verts[i].y*scale_y;
            float x1 = verts[j].x*scale_x, y1 = verts[j].y*scale_y;
            float dist = (float) STBTT_sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
            precompute[i] = (dist == 0) ? 0.0f : 1.0f / dist;
         } else if (verts[i].type == STBTT_vcurve) {
            float x2 = verts[j].x *scale_x, y2 = verts[j].y *scale_y;
            float x1 = verts[i].cx*scale_x, y1 = verts[i].cy*scale_y;
            float x0 = verts[i].x *scale_x, y0 = verts[i].y *scale_y;
            float bx = x0 - 2*x1 + x2, by = y0 - 2*y1 + y2;
            float len2 = bx*bx + by*by;
            if (len2 != 0.0f)
               precompute[i] = 1.0f / (bx*bx + by*by);
            else
               precompute[i] = 0.0f;
         } else
            precompute[i] = 0.0f;
      }

      for (y=iy0; y < iy1; ++y) {
         for (x=ix0; x < ix1; ++x) {
            float val;
            float min_dist = 999999.0f;
            float sx = (float) x + 0.5f;
            float sy = (float) y + 0.5f;
            float x_gspace = (sx / scale_x);
            float y_gspace = (sy / scale_y);

            int winding = stbtt__compute_crossings_x(x_gspace, y_gspace, num_verts, verts); // @OPTIMIZE: this could just be a rasterization, but needs to be line vs. non-tesselated curves so a new path

            for (i=0; i < num_verts; ++i) {
               float x0 = verts[i].x*scale_x, y0 = verts[i].y*scale_y;

               if (verts[i].type == STBTT_vline && precompute[i] != 0.0f) {
                  float x1 = verts[i-1].x*scale_x, y1 = verts[i-1].y*scale_y;

                  float dist,dist2 = (x0-sx)*(x0-sx) + (y0-sy)*(y0-sy);
                  if (dist2 < min_dist*min_dist)
                     min_dist = (float) STBTT_sqrt(dist2);

                  // coarse culling against bbox
                  //if (sx > STBTT_min(x0,x1)-min_dist && sx < STBTT_max(x0,x1)+min_dist &&
                  //    sy > STBTT_min(y0,y1)-min_dist && sy < STBTT_max(y0,y1)+min_dist)
                  dist = (float) STBTT_fabs((x1-x0)*(y0-sy) - (y1-y0)*(x0-sx)) * precompute[i];
                  STBTT_assert(i != 0);
                  if (dist < min_dist) {
                     // check position along line
                     // x' = x0 + t*(x1-x0), y' = y0 + t*(y1-y0)
                     // minimize (x'-sx)*(x'-sx)+(y'-sy)*(y'-sy)
                     float dx = x1-x0, dy = y1-y0;
                     float px = x0-sx, py = y0-sy;
                     // minimize (px+t*dx)^2 + (py+t*dy)^2 = px*px + 2*px*dx*t + t^2*dx*dx + py*py + 2*py*dy*t + t^2*dy*dy
                     // derivative: 2*px*dx + 2*py*dy + (2*dx*dx+2*dy*dy)*t, set to 0 and solve
                     float t = -(px*dx + py*dy) / (dx*dx + dy*dy);
                     if (t >= 0.0f && t <= 1.0f)
                        min_dist = dist;
                  }
               } else if (verts[i].type == STBTT_vcurve) {
                  float x2 = verts[i-1].x *scale_x, y2 = verts[i-1].y *scale_y;
                  float x1 = verts[i  ].cx*scale_x, y1 = verts[i  ].cy*scale_y;
                  float box_x0 = STBTT_min(STBTT_min(x0,x1),x2);
                  float box_y0 = STBTT_min(STBTT_min(y0,y1),y2);
                  float box_x1 = STBTT_max(STBTT_max(x0,x1),x2);
                  float box_y1 = STBTT_max(STBTT_max(y0,y1),y2);
                  // coarse culling against bbox to avoid computing cubic unnecessarily
                  if (sx > box_x0-min_dist && sx < box_x1+min_dist && sy > box_y0-min_dist && sy < box_y1+min_dist) {
                     int num=0;
                     float ax = x1-x0, ay = y1-y0;
                     float bx = x0 - 2*x1 + x2, by = y0 - 2*y1 + y2;
                     float mx = x0 - sx, my = y0 - sy;
                     float res[3] = {0.f,0.f,0.f};
                     float px,py,t,it,dist2;
                     float a_inv = precompute[i];
                     if (a_inv == 0.0) { // if a_inv is 0, it's 2nd degree so use quadratic formula
                        float a = 3*(ax*bx + ay*by);
                        float b = 2*(ax*ax + ay*ay) + (mx*bx+my*by);
                        float c = mx*ax+my*ay;
                        if (a == 0.0) { // if a is 0, it's linear
                           if (b != 0.0) {
                              res[num++] = -c/b;
                           }
                        } else {
                           float discriminant = b*b - 4*a*c;
                           if (discriminant < 0)
                              num = 0;
                           else {
                              float root = (float) STBTT_sqrt(discriminant);
                              res[0] = (-b - root)/(2*a);
                              res[1] = (-b + root)/(2*a);
                              num = 2; // don't bother distinguishing 1-solution case, as code below will still work
                           }
                        }
                     } else {
                        float b = 3*(ax*bx + ay*by) * a_inv; // could precompute this as it doesn't depend on sample point
                        float c = (2*(ax*ax + ay*ay) + (mx*bx+my*by)) * a_inv;
                        float d = (mx*ax+my*ay) * a_inv;
                        num = stbtt__solve_cubic(b, c, d, res);
                     }
                     dist2 = (x0-sx)*(x0-sx) + (y0-sy)*(y0-sy);
                     if (dist2 < min_dist*min_dist)
                        min_dist = (float) STBTT_sqrt(dist2);

                     if (num >= 1 && res[0] >= 0.0f && res[0] <= 1.0f) {
                        t = res[0], it = 1.0f - t;
                        px = it*it*x0 + 2*t*it*x1 + t*t*x2;
                        py = it*it*y0 + 2*t*it*y1 + t*t*y2;
                        dist2 = (px-sx)*(px-sx) + (py-sy)*(py-sy);
                        if (dist2 < min_dist * min_dist)
                           min_dist = (float) STBTT_sqrt(dist2);
                     }
                     if (num >= 2 && res[1] >= 0.0f && res[1] <= 1.0f) {
                        t = res[1], it = 1.0f - t;
                        px = it*it*x0 + 2*t*it*x1 + t*t*x2;
                        py = it*it*y0 + 2*t*it*y1 + t*t*y2;
                        dist2 = (px-sx)*(px-sx) + (py-sy)*(py-sy);
                        if (dist2 < min_dist * min_dist)
                           min_dist = (float) STBTT_sqrt(dist2);
                     }
                     if (num >= 3 && res[2] >= 0.0f && res[2] <= 1.0f) {
                        t = res[2], it = 1.0f - t;
                        px = it*it*x0 + 2*t*it*x1 + t*t*x2;
                        py = it*it*y0 + 2*t*it*y1 + t*t*y2;
                        dist2 = (px-sx)*(px-sx) + (py-sy)*(py-sy);
                        if (dist2 < min_dist * min_dist)
                           min_dist = (float) STBTT_sqrt(dist2);
                     }
                  }
               }
            }
            if (winding == 0)
               min_dist = -min_dist;  // if outside the shape, value is negative
            val = onedge_value + pixel_dist_scale * min_dist;
            if (val < 0)
               val = 0;
            else if (val > 255)
               val = 255;
            data[(y-iy0)*w+(x-ix0)] = (unsigned char) val;
         }
      }
      STBTT_free(precompute, info->userdata);
      STBTT_free(verts, info->userdata);
   }
   return data;
}

STBTT_DEF unsigned char * stbtt_GetCodepointSDF(const stbtt_fontinfo *info, float scale, int codepoint, int padding, unsigned char onedge_value, float pixel_dist_scale, int *width, int *height, int *xoff, int *yoff)
{
   return stbtt_GetGlyphSDF(info, scale, stbtt_FindGlyphIndex(info, codepoint), padding, onedge_value, pixel_dist_scale, width, height, xoff, yoff);
}

STBTT_DEF void stbtt_FreeSDF(unsigned char *bitmap, void *userdata)
{
   STBTT_free(bitmap, userdata);
}

//////////////////////////////////////////////////////////////////////////////
//
// font name matching -- recommended not to use this
//

// check if a utf8 string contains a prefix which is the utf16 string; if so return length of matching utf8 string
static stbtt_int32 stbtt__CompareUTF8toUTF16_bigendian_prefix(stbtt_uint8 *s1, stbtt_int32 len1, stbtt_uint8 *s2, stbtt_int32 len2)
{
   stbtt_int32 i=0;

   // convert utf16 to utf8 and compare the results while converting
   while (len2) {
      stbtt_uint16 ch = s2[0]*256 + s2[1];
      if (ch < 0x80) {
         if (i >= len1) return -1;
         if (s1[i++] != ch) return -1;
      } else if (ch < 0x800) {
         if (i+1 >= len1) return -1;
         if (s1[i++] != 0xc0 + (ch >> 6)) return -1;
         if (s1[i++] != 0x80 + (ch & 0x3f)) return -1;
      } else if (ch >= 0xd800 && ch < 0xdc00) {
         stbtt_uint32 c;
         stbtt_uint16 ch2 = s2[2]*256 + s2[3];
         if (i+3 >= len1) return -1;
         c = ((ch - 0xd800) << 10) + (ch2 - 0xdc00) + 0x10000;
         if (s1[i++] != 0xf0 + (c >> 18)) return -1;
         if (s1[i++] != 0x80 + ((c >> 12) & 0x3f)) return -1;
         if (s1[i++] != 0x80 + ((c >>  6) & 0x3f)) return -1;
         if (s1[i++] != 0x80 + ((c      ) & 0x3f)) return -1;
         s2 += 2; // plus another 2 below
         len2 -= 2;
      } else if (ch >= 0xdc00 && ch < 0xe000) {
         return -1;
      } else {
         if (i+2 >= len1) return -1;
         if (s1[i++] != 0xe0 + (ch >> 12)) return -1;
         if (s1[i++] != 0x80 + ((ch >> 6) & 0x3f)) return -1;
         if (s1[i++] != 0x80 + ((ch     ) & 0x3f)) return -1;
      }
      s2 += 2;
      len2 -= 2;
   }
   return i;
}

static int stbtt_CompareUTF8toUTF16_bigendian_internal(char *s1, int len1, char *s2, int len2)
{
   return len1 == stbtt__CompareUTF8toUTF16_bigendian_prefix((stbtt_uint8*) s1, len1, (stbtt_uint8*) s2, len2);
}

// returns results in whatever encoding you request... but note that 2-byte encodings
// will be BIG-ENDIAN... use stbtt_CompareUTF8toUTF16_bigendian() to compare
STBTT_DEF const char *stbtt_GetFontNameString(const stbtt_fontinfo *font, int *length, int platformID, int encodingID, int languageID, int nameID)
{
   stbtt_int32 i,count,stringOffset;
   stbtt_uint8 *fc = font->data;
   stbtt_uint32 offset = font->fontstart;
   stbtt_uint32 nm = stbtt__find_table(fc, offset, "name");
   if (!nm) return NULL;

   count = ttUSHORT(fc+nm+2);
   stringOffset = nm + ttUSHORT(fc+nm+4);
   for (i=0; i < count; ++i) {
      stbtt_uint32 loc = nm + 6 + 12 * i;
      if (platformID == ttUSHORT(fc+loc+0) && encodingID == ttUSHORT(fc+loc+2)
          && languageID == ttUSHORT(fc+loc+4) && nameID == ttUSHORT(fc+loc+6)) {
         *length = ttUSHORT(fc+loc+8);
         return (const char *) (fc+stringOffset+ttUSHORT(fc+loc+10));
      }
   }
   return NULL;
}

static int stbtt__matchpair(stbtt_uint8 *fc, stbtt_uint32 nm, stbtt_uint8 *name, stbtt_int32 nlen, stbtt_int32 target_id, stbtt_int32 next_id)
{
   stbtt_int32 i;
   stbtt_int32 count = ttUSHORT(fc+nm+2);
   stbtt_int32 stringOffset = nm + ttUSHORT(fc+nm+4);

   for (i=0; i < count; ++i) {
      stbtt_uint32 loc = nm + 6 + 12 * i;
      stbtt_int32 id = ttUSHORT(fc+loc+6);
      if (id == target_id) {
         // find the encoding
         stbtt_int32 platform = ttUSHORT(fc+loc+0), encoding = ttUSHORT(fc+loc+2), language = ttUSHORT(fc+loc+4);

         // is this a Unicode encoding?
         if (platform == 0 || (platform == 3 && encoding == 1) || (platform == 3 && encoding == 10)) {
            stbtt_int32 slen = ttUSHORT(fc+loc+8);
            stbtt_int32 off = ttUSHORT(fc+loc+10);

            // check if there's a prefix match
            stbtt_int32 matchlen = stbtt__CompareUTF8toUTF16_bigendian_prefix(name, nlen, fc+stringOffset+off,slen);
            if (matchlen >= 0) {
               // check for target_id+1 immediately following, with same encoding & language
               if (i+1 < count && ttUSHORT(fc+loc+12+6) == next_id && ttUSHORT(fc+loc+12) == platform && ttUSHORT(fc+loc+12+2) == encoding && ttUSHORT(fc+loc+12+4) == language) {
                  slen = ttUSHORT(fc+loc+12+8);
                  off = ttUSHORT(fc+loc+12+10);
                  if (slen == 0) {
                     if (matchlen == nlen)
                        return 1;
                  } else if (matchlen < nlen && name[matchlen] == ' ') {
                     ++matchlen;
                     if (stbtt_CompareUTF8toUTF16_bigendian_internal((char*) (name+matchlen), nlen-matchlen, (char*)(fc+stringOffset+off),slen))
                        return 1;
                  }
               } else {
                  // if nothing immediately following
                  if (matchlen == nlen)
                     return 1;
               }
            }
         }

         // @TODO handle other encodings
      }
   }
   return 0;
}

static int stbtt__matches(stbtt_uint8 *fc, stbtt_uint32 offset, stbtt_uint8 *name, stbtt_int32 flags)
{
   stbtt_int32 nlen = (stbtt_int32) STBTT_strlen((char *) name);
   stbtt_uint32 nm,hd;
   if (!stbtt__isfont(fc+offset)) return 0;

   // check italics/bold/underline flags in macStyle...
   if (flags) {
      hd = stbtt__find_table(fc, offset, "head");
      if ((ttUSHORT(fc+hd+44) & 7) != (flags & 7)) return 0;
   }

   nm = stbtt__find_table(fc, offset, "name");
   if (!nm) return 0;

   if (flags) {
      // if we checked the macStyle flags, then just check the family and ignore the subfamily
      if (stbtt__matchpair(fc, nm, name, nlen, 16, -1))  return 1;
      if (stbtt__matchpair(fc, nm, name, nlen,  1, -1))  return 1;
      if (stbtt__matchpair(fc, nm, name, nlen,  3, -1))  return 1;
   } else {
      if (stbtt__matchpair(fc, nm, name, nlen, 16, 17))  return 1;
      if (stbtt__matchpair(fc, nm, name, nlen,  1,  2))  return 1;
      if (stbtt__matchpair(fc, nm, name, nlen,  3, -1))  return 1;
   }

   return 0;
}

static int stbtt_FindMatchingFont_internal(unsigned char *font_collection, char *name_utf8, stbtt_int32 flags)
{
   stbtt_int32 i;
   for (i=0;;++i) {
      stbtt_int32 off = stbtt_GetFontOffsetForIndex(font_collection, i);
      if (off < 0) return off;
      if (stbtt__matches((stbtt_uint8 *) font_collection, off, (stbtt_uint8*) name_utf8, flags))
         return off;
   }
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

STBTT_DEF int stbtt_BakeFontBitmap(const unsigned char *data, int offset,
                                float pixel_height, unsigned char *pixels, int pw, int ph,
                                int first_char, int num_chars, stbtt_bakedchar *chardata)
{
   return stbtt_BakeFontBitmap_internal((unsigned char *) data, offset, pixel_height, pixels, pw, ph, first_char, num_chars, chardata);
}

STBTT_DEF int stbtt_GetFontOffsetForIndex(const unsigned char *data, int index)
{
   return stbtt_GetFontOffsetForIndex_internal((unsigned char *) data, index);
}

STBTT_DEF int stbtt_GetNumberOfFonts(const unsigned char *data)
{
   return stbtt_GetNumberOfFonts_internal((unsigned char *) data);
}

STBTT_DEF int stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset)
{
   return stbtt_InitFont_internal(info, (unsigned char *) data, offset);
}

STBTT_DEF int stbtt_FindMatchingFont(const unsigned char *fontdata, const char *name, int flags)
{
   return stbtt_FindMatchingFont_internal((unsigned char *) fontdata, (char *) name, flags);
}

STBTT_DEF int stbtt_CompareUTF8toUTF16_bigendian(const char *s1, int len1, const char *s2, int len2)
{
   return stbtt_CompareUTF8toUTF16_bigendian_internal((char *) s1, len1, (char *) s2, len2);
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif // STB_TRUETYPE_IMPLEMENTATION


// FULL VERSION HISTORY
//
//   1.25 (2021-07-11) many fixes
//   1.24 (2020-02-05) fix warning
//   1.23 (2020-02-02) query SVG data for glyphs; query whole kerning table (but only kern not GPOS)
//   1.22 (2019-08-11) minimize missing-glyph duplication; fix kerning if both 'GPOS' and 'kern' are defined
//   1.21 (2019-02-25) fix warning
//   1.20 (2019-02-07) PackFontRange skips missing codepoints; GetScaleFontVMetrics()
//   1.19 (2018-02-11) OpenType GPOS kerning (horizontal only), STBTT_fmod
//   1.18 (2018-01-29) add missing function
//   1.17 (2017-07-23) make more arguments const; doc fix
//   1.16 (2017-07-12) SDF support
//   1.15 (2017-03-03) make more arguments const
//   1.14 (2017-01-16) num-fonts-in-TTC function
//   1.13 (2017-01-02) support OpenType fonts, certain Apple fonts
//   1.12 (2016-10-25) suppress warnings about casting away const with -Wcast-qual
//   1.11 (2016-04-02) fix unused-variable warning
//   1.10 (2016-04-02) allow user-defined fabs() replacement
//                     fix memory leak if fontsize=0.0
//                     fix warning from duplicate typedef
//   1.09 (2016-01-16) warning fix; avoid crash on outofmem; use alloc userdata for PackFontRanges
//   1.08 (2015-09-13) document stbtt_Rasterize(); fixes for vertical & horizontal edges
//   1.07 (2015-08-01) allow PackFontRanges to accept arrays of sparse codepoints;
//                     allow PackFontRanges to pack and render in separate phases;
//                     fix stbtt_GetFontOFfsetForIndex (never worked for non-0 input?);
//                     fixed an assert() bug in the new rasterizer
//                     replace assert() with STBTT_assert() in new rasterizer
//   1.06 (2015-07-14) performance improvements (~35% faster on x86 and x64 on test machine)
//                     also more precise AA rasterizer, except if shapes overlap
//                     remove need for STBTT_sort
//   1.05 (2015-04-15) fix misplaced definitions for STBTT_STATIC
//   1.04 (2015-04-15) typo in example
//   1.03 (2015-04-12) STBTT_STATIC, fix memory leak in new packing, various fixes
//   1.02 (2014-12-10) fix various warnings & compile issues w/ stb_rect_pack, C++
//   1.01 (2014-12-08) fix subpixel position when oversampling to exactly match
//                        non-oversampled; STBTT_POINT_SIZE for packed case only
//   1.00 (2014-12-06) add new PackBegin etc. API, w/ support for oversampling
//   0.99 (2014-09-18) fix multiple bugs with subpixel rendering (ryg)
//   0.9  (2014-08-07) support certain mac/iOS fonts without an MS platformID
//   0.8b (2014-07-07) fix a warning
//   0.8  (2014-05-25) fix a few more warnings
//   0.7  (2013-09-25) bugfix: subpixel glyph bug fixed in 0.5 had come back
//   0.6c (2012-07-24) improve documentation
//   0.6b (2012-07-20) fix a few more warnings
//   0.6  (2012-07-17) fix warnings; added stbtt_ScaleForMappingEmToPixels,
//                        stbtt_GetFontBoundingBox, stbtt_IsGlyphEmpty
//   0.5  (2011-12-09) bugfixes:
//                        subpixel glyph renderer computed wrong bounding box
//                        first vertex of shape can be off-curve (FreeSans)
//   0.4b (2011-12-03) fixed an error in the font baking example
//   0.4  (2011-12-01) kerning, subpixel rendering (tor)
//                    bugfixes for:
//                        codepoint-to-glyph conversion using table fmt=12
//                        codepoint-to-glyph conversion using table fmt=4
//                        stbtt_GetBakedQuad with non-square texture (Zer)
//                    updated Hello World! sample to use kerning and subpixel
//                    fixed some warnings
//   0.3  (2009-06-24) cmap fmt=12, compound shapes (MM)
//                    userdata, malloc-from-userdata, non-zero fill (stb)
//   0.2  (2009-03-11) Fix unsigned/signed char warnings
//   0.1  (2009-03-09) First public release
//

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/


#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//---INTERNAL STATE---
static SDL_Window *sdl_win = NULL;
static SDL_GLContext* sdl_glcontext = NULL;
static int shouldquit = 0;
static int __CBAS__width = 640, __CBAS__height = 480, __CBAS__display_scale = 1;
static int __CBAS__mousex = 0, __CBAS__mousey = 0, __CBAS__mousexrel=0, __CBAS__mouseyrel=0;
static int is_window_open = 0;

unsigned char ttf_buffer[1<<20];
#define TEMP_BITMAP_DIM 1024
unsigned char temp_bitmap[TEMP_BITMAP_DIM*TEMP_BITMAP_DIM];

typedef struct{
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    unsigned ftex;
} rfont;


static unsigned char* __CBAS__loadFont(unsigned char* fname, float pxsz)
{
    FILE* f = fopen((char*)fname, "rb");
    if(f == NULL){
        puts("<FONT LOADING ERROR>");
        puts("Could not open font:");
        puts((char*)fname);
        exit(1);
    }
    rfont* ft = malloc(sizeof(rfont));
    fread(ttf_buffer, 1, 1<<20, f);
    stbtt_BakeFontBitmap(ttf_buffer,0, pxsz, temp_bitmap,
    TEMP_BITMAP_DIM,
    TEMP_BITMAP_DIM, 
    32,96, ft->cdata); // no guarantee this fits!
    // can free ttf_buffer at this point
    glGenTextures(1, &ft->ftex);
    glBindTexture(GL_TEXTURE_2D, ft->ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 
    TEMP_BITMAP_DIM,
    TEMP_BITMAP_DIM, 
    0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    // can free temp_bitmap at this point
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return (unsigned char*)ft;
}
/*
static void __CBAS__beginUI(){
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,width,height,0,-1,1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void __CBAS__endUI(){
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
*/

static void __CBAS__renderText(unsigned char *text, unsigned char* fontin, float x, float y, float vert)
{
    rfont* ft = (rfont*)fontin;
   // assume orthographic projection with units = screen pixels, origin at top left
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, ft->ftex);
   glBegin(GL_QUADS);
    float w;
    float yoff = 0;
    float x_orig = x;
    float y_orig = y;

    while (*text) {
        w = x;
        y = y_orig + yoff;
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(ft->cdata, TEMP_BITMAP_DIM,TEMP_BITMAP_DIM, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
            q.x1 = q.x1 - q.x0;
            q.x0 = w;
            q.x1 = w + q.x1;
            glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
            glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
            glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
            glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
        }
        ++text;
        //x = w + spacing;
        if(text[0] == '\n') {yoff= yoff + vert;w = x_orig;x = x_orig;}
    }
   glEnd();
}
static void __CBAS__renderTextMono(unsigned char *text, unsigned char* fontin, float x, float y, float spacing, float vert)
{
    rfont* ft = (rfont*)fontin;
    // assume orthographic projection with units = screen pixels, origin at top left
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ft->ftex);
    glBegin(GL_QUADS);
    float w;
    float yoff = 0;
    float x_orig = x;
    float y_orig = y;

    while (*text) {
        w = x;
        y = y_orig + yoff;
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(ft->cdata, TEMP_BITMAP_DIM,TEMP_BITMAP_DIM, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
            q.x1 = q.x1 - q.x0;
            q.x0 = w;
            q.x1 = w + q.x1;
            glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
            glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
            glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
            glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
        }
        ++text;
        x = w + spacing;
        if(text[0] == '\n') {yoff= yoff + vert;w = x_orig;x = x_orig;}
    }
    glEnd();
}


static void __CBAS__onClick(int btn, int state);
static void __CBAS__appInit();
static void __CBAS__appUpdate();
static void __CBAS__appClose();
static void __CBAS__onTextInput(unsigned char* text);
static void __CBAS__onTextEdit(unsigned char* text, int start, int length);
static void __CBAS__onKey(int keycode, unsigned char state);
static void __CBAS__onKeyRepeat(int keycode, unsigned char state);
static void __CBAS__onScroll(int x, int y);

static void __CBAS__onResize(int w, int h);
//game controller stuff...



static unsigned long long get_gamerbuttons(){
	unsigned long long retval = 0;
	const unsigned char *state;
	state = SDL_GetKeyboardState(NULL);
	retval |= 0x1 * (state[SDL_SCANCODE_UP]!=0);
	retval |= 0x2 * (state[SDL_SCANCODE_DOWN]!=0);
	retval |= 0x4 * (state[SDL_SCANCODE_LEFT]!=0);
	retval |= 0x8 * (state[SDL_SCANCODE_RIGHT]!=0);
	retval |= 0x10 * (state[SDL_SCANCODE_RETURN]!=0);
	retval |= 0x20 * (state[SDL_SCANCODE_RSHIFT]!=0);
	retval |= 0x40 * (state[SDL_SCANCODE_Z]!=0);
	retval |= 0x80 * (state[SDL_SCANCODE_X]!=0);
	retval |= 0x100 * (state[SDL_SCANCODE_C]!=0);
	retval |= 0x200 * (state[SDL_SCANCODE_A]!=0);
	retval |= 0x400 * (state[SDL_SCANCODE_S]!=0);
	retval |= 0x800 * (state[SDL_SCANCODE_D]!=0);
	retval |= 0x1000 * (state[SDL_SCANCODE_W]!=0);
	retval |= 0x2000 * (state[SDL_SCANCODE_E]!=0);
	retval |= 0x4000 * (state[SDL_SCANCODE_R]!=0);
	retval |= 0x8000 * (state[SDL_SCANCODE_F]!=0);
	retval |= 0x10000 * (state[SDL_SCANCODE_Q]!=0);
	retval |= 0x20000 * (state[SDL_SCANCODE_LSHIFT]!=0);
	return retval;
}

static void pollevents(){
	SDL_Event ev;
	while(SDL_PollEvent(&ev)){
		if(ev.type == SDL_QUIT) {shouldquit = 0xFFff; /*Magic value for quit.*/}
		else if(ev.type == SDL_MOUSEMOTION){
			__CBAS__mousex = ev.motion.x;
			__CBAS__mousey = ev.motion.y;
			__CBAS__mousexrel = ev.motion.xrel;
			__CBAS__mouseyrel = ev.motion.yrel;
		} else if (ev.type == SDL_MOUSEBUTTONDOWN){
			if(ev.button.button == SDL_BUTTON_LEFT){
				__CBAS__onClick(0,1);
			} else if(ev.button.button == SDL_BUTTON_RIGHT){
				__CBAS__onClick(1,1);
			}else if(ev.button.button == SDL_BUTTON_MIDDLE){
				__CBAS__onClick(2,1);
			}
		} else if (ev.type == SDL_MOUSEBUTTONUP){
			if(ev.button.button == SDL_BUTTON_LEFT){
				__CBAS__onClick(0,0);
			} else if(ev.button.button == SDL_BUTTON_RIGHT){
				__CBAS__onClick(1,0);
			}else if(ev.button.button == SDL_BUTTON_MIDDLE){
				__CBAS__onClick(2,0);
			}
		} else if (ev.type == SDL_TEXTINPUT){
		    __CBAS__onTextInput((unsigned char*)ev.text.text);
		} else if (ev.type == SDL_TEXTEDITING){
		    __CBAS__onTextEdit((unsigned char*)ev.edit.text, ev.edit.start, ev.edit.length);
		} else if (ev.type == SDL_KEYDOWN){
		    if(ev.key.repeat == 0) 
		        __CBAS__onKey(SDL_GetKeyFromScancode(ev.key.keysym.scancode), 1);
		    else
		        __CBAS__onKeyRepeat(SDL_GetKeyFromScancode(ev.key.keysym.scancode), 1);
		} else if (ev.type == SDL_KEYUP){
		    if(ev.key.repeat == 0)
		        __CBAS__onKey(SDL_GetKeyFromScancode(ev.key.keysym.scancode), 0);
            else
  		        __CBAS__onKeyRepeat(SDL_GetKeyFromScancode(ev.key.keysym.scancode), 0);
		} else if(ev.type == SDL_WINDOWEVENT){
		    if(ev.window.event == SDL_WINDOWEVENT_RESIZED){
		        __CBAS__width = ev.window.data1;
		        __CBAS__height = ev.window.data2;
		        __CBAS__onResize(__CBAS__width, __CBAS__height);
		    }
		} else if(ev.type == SDL_MOUSEWHEEL){
		    __CBAS__onScroll(ev.wheel.x, ev.wheel.y);
		}
	}
}


typedef Mix_Music track;

unsigned char* __CBAS__loadMusic(unsigned char* t){
    return (unsigned char*)Mix_LoadMUS((char*)t);

}
void __CBAS__setSoundPosition(int chan, int angle, unsigned char dist){Mix_SetPosition(chan,angle,dist);}
void __CBAS__haltSound(int chan){Mix_HaltChannel(chan);}
int __CBAS__playMusic(unsigned char* mus,int loops, int ms){return Mix_FadeInMusic((track*)mus,loops,ms);}
void __CBAS__haltMusic(){Mix_HaltMusic();}

static unsigned char* __CBAS__loadSample(unsigned char* name){
	Mix_Chunk* v = NULL;
	
	v = Mix_LoadWAV((char*)name);
	if(v == NULL) 
	{exit(1);return NULL;}
	//samples[nsamples++] = v;
	return (unsigned char*)v;
}

static void __CBAS__freeSample(unsigned char* buf){
	Mix_Chunk* v = (Mix_Chunk*)buf;
    Mix_FreeChunk(v);
}

static int __CBAS__playSample(unsigned char* sampl, int loops){
    Mix_Chunk* samp= (Mix_Chunk*)sampl;
	return Mix_PlayChannel(-1, samp, loops);
}

static unsigned __CBAS__loadRGBTexture(unsigned char* buf, unsigned int w, unsigned int h) {
	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
	return t;
}

static unsigned __CBAS__loadRGBATexture(unsigned char* buf, unsigned int w, unsigned int h) {
	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	return t;
}







#define MKCONST(d) static inline unsigned __CBAS__KEY_##d() { return SDLK_##d; }
#define MKCONST2(d,b) static inline unsigned __CBAS__KEY_##d() {return SDLK_##b;}
MKCONST(0)
MKCONST(1)
MKCONST(2)
MKCONST(3)
MKCONST(4)
MKCONST(5)
MKCONST(6)
MKCONST(7)
MKCONST(8)
MKCONST(9)

MKCONST(a)
MKCONST2(A,a)
MKCONST(b)
MKCONST2(B,b)
MKCONST(c)
MKCONST2(C,c)
MKCONST(d)
MKCONST2(D,d)
MKCONST(e)
MKCONST2(E,e)
MKCONST(f)
MKCONST2(F,f)
MKCONST(g)
MKCONST2(G,g)
MKCONST(h)
MKCONST2(H,h)
MKCONST(i)
MKCONST2(I,i)
MKCONST(j)
MKCONST2(J,j)
MKCONST(k)
MKCONST2(K,k)
MKCONST(l)
MKCONST2(L,l)
MKCONST(m)
MKCONST2(M,m)
MKCONST(n)
MKCONST2(N,n)
MKCONST(o)
MKCONST2(O,o)
MKCONST(p)
MKCONST2(P,p)
MKCONST(q)
MKCONST2(Q,q)
MKCONST(r)
MKCONST2(R,r)
MKCONST(s)
MKCONST2(S,s)
MKCONST(t)
MKCONST2(T,t)
MKCONST(u)
MKCONST2(U,u)
MKCONST(v)
MKCONST2(V,v)
MKCONST(w)
MKCONST2(W,w)
MKCONST(x)
MKCONST2(X,x)
MKCONST(y)
MKCONST2(Y,y)
MKCONST(z)
MKCONST2(Z,z)

MKCONST(ALTERASE)
MKCONST(LSHIFT)
MKCONST(RSHIFT)
MKCONST(RETURN)
MKCONST(RETURN2)
MKCONST(TAB)
MKCONST(QUOTE)

MKCONST(BACKSLASH)
MKCONST(BACKSPACE)

MKCONST(CAPSLOCK)
MKCONST(COMMA)
MKCONST(UP)
MKCONST(DOWN)
MKCONST(LEFT)
MKCONST(RIGHT)
MKCONST(HOME)
MKCONST(END)
MKCONST(ESCAPE)
MKCONST(PAGEUP)
MKCONST(PAGEDOWN)
static inline unsigned __CBAS__KEY_PGDN(){ return SDLK_PAGEDOWN;}
static inline unsigned __CBAS__KEY_PGUP(){ return SDLK_PAGEUP;}
MKCONST(DELETE)
MKCONST(INSERT)
MKCONST(PERIOD)
MKCONST(F1)
MKCONST(F2)
MKCONST(F3)
MKCONST(F4)
MKCONST(F5)
MKCONST(F6)
MKCONST(F7)
MKCONST(F8)
MKCONST(F9)
MKCONST(F10)
MKCONST(F11)
MKCONST(F12)
MKCONST(BACKQUOTE)
static inline unsigned __CBAS__KEY_GRAVE(){ return SDLK_BACKQUOTE;}
MKCONST(KP_0)
MKCONST(KP_1)
MKCONST(KP_2)
MKCONST(KP_3)
MKCONST(KP_4)
MKCONST(KP_5)
MKCONST(KP_6)
MKCONST(KP_7)
MKCONST(KP_8)
MKCONST(KP_9)
MKCONST(KP_PERIOD)
MKCONST(KP_PLUS)
MKCONST(KP_MINUS)
MKCONST(KP_ENTER)
MKCONST(KP_MULTIPLY)
static inline unsigned __CBAS__KEY_KP_STAR(){return SDLK_KP_MULTIPLY;}
MKCONST(KP_DIVIDE)
static inline unsigned __CBAS__KEY_KP_SLASH(){return SDLK_KP_DIVIDE;}
MKCONST(SCROLLLOCK)
MKCONST(SEMICOLON)
MKCONST(SLASH)
MKCONST(RIGHTBRACKET)
MKCONST(LEFTBRACKET)
MKCONST(LEFTPAREN)
MKCONST(RIGHTPAREN)
MKCONST(LCTRL)
MKCONST(LALT)
MKCONST(RCTRL)
MKCONST(RALT)
MKCONST(SYSREQ)
MKCONST(EQUALS)
MKCONST(MINUS)
#undef MKCONST
#undef MKCONST2

    static int __CBAS__argc = 0;
    static unsigned char** __CBAS__argv = NULL;
    int main(int argc, char** argv){
    	__CBAS__argv = (unsigned char**)argv;
    	__CBAS__argc = argc;
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) < 0)
        {
            printf("SDL2 could not be initialized!\n"
                   "SDL_Error: %s\n", SDL_GetError());
            exit(1);
        }
    	Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
    	if (Mix_OpenAudio(44100,/*AUDIO_S16SYS*/ MIX_DEFAULT_FORMAT, 2, 1024) < 0){
            fprintf(stderr,"\r\nSDL2 mixer audio opening failed!\r\n"
            "SDL_Error: %s\r\n", SDL_GetError());
            exit(-1);
    	}
    	if(Mix_AllocateChannels(32) < 0){
            fprintf(stderr,"\r\nSDL2 mixer channel allocation failed!\r\n"
            "SDL_Error: %s\r\n", SDL_GetError());
            exit(-1);
    	}
    	__CBAS__appInit();
    	while(!shouldquit){
    		pollevents();
    		__CBAS__appUpdate();
    	}
    	__CBAS__appClose();
    	Mix_CloseAudio();
        SDL_Quit();
    }
    
/*
TYPEDECLS
*/
struct __CBAS__type__str{
    unsigned char*  __CBAS__member__d;
    unsigned long long  __CBAS__member__len;
} ;
struct __CBAS__type__pstr{
    unsigned char*  __CBAS__member__d;
    unsigned long long  __CBAS__member__len;
} ;
struct __CBAS__type__snake_segment{
    int  __CBAS__member__x;
    int  __CBAS__member__y;
} ;
/*
GLOBAL VARIABLES
*/
extern int __CBAS__width;
extern int __CBAS__height;
extern int __CBAS__display_scale;
extern int __CBAS__mousex;
extern int __CBAS__mousey;
extern int __CBAS__mousex_rel;
extern int __CBAS__mousey_rel;
extern int __CBAS__argc;
extern unsigned char** __CBAS__argv;
static unsigned char* __CBAS__ourFont = 0ull;
static unsigned char* __CBAS__eatSound = 0ull;
static unsigned char* __CBAS__music = 0ull;
static struct __CBAS__type__snake_segment __CBAS__segs[20ll];
static unsigned int __CBAS__snake_length = 2ull;
static int __CBAS__mode = -1ll;
static int __CBAS__direction = 3ll;
static struct __CBAS__type__snake_segment __CBAS__food;
static int __CBAS__stall = 0ll;
static int __CBAS__stored_direction_change = 0ll;
/*
FUNCTION PROTOTYPES
*/
static inline long long __CBAS__SEEK_SET();
static inline long long __CBAS__SEEK_CUR();
static inline long long __CBAS__SEEK_END();
static inline long long __CBAS__EOF();
static inline double __CBAS__PI();
static inline double __CBAS__M_PI();
static inline int __CBAS__E2BIG();
static inline int __CBAS__EACCES();
static inline int __CBAS__EADDRINUSE();
static inline int __CBAS__EADDRNOTAVAIL();
static inline int __CBAS__EAFNOSUPPORT();
static inline int __CBAS__EAGAIN();
static inline int __CBAS__EALREADY();
static inline int __CBAS__EBADF();
static inline int __CBAS__EBADMSG();
static inline int __CBAS__EBUSY();
static inline int __CBAS__ECANCELED();
static inline int __CBAS__ECHILD();
static inline int __CBAS__ECONNABORTED();
static inline int __CBAS__ECONNREFUSED();
static inline int __CBAS__ECONNRESET();
static inline int __CBAS__EDEADLK();
static inline int __CBAS__EDESTADDRREQ();
static inline int __CBAS__EDOM();
static inline int __CBAS__EEXIST();
static inline int __CBAS__EFAULT();
static inline int __CBAS__EFBIG();
static inline int __CBAS__EHOSTUNREACH();
static inline int __CBAS__EIDRM();
static inline int __CBAS__EILSEQ();
static inline int __CBAS__EINPROGRESS();
static inline int __CBAS__EINTR();
static inline int __CBAS__EINVAL();
static inline int __CBAS__EIO();
static inline int __CBAS__EISCONN();
static inline int __CBAS__EISDIR();
static inline int __CBAS__ELOOP();
static inline int __CBAS__EMFILE();
static inline int __CBAS__EMLINK();
static inline int __CBAS__EMSGSIZE();
static inline int __CBAS__ENAMETOOLONG();
static inline int __CBAS__ENETDOWN();
static inline int __CBAS__ENETRESET();
static inline int __CBAS__ENETUNREACH();
static inline int __CBAS__ENFILE();
static inline int __CBAS__ENOBUFS();
static inline int __CBAS__ENODEV();
static inline int __CBAS__ENOEXEC();
static inline int __CBAS__ENOLCK();
static inline int __CBAS__ENOLINK();
static inline int __CBAS__ENOMEM();
static inline int __CBAS__ENOMSG();
static inline int __CBAS__ENOPROTOOPT();
static inline int __CBAS__ENOSPC();
static inline int __CBAS__ENOSYS();
static inline int __CBAS__ENOTCONN();
static inline int __CBAS__ENOTDIR();
static inline int __CBAS__ENOTEMPTY();
static inline int __CBAS__ENOTRECOVERABLE();
static inline int __CBAS__ENOTSOCK();
static inline int __CBAS__ENOTSUP();
static inline int __CBAS__ENOTTY();
static inline int __CBAS__ENXIO();
static inline int __CBAS__EOPNOTSUPP();
static inline int __CBAS__EOVERFLOW();
static inline int __CBAS__EOWNERDEAD();
static inline int __CBAS__EPERM();
static inline int __CBAS__EPIPE();
static inline int __CBAS__EPROTO();
static inline int __CBAS__EPROTONOSUPPORT();
static inline int __CBAS__EPROTOTYPE();
static inline int __CBAS__ERANGE();
static inline int __CBAS__EROFS();
static inline int __CBAS__ESPIPE();
static inline int __CBAS__ESRCH();
static inline int __CBAS__ETIMEDOUT();
static inline int __CBAS__ETXTBSY();
static inline int __CBAS__EWOULDBLOCK();
static inline int __CBAS__EXDEV();
static inline void __CBAS__memcpy(unsigned char* __cbas_local_variable_mangled_name_dst,unsigned char* __cbas_local_variable_mangled_name_src,unsigned long long __cbas_local_variable_mangled_name_sz);
static inline void __CBAS__memmove(unsigned char* __cbas_local_variable_mangled_name_dst,unsigned char* __cbas_local_variable_mangled_name_src,unsigned long long __cbas_local_variable_mangled_name_sz);
static inline long long __CBAS__impl_streq(unsigned char* __cbas_local_variable_mangled_name_a,unsigned char* __cbas_local_variable_mangled_name_b);
static inline long long __CBAS__memeq(unsigned char* __cbas_local_variable_mangled_name_a,unsigned char* __cbas_local_variable_mangled_name_b,unsigned long long __cbas_local_variable_mangled_name_l);
static inline unsigned long long __CBAS__utf8_strlen(unsigned char* __cbas_local_variable_mangled_name_s);
static inline long long __CBAS__utf8_isascii(unsigned char* __cbas_local_variable_mangled_name_s);
static inline long long __CBAS__utf8_getchar(unsigned char* __cbas_local_variable_mangled_name_dest,unsigned char* __cbas_local_variable_mangled_name_src,unsigned char** __cbas_local_variable_mangled_name_endptr,long long* __cbas_local_variable_mangled_name_lenout);
static inline void __CBAS____method_str_____ctor(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this);
static inline void __CBAS____method_str_____dtor(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this);
static inline void __CBAS____method_pstr_____ctor(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this);
static inline void __CBAS____method_pstr_____dtor(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this);
static inline void __CBAS____method_str_____free(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this);
static inline void __CBAS____method_pstr_____free(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this);
static inline long long __CBAS____method_str_____equal(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline long long __CBAS____method_str_____nequal(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline long long __CBAS____method_pstr_____equal(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline long long __CBAS____method_pstr_____nequal(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline struct __CBAS__type__str* __CBAS____method_str_____copy(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline struct __CBAS__type__str* __CBAS____method_str_____frompstr(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____copy(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____fromstr(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline struct __CBAS__type__str* __CBAS____method_str_____new(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,unsigned char* __cbas_local_variable_mangled_name_o);
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____new(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,unsigned char* __cbas_local_variable_mangled_name_o);
static inline void __CBAS____method_str_____copyto(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline void __CBAS____method_pstr_____copyto(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline void __CBAS____method_str_____move(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline void __CBAS____method_pstr_____move(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline void __CBAS____method_str_____moveto(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline void __CBAS____method_pstr_____moveto(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline struct __CBAS__type__str* __CBAS____method_str_____clone(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this);
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____clone(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this);
static inline unsigned long long __CBAS____method_str_____isnull(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this);
static inline unsigned long long __CBAS____method_pstr_____isnull(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this);
static inline unsigned long long __CBAS____method_str_____isempty(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this);
static inline unsigned long long __CBAS____method_str_____empty(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this);
static inline unsigned long long __CBAS____method_pstr_____isempty(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this);
static inline unsigned long long __CBAS____method_pstr_____empty(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this);
static inline unsigned long long __CBAS____method_str_____prefixed(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline unsigned long long __CBAS____method_pstr_____prefixed(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline unsigned long long __CBAS____method_str_____postfixed(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline unsigned long long __CBAS____method_pstr_____postfixed(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline long long __CBAS____method_str_____findfrom(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_findme,unsigned long long __cbas_local_variable_mangled_name_where);
static inline long long __CBAS____method_pstr_____findfrom(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_findme,unsigned long long __cbas_local_variable_mangled_name_where);
static inline long long __CBAS____method_str_____find(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_findme);
static inline long long __CBAS____method_pstr_____find(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_findme);
static inline unsigned long long __CBAS____method_str_____add(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o);
static inline unsigned long long __CBAS____method_pstr_____add(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o);
static inline unsigned long long __CBAS____method_str_____addc(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,unsigned char* __cbas_local_variable_mangled_name_oo);
static inline unsigned long long __CBAS____method_pstr_____addc(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,unsigned char* __cbas_local_variable_mangled_name_oo);
static inline struct __CBAS__type__str* __CBAS____method_str_____substr(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,unsigned long long __cbas_local_variable_mangled_name_beginning,unsigned long long __cbas_local_variable_mangled_name_howmuch);
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____substr(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,unsigned long long __cbas_local_variable_mangled_name_beginning,unsigned long long __cbas_local_variable_mangled_name_howmuch);
static inline long long __CBAS____method_str_____replace_first_from(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_what,struct __CBAS__type__str* __cbas_local_variable_mangled_name_with,unsigned long long __cbas_local_variable_mangled_name_maxwhere);
static inline long long __CBAS____method_pstr_____replace_first_from(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_what,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_with,unsigned long long __cbas_local_variable_mangled_name_maxwhere);
static inline long long __CBAS____method_str_____replace_first(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_what,struct __CBAS__type__str* __cbas_local_variable_mangled_name_with);
static inline long long __CBAS____method_pstr_____replace_first(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_what,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_with);
static inline long long __CBAS____method_str_____replace(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_what,struct __CBAS__type__str* __cbas_local_variable_mangled_name_with);
static inline long long __CBAS____method_pstr_____replace(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_what,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_with);
static inline void __CBAS__utoa(unsigned char* __cbas_local_variable_mangled_name_dest,unsigned long long __cbas_local_variable_mangled_name_value);
static inline void __CBAS__itoa(unsigned char* __cbas_local_variable_mangled_name_dest,long long __cbas_local_variable_mangled_name_value);
static inline void __CBAS__ftoa_n(unsigned char* __cbas_local_variable_mangled_name_dest,double __cbas_local_variable_mangled_name_v,int __cbas_local_variable_mangled_name_ndigits);
static inline void __CBAS__ftoa(unsigned char* __cbas_local_variable_mangled_name_dest,double __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glAccum(unsigned int __cbas_local_variable_mangled_name_op,float __cbas_local_variable_mangled_name_value);
static inline void __CBAS__glAlphaFunc(unsigned int __cbas_local_variable_mangled_name_funk,float __cbas_local_variable_mangled_name_ref);
static inline unsigned char __CBAS__glAreTexturesResident(unsigned int __cbas_local_variable_mangled_name_n,unsigned int* __cbas_local_variable_mangled_name_textures,unsigned char* __cbas_local_variable_mangled_name_residences);
static inline void __CBAS__glArrayElement(int __cbas_local_variable_mangled_name_i);
static inline void __CBAS__glBegin(unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glEnd();
static inline void __CBAS__glBindTexture(unsigned int __cbas_local_variable_mangled_name_target,unsigned int __cbas_local_variable_mangled_name_texture);
static inline void __CBAS__glBitmap(int __cbas_local_variable_mangled_name_width,int __cbas_local_variable_mangled_name_height,int __cbas_local_variable_mangled_name_xorig,int __cbas_local_variable_mangled_name_yorig,int __cbas_local_variable_mangled_name_xmove,int __cbas_local_variable_mangled_name_ymove,unsigned char* __cbas_local_variable_mangled_name_bitmap);
static inline void __CBAS__glBlendFunc(unsigned int __cbas_local_variable_mangled_name_sfactor,unsigned int __cbas_local_variable_mangled_name_dfactor);
static inline void __CBAS__glCallList(unsigned int __cbas_local_variable_mangled_name_l);
static inline void __CBAS__glCallLists(unsigned int __cbas_local_variable_mangled_name_n,unsigned int __cbas_local_variable_mangled_name_type,unsigned char* __cbas_local_variable_mangled_name_lists);
static inline void __CBAS__glClear(unsigned int __cbas_local_variable_mangled_name_mask);
static inline void __CBAS__glClearAccum(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b,float __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glClearColor(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b,float __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glClearDepth(double __cbas_local_variable_mangled_name_d);
static inline void __CBAS__glClearIndex(float __cbas_local_variable_mangled_name_d);
static inline void __CBAS__glClearStencil(int __cbas_local_variable_mangled_name_d);
static inline void __CBAS__glClipPlane(unsigned int __cbas_local_variable_mangled_name_p,double* __cbas_local_variable_mangled_name_equa);
static inline void __CBAS__glColor3b(char __cbas_local_variable_mangled_name_r,char __cbas_local_variable_mangled_name_g,char __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glColor3d(double __cbas_local_variable_mangled_name_r,double __cbas_local_variable_mangled_name_g,double __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glColor3f(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glColor3i(int __cbas_local_variable_mangled_name_r,int __cbas_local_variable_mangled_name_g,int __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glColor3ub(unsigned char __cbas_local_variable_mangled_name_r,unsigned char __cbas_local_variable_mangled_name_g,unsigned char __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glColor3ui(unsigned int __cbas_local_variable_mangled_name_r,unsigned int __cbas_local_variable_mangled_name_g,unsigned int __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glColor3us(unsigned short __cbas_local_variable_mangled_name_r,unsigned short __cbas_local_variable_mangled_name_g,unsigned short __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glColor3s(signed short __cbas_local_variable_mangled_name_r,signed short __cbas_local_variable_mangled_name_g,signed short __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glColor4b(char __cbas_local_variable_mangled_name_r,char __cbas_local_variable_mangled_name_g,char __cbas_local_variable_mangled_name_b,char __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColor4d(double __cbas_local_variable_mangled_name_r,double __cbas_local_variable_mangled_name_g,double __cbas_local_variable_mangled_name_b,double __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColor4f(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b,float __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColor4i(int __cbas_local_variable_mangled_name_r,int __cbas_local_variable_mangled_name_g,int __cbas_local_variable_mangled_name_b,int __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColor4s(signed short __cbas_local_variable_mangled_name_r,signed short __cbas_local_variable_mangled_name_g,signed short __cbas_local_variable_mangled_name_b,signed short __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColor4ub(unsigned char __cbas_local_variable_mangled_name_r,unsigned char __cbas_local_variable_mangled_name_g,unsigned char __cbas_local_variable_mangled_name_b,unsigned char __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColor4ui(unsigned int __cbas_local_variable_mangled_name_r,unsigned int __cbas_local_variable_mangled_name_g,unsigned int __cbas_local_variable_mangled_name_b,unsigned int __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColor4us(unsigned short __cbas_local_variable_mangled_name_r,unsigned short __cbas_local_variable_mangled_name_g,unsigned short __cbas_local_variable_mangled_name_b,unsigned short __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColor3bv(char* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor3ubv(unsigned char* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor3dv(double* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor3fv(float* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor3iv(int* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor3uiv(unsigned int* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor3sv(signed short* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor3usv(unsigned short* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor4bv(char* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor4ubv(unsigned char* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor4dv(double* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor4fv(float* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor4iv(int* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor4uiv(unsigned int* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor4sv(signed short* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColor4usv(unsigned short* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glColorMask(unsigned char __cbas_local_variable_mangled_name_r,unsigned char __cbas_local_variable_mangled_name_g,unsigned char __cbas_local_variable_mangled_name_b,unsigned char __cbas_local_variable_mangled_name_a);
static inline void __CBAS__glColorMaterial(unsigned int __cbas_local_variable_mangled_name_face,unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glColorPointer(int __cbas_local_variable_mangled_name_sz,unsigned int __cbas_local_variable_mangled_name_type,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glCopyPixels(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_t);
static inline void __CBAS__glCopyTexImage1D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_internalfmt,int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_width,int __cbas_local_variable_mangled_name_border);
static inline void __CBAS__glCopyTexSubImage1D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_xoff,int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_width);
static inline void __CBAS__glCopyTexImage2D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_internalfmt,int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_width,unsigned int __cbas_local_variable_mangled_name_height,int __cbas_local_variable_mangled_name_border);
static inline void __CBAS__glCopyTexSubImage2D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_xoff,int __cbas_local_variable_mangled_name_yoff,int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_width,unsigned int __cbas_local_variable_mangled_name_height);
static inline void __CBAS__glCullFace(unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glDeleteLists(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_range);
static inline void __CBAS__glDeleteTextures(unsigned int __cbas_local_variable_mangled_name_n,unsigned int* __cbas_local_variable_mangled_name_textures);
static inline void __CBAS__glDepthFunc(unsigned int __cbas_local_variable_mangled_name_funk);
static inline void __CBAS__glDepthMask(unsigned char __cbas_local_variable_mangled_name_flag);
static inline void __CBAS__glDepthRange(double __cbas_local_variable_mangled_name_zNear,double __cbas_local_variable_mangled_name_zFar);
static inline void __CBAS__glDrawArrays(unsigned int __cbas_local_variable_mangled_name_mode,int __cbas_local_variable_mangled_name_first,unsigned int __cbas_local_variable_mangled_name_count);
static inline void __CBAS__glDrawBuffer(unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glDrawElements(unsigned int __cbas_local_variable_mangled_name_mode,unsigned int __cbas_local_variable_mangled_name_count,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_indices);
static inline void __CBAS__glDrawPixels(unsigned int __cbas_local_variable_mangled_name_width,unsigned int __cbas_local_variable_mangled_name_height,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glEdgeFlag(unsigned char __cbas_local_variable_mangled_name_flag);
static inline void __CBAS__glEdgeFlagv(unsigned char* __cbas_local_variable_mangled_name_flag);
static inline void __CBAS__glEdgeFlagPointer(unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glEnable(unsigned int __cbas_local_variable_mangled_name_cap);
static inline void __CBAS__glDisable(unsigned int __cbas_local_variable_mangled_name_cap);
static inline void __CBAS__glEnableClientState(unsigned int __cbas_local_variable_mangled_name_cap);
static inline void __CBAS__glDisableClientState(unsigned int __cbas_local_variable_mangled_name_cap);
static inline void __CBAS__glEvalCoord1d(double __cbas_local_variable_mangled_name_u);
static inline void __CBAS__glEvalCoord1f(float __cbas_local_variable_mangled_name_u);
static inline void __CBAS__glEvalCoord2d(double __cbas_local_variable_mangled_name_u,double __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glEvalCoord2f(float __cbas_local_variable_mangled_name_u,float __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glEvalCoord1dv(double* __cbas_local_variable_mangled_name_u);
static inline void __CBAS__glEvalCoord1fv(float* __cbas_local_variable_mangled_name_u);
static inline void __CBAS__glEvalCoord2dv(double* __cbas_local_variable_mangled_name_u);
static inline void __CBAS__glEvalCoord2fv(float* __cbas_local_variable_mangled_name_u);
static inline void __CBAS__glEvalMesh1(unsigned int __cbas_local_variable_mangled_name_mode,int __cbas_local_variable_mangled_name_i1,int __cbas_local_variable_mangled_name_i2);
static inline void __CBAS__glEvalMesh2(unsigned int __cbas_local_variable_mangled_name_mode,int __cbas_local_variable_mangled_name_i1,int __cbas_local_variable_mangled_name_i2,int __cbas_local_variable_mangled_name_j1,int __cbas_local_variable_mangled_name_j2);
static inline void __CBAS__glEvalPoint1(int __cbas_local_variable_mangled_name_i);
static inline void __CBAS__glEvalPoint2(int __cbas_local_variable_mangled_name_i,int __cbas_local_variable_mangled_name_j);
static inline void __CBAS__glFeedbackBuffer(unsigned int __cbas_local_variable_mangled_name_sz,unsigned int __cbas_local_variable_mangled_name_t,float* __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glFinish();
static inline void __CBAS__glFlush();
static inline void __CBAS__glFogf(unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glFogi(unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glFogfv(unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glFogiv(unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glFrontFace(unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glFrustum(double __cbas_local_variable_mangled_name_left,double __cbas_local_variable_mangled_name_right,double __cbas_local_variable_mangled_name_bottom,double __cbas_local_variable_mangled_name_top,double __cbas_local_variable_mangled_name_zNear,double __cbas_local_variable_mangled_name_zFar);
static inline void __CBAS__glPerspective(double __cbas_local_variable_mangled_name_fov,double __cbas_local_variable_mangled_name_aspect,double __cbas_local_variable_mangled_name_zn,double __cbas_local_variable_mangled_name_zf);
static inline unsigned int __CBAS__glGenLists(unsigned int __cbas_local_variable_mangled_name_range);
static inline void __CBAS__glGenTextures(unsigned int __cbas_local_variable_mangled_name_n,unsigned int* __cbas_local_variable_mangled_name_arr);
static inline void __CBAS__glGetBooleanv(unsigned int __cbas_local_variable_mangled_name_pname,unsigned char* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetDoublev(unsigned int __cbas_local_variable_mangled_name_pname,double* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetFloatv(unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetIntegerv(unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetClipPlane(unsigned int __cbas_local_variable_mangled_name_p,double* __cbas_local_variable_mangled_name_equa);
static inline unsigned int __CBAS__glGetError();
static inline void __CBAS__glGetLightfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetLightiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetMapdv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,double* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetMapfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetMapiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetMaterialfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetMaterialiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetPixelMapfv(unsigned int __cbas_local_variable_mangled_name_l,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetPixelMapuiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetPixelMapusv(unsigned int __cbas_local_variable_mangled_name_l,unsigned short* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetPointerv(unsigned int __cbas_local_variable_mangled_name_pname,unsigned char* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetPolygonStipple(unsigned char* __cbas_local_variable_mangled_name_mask);
static inline unsigned char* __CBAS__glGetString(unsigned int __cbas_local_variable_mangled_name_nm);
static inline void __CBAS__glGetTexEnvfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetTexEnviv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetTexGendv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,double* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetTexGenfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetTexGeniv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetTexImage(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_px);
static inline void __CBAS__glGetTexLevelParameterfv(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetTexLevelParameteriv(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetTexParameterfv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glGetTexParameteriv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params);
static inline void __CBAS__glHint(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_md);
static inline void __CBAS__glIndexd(double __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexf(float __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexi(int __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexs(signed short __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexub(unsigned char __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexdv(double* __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexfv(float* __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexiv(int* __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexsv(signed short* __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexubv(unsigned char* __cbas_local_variable_mangled_name_c);
static inline void __CBAS__glIndexMask(unsigned int __cbas_local_variable_mangled_name_m);
static inline void __CBAS__glIndexPointer(unsigned int __cbas_local_variable_mangled_name_t,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glInitNames();
static inline void __CBAS__glInterleavedArrays(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p);
static inline unsigned char __CBAS__glIsEnabled(unsigned int __cbas_local_variable_mangled_name_cap);
static inline unsigned char __CBAS__glIsList(unsigned int __cbas_local_variable_mangled_name_cap);
static inline unsigned char __CBAS__glIsTexture(unsigned int __cbas_local_variable_mangled_name_cap);
static inline void __CBAS__glLightf(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glLighti(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glLightfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glLightiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glLightModelf(unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glLightModeli(unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glLightModelfv(unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glLightModeliv(unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glLineStipple(int __cbas_local_variable_mangled_name_factor,unsigned short __cbas_local_variable_mangled_name_pattern);
static inline void __CBAS__glLineWidth(float __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glListBase(unsigned int __cbas_local_variable_mangled_name_b);
static inline void __CBAS__glLoadIdentity();
static inline void __CBAS__glLoadMatrixd(double* __cbas_local_variable_mangled_name_m);
static inline void __CBAS__glLoadMatrixf(float* __cbas_local_variable_mangled_name_m);
static inline void __CBAS__glLoadName(unsigned int __cbas_local_variable_mangled_name_n);
static inline void __CBAS__glLogicOp(unsigned int __cbas_local_variable_mangled_name_n);
static inline void __CBAS__glMap1d(unsigned int __cbas_local_variable_mangled_name_tg,double __cbas_local_variable_mangled_name_u1,double __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_stride,int __cbas_local_variable_mangled_name_order,double* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glMap1f(unsigned int __cbas_local_variable_mangled_name_tg,float __cbas_local_variable_mangled_name_u1,float __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_stride,int __cbas_local_variable_mangled_name_order,float* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glMap2d(unsigned int __cbas_local_variable_mangled_name_tg,double __cbas_local_variable_mangled_name_u1,double __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_ustride,int __cbas_local_variable_mangled_name_uorder,double __cbas_local_variable_mangled_name_v1,double __cbas_local_variable_mangled_name_v2,int __cbas_local_variable_mangled_name_vstride,int __cbas_local_variable_mangled_name_vorder,double* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glMap2f(unsigned int __cbas_local_variable_mangled_name_tg,float __cbas_local_variable_mangled_name_u1,float __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_ustride,int __cbas_local_variable_mangled_name_uorder,float __cbas_local_variable_mangled_name_v1,float __cbas_local_variable_mangled_name_v2,int __cbas_local_variable_mangled_name_vstride,int __cbas_local_variable_mangled_name_vorder,float* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glMapGrid1d(int __cbas_local_variable_mangled_name_un,double __cbas_local_variable_mangled_name_u1,double __cbas_local_variable_mangled_name_u2);
static inline void __CBAS__glMapGrid1f(int __cbas_local_variable_mangled_name_un,float __cbas_local_variable_mangled_name_u1,float __cbas_local_variable_mangled_name_u2);
static inline void __CBAS__glMapGrid2d(int __cbas_local_variable_mangled_name_un,double __cbas_local_variable_mangled_name_u1,double __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_vn,double __cbas_local_variable_mangled_name_v1,double __cbas_local_variable_mangled_name_v2);
static inline void __CBAS__glMapGrid2f(int __cbas_local_variable_mangled_name_un,float __cbas_local_variable_mangled_name_u1,float __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_vn,float __cbas_local_variable_mangled_name_v1,float __cbas_local_variable_mangled_name_v2);
static inline void __CBAS__glMaterialf(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glMateriali(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glMaterialfv(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glMaterialiv(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glMatrixMode(unsigned int __cbas_local_variable_mangled_name_n);
static inline void __CBAS__glMultMatrixd(double* __cbas_local_variable_mangled_name_m);
static inline void __CBAS__glMultMatrixf(float* __cbas_local_variable_mangled_name_m);
static inline void __CBAS__glNewList(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_m);
static inline void __CBAS__glEndList();
static inline void __CBAS__glNormal3bv(char* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glNormal3dv(double* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glNormal3fv(float* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glNormal3iv(int* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glNormal3sv(signed short* __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glNormal3b(char __cbas_local_variable_mangled_name_x,char __cbas_local_variable_mangled_name_y,char __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glNormal3d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glNormal3f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glNormal3i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glNormal3s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glNormalPointer(unsigned int __cbas_local_variable_mangled_name_t,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glOrtho(double __cbas_local_variable_mangled_name_left,double __cbas_local_variable_mangled_name_right,double __cbas_local_variable_mangled_name_bottom,double __cbas_local_variable_mangled_name_top,double __cbas_local_variable_mangled_name_zNear,double __cbas_local_variable_mangled_name_zFar);
static inline void __CBAS__glPixelMapfv(unsigned int __cbas_local_variable_mangled_name_m,unsigned int __cbas_local_variable_mangled_name_msz,float* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glPixelMapuiv(unsigned int __cbas_local_variable_mangled_name_m,unsigned int __cbas_local_variable_mangled_name_msz,unsigned int* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glPixelMapusv(unsigned int __cbas_local_variable_mangled_name_m,unsigned int __cbas_local_variable_mangled_name_msz,unsigned short* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glPixelStoref(unsigned int __cbas_local_variable_mangled_name_m,float __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glPixelStorei(unsigned int __cbas_local_variable_mangled_name_m,int __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glPixelTransferf(unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glPixelTransferi(unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glPixelZoom(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glPointSize(float __cbas_local_variable_mangled_name_sz);
static inline void __CBAS__glPolygonMode(unsigned int __cbas_local_variable_mangled_name_face,unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glPolygonOffset(float __cbas_local_variable_mangled_name_factor,float __cbas_local_variable_mangled_name_units);
static inline void __CBAS__glPolygonStipple(unsigned char* __cbas_local_variable_mangled_name_mask);
static inline void __CBAS__glPrioritizeTextures(unsigned int __cbas_local_variable_mangled_name_n,unsigned int* __cbas_local_variable_mangled_name_textures,float* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glPushAttrib(unsigned int __cbas_local_variable_mangled_name_mask);
static inline void __CBAS__glPopAttrib();
static inline void __CBAS__glPushClientAttrib(unsigned int __cbas_local_variable_mangled_name_mask);
static inline void __CBAS__glPopClientAttrib();
static inline void __CBAS__glPushMatrix();
static inline void __CBAS__glPopMatrix();
static inline void __CBAS__glPushName(unsigned int __cbas_local_variable_mangled_name_name);
static inline void __CBAS__glPopName();
static inline void __CBAS__glRasterPos2d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glRasterPos2f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glRasterPos2i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glRasterPos2s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glRasterPos3d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glRasterPos3f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glRasterPos3i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glRasterPos3s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glRasterPos4d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z,double __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glRasterPos4f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z,float __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glRasterPos4i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z,int __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glRasterPos4s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z,signed short __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glRasterPos2dv(double* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos2fv(float* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos2iv(int* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos2sv(signed short* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos3dv(double* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos3fv(float* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos3iv(int* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos3sv(signed short* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos4dv(double* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos4fv(float* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos4iv(int* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glRasterPos4sv(signed short* __cbas_local_variable_mangled_name_x);
static inline void __CBAS__glReadBuffer(unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glReadPixels(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_px);
static inline void __CBAS__glRectd(double __cbas_local_variable_mangled_name_x1,double __cbas_local_variable_mangled_name_y1,double __cbas_local_variable_mangled_name_x2,double __cbas_local_variable_mangled_name_y2);
static inline void __CBAS__glRectf(float __cbas_local_variable_mangled_name_x1,float __cbas_local_variable_mangled_name_y1,float __cbas_local_variable_mangled_name_x2,float __cbas_local_variable_mangled_name_y2);
static inline void __CBAS__glRecti(int __cbas_local_variable_mangled_name_x1,int __cbas_local_variable_mangled_name_y1,int __cbas_local_variable_mangled_name_x2,int __cbas_local_variable_mangled_name_y2);
static inline void __CBAS__glRects(signed short __cbas_local_variable_mangled_name_x1,signed short __cbas_local_variable_mangled_name_y1,signed short __cbas_local_variable_mangled_name_x2,signed short __cbas_local_variable_mangled_name_y2);
static inline void __CBAS__glRectdv(double* __cbas_local_variable_mangled_name_v1,double* __cbas_local_variable_mangled_name_v2);
static inline void __CBAS__glRectfv(float* __cbas_local_variable_mangled_name_v1,float* __cbas_local_variable_mangled_name_v2);
static inline void __CBAS__glRectiv(int* __cbas_local_variable_mangled_name_v1,int* __cbas_local_variable_mangled_name_v2);
static inline void __CBAS__glRectsv(signed short* __cbas_local_variable_mangled_name_v1,signed short* __cbas_local_variable_mangled_name_v2);
static inline void __CBAS__glRenderMode(unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glRotated(double __cbas_local_variable_mangled_name_angle,double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glRotatef(float __cbas_local_variable_mangled_name_angle,float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glScaled(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glScalef(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glScissor(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h);
static inline void __CBAS__glSelectBuffer(unsigned int __cbas_local_variable_mangled_name_sz,unsigned int* __cbas_local_variable_mangled_name_buf);
static inline void __CBAS__glShadeModel(unsigned int __cbas_local_variable_mangled_name_mode);
static inline void __CBAS__glStencilFunc(unsigned int __cbas_local_variable_mangled_name_funk,int __cbas_local_variable_mangled_name_ref,unsigned int __cbas_local_variable_mangled_name_mask);
static inline void __CBAS__glStencilMask(unsigned int __cbas_local_variable_mangled_name_mask);
static inline void __CBAS__glStencilOp(unsigned int __cbas_local_variable_mangled_name_fail,unsigned int __cbas_local_variable_mangled_name_zfail,unsigned int __cbas_local_variable_mangled_name_zpass);
static inline void __CBAS__glTexCoord1d(double __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord1f(float __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord1i(int __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord1s(signed short __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord2d(double __cbas_local_variable_mangled_name_s,double __cbas_local_variable_mangled_name_t);
static inline void __CBAS__glTexCoord2f(float __cbas_local_variable_mangled_name_s,float __cbas_local_variable_mangled_name_t);
static inline void __CBAS__glTexCoord2i(int __cbas_local_variable_mangled_name_s,int __cbas_local_variable_mangled_name_t);
static inline void __CBAS__glTexCoord2s(signed short __cbas_local_variable_mangled_name_s,signed short __cbas_local_variable_mangled_name_t);
static inline void __CBAS__glTexCoord3d(double __cbas_local_variable_mangled_name_s,double __cbas_local_variable_mangled_name_t,double __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glTexCoord3f(float __cbas_local_variable_mangled_name_s,float __cbas_local_variable_mangled_name_t,float __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glTexCoord3i(int __cbas_local_variable_mangled_name_s,int __cbas_local_variable_mangled_name_t,int __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glTexCoord3s(signed short __cbas_local_variable_mangled_name_s,signed short __cbas_local_variable_mangled_name_t,signed short __cbas_local_variable_mangled_name_r);
static inline void __CBAS__glTexCoord4d(double __cbas_local_variable_mangled_name_s,double __cbas_local_variable_mangled_name_t,double __cbas_local_variable_mangled_name_r,double __cbas_local_variable_mangled_name_q);
static inline void __CBAS__glTexCoord4f(float __cbas_local_variable_mangled_name_s,float __cbas_local_variable_mangled_name_t,float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_q);
static inline void __CBAS__glTexCoord4i(int __cbas_local_variable_mangled_name_s,int __cbas_local_variable_mangled_name_t,int __cbas_local_variable_mangled_name_r,int __cbas_local_variable_mangled_name_q);
static inline void __CBAS__glTexCoord4s(signed short __cbas_local_variable_mangled_name_s,signed short __cbas_local_variable_mangled_name_t,signed short __cbas_local_variable_mangled_name_r,signed short __cbas_local_variable_mangled_name_q);
static inline void __CBAS__glTexCoord1dv(double* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord1fv(float* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord1iv(int* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord1sv(signed short* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord2dv(double* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord2fv(float* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord2iv(int* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord2sv(signed short* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord3dv(double* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord3fv(float* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord3iv(int* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord3sv(signed short* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord4dv(double* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord4fv(float* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord4iv(int* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoord4sv(signed short* __cbas_local_variable_mangled_name_s);
static inline void __CBAS__glTexCoordPointer(int __cbas_local_variable_mangled_name_sz,unsigned int __cbas_local_variable_mangled_name_t,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glTexEnvf(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexEnvi(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexEnvfv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexEnviv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexGend(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,double __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexGenf(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexGeni(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexGendv(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,double* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexGenfv(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexGeniv(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexImage1D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_internalfmt,unsigned int __cbas_local_variable_mangled_name_width,int __cbas_local_variable_mangled_name_border,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_px);
static inline void __CBAS__glTexImage2D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_internalfmt,unsigned int __cbas_local_variable_mangled_name_width,unsigned int __cbas_local_variable_mangled_name_height,int __cbas_local_variable_mangled_name_border,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_px);
static inline void __CBAS__glTexParameterf(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexParameteri(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexParameterfv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexParameteriv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param);
static inline void __CBAS__glTexSubImage1D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_xoff,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glTexSubImage2D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_xoff,int __cbas_local_variable_mangled_name_yoff,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glTranslated(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glTranslatef(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glVertex2d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glVertex2f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glVertex2i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glVertex2s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y);
static inline void __CBAS__glVertex3d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glVertex3f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glVertex3i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glVertex3s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z);
static inline void __CBAS__glVertex4d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z,double __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glVertex4f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z,float __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glVertex4i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z,int __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glVertex4s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z,signed short __cbas_local_variable_mangled_name_w);
static inline void __CBAS__glVertex2dv(double* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex2fv(float* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex2iv(int* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex2sv(signed short* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex3dv(double* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex3fv(float* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex3iv(int* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex3sv(signed short* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex4dv(double* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex4fv(float* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex4iv(int* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertex4sv(signed short* __cbas_local_variable_mangled_name_v);
static inline void __CBAS__glVertexPointer(int __cbas_local_variable_mangled_name_sz,unsigned int __cbas_local_variable_mangled_name_t,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__glViewport(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h);
static inline unsigned int __CBAS__GL_2D();
static inline unsigned int __CBAS__GL_3D();
static inline unsigned int __CBAS__GL_3D_COLOR();
static inline unsigned int __CBAS__GL_3D_COLOR_TEXTURE();
static inline unsigned int __CBAS__GL_4D_COLOR_TEXTURE();
static inline unsigned int __CBAS__GL_2_BYTES();
static inline unsigned int __CBAS__GL_3_BYTES();
static inline unsigned int __CBAS__GL_4_BYTES();
static inline unsigned int __CBAS__GL_ACCUM();
static inline unsigned int __CBAS__GL_ACCUM_ALPHA_BITS();
static inline unsigned int __CBAS__GL_ACCUM_BLUE_BITS();
static inline unsigned int __CBAS__GL_ACCUM_BUFFER_BIT();
static inline unsigned int __CBAS__GL_ACCUM_CLEAR_VALUE();
static inline unsigned int __CBAS__GL_ACCUM_GREEN_BITS();
static inline unsigned int __CBAS__GL_ACCUM_RED_BITS();
static inline unsigned int __CBAS__GL_ADD();
static inline unsigned int __CBAS__GL_ALL_ATTRIB_BITS();
static inline unsigned int __CBAS__GL_ALPHA();
static inline unsigned int __CBAS__GL_ALPHA_BIAS();
static inline unsigned int __CBAS__GL_ALPHA_BITS();
static inline unsigned int __CBAS__GL_ALPHA_SCALE();
static inline unsigned int __CBAS__GL_ALPHA_TEST();
static inline unsigned int __CBAS__GL_ALPHA_TEST_FUNC();
static inline unsigned int __CBAS__GL_ALPHA_TEST_REF();
static inline unsigned int __CBAS__GL_ALWAYS();
static inline unsigned int __CBAS__GL_AMBIENT();
static inline unsigned int __CBAS__GL_AMBIENT_AND_DIFFUSE();
static inline unsigned int __CBAS__GL_AND();
static inline unsigned int __CBAS__GL_AND_INVERTED();
static inline unsigned int __CBAS__GL_AND_REVERSE();
static inline unsigned int __CBAS__GL_ATTRIB_STACK_DEPTH();
static inline unsigned int __CBAS__GL_AUTO_NORMAL();
static inline unsigned int __CBAS__GL_AUX0();
static inline unsigned int __CBAS__GL_AUX1();
static inline unsigned int __CBAS__GL_AUX2();
static inline unsigned int __CBAS__GL_AUX3();
static inline unsigned int __CBAS__GL_AUX_BUFFERS();
static inline unsigned int __CBAS__GL_BACK();
static inline unsigned int __CBAS__GL_BACK_LEFT();
static inline unsigned int __CBAS__GL_BACK_RIGHT();
static inline unsigned int __CBAS__GL_BITMAP();
static inline unsigned int __CBAS__GL_BITMAP_TOKEN();
static inline unsigned int __CBAS__GL_BLEND();
static inline unsigned int __CBAS__GL_BLEND_DST();
static inline unsigned int __CBAS__GL_BLEND_SRC();
static inline unsigned int __CBAS__GL_BLUE();
static inline unsigned int __CBAS__GL_BLUE_BIAS();
static inline unsigned int __CBAS__GL_BLUE_BITS();
static inline unsigned int __CBAS__GL_BLUE_SCALE();
static inline unsigned int __CBAS__GL_BYTE();
static inline unsigned int __CBAS__GL_CCW();
static inline unsigned int __CBAS__GL_CLAMP();
static inline unsigned int __CBAS__GL_CLEAR();
static inline unsigned int __CBAS__GL_CLIP_PLANE0();
static inline unsigned int __CBAS__GL_CLIP_PLANE1();
static inline unsigned int __CBAS__GL_CLIP_PLANE2();
static inline unsigned int __CBAS__GL_CLIP_PLANE3();
static inline unsigned int __CBAS__GL_CLIP_PLANE4();
static inline unsigned int __CBAS__GL_CLIP_PLANE5();
static inline unsigned int __CBAS__GL_COEFF();
static inline unsigned int __CBAS__GL_COLOR();
static inline unsigned int __CBAS__GL_COLOR_BUFFER_BIT();
static inline unsigned int __CBAS__GL_COLOR_CLEAR_VALUE();
static inline unsigned int __CBAS__GL_COLOR_INDEX();
static inline unsigned int __CBAS__GL_COLOR_INDEXES();
static inline unsigned int __CBAS__GL_COLOR_MATERIAL();
static inline unsigned int __CBAS__GL_COLOR_MATERIAL_FACE();
static inline unsigned int __CBAS__GL_COLOR_MATERIAL_PARAMETER();
static inline unsigned int __CBAS__GL_COLOR_WRITEMASK();
static inline unsigned int __CBAS__GL_COMPILE();
static inline unsigned int __CBAS__GL_COMPILE_AND_EXECUTE();
static inline unsigned int __CBAS__GL_CONSTANT_ATTENUATION();
static inline unsigned int __CBAS__GL_COPY();
static inline unsigned int __CBAS__GL_COPY_INVERTED();
static inline unsigned int __CBAS__GL_COPY_PIXEL_TOKEN();
static inline unsigned int __CBAS__GL_CULL_FACE();
static inline unsigned int __CBAS__GL_CULL_FACE_MODE();
static inline unsigned int __CBAS__GL_CURRENT_BIT();
static inline unsigned int __CBAS__GL_CURRENT_COLOR();
static inline unsigned int __CBAS__GL_CURRENT_INDEX();
static inline unsigned int __CBAS__GL_CURRENT_NORMAL();
static inline unsigned int __CBAS__GL_CURRENT_RASTER_COLOR();
static inline unsigned int __CBAS__GL_CURRENT_RASTER_INDEX();
static inline unsigned int __CBAS__GL_CURRENT_RASTER_POSITION();
static inline unsigned int __CBAS__GL_CURRENT_RASTER_POSITION_VALID();
static inline unsigned int __CBAS__GL_CURRENT_RASTER_TEXTURE_COORDS();
static inline unsigned int __CBAS__GL_CURRENT_TEXTURE_COORDS();
static inline unsigned int __CBAS__GL_CW();
static inline unsigned int __CBAS__GL_DECAL();
static inline unsigned int __CBAS__GL_DECR();
static inline unsigned int __CBAS__GL_DEPTH();
static inline unsigned int __CBAS__GL_DEPTH_BIAS();
static inline unsigned int __CBAS__GL_DEPTH_BITS();
static inline unsigned int __CBAS__GL_DEPTH_BUFFER_BIT();
static inline unsigned int __CBAS__GL_DEPTH_CLEAR_VALUE();
static inline unsigned int __CBAS__GL_DEPTH_COMPONENT();
static inline unsigned int __CBAS__GL_DEPTH_FUNC();
static inline unsigned int __CBAS__GL_DEPTH_RANGE();
static inline unsigned int __CBAS__GL_DEPTH_SCALE();
static inline unsigned int __CBAS__GL_DEPTH_TEST();
static inline unsigned int __CBAS__GL_DEPTH_WRITEMASK();
static inline unsigned int __CBAS__GL_DIFFUSE();
static inline unsigned int __CBAS__GL_DITHER();
static inline unsigned int __CBAS__GL_DOMAIN();
static inline unsigned int __CBAS__GL_DONT_CARE();
static inline unsigned int __CBAS__GL_DOUBLEBUFFER();
static inline unsigned int __CBAS__GL_DRAW_BUFFER();
static inline unsigned int __CBAS__GL_DRAW_PIXEL_TOKEN();
static inline unsigned int __CBAS__GL_DST_ALPHA();
static inline unsigned int __CBAS__GL_DST_COLOR();
static inline unsigned int __CBAS__GL_EDGE_FLAG();
static inline unsigned int __CBAS__GL_EMISSION();
static inline unsigned int __CBAS__GL_ENABLE_BIT();
static inline unsigned int __CBAS__GL_EQUAL();
static inline unsigned int __CBAS__GL_EQUIV();
static inline unsigned int __CBAS__GL_EVAL_BIT();
static inline unsigned int __CBAS__GL_EXP();
static inline unsigned int __CBAS__GL_EXP2();
static inline unsigned int __CBAS__GL_EXTENSIONS();
static inline unsigned int __CBAS__GL_EYE_LINEAR();
static inline unsigned int __CBAS__GL_EYE_PLANE();
static inline unsigned int __CBAS__GL_FALSE();
static inline unsigned int __CBAS__GL_FASTEST();
static inline unsigned int __CBAS__GL_FEEDBACK();
static inline unsigned int __CBAS__GL_FILL();
static inline unsigned int __CBAS__GL_FLAT();
static inline unsigned int __CBAS__GL_FLOAT();
static inline unsigned int __CBAS__GL_FOG();
static inline unsigned int __CBAS__GL_FOG_BIT();
static inline unsigned int __CBAS__GL_FOG_COLOR();
static inline unsigned int __CBAS__GL_FOG_DENSITY();
static inline unsigned int __CBAS__GL_FOG_END();
static inline unsigned int __CBAS__GL_FOG_HINT();
static inline unsigned int __CBAS__GL_FOG_INDEX();
static inline unsigned int __CBAS__GL_FOG_MODE();
static inline unsigned int __CBAS__GL_FOG_START();
static inline unsigned int __CBAS__GL_FRONT();
static inline unsigned int __CBAS__GL_FRONT_AND_BACK();
static inline unsigned int __CBAS__GL_FRONT_FACE();
static inline unsigned int __CBAS__GL_FRONT_LEFT();
static inline unsigned int __CBAS__GL_FRONT_RIGHT();
static inline unsigned int __CBAS__GL_GEQUAL();
static inline unsigned int __CBAS__GL_GREATER();
static inline unsigned int __CBAS__GL_GREEN();
static inline unsigned int __CBAS__GL_GREEN_BIAS();
static inline unsigned int __CBAS__GL_GREEN_BITS();
static inline unsigned int __CBAS__GL_GREEN_SCALE();
static inline unsigned int __CBAS__GL_HINT_BIT();
static inline unsigned int __CBAS__GL_INCR();
static inline unsigned int __CBAS__GL_INDEX_BITS();
static inline unsigned int __CBAS__GL_INDEX_CLEAR_VALUE();
static inline unsigned int __CBAS__GL_INDEX_MODE();
static inline unsigned int __CBAS__GL_INDEX_OFFSET();
static inline unsigned int __CBAS__GL_INDEX_SHIFT();
static inline unsigned int __CBAS__GL_INDEX_WRITEMASK();
static inline unsigned int __CBAS__GL_INT();
static inline unsigned int __CBAS__GL_INVALID_ENUM();
static inline unsigned int __CBAS__GL_INVALID_OPERATION();
static inline unsigned int __CBAS__GL_INVALID_VALUE();
static inline unsigned int __CBAS__GL_INVERT();
static inline unsigned int __CBAS__GL_KEEP();
static inline unsigned int __CBAS__GL_LEFT();
static inline unsigned int __CBAS__GL_LEQUAL();
static inline unsigned int __CBAS__GL_LESS();
static inline unsigned int __CBAS__GL_LIGHT0();
static inline unsigned int __CBAS__GL_LIGHT1();
static inline unsigned int __CBAS__GL_LIGHT2();
static inline unsigned int __CBAS__GL_LIGHT3();
static inline unsigned int __CBAS__GL_LIGHT4();
static inline unsigned int __CBAS__GL_LIGHT5();
static inline unsigned int __CBAS__GL_LIGHT6();
static inline unsigned int __CBAS__GL_LIGHT7();
static inline unsigned int __CBAS__GL_LIGHTING();
static inline unsigned int __CBAS__GL_LIGHTING_BIT();
static inline unsigned int __CBAS__GL_LIGHT_MODEL_AMBIENT();
static inline unsigned int __CBAS__GL_LIGHT_MODEL_LOCAL_VIEWER();
static inline unsigned int __CBAS__GL_LIGHT_MODEL_TWO_SIDE();
static inline unsigned int __CBAS__GL_LINE();
static inline unsigned int __CBAS__GL_LINEAR();
static inline unsigned int __CBAS__GL_LINEAR_ATTENUATION();
static inline unsigned int __CBAS__GL_LINEAR_MIPMAP_LINEAR();
static inline unsigned int __CBAS__GL_LINEAR_MIPMAP_NEAREST();
static inline unsigned int __CBAS__GL_LINES();
static inline unsigned int __CBAS__GL_LINE_BIT();
static inline unsigned int __CBAS__GL_LINE_LOOP();
static inline unsigned int __CBAS__GL_LINE_RESET_TOKEN();
static inline unsigned int __CBAS__GL_LINE_SMOOTH();
static inline unsigned int __CBAS__GL_LINE_SMOOTH_HINT();
static inline unsigned int __CBAS__GL_LINE_STIPPLE();
static inline unsigned int __CBAS__GL_LINE_STIPPLE_PATTERN();
static inline unsigned int __CBAS__GL_LINE_STIPPLE_REPEAT();
static inline unsigned int __CBAS__GL_LINE_STRIP();
static inline unsigned int __CBAS__GL_LINE_TOKEN();
static inline unsigned int __CBAS__GL_LINE_WIDTH();
static inline unsigned int __CBAS__GL_LINE_WIDTH_GRANULARITY();
static inline unsigned int __CBAS__GL_LINE_WIDTH_RANGE();
static inline unsigned int __CBAS__GL_LIST_BASE();
static inline unsigned int __CBAS__GL_LIST_BIT();
static inline unsigned int __CBAS__GL_LIST_INDEX();
static inline unsigned int __CBAS__GL_LIST_MODE();
static inline unsigned int __CBAS__GL_LOAD();
static inline unsigned int __CBAS__GL_LOGIC_OP();
static inline unsigned int __CBAS__GL_LOGIC_OP_MODE();
static inline unsigned int __CBAS__GL_LUMINANCE();
static inline unsigned int __CBAS__GL_LUMINANCE_ALPHA();
static inline unsigned int __CBAS__GL_MAP1_COLOR_4();
static inline unsigned int __CBAS__GL_MAP1_GRID_DOMAIN();
static inline unsigned int __CBAS__GL_MAP1_GRID_SEGMENTS();
static inline unsigned int __CBAS__GL_MAP1_INDEX();
static inline unsigned int __CBAS__GL_MAP1_NORMAL();
static inline unsigned int __CBAS__GL_MAP1_TEXTURE_COORD_1();
static inline unsigned int __CBAS__GL_MAP1_TEXTURE_COORD_2();
static inline unsigned int __CBAS__GL_MAP1_TEXTURE_COORD_3();
static inline unsigned int __CBAS__GL_MAP1_TEXTURE_COORD_4();
static inline unsigned int __CBAS__GL_MAP1_VERTEX_3();
static inline unsigned int __CBAS__GL_MAP1_VERTEX_4();
static inline unsigned int __CBAS__GL_MAP2_COLOR_4();
static inline unsigned int __CBAS__GL_MAP2_GRID_DOMAIN();
static inline unsigned int __CBAS__GL_MAP2_GRID_SEGMENTS();
static inline unsigned int __CBAS__GL_MAP2_INDEX();
static inline unsigned int __CBAS__GL_MAP2_NORMAL();
static inline unsigned int __CBAS__GL_MAP2_TEXTURE_COORD_1();
static inline unsigned int __CBAS__GL_MAP2_TEXTURE_COORD_2();
static inline unsigned int __CBAS__GL_MAP2_TEXTURE_COORD_3();
static inline unsigned int __CBAS__GL_MAP2_TEXTURE_COORD_4();
static inline unsigned int __CBAS__GL_MAP_COLOR();
static inline unsigned int __CBAS__GL_MAP_STENCIL();
static inline unsigned int __CBAS__GL_MATRIX_MODE();
static inline unsigned int __CBAS__GL_MAX_ATTRIB_STACK_DEPTH();
static inline unsigned int __CBAS__GL_MAX_CLIP_PLANES();
static inline unsigned int __CBAS__GL_MAX_EVAL_ORDER();
static inline unsigned int __CBAS__GL_MAX_LIGHTS();
static inline unsigned int __CBAS__GL_MAX_LIST_NESTING();
static inline unsigned int __CBAS__GL_MAX_MODELVIEW_STACK_DEPTH();
static inline unsigned int __CBAS__GL_MAX_NAME_STACK_DEPTH();
static inline unsigned int __CBAS__GL_MAX_PIXEL_MAP_TABLE();
static inline unsigned int __CBAS__GL_MAX_PROJECTION_STACK_DEPTH();
static inline unsigned int __CBAS__GL_MAX_TEXTURE_SIZE();
static inline unsigned int __CBAS__GL_MAX_TEXTURE_STACK_DEPTH();
static inline unsigned int __CBAS__GL_MAX_VIEWPORT_DIMS();
static inline unsigned int __CBAS__GL_MODELVIEW();
static inline unsigned int __CBAS__GL_MODELVIEW_MATRIX();
static inline unsigned int __CBAS__GL_MODELVIEW_STACK_DEPTH();
static inline unsigned int __CBAS__GL_MODULATE();
static inline unsigned int __CBAS__GL_MULT();
static inline unsigned int __CBAS__GL_NAME_STACK_DEPTH();
static inline unsigned int __CBAS__GL_NAND();
static inline unsigned int __CBAS__GL_NEAREST();
static inline unsigned int __CBAS__GL_NEAREST_MIPMAP_LINEAR();
static inline unsigned int __CBAS__GL_NEAREST_MIPMAP_NEAREST();
static inline unsigned int __CBAS__GL_NEVER();
static inline unsigned int __CBAS__GL_NICEST();
static inline unsigned int __CBAS__GL_NONE();
static inline unsigned int __CBAS__GL_NOOP();
static inline unsigned int __CBAS__GL_NOR();
static inline unsigned int __CBAS__GL_NORMALIZE();
static inline unsigned int __CBAS__GL_NOTEQUAL();
static inline unsigned int __CBAS__GL_NO_ERROR();
static inline unsigned int __CBAS__GL_OBJECT_LINEAR();
static inline unsigned int __CBAS__GL_OBJECT_PLANE();
static inline unsigned int __CBAS__GL_ONE();
static inline unsigned int __CBAS__GL_ONE_MINUS_DST_ALPHA();
static inline unsigned int __CBAS__GL_ONE_MINUS_DST_COLOR();
static inline unsigned int __CBAS__GL_ONE_MINUS_SRC_ALPHA();
static inline unsigned int __CBAS__GL_ONE_MINUS_SRC_COLOR();
static inline unsigned int __CBAS__GL_OR();
static inline unsigned int __CBAS__GL_OR_INVERTED();
static inline unsigned int __CBAS__GL_OR_REVERSE();
static inline unsigned int __CBAS__GL_ORDER();
static inline unsigned int __CBAS__GL_OUT_OF_MEMORY();
static inline unsigned int __CBAS__GL_PACK_ALIGNMENT();
static inline unsigned int __CBAS__GL_PACK_LSB_FIRST();
static inline unsigned int __CBAS__GL_PACK_ROW_LENGTH();
static inline unsigned int __CBAS__GL_PACK_SKIP_PIXELS();
static inline unsigned int __CBAS__GL_PACK_SKIP_ROWS();
static inline unsigned int __CBAS__GL_PACK_SWAP_BYTES();
static inline unsigned int __CBAS__GL_PASS_THROUGH_TOKEN();
static inline unsigned int __CBAS__GL_PERSPECTIVE_CORRECTION_HINT();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_I();
static inline unsigned int __CBAS__GL_PIXEL_MAP_S_TO_S();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_R();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_G();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_B();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_A();
static inline unsigned int __CBAS__GL_PIXEL_MAP_R_TO_R();
static inline unsigned int __CBAS__GL_PIXEL_MAP_G_TO_G();
static inline unsigned int __CBAS__GL_PIXEL_MAP_B_TO_B();
static inline unsigned int __CBAS__GL_PIXEL_MAP_A_TO_A();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_I_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_S_TO_S_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_R_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_G_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_B_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_A_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_R_TO_R_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_G_TO_G_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_B_TO_B_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MAP_A_TO_A_SIZE();
static inline unsigned int __CBAS__GL_PIXEL_MODE_BIT();
static inline unsigned int __CBAS__GL_POINT();
static inline unsigned int __CBAS__GL_POINTS();
static inline unsigned int __CBAS__GL_POINT_BIT();
static inline unsigned int __CBAS__GL_POINT_SIZE();
static inline unsigned int __CBAS__GL_POINT_SIZE_GRANULARITY();
static inline unsigned int __CBAS__GL_POINT_SIZE_RANGE();
static inline unsigned int __CBAS__GL_POINT_SMOOTH();
static inline unsigned int __CBAS__GL_POINT_SMOOTH_HINT();
static inline unsigned int __CBAS__GL_POINT_TOKEN();
static inline unsigned int __CBAS__GL_POLYGON();
static inline unsigned int __CBAS__GL_POLYGON_BIT();
static inline unsigned int __CBAS__GL_POLYGON_MODE();
static inline unsigned int __CBAS__GL_POLYGON_SMOOTH();
static inline unsigned int __CBAS__GL_POLYGON_SMOOTH_HINT();
static inline unsigned int __CBAS__GL_POLYGON_STIPPLE();
static inline unsigned int __CBAS__GL_POLYGON_STIPPLE_BIT();
static inline unsigned int __CBAS__GL_POLYGON_TOKEN();
static inline unsigned int __CBAS__GL_POSITION();
static inline unsigned int __CBAS__GL_PROJECTION();
static inline unsigned int __CBAS__GL_PROJECTION_MATRIX();
static inline unsigned int __CBAS__GL_PROJECTION_STACK_DEPTH();
static inline unsigned int __CBAS__GL_Q();
static inline unsigned int __CBAS__GL_QUADRATIC_ATTENUATION();
static inline unsigned int __CBAS__GL_QUADS();
static inline unsigned int __CBAS__GL_QUAD_STRIP();
static inline unsigned int __CBAS__GL_R();
static inline unsigned int __CBAS__GL_READ_BUFFER();
static inline unsigned int __CBAS__GL_RED();
static inline unsigned int __CBAS__GL_RED_BIAS();
static inline unsigned int __CBAS__GL_RED_BITS();
static inline unsigned int __CBAS__GL_RED_SCALE();
static inline unsigned int __CBAS__GL_RENDER();
static inline unsigned int __CBAS__GL_RENDERER();
static inline unsigned int __CBAS__GL_RENDER_MODE();
static inline unsigned int __CBAS__GL_REPEAT();
static inline unsigned int __CBAS__GL_REPLACE();
static inline unsigned int __CBAS__GL_RETURN();
static inline unsigned int __CBAS__GL_RGB();
static inline unsigned int __CBAS__GL_RGBA();
static inline unsigned int __CBAS__GL_RGBA_MODE();
static inline unsigned int __CBAS__GL_RIGHT();
static inline unsigned int __CBAS__GL_S();
static inline unsigned int __CBAS__GL_SCISSOR_BIT();
static inline unsigned int __CBAS__GL_SCISSOR_BOX();
static inline unsigned int __CBAS__GL_SCISSOR_TEST();
static inline unsigned int __CBAS__GL_SELECT();
static inline unsigned int __CBAS__GL_SET();
static inline unsigned int __CBAS__GL_SHININESS();
static inline unsigned int __CBAS__GL_SHADE_MODEL();
static inline unsigned int __CBAS__GL_SHORT();
static inline unsigned int __CBAS__GL_SMOOTH();
static inline unsigned int __CBAS__GL_SPECULAR();
static inline unsigned int __CBAS__GL_SPHERE_MAP();
static inline unsigned int __CBAS__GL_SPOT_CUTOFF();
static inline unsigned int __CBAS__GL_SPOT_DIRECTION();
static inline unsigned int __CBAS__GL_SPOT_EXPONENT();
static inline unsigned int __CBAS__GL_SRC_ALPHA();
static inline unsigned int __CBAS__GL_SRC_ALPHA_SATURATE();
static inline unsigned int __CBAS__GL_SRC_COLOR();
static inline unsigned int __CBAS__GL_STACK_OVERFLOW();
static inline unsigned int __CBAS__GL_STACK_UNDERFLOW();
static inline unsigned int __CBAS__GL_STENCIL();
static inline unsigned int __CBAS__GL_STENCIL_BITS();
static inline unsigned int __CBAS__GL_STENCIL_BUFFER_BIT();
static inline unsigned int __CBAS__GL_STENCIL_INDEX();
static inline unsigned int __CBAS__GL_STENCIL_CLEAR_VALUE();
static inline unsigned int __CBAS__GL_STENCIL_FAIL();
static inline unsigned int __CBAS__GL_STENCIL_FUNC();
static inline unsigned int __CBAS__GL_STENCIL_PASS_DEPTH_FAIL();
static inline unsigned int __CBAS__GL_STENCIL_PASS_DEPTH_PASS();
static inline unsigned int __CBAS__GL_STENCIL_REF();
static inline unsigned int __CBAS__GL_STENCIL_TEST();
static inline unsigned int __CBAS__GL_STENCIL_VALUE_MASK();
static inline unsigned int __CBAS__GL_STENCIL_WRITEMASK();
static inline unsigned int __CBAS__GL_STEREO();
static inline unsigned int __CBAS__GL_SUBPIXEL_BITS();
static inline unsigned int __CBAS__GL_T();
static inline unsigned int __CBAS__GL_TEXTURE();
static inline unsigned int __CBAS__GL_TEXTURE_1D();
static inline unsigned int __CBAS__GL_TEXTURE_2D();
static inline unsigned int __CBAS__GL_TEXTURE_BIT();
static inline unsigned int __CBAS__GL_TEXTURE_BORDER();
static inline unsigned int __CBAS__GL_TEXTURE_BORDER_COLOR();
static inline unsigned int __CBAS__GL_TEXTURE_COMPONENTS();
static inline unsigned int __CBAS__GL_TEXTURE_ENV();
static inline unsigned int __CBAS__GL_TEXTURE_ENV_COLOR();
static inline unsigned int __CBAS__GL_TEXTURE_ENV_MODE();
static inline unsigned int __CBAS__GL_TEXTURE_GEN_MODE();
static inline unsigned int __CBAS__GL_TEXTURE_GEN_Q();
static inline unsigned int __CBAS__GL_TEXTURE_GEN_R();
static inline unsigned int __CBAS__GL_TEXTURE_GEN_S();
static inline unsigned int __CBAS__GL_TEXTURE_GEN_T();
static inline unsigned int __CBAS__GL_TEXTURE_HEIGHT();
static inline unsigned int __CBAS__GL_TEXTURE_MAG_FILTER();
static inline unsigned int __CBAS__GL_TEXTURE_MATRIX();
static inline unsigned int __CBAS__GL_TEXTURE_MIN_FILTER();
static inline unsigned int __CBAS__GL_TEXTURE_STACK_DEPTH();
static inline unsigned int __CBAS__GL_TEXTURE_WIDTH();
static inline unsigned int __CBAS__GL_TEXTURE_WRAP_S();
static inline unsigned int __CBAS__GL_TEXTURE_WRAP_T();
static inline unsigned int __CBAS__GL_TRANSFORM_BIT();
static inline unsigned int __CBAS__GL_TRIANGLES();
static inline unsigned int __CBAS__GL_TRIANGLE_FAN();
static inline unsigned int __CBAS__GL_TRIANGLE_STRIP();
static inline unsigned int __CBAS__GL_TRUE();
static inline unsigned int __CBAS__GL_UNPACK_ALIGNMENT();
static inline unsigned int __CBAS__GL_UNPACK_LSB_FIRST();
static inline unsigned int __CBAS__GL_UNPACK_ROW_LENGTH();
static inline unsigned int __CBAS__GL_UNPACK_SKIP_PIXELS();
static inline unsigned int __CBAS__GL_UNPACK_SKIP_ROWS();
static inline unsigned int __CBAS__GL_UNPACK_SWAP_BYTES();
static inline unsigned int __CBAS__GL_UNSIGNED_BYTE();
static inline unsigned int __CBAS__GL_UNSIGNED_INT();
static inline unsigned int __CBAS__GL_UNSIGNED_SHORT();
static inline unsigned int __CBAS__GL_VENDOR();
static inline unsigned int __CBAS__GL_VERSION();
static inline unsigned int __CBAS__GL_VIEWPORT();
static inline unsigned int __CBAS__GL_VIEWPORT_BIT();
static inline unsigned int __CBAS__GL_XOR();
static inline unsigned int __CBAS__GL_ZERO();
static inline unsigned int __CBAS__GL_ZOOM_X();
static inline unsigned int __CBAS__GL_ZOOM_Y();
static inline void __CBAS__openWindow(unsigned char* __cbas_local_variable_mangled_name_text,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h);
static inline void __CBAS__closeWindow();
static inline void __CBAS__clearScreen(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b);
static inline void __CBAS__displayModel(unsigned int __cbas_local_variable_mangled_name_dl,float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z,float __cbas_local_variable_mangled_name_rx,float __cbas_local_variable_mangled_name_ry,float __cbas_local_variable_mangled_name_rz);
static inline int __CBAS__setSwapInterval(int __cbas_local_variable_mangled_name_a);
static inline int __CBAS__lockCursor(int __cbas_local_variable_mangled_name_state);
static inline void __CBAS__freeMusic(unsigned char* __cbas_local_variable_mangled_name_p);
static inline void __CBAS__beginUI();
static inline void __CBAS__endUI();
static inline void __CBAS__swapDisplay();
static inline int __CBAS__numJoysticks();
static inline unsigned char __CBAS__isGameController(int __cbas_local_variable_mangled_name_ind);
static inline unsigned char* __CBAS__openGameController(int __cbas_local_variable_mangled_name_ind);
static inline void __CBAS__closeGameController(unsigned char* __cbas_local_variable_mangled_name_h);
static inline signed short __CBAS__gameControllerGetAxis(unsigned char* __cbas_local_variable_mangled_name_h,int __cbas_local_variable_mangled_name_which);
static inline unsigned char __CBAS__gameControllerGetButton(unsigned char* __cbas_local_variable_mangled_name_h,int __cbas_local_variable_mangled_name_which);
static inline int __CBAS__CONTROLLER_BUTTON_A();
static inline int __CBAS__CONTROLLER_BUTTON_B();
static inline int __CBAS__CONTROLLER_BUTTON_X();
static inline int __CBAS__CONTROLLER_BUTTON_Y();
static inline int __CBAS__CONTROLLER_BUTTON_BACK();
static inline int __CBAS__CONTROLLER_BUTTON_GUIDE();
static inline int __CBAS__CONTROLLER_BUTTON_START();
static inline int __CBAS__CONTROLLER_BUTTON_LEFTSTICK();
static inline int __CBAS__CONTROLLER_BUTTON_RIGHTSTICK();
static inline int __CBAS__CONTROLLER_BUTTON_LEFTSHOULDER();
static inline int __CBAS__CONTROLLER_BUTTON_RIGHTSHOULDER();
static inline int __CBAS__CONTROLLER_BUTTON_DPAD_UP();
static inline int __CBAS__CONTROLLER_BUTTON_DPAD_DOWN();
static inline int __CBAS__CONTROLLER_BUTTON_DPAD_LEFT();
static inline int __CBAS__CONTROLLER_BUTTON_DPAD_RIGHT();
static inline int __CBAS__CONTROLLER_BUTTON_MISC1();
static inline int __CBAS__CONTROLLER_BUTTON_PADDLE1();
static inline int __CBAS__CONTROLLER_BUTTON_PADDLE2();
static inline int __CBAS__CONTROLLER_BUTTON_PADDLE3();
static inline int __CBAS__CONTROLLER_BUTTON_PADDLE4();
static inline int __CBAS__CONTROLLER_BUTTON_TOUCHPAD();
static inline int __CBAS__CONTROLLER_BUTTON_MAX();
static inline void __CBAS__setWindowResizable(unsigned char __cbas_local_variable_mangled_name_toggle);
static inline unsigned char* __CBAS__getClipboardText();
static inline int __CBAS__setClipboardText(unsigned char* __cbas_local_variable_mangled_name_txt);
static inline unsigned char __CBAS__hasClipboardText();
static inline void __CBAS__startTextInput();
static inline void __CBAS__stopTextInput();
static inline void __CBAS__setTextInputRect(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_w,int __cbas_local_variable_mangled_name_h);
static inline void __CBAS__drawBox(float __cbas_local_variable_mangled_name_xdim,float __cbas_local_variable_mangled_name_ydim,float __cbas_local_variable_mangled_name_zdim);
static inline void __CBAS__drawSphere(float __cbas_local_variable_mangled_name_r,unsigned int __cbas_local_variable_mangled_name_midrings,unsigned int __cbas_local_variable_mangled_name_vrings);
static inline void __CBAS__drawCone(float __cbas_local_variable_mangled_name_rbase,float __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_vrings);
static inline void __CBAS__drawCylinder(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_vrings);
static void __CBAS__randomizeFood();
static void __CBAS__advanceSnake();
static void __CBAS__renderSnake();
static void __CBAS__renderFood();
static void __CBAS__checkEatFood();
static void __CBAS__checkCollide();
static void __CBAS__appInit();
static void __CBAS__updateSnakeDir();
static void __CBAS__appUpdate();
static void __CBAS__appClose();
static void __CBAS__onClick(int __cbas_local_variable_mangled_name_btn,int __cbas_local_variable_mangled_name_state);
static void __CBAS__onTextInput(unsigned char* __cbas_local_variable_mangled_name_text);
static void __CBAS__onTextEdit(unsigned char* __cbas_local_variable_mangled_name_text,int __cbas_local_variable_mangled_name_start,int __cbas_local_variable_mangled_name_length);
static void __CBAS__onKey(int __cbas_local_variable_mangled_name_kc,unsigned char __cbas_local_variable_mangled_name_state);
static void __CBAS__onKeyRepeat(int __cbas_local_variable_mangled_name_kc,unsigned char __cbas_local_variable_mangled_name_state);
static void __CBAS__onResize(int __cbas_local_variable_mangled_name_w,int __cbas_local_variable_mangled_name_h);
static void __CBAS__onScroll(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y);
/*
FUNCTION BODIES
*/
static inline long long __CBAS__SEEK_SET()
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SEEK_SET;
    
}
static inline long long __CBAS__SEEK_CUR()
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SEEK_CUR;
    
}
static inline long long __CBAS__SEEK_END()
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SEEK_END;
    
}
static inline long long __CBAS__EOF()
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EOF;
    
}
static inline double __CBAS__PI()
{
    double __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513;
    
}
static inline double __CBAS__M_PI()
{
    double __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((double)__CBAS__PI());
    
    return (double)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline int __CBAS__E2BIG()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return E2BIG;
    
}
static inline int __CBAS__EACCES()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EACCES;
    
}
static inline int __CBAS__EADDRINUSE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EADDRINUSE;
    
}
static inline int __CBAS__EADDRNOTAVAIL()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EADDRNOTAVAIL;
    
}
static inline int __CBAS__EAFNOSUPPORT()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EAFNOSUPPORT;
    
}
static inline int __CBAS__EAGAIN()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EAGAIN;
    
}
static inline int __CBAS__EALREADY()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EALREADY;
    
}
static inline int __CBAS__EBADF()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EBADF;
    
}
static inline int __CBAS__EBADMSG()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EBADMSG;
    
}
static inline int __CBAS__EBUSY()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EBUSY;
    
}
static inline int __CBAS__ECANCELED()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ECANCELED;
    
}
static inline int __CBAS__ECHILD()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ECHILD;
    
}
static inline int __CBAS__ECONNABORTED()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ECONNABORTED;
    
}
static inline int __CBAS__ECONNREFUSED()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ECONNREFUSED;
    
}
static inline int __CBAS__ECONNRESET()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ECONNRESET;
    
}
static inline int __CBAS__EDEADLK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EDEADLK;
    
}
static inline int __CBAS__EDESTADDRREQ()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EDESTADDRREQ;
    
}
static inline int __CBAS__EDOM()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EDOM;
    
}
static inline int __CBAS__EEXIST()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EEXIST;
    
}
static inline int __CBAS__EFAULT()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EFAULT;
    
}
static inline int __CBAS__EFBIG()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EFBIG;
    
}
static inline int __CBAS__EHOSTUNREACH()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EHOSTUNREACH;
    
}
static inline int __CBAS__EIDRM()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EIDRM;
    
}
static inline int __CBAS__EILSEQ()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EILSEQ;
    
}
static inline int __CBAS__EINPROGRESS()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EINPROGRESS;
    
}
static inline int __CBAS__EINTR()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EINTR;
    
}
static inline int __CBAS__EINVAL()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EINVAL;
    
}
static inline int __CBAS__EIO()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EIO;
    
}
static inline int __CBAS__EISCONN()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EISCONN;
    
}
static inline int __CBAS__EISDIR()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EISDIR;
    
}
static inline int __CBAS__ELOOP()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ELOOP;
    
}
static inline int __CBAS__EMFILE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EMFILE;
    
}
static inline int __CBAS__EMLINK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EMLINK;
    
}
static inline int __CBAS__EMSGSIZE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EMSGSIZE;
    
}
static inline int __CBAS__ENAMETOOLONG()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENAMETOOLONG;
    
}
static inline int __CBAS__ENETDOWN()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENETDOWN;
    
}
static inline int __CBAS__ENETRESET()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENETRESET;
    
}
static inline int __CBAS__ENETUNREACH()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENETUNREACH;
    
}
static inline int __CBAS__ENFILE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENFILE;
    
}
static inline int __CBAS__ENOBUFS()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOBUFS;
    
}
static inline int __CBAS__ENODEV()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENODEV;
    
}
static inline int __CBAS__ENOEXEC()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOEXEC;
    
}
static inline int __CBAS__ENOLCK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOLCK;
    
}
static inline int __CBAS__ENOLINK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOLINK;
    
}
static inline int __CBAS__ENOMEM()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOMEM;
    
}
static inline int __CBAS__ENOMSG()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOMSG;
    
}
static inline int __CBAS__ENOPROTOOPT()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOPROTOOPT;
    
}
static inline int __CBAS__ENOSPC()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOSPC;
    
}
static inline int __CBAS__ENOSYS()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOSYS;
    
}
static inline int __CBAS__ENOTCONN()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOTCONN;
    
}
static inline int __CBAS__ENOTDIR()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOTDIR;
    
}
static inline int __CBAS__ENOTEMPTY()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOTEMPTY;
    
}
static inline int __CBAS__ENOTRECOVERABLE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOTRECOVERABLE;
    
}
static inline int __CBAS__ENOTSOCK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOTSOCK;
    
}
static inline int __CBAS__ENOTSUP()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOTSUP;
    
}
static inline int __CBAS__ENOTTY()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENOTTY;
    
}
static inline int __CBAS__ENXIO()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ENXIO;
    
}
static inline int __CBAS__EOPNOTSUPP()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EOPNOTSUPP;
    
}
static inline int __CBAS__EOVERFLOW()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EOVERFLOW;
    
}
static inline int __CBAS__EOWNERDEAD()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EOWNERDEAD;
    
}
static inline int __CBAS__EPERM()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EPERM;
    
}
static inline int __CBAS__EPIPE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EPIPE;
    
}
static inline int __CBAS__EPROTO()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EPROTO;
    
}
static inline int __CBAS__EPROTONOSUPPORT()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EPROTONOSUPPORT;
    
}
static inline int __CBAS__EPROTOTYPE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EPROTOTYPE;
    
}
static inline int __CBAS__ERANGE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ERANGE;
    
}
static inline int __CBAS__EROFS()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EROFS;
    
}
static inline int __CBAS__ESPIPE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ESPIPE;
    
}
static inline int __CBAS__ESRCH()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ESRCH;
    
}
static inline int __CBAS__ETIMEDOUT()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ETIMEDOUT;
    
}
static inline int __CBAS__ETXTBSY()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return ETXTBSY;
    
}
static inline int __CBAS__EWOULDBLOCK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EWOULDBLOCK;
    
}
static inline int __CBAS__EXDEV()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return EXDEV;
    
}
static inline void __CBAS__memcpy(unsigned char* __cbas_local_variable_mangled_name_dst,unsigned char* __cbas_local_variable_mangled_name_src,unsigned long long __cbas_local_variable_mangled_name_sz)
{
    (void)__CBAS__mcpy(((unsigned char*)(__cbas_local_variable_mangled_name_dst)),((unsigned char*)(__cbas_local_variable_mangled_name_src)),((unsigned long long)(__cbas_local_variable_mangled_name_sz)));
    
}
static inline void __CBAS__memmove(unsigned char* __cbas_local_variable_mangled_name_dst,unsigned char* __cbas_local_variable_mangled_name_src,unsigned long long __cbas_local_variable_mangled_name_sz)
{
    (void)__CBAS__mmove(((unsigned char*)(__cbas_local_variable_mangled_name_dst)),((unsigned char*)(__cbas_local_variable_mangled_name_src)),((unsigned long long)(__cbas_local_variable_mangled_name_sz)));
    
}
static inline long long __CBAS__impl_streq(unsigned char* __cbas_local_variable_mangled_name_a,unsigned char* __cbas_local_variable_mangled_name_b)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(((long long)__CBAS__strcmp(((unsigned char*)(__cbas_local_variable_mangled_name_a)),((unsigned char*)(__cbas_local_variable_mangled_name_b)))) == ((long long)((unsigned long long)0ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS__memeq(unsigned char* __cbas_local_variable_mangled_name_a,unsigned char* __cbas_local_variable_mangled_name_b,unsigned long long __cbas_local_variable_mangled_name_l)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(((long long)__CBAS__memcmp(((unsigned char*)(__cbas_local_variable_mangled_name_a)),((unsigned char*)(__cbas_local_variable_mangled_name_b)),((unsigned long long)(__cbas_local_variable_mangled_name_l)))) == ((long long)((unsigned long long)0ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS__utf8_strlen(unsigned char* __cbas_local_variable_mangled_name_s)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned long long __cbas_local_variable_mangled_name_l;
    (__cbas_local_variable_mangled_name_l) = ((unsigned long long)0ull);
    while((long long)(((unsigned char*)(__cbas_local_variable_mangled_name_s))[(long long)((unsigned long long)0ull)])){

    if((long long)(((unsigned long long)(((unsigned long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_s))[(long long)((unsigned long long)0ull)])) >> ((unsigned long long)6ull))) & ((unsigned long long)3ull))) != ((unsigned long long)2ull))){

    (unsigned long long)((__cbas_local_variable_mangled_name_l)++);
    }

    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)(__cbas_local_variable_mangled_name_l));
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS__utf8_isascii(unsigned char* __cbas_local_variable_mangled_name_s)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    while((long long)(((unsigned char*)(__cbas_local_variable_mangled_name_s))[(long long)((unsigned long long)0ull)])){

    if((long long)((unsigned long long)(((unsigned long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_s))[(long long)((unsigned long long)0ull)])) >> ((unsigned long long)7ull))) & ((unsigned long long)1ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)1ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS__utf8_getchar(unsigned char* __cbas_local_variable_mangled_name_dest,unsigned char* __cbas_local_variable_mangled_name_src,unsigned char** __cbas_local_variable_mangled_name_endptr,long long* __cbas_local_variable_mangled_name_lenout)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    long long __cbas_local_variable_mangled_name_len;
    long long __cbas_local_variable_mangled_name_nb;
    (__cbas_local_variable_mangled_name_len) = ((long long)((unsigned long long)0ull));
    if((long long)(((unsigned long long)(((unsigned long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_src))[(long long)((unsigned long long)0ull)])) >> ((unsigned long long)7ull))) & ((unsigned long long)1ull))) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name_len) = ((long long)((unsigned long long)1ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_src))[(long long)((unsigned long long)0ull)]));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (unsigned char*)((__cbas_local_variable_mangled_name_src)++);
    goto __cbas_mangled_label__setendptr;
    }

    if((long long)(((unsigned long long)(((unsigned long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_src))[(long long)((unsigned long long)0ull)])) >> ((unsigned long long)6ull))) & ((unsigned long long)3ull))) == ((unsigned long long)2ull))){

    goto __cbas_mangled_label__onerr;
    }

    (__cbas_local_variable_mangled_name_nb) = ((long long)((unsigned long long)2ull));
    if((long long)((unsigned long long)(((unsigned long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_src))[(long long)((unsigned long long)0ull)])) >> ((unsigned long long)5ull))) & ((unsigned long long)1ull)))){

    (long long)((__cbas_local_variable_mangled_name_nb)++);
    }

    else {
    goto __cbas_mangled_label__end_of_len_cal;
    }

    if((long long)((unsigned long long)(((unsigned long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_src))[(long long)((unsigned long long)0ull)])) >> ((unsigned long long)4ull))) & ((unsigned long long)1ull)))){

    (long long)((__cbas_local_variable_mangled_name_nb)++);
    }

    else {
    goto __cbas_mangled_label__end_of_len_cal;
    }

    __cbas_mangled_label__end_of_len_cal:1;/*Statement with no effect generated for label...*/
    (__cbas_local_variable_mangled_name_len) = ((long long)(__cbas_local_variable_mangled_name_nb));
    while((long long)(__cbas_local_variable_mangled_name_nb)){

    if((long long)(((unsigned long long)(((unsigned long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_src))[(long long)((unsigned long long)0ull)])) >> ((unsigned long long)6ull))) & ((unsigned long long)3ull))) != ((unsigned long long)2ull))){

    goto __cbas_mangled_label__onerr;
    }

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_src))[(long long)((unsigned long long)0ull)]));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (unsigned char*)((__cbas_local_variable_mangled_name_src)++);
    (long long)((__cbas_local_variable_mangled_name_nb)--);
    }

    __cbas_mangled_label__setendptr:1;/*Statement with no effect generated for label...*/
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)0ull));
    if((long long)(((unsigned char**)(__cbas_local_variable_mangled_name_endptr)) != ((unsigned char**)((unsigned long long)0ull)))){

    (((unsigned char**)(__cbas_local_variable_mangled_name_endptr))[(long long)((unsigned long long)0ull)]) = ((unsigned char*)(__cbas_local_variable_mangled_name_src));
    }

    if((long long)(((long long*)(__cbas_local_variable_mangled_name_lenout)) != ((long long*)((unsigned long long)0ull)))){

    (((long long*)(__cbas_local_variable_mangled_name_lenout))[(long long)((unsigned long long)0ull)]) = ((long long)(__cbas_local_variable_mangled_name_len));
    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    __cbas_mangled_label__onerr:1;/*Statement with no effect generated for label...*/
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)0ull));
    if((long long)(((unsigned char**)(__cbas_local_variable_mangled_name_endptr)) != ((unsigned char**)((unsigned long long)0ull)))){

    (((unsigned char**)(__cbas_local_variable_mangled_name_endptr))[(long long)((unsigned long long)0ull)]) = ((unsigned char*)(__cbas_local_variable_mangled_name_src));
    }

    if((long long)(((long long*)(__cbas_local_variable_mangled_name_lenout)) != ((long long*)((unsigned long long)0ull)))){

    (((long long*)(__cbas_local_variable_mangled_name_lenout))[(long long)((unsigned long long)0ull)]) = ((long long)(__cbas_local_variable_mangled_name_len));
    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)1ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline void __CBAS____method_str_____ctor(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this)
{
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)((unsigned long long)0ull));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)0ull);
    
}
static inline void __CBAS____method_str_____dtor(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this)
{
    if((long long)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) != ((unsigned char*)((unsigned long long)0ull)))){

    (void)__CBAS__free(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))));
    }

    (void)__CBAS____method_str_____ctor(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)));
    
}
static inline void __CBAS____method_pstr_____ctor(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this)
{
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)((unsigned long long)0ull));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)0ull);
    
}
static inline void __CBAS____method_pstr_____dtor(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this)
{
    if((long long)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) != ((unsigned char*)((unsigned long long)0ull)))){

    (void)__CBAS__free(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))));
    }

    (void)__CBAS____method_pstr_____ctor(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)));
    
}
static inline void __CBAS____method_str_____free(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this)
{
    (void)__CBAS____method_str_____dtor(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)));
    (void)__CBAS__free(((unsigned char*)(__cbas_local_variable_mangled_name_this)));
    
}
static inline void __CBAS____method_pstr_____free(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this)
{
    (void)__CBAS____method_pstr_____dtor(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)));
    (void)__CBAS__free(((unsigned char*)(__cbas_local_variable_mangled_name_this)));
    
}
static inline long long __CBAS____method_str_____equal(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    if((long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) != ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)1ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(((long long)__CBAS__memcmp(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))) == ((long long)((unsigned long long)0ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_str_____nequal(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)(!((long long)__CBAS____method_str_____equal(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o)))))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_pstr_____equal(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    if((long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) != ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)1ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(((long long)__CBAS__memcmp(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))) == ((long long)((unsigned long long)0ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_pstr_____nequal(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)(!((long long)__CBAS____method_pstr_____equal(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o)))))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__str* __CBAS____method_str_____copy(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    struct __CBAS__type__str* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (void)__CBAS____method_str_____dtor(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((unsigned long long)0ull)),((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) + ((unsigned long long)1ull)))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d))),((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) + ((unsigned long long)1ull))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__str* __CBAS____method_str_____frompstr(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    struct __CBAS__type__str* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (void)__CBAS____method_str_____dtor(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((unsigned long long)0ull)),((unsigned long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) + ((unsigned long long)1ull)))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))));
    (((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)))[(long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))]) = ((unsigned char)((unsigned long long)0ull));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)__CBAS__strlen(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____copy(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    struct __CBAS__type__pstr* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (void)__CBAS____method_pstr_____dtor(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((unsigned long long)0ull)),((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____fromstr(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    struct __CBAS__type__pstr* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (void)__CBAS____method_pstr_____dtor(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((unsigned long long)0ull)),((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__str* __CBAS____method_str_____new(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,unsigned char* __cbas_local_variable_mangled_name_o)
{
    struct __CBAS__type__str* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned long long __cbas_local_variable_mangled_name_qq;
    (void)__CBAS____method_str_____dtor(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)));
    (__cbas_local_variable_mangled_name_qq) = ((unsigned long long)__CBAS__strlen(((unsigned char*)(__cbas_local_variable_mangled_name_o))));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)(__cbas_local_variable_mangled_name_qq));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((unsigned long long)0ull)),((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_qq)) + ((unsigned long long)1ull)))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned char*)(__cbas_local_variable_mangled_name_o)),((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_qq)) + ((unsigned long long)1ull))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____new(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,unsigned char* __cbas_local_variable_mangled_name_o)
{
    struct __CBAS__type__pstr* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned long long __cbas_local_variable_mangled_name_qq;
    (void)__CBAS____method_pstr_____dtor(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)));
    (__cbas_local_variable_mangled_name_qq) = ((unsigned long long)__CBAS__strlen(((unsigned char*)(__cbas_local_variable_mangled_name_o))));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)(__cbas_local_variable_mangled_name_qq));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((unsigned long long)0ull)),((unsigned long long)(__cbas_local_variable_mangled_name_qq))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned char*)(__cbas_local_variable_mangled_name_o)),((unsigned long long)(__cbas_local_variable_mangled_name_qq)));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline void __CBAS____method_str_____copyto(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    (struct __CBAS__type__str*)__CBAS____method_str_____copy(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)));
    
}
static inline void __CBAS____method_pstr_____copyto(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    (struct __CBAS__type__pstr*)__CBAS____method_pstr_____copy(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)));
    
}
static inline void __CBAS____method_str_____move(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d)) = ((unsigned char*)((unsigned long long)0ull));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)) = ((unsigned long long)0ull);
    
}
static inline void __CBAS____method_pstr_____move(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d)));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d)) = ((unsigned char*)((unsigned long long)0ull));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)) = ((unsigned long long)0ull);
    
}
static inline void __CBAS____method_str_____moveto(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    (void)__CBAS____method_str_____move(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)));
    
}
static inline void __CBAS____method_pstr_____moveto(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    (void)__CBAS____method_pstr_____move(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)));
    
}
static inline struct __CBAS__type__str* __CBAS____method_str_____clone(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this)
{
    struct __CBAS__type__str* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    struct __CBAS__type__str* __cbas_local_variable_mangled_name_p;
    (__cbas_local_variable_mangled_name_p) = ((struct __CBAS__type__str*)((unsigned char*)__CBAS__malloc(((unsigned long long)(sizeof(struct __CBAS__type__str))))));
    if((long long)(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p)) == ((struct __CBAS__type__str*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (struct __CBAS__type__str*)__CBAS____method_str_____copy(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____clone(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this)
{
    struct __CBAS__type__pstr* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_p;
    (__cbas_local_variable_mangled_name_p) = ((struct __CBAS__type__pstr*)((unsigned char*)__CBAS__malloc(((unsigned long long)(sizeof(struct __CBAS__type__pstr))))));
    if((long long)(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p)) == ((struct __CBAS__type__pstr*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (struct __CBAS__type__pstr*)__CBAS____method_pstr_____copy(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_str_____isnull(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)((long long)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))));
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_pstr_____isnull(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)((long long)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))));
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_str_____isempty(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    if((long long)((unsigned long long)(((unsigned long long)((long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) == ((unsigned long long)0ull)))) || ((unsigned long long)((long long)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_str_____empty(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)__CBAS____method_str_____isempty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))));
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_pstr_____isempty(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    if((long long)((unsigned long long)(((unsigned long long)((long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) == ((unsigned long long)0ull)))) || ((unsigned long long)((long long)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_pstr_____empty(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)__CBAS____method_pstr_____isempty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))));
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_str_____prefixed(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned char* __cbas_local_variable_mangled_name_a;
    unsigned char* __cbas_local_variable_mangled_name_b;
    if((long long)((unsigned long long)__CBAS____method_str_____isempty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_str_____isempty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) > ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_a) = ((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d)));
    (__cbas_local_variable_mangled_name_b) = ((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)));
    while((long long)((unsigned long long)1ull)){

    if((long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_a))[(long long)((unsigned long long)0ull)])) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_a))[(long long)((unsigned long long)0ull)])) != ((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_b))[(long long)((unsigned long long)0ull)])))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (unsigned char*)((__cbas_local_variable_mangled_name_a)++);
    (unsigned char*)((__cbas_local_variable_mangled_name_b)++);
    }

    
}
static inline unsigned long long __CBAS____method_pstr_____prefixed(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned char* __cbas_local_variable_mangled_name_a;
    unsigned char* __cbas_local_variable_mangled_name_b;
    unsigned long long __cbas_local_variable_mangled_name_i;
    if((long long)((unsigned long long)__CBAS____method_pstr_____isempty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_pstr_____isempty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) > ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_a) = ((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d)));
    (__cbas_local_variable_mangled_name_b) = ((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)));
    for((__cbas_local_variable_mangled_name_i) = ((unsigned long long)0ull);(long long)(((unsigned long long)(__cbas_local_variable_mangled_name_i)) < ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))));(unsigned long long)((__cbas_local_variable_mangled_name_i)++)){

    if((long long)(((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_a))[(long long)((unsigned long long)0ull)])) != ((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_b))[(long long)((unsigned long long)0ull)])))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (unsigned char*)((__cbas_local_variable_mangled_name_a)++);
    (unsigned char*)((__cbas_local_variable_mangled_name_b)++);
    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_str_____postfixed(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned char* __cbas_local_variable_mangled_name_a;
    unsigned char* __cbas_local_variable_mangled_name_b;
    if((long long)((unsigned long long)__CBAS____method_str_____isempty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_str_____isempty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) > ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_a) = ((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d)));
    (__cbas_local_variable_mangled_name_b) = ((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))))))));
    while((long long)((unsigned long long)1ull)){

    if((long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_a))[(long long)((unsigned long long)0ull)])) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_a))[(long long)((unsigned long long)0ull)])) != ((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_b))[(long long)((unsigned long long)0ull)])))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (unsigned char*)((__cbas_local_variable_mangled_name_a)++);
    (unsigned char*)((__cbas_local_variable_mangled_name_b)++);
    }

    
}
static inline unsigned long long __CBAS____method_pstr_____postfixed(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned char* __cbas_local_variable_mangled_name_a;
    unsigned char* __cbas_local_variable_mangled_name_b;
    unsigned long long __cbas_local_variable_mangled_name_i;
    if((long long)((unsigned long long)__CBAS____method_pstr_____isempty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_pstr_____isempty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) > ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_a) = ((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d)));
    (__cbas_local_variable_mangled_name_b) = ((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)((unsigned long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))))))));
    for((__cbas_local_variable_mangled_name_i) = ((unsigned long long)0ull);(long long)(((unsigned long long)(__cbas_local_variable_mangled_name_i)) < ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))));(unsigned long long)((__cbas_local_variable_mangled_name_i)++)){

    if((long long)(((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_a))[(long long)((unsigned long long)0ull)])) != ((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_b))[(long long)((unsigned long long)0ull)])))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (unsigned char*)((__cbas_local_variable_mangled_name_a)++);
    (unsigned char*)((__cbas_local_variable_mangled_name_b)++);
    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_str_____findfrom(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_findme,unsigned long long __cbas_local_variable_mangled_name_where)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    long long __cbas_local_variable_mangled_name_retval;
    unsigned char* __cbas_local_variable_mangled_name_text;
    unsigned char* __cbas_local_variable_mangled_name_subtext;
    long long __cbas_local_variable_mangled_name_ti;
    long long __cbas_local_variable_mangled_name_si;
    long long __cbas_local_variable_mangled_name_st;
    (__cbas_local_variable_mangled_name_retval) = ((long long)(-((long long)((unsigned long long)1ull))));
    if((long long)((unsigned long long)__CBAS____method_str_____isempty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_findme))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_str_____isempty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_findme))->__CBAS__member__len))) > ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_where)) >= ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_text) = ((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)));
    (__cbas_local_variable_mangled_name_subtext) = ((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_findme))->__CBAS__member__d)));
    (__cbas_local_variable_mangled_name_ti) = ((long long)(__cbas_local_variable_mangled_name_where));
    (__cbas_local_variable_mangled_name_si) = ((long long)((unsigned long long)0ull));
    (__cbas_local_variable_mangled_name_st) = ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_findme))->__CBAS__member__len)));
    for((unsigned long long)0ull;(long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_text))[(long long)(__cbas_local_variable_mangled_name_ti)])) != ((unsigned long long)0ull));(long long)((__cbas_local_variable_mangled_name_ti)++)){

    if((long long)(((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_text))[(long long)(__cbas_local_variable_mangled_name_ti)])) == ((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_subtext))[(long long)(__cbas_local_variable_mangled_name_si)])))){

    (long long)((__cbas_local_variable_mangled_name_si)++);
    if((long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_subtext))[(long long)(__cbas_local_variable_mangled_name_si)])) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(((long long)(((long long)(__cbas_local_variable_mangled_name_ti)) - ((long long)(__cbas_local_variable_mangled_name_st)))) + ((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    }

    else {
    if((long long)(((unsigned long long)(((unsigned char*)(__cbas_local_variable_mangled_name_subtext))[(long long)(__cbas_local_variable_mangled_name_si)])) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(((long long)(((long long)(__cbas_local_variable_mangled_name_ti)) - ((long long)(__cbas_local_variable_mangled_name_st)))) + ((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_ti) = ((long long)(((long long)(__cbas_local_variable_mangled_name_ti)) - ((long long)(__cbas_local_variable_mangled_name_si))));
    (__cbas_local_variable_mangled_name_si) = ((long long)((unsigned long long)0ull));
    }

    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_pstr_____findfrom(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_findme,unsigned long long __cbas_local_variable_mangled_name_where)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    long long __cbas_local_variable_mangled_name_retval;
    unsigned char* __cbas_local_variable_mangled_name_text;
    unsigned char* __cbas_local_variable_mangled_name_subtext;
    long long __cbas_local_variable_mangled_name_ti;
    long long __cbas_local_variable_mangled_name_si;
    long long __cbas_local_variable_mangled_name_st;
    (__cbas_local_variable_mangled_name_retval) = ((long long)(-((long long)((unsigned long long)1ull))));
    if((long long)((unsigned long long)__CBAS____method_pstr_____isempty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_findme))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_pstr_____isempty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_findme))->__CBAS__member__len))) > ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_where)) >= ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_text) = ((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)));
    (__cbas_local_variable_mangled_name_subtext) = ((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_findme))->__CBAS__member__d)));
    (__cbas_local_variable_mangled_name_ti) = ((long long)(__cbas_local_variable_mangled_name_where));
    (__cbas_local_variable_mangled_name_si) = ((long long)((unsigned long long)0ull));
    (__cbas_local_variable_mangled_name_st) = ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_findme))->__CBAS__member__len)));
    for((unsigned long long)0ull;(long long)(((long long)(__cbas_local_variable_mangled_name_ti)) < ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))));(long long)((__cbas_local_variable_mangled_name_ti)++)){

    if((long long)(((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_text))[(long long)(__cbas_local_variable_mangled_name_ti)])) == ((unsigned char)(((unsigned char*)(__cbas_local_variable_mangled_name_subtext))[(long long)(__cbas_local_variable_mangled_name_si)])))){

    (long long)((__cbas_local_variable_mangled_name_si)++);
    if((long long)(((long long)(__cbas_local_variable_mangled_name_si)) >= ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_findme))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(((long long)(((long long)(__cbas_local_variable_mangled_name_ti)) - ((long long)(__cbas_local_variable_mangled_name_st)))) + ((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    }

    else {
    if((long long)(((long long)(__cbas_local_variable_mangled_name_si)) >= ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_findme))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(((long long)(((long long)(__cbas_local_variable_mangled_name_ti)) - ((long long)(__cbas_local_variable_mangled_name_st)))) + ((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_ti) = ((long long)(((long long)(__cbas_local_variable_mangled_name_ti)) - ((long long)(__cbas_local_variable_mangled_name_si))));
    (__cbas_local_variable_mangled_name_si) = ((long long)((unsigned long long)0ull));
    }

    }

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_str_____find(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_findme)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)__CBAS____method_str_____findfrom(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_findme)),((unsigned long long)0ull)));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_pstr_____find(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_findme)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)__CBAS____method_pstr_____findfrom(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_findme)),((unsigned long long)0ull)));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_str_____add(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_o)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned long long __cbas_local_variable_mangled_name_old_len;
    (__cbas_local_variable_mangled_name_old_len) = ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)));
    if((long long)((unsigned long long)__CBAS____method_str_____isempty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)(((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) + ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))) + ((unsigned long long)1ull)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned long long)(((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) + ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))))) + ((unsigned long long)1ull)))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_old_len)))),((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d))),((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) + ((unsigned long long)1ull))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_pstr_____add(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_o)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned long long __cbas_local_variable_mangled_name_old_len;
    (__cbas_local_variable_mangled_name_old_len) = ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)));
    if((long long)((unsigned long long)__CBAS____method_pstr_____isempty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))) + ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)))));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) + ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len)))))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_old_len)))),((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_o))->__CBAS__member__len))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_str_____addc(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,unsigned char* __cbas_local_variable_mangled_name_oo)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned long long __cbas_local_variable_mangled_name_qq;
    unsigned long long __cbas_local_variable_mangled_name_old_len;
    (__cbas_local_variable_mangled_name_qq) = ((unsigned long long)0ull);
    if((long long)(((unsigned char*)(__cbas_local_variable_mangled_name_oo)) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_qq) = ((unsigned long long)__CBAS__strlen(((unsigned char*)(__cbas_local_variable_mangled_name_oo))));
    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_qq)) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_old_len) = ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)(((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_qq)) + ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))) + ((unsigned long long)1ull)));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned long long)(((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) + ((unsigned long long)(__cbas_local_variable_mangled_name_qq)))) + ((unsigned long long)1ull)))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_old_len)))),((unsigned char*)(__cbas_local_variable_mangled_name_oo)),((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_qq)) + ((unsigned long long)1ull))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline unsigned long long __CBAS____method_pstr_____addc(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,unsigned char* __cbas_local_variable_mangled_name_oo)
{
    unsigned long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned long long __cbas_local_variable_mangled_name_qq;
    unsigned long long __cbas_local_variable_mangled_name_old_len;
    (__cbas_local_variable_mangled_name_qq) = ((unsigned long long)0ull);
    if((long long)(((unsigned char*)(__cbas_local_variable_mangled_name_oo)) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_qq) = ((unsigned long long)__CBAS__strlen(((unsigned char*)(__cbas_local_variable_mangled_name_oo))));
    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_qq)) == ((unsigned long long)0ull))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_old_len) = ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_qq)) + ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)))));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) + ((unsigned long long)(__cbas_local_variable_mangled_name_qq))))));
    if((long long)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)1ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_old_len)))),((unsigned char*)(__cbas_local_variable_mangled_name_oo)),((unsigned long long)(__cbas_local_variable_mangled_name_qq)));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((unsigned long long)0ull);
    
    return (unsigned long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__str* __CBAS____method_str_____substr(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,unsigned long long __cbas_local_variable_mangled_name_beginning,unsigned long long __cbas_local_variable_mangled_name_howmuch)
{
    struct __CBAS__type__str* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned char* __cbas_local_variable_mangled_name_b;
    struct __CBAS__type__str* __cbas_local_variable_mangled_name_p;
    if((long long)((unsigned long long)(((unsigned long long)((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_beginning)) >= ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)))))) || ((unsigned long long)((long long)(((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_beginning)) + ((unsigned long long)(__cbas_local_variable_mangled_name_howmuch)))) >= ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_howmuch)) == ((unsigned long long)0ull))){

    struct __CBAS__type__str* __cbas_local_variable_mangled_name_p2;
    (__cbas_local_variable_mangled_name_p2) = ((struct __CBAS__type__str*)((unsigned char*)__CBAS__malloc(((unsigned long long)(sizeof(struct __CBAS__type__str))))));
    if((long long)(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p2)) == ((struct __CBAS__type__str*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS____method_str_____ctor(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p2)));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p2));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_b) = ((unsigned char*)__CBAS__malloc(((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_howmuch)) + ((unsigned long long)1ull)))));
    if((long long)(((unsigned char*)(__cbas_local_variable_mangled_name_b)) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)(__cbas_local_variable_mangled_name_b)),((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_beginning)))),((unsigned long long)(__cbas_local_variable_mangled_name_howmuch)));
    (((unsigned char*)(__cbas_local_variable_mangled_name_b))[(long long)(__cbas_local_variable_mangled_name_howmuch)]) = ((unsigned char)((unsigned long long)0ull));
    (__cbas_local_variable_mangled_name_p) = ((struct __CBAS__type__str*)((unsigned char*)__CBAS__malloc(((unsigned long long)(sizeof(struct __CBAS__type__str))))));
    if((long long)(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p)) == ((struct __CBAS__type__str*)((unsigned long long)0ull)))){

    (void)__CBAS__free(((unsigned char*)(__cbas_local_variable_mangled_name_b)));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p))->__CBAS__member__d)) = ((unsigned char*)(__cbas_local_variable_mangled_name_b));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p))->__CBAS__member__len)) = ((unsigned long long)(__cbas_local_variable_mangled_name_howmuch));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_p));
    
    return (struct __CBAS__type__str*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline struct __CBAS__type__pstr* __CBAS____method_pstr_____substr(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,unsigned long long __cbas_local_variable_mangled_name_beginning,unsigned long long __cbas_local_variable_mangled_name_howmuch)
{
    struct __CBAS__type__pstr* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    unsigned char* __cbas_local_variable_mangled_name_b;
    struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_p;
    if((long long)((unsigned long long)(((unsigned long long)((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_beginning)) >= ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)))))) || ((unsigned long long)((long long)(((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_beginning)) + ((unsigned long long)(__cbas_local_variable_mangled_name_howmuch)))) >= ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_howmuch)) == ((unsigned long long)0ull))){

    struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_p2;
    (__cbas_local_variable_mangled_name_p2) = ((struct __CBAS__type__pstr*)((unsigned char*)__CBAS__malloc(((unsigned long long)(sizeof(struct __CBAS__type__pstr))))));
    if((long long)(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p2)) == ((struct __CBAS__type__pstr*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS____method_pstr_____ctor(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p2)));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p2));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_b) = ((unsigned char*)__CBAS__malloc(((unsigned long long)(__cbas_local_variable_mangled_name_howmuch))));
    if((long long)(((unsigned char*)(__cbas_local_variable_mangled_name_b)) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)(__cbas_local_variable_mangled_name_b)),((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_beginning)))),((unsigned long long)(__cbas_local_variable_mangled_name_howmuch)));
    (__cbas_local_variable_mangled_name_p) = ((struct __CBAS__type__pstr*)((unsigned char*)__CBAS__malloc(((unsigned long long)(sizeof(struct __CBAS__type__str))))));
    if((long long)(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p)) == ((struct __CBAS__type__pstr*)((unsigned long long)0ull)))){

    (void)__CBAS__free(((unsigned char*)(__cbas_local_variable_mangled_name_b)));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)((unsigned long long)0ull));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p))->__CBAS__member__d)) = ((unsigned char*)(__cbas_local_variable_mangled_name_b));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p))->__CBAS__member__len)) = ((unsigned long long)(__cbas_local_variable_mangled_name_howmuch));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_p));
    
    return (struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_str_____replace_first_from(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_what,struct __CBAS__type__str* __cbas_local_variable_mangled_name_with,unsigned long long __cbas_local_variable_mangled_name_maxwhere)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    long long __cbas_local_variable_mangled_name_where;
    unsigned long long __cbas_local_variable_mangled_name_is_shorter;
    unsigned long long __cbas_local_variable_mangled_name_is_equal;
    if((long long)((unsigned long long)__CBAS____method_str_____empty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_maxwhere)) >= ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_str_____empty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_where) = ((long long)__CBAS____method_str_____findfrom(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what)),((unsigned long long)(__cbas_local_variable_mangled_name_maxwhere))));
    if((long long)(((long long)(__cbas_local_variable_mangled_name_where)) == ((long long)(-((long long)((unsigned long long)1ull)))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)2ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_is_shorter) = ((unsigned long long)((long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))) < ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))));
    (__cbas_local_variable_mangled_name_is_equal) = ((unsigned long long)((long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))) == ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))));
    if((long long)(__cbas_local_variable_mangled_name_is_equal)){

    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))),((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(__cbas_local_variable_mangled_name_where));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    else if((long long)(__cbas_local_variable_mangled_name_is_shorter)){

    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))),((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))));
    (void)__CBAS__mmove(((unsigned char*)(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))))),((unsigned char*)(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))),((unsigned long long)((long long)(((long long)(((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))))) + ((long long)((unsigned long long)1ull))))));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((long long)(((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))))) + ((long long)(((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))))))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(__cbas_local_variable_mangled_name_where));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    else {
    unsigned char* __cbas_local_variable_mangled_name_resbuf;
    (__cbas_local_variable_mangled_name_resbuf) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((unsigned long long)0ull)),((unsigned long long)(((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) + ((unsigned long long)(((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))) - ((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))))) + ((unsigned long long)1ull)))));
    if((long long)(((unsigned char*)(__cbas_local_variable_mangled_name_resbuf)) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)(__cbas_local_variable_mangled_name_resbuf)),((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned long long)(__cbas_local_variable_mangled_name_where)));
    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)(__cbas_local_variable_mangled_name_resbuf)) + ((long long)(__cbas_local_variable_mangled_name_where)))),((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))));
    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)(((unsigned char*)(__cbas_local_variable_mangled_name_resbuf)) + ((long long)(__cbas_local_variable_mangled_name_where)))) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))))),((unsigned char*)(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))),((unsigned long long)((long long)(((long long)(((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))))) + ((long long)((unsigned long long)1ull))))));
    (void)__CBAS__free(((unsigned char*)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)(__cbas_local_variable_mangled_name_resbuf));
    ((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((long long)(((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))))) + ((long long)(((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))))))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(__cbas_local_variable_mangled_name_where));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    
}
static inline long long __CBAS____method_pstr_____replace_first_from(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_what,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_with,unsigned long long __cbas_local_variable_mangled_name_maxwhere)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    long long __cbas_local_variable_mangled_name_where;
    unsigned long long __cbas_local_variable_mangled_name_is_shorter;
    unsigned long long __cbas_local_variable_mangled_name_is_equal;
    if((long long)((unsigned long long)__CBAS____method_pstr_____empty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_maxwhere)) >= ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_pstr_____empty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_where) = ((long long)__CBAS____method_pstr_____findfrom(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what)),((unsigned long long)(__cbas_local_variable_mangled_name_maxwhere))));
    if((long long)(((long long)(__cbas_local_variable_mangled_name_where)) == ((long long)(-((long long)((unsigned long long)1ull)))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)2ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_is_shorter) = ((unsigned long long)((long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))) < ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))));
    (__cbas_local_variable_mangled_name_is_equal) = ((unsigned long long)((long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))) == ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))));
    if((long long)(__cbas_local_variable_mangled_name_is_equal)){

    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))),((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(__cbas_local_variable_mangled_name_where));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    else if((long long)(__cbas_local_variable_mangled_name_is_shorter)){

    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))),((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))));
    (void)__CBAS__mmove(((unsigned char*)(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))))),((unsigned char*)(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))),((unsigned long long)((long long)(((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len)))))))));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((long long)(((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))))) + ((long long)(((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))))))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(__cbas_local_variable_mangled_name_where));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    else {
    unsigned char* __cbas_local_variable_mangled_name_resbuf;
    (__cbas_local_variable_mangled_name_resbuf) = ((unsigned char*)__CBAS__realloc(((unsigned char*)((unsigned long long)0ull)),((unsigned long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) + ((unsigned long long)(((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))) - ((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len)))))))));
    if((long long)(((unsigned char*)(__cbas_local_variable_mangled_name_resbuf)) == ((unsigned char*)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (void)__CBAS__mcpy(((unsigned char*)(__cbas_local_variable_mangled_name_resbuf)),((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))),((unsigned long long)(__cbas_local_variable_mangled_name_where)));
    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)(__cbas_local_variable_mangled_name_resbuf)) + ((long long)(__cbas_local_variable_mangled_name_where)))),((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__d))),((unsigned long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))));
    (void)__CBAS__mcpy(((unsigned char*)(((unsigned char*)(((unsigned char*)(__cbas_local_variable_mangled_name_resbuf)) + ((long long)(__cbas_local_variable_mangled_name_where)))) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))))),((unsigned char*)(((unsigned char*)(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))) + ((long long)(__cbas_local_variable_mangled_name_where)))) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))),((unsigned long long)((long long)(((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len)))))))));
    (void)__CBAS__free(((unsigned char*)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d))));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__d)) = ((unsigned char*)(__cbas_local_variable_mangled_name_resbuf));
    ((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len)) = ((unsigned long long)((long long)(((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len))))) + ((long long)(((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))->__CBAS__member__len))) - ((long long)(((long long)(__cbas_local_variable_mangled_name_where)) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))->__CBAS__member__len))))))))));
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(__cbas_local_variable_mangled_name_where));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    
}
static inline long long __CBAS____method_str_____replace_first(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_what,struct __CBAS__type__str* __cbas_local_variable_mangled_name_with)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)__CBAS____method_str_____replace_first_from(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with)),((unsigned long long)0ull)));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_pstr_____replace_first(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_what,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_with)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)__CBAS____method_pstr_____replace_first_from(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with)),((unsigned long long)0ull)));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    
}
static inline long long __CBAS____method_str_____replace(struct __CBAS__type__str* __cbas_local_variable_mangled_name_this,struct __CBAS__type__str* __cbas_local_variable_mangled_name_what,struct __CBAS__type__str* __cbas_local_variable_mangled_name_with)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    long long __cbas_local_variable_mangled_name_w;
    if((long long)((unsigned long long)__CBAS____method_str_____empty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_str_____empty(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_w) = ((long long)((unsigned long long)0ull));
    while((long long)((unsigned long long)1ull)){

    long long __cbas_local_variable_mangled_name_w_new;
    (__cbas_local_variable_mangled_name_w_new) = ((long long)__CBAS____method_str_____replace_first_from(((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_what)),((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with)),((unsigned long long)(__cbas_local_variable_mangled_name_w))));
    if((long long)(((long long)(__cbas_local_variable_mangled_name_w_new)) == ((long long)(-((long long)((unsigned long long)1ull)))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((long long)(__cbas_local_variable_mangled_name_w_new)) == ((long long)(-((long long)((unsigned long long)2ull)))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((long long)(__cbas_local_variable_mangled_name_w_new)) == ((long long)(__cbas_local_variable_mangled_name_w)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_w) = ((long long)(((long long)(__cbas_local_variable_mangled_name_w_new)) + ((long long)((((struct __CBAS__type__str*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len)))));
    }

    
}
static inline long long __CBAS____method_pstr_____replace(struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_this,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_what,struct __CBAS__type__pstr* __cbas_local_variable_mangled_name_with)
{
    long long __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    long long __cbas_local_variable_mangled_name_w;
    if((long long)((unsigned long long)__CBAS____method_pstr_____empty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)(-((long long)((unsigned long long)1ull))));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)((unsigned long long)__CBAS____method_pstr_____empty(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_w) = ((long long)((unsigned long long)0ull));
    while((long long)((unsigned long long)1ull)){

    long long __cbas_local_variable_mangled_name_w_new;
    (__cbas_local_variable_mangled_name_w_new) = ((long long)__CBAS____method_pstr_____replace_first_from(((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_this)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_what)),((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with)),((unsigned long long)(__cbas_local_variable_mangled_name_w))));
    if((long long)(((long long)(__cbas_local_variable_mangled_name_w_new)) == ((long long)(-((long long)((unsigned long long)1ull)))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((long long)(__cbas_local_variable_mangled_name_w_new)) == ((long long)(-((long long)((unsigned long long)2ull)))))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    if((long long)(((long long)(__cbas_local_variable_mangled_name_w_new)) == ((long long)(__cbas_local_variable_mangled_name_w)))){

    (__cbas_local_variable_mangled_name___SEABASS_____RETVAL) = ((long long)((unsigned long long)0ull));
    
    return (long long)(__cbas_local_variable_mangled_name___SEABASS_____RETVAL);
    }

    (__cbas_local_variable_mangled_name_w) = ((long long)(((long long)(__cbas_local_variable_mangled_name_w_new)) + ((long long)((((struct __CBAS__type__pstr*)(__cbas_local_variable_mangled_name_with))->__CBAS__member__len)))));
    }

    
}
static inline void __CBAS__utoa(unsigned char* __cbas_local_variable_mangled_name_dest,unsigned long long __cbas_local_variable_mangled_name_value)
{
    if((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_value)) == ((unsigned long long)0ull))){

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)48ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)1ull)]) = ((unsigned char)((unsigned long long)0ull));
    
    return;
    }

    if((long long)((unsigned long long)1ull)){

    unsigned long long __cbas_local_variable_mangled_name_power;
    (__cbas_local_variable_mangled_name_power) = ((unsigned long long)1ull);
    while((long long)(((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_value)) / ((unsigned long long)(__cbas_local_variable_mangled_name_power)))) >= ((unsigned long long)10ull))){

    (__cbas_local_variable_mangled_name_power) = ((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_power)) * ((unsigned long long)10ull)));
    }

    while((long long)(__cbas_local_variable_mangled_name_power)){

    unsigned int __cbas_local_variable_mangled_name_temp;
    (__cbas_local_variable_mangled_name_temp) = ((unsigned int)((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_value)) / ((unsigned long long)(__cbas_local_variable_mangled_name_power)))));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_temp)) + ((unsigned long long)48ull))));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (__cbas_local_variable_mangled_name_value) = ((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_value)) - ((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_temp)) * ((unsigned long long)(__cbas_local_variable_mangled_name_power))))));
    (__cbas_local_variable_mangled_name_power) = ((unsigned long long)(((unsigned long long)(__cbas_local_variable_mangled_name_power)) / ((unsigned long long)10ull)));
    }

    }

    __cbas_mangled_label__ending:1;/*Statement with no effect generated for label...*/
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)0ull));
    
    return;
    
}
static inline void __CBAS__itoa(unsigned char* __cbas_local_variable_mangled_name_dest,long long __cbas_local_variable_mangled_name_value)
{
    if((long long)(((long long)(__cbas_local_variable_mangled_name_value)) >= ((long long)((unsigned long long)0ull)))){

    (void)__CBAS__utoa(((unsigned char*)(__cbas_local_variable_mangled_name_dest)),((unsigned long long)(__cbas_local_variable_mangled_name_value)));
    }

    else {
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)45ull));
    (void)__CBAS__utoa(((unsigned char*)(((unsigned char*)(__cbas_local_variable_mangled_name_dest)) + ((long long)((unsigned long long)1ull)))),((unsigned long long)((long long)(-((long long)(__cbas_local_variable_mangled_name_value))))));
    }

    
}
static inline void __CBAS__ftoa_n(unsigned char* __cbas_local_variable_mangled_name_dest,double __cbas_local_variable_mangled_name_v,int __cbas_local_variable_mangled_name_ndigits)
{
    unsigned long long __cbas_local_variable_mangled_name_is_neg;
    unsigned long long __cbas_local_variable_mangled_name_dg;
    long long __cbas_local_variable_mangled_name_pow10;
    unsigned long long __cbas_local_variable_mangled_name_dgi;
    double __cbas_local_variable_mangled_name_ftoa_errmargin;
    (__cbas_local_variable_mangled_name_is_neg) = ((unsigned long long)0ull);
    (__cbas_local_variable_mangled_name_dg) = ((unsigned long long)0ull);
    (__cbas_local_variable_mangled_name_pow10) = ((long long)((unsigned long long)0ull));
    (__cbas_local_variable_mangled_name_dgi) = ((unsigned long long)0ull);
    (int)((__cbas_local_variable_mangled_name_ndigits)++);
    if((long long)(((long long)(__cbas_local_variable_mangled_name_ndigits)) <= ((long long)((unsigned long long)0ull)))){

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)69ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)1ull)]) = ((unsigned char)((unsigned long long)82ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)2ull)]) = ((unsigned char)((unsigned long long)82ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)3ull)]) = ((unsigned char)((unsigned long long)0ull));
    
    return;
    }

    if((long long)((int)__CBAS__signbit(((double)(__cbas_local_variable_mangled_name_v))))){

    (__cbas_local_variable_mangled_name_is_neg) = ((unsigned long long)1ull);
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)45ull));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (__cbas_local_variable_mangled_name_v) = ((double)(-((double)(__cbas_local_variable_mangled_name_v))));
    }

    if((long long)((int)__CBAS__isnan(((double)(__cbas_local_variable_mangled_name_v))))){

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)78ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)1ull)]) = ((unsigned char)((unsigned long long)65ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)2ull)]) = ((unsigned char)((unsigned long long)78ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)3ull)]) = ((unsigned char)((unsigned long long)0ull));
    
    return;
    }

    if((long long)((unsigned long long)(!((long long)((int)__CBAS__isfinite(((double)(__cbas_local_variable_mangled_name_v)))))))){

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)73ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)1ull)]) = ((unsigned char)((unsigned long long)78ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)2ull)]) = ((unsigned char)((unsigned long long)70ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)3ull)]) = ((unsigned char)((unsigned long long)0ull));
    
    return;
    }

    if((long long)(((double)(__cbas_local_variable_mangled_name_v)) == ((double)((unsigned long long)0ull)))){

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)48ull));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)1ull)]) = ((unsigned char)((unsigned long long)0ull));
    
    return;
    }

    (__cbas_local_variable_mangled_name_pow10) = ((long long)((double)__CBAS__log10(((double)(__cbas_local_variable_mangled_name_v)))));
    if((long long)(((long long)(__cbas_local_variable_mangled_name_pow10)) >= ((long long)(((long long)(__cbas_local_variable_mangled_name_ndigits)) + ((long long)((unsigned long long)1ull)))))){

    goto __cbas_mangled_label__science;
    }

    else if((long long)(((long long)(__cbas_local_variable_mangled_name_pow10)) <= ((long long)(-((long long)(((long long)(__cbas_local_variable_mangled_name_ndigits)) + ((long long)((unsigned long long)1ull)))))))){

    goto __cbas_mangled_label__science;
    }

    else {
    while((long long)(((long long)(__cbas_local_variable_mangled_name_pow10)) >= ((long long)((unsigned long long)0ull)))){

    long long __cbas_local_variable_mangled_name_temp;
    (__cbas_local_variable_mangled_name_temp) = ((long long)((double)(((double)(__cbas_local_variable_mangled_name_v)) / ((double)__CBAS__pow(((double)((unsigned long long)10ull)),((double)(__cbas_local_variable_mangled_name_pow10)))))));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((long long)(((long long)(__cbas_local_variable_mangled_name_temp)) + ((long long)((unsigned long long)48ull)))));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (int)((__cbas_local_variable_mangled_name_ndigits)--);
    (__cbas_local_variable_mangled_name_v) = ((double)(((double)(__cbas_local_variable_mangled_name_v)) - ((double)(((double)(__cbas_local_variable_mangled_name_temp)) * ((double)__CBAS__pow(((double)((unsigned long long)10ull)),((double)(__cbas_local_variable_mangled_name_pow10))))))));
    (long long)((__cbas_local_variable_mangled_name_pow10)--);
    }

    if((long long)((unsigned long long)(((unsigned long long)((long long)(((double)(__cbas_local_variable_mangled_name_v)) == ((double)((unsigned long long)0ull))))) || ((unsigned long long)((long long)(((long long)(__cbas_local_variable_mangled_name_ndigits)) == ((long long)((unsigned long long)0ull)))))))){

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)46ull));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)48ull));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)0ull));
    
    return;
    }

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)46ull));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    while((long long)(((long long)(__cbas_local_variable_mangled_name_ndigits)) > ((long long)((unsigned long long)0ull)))){

    long long __cbas_local_variable_mangled_name_temp;
    (__cbas_local_variable_mangled_name_v) = ((double)(((double)(__cbas_local_variable_mangled_name_v)) * ((double)((unsigned long long)10ull))));
    (__cbas_local_variable_mangled_name_temp) = ((long long)(__cbas_local_variable_mangled_name_v));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((long long)(((long long)(__cbas_local_variable_mangled_name_temp)) + ((long long)((unsigned long long)48ull)))));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (__cbas_local_variable_mangled_name_v) = ((double)(((double)(__cbas_local_variable_mangled_name_v)) - ((double)(__cbas_local_variable_mangled_name_temp))));
    (int)((__cbas_local_variable_mangled_name_ndigits)--);
    }

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)0ull));
    
    return;
    }

    
    return;
    __cbas_mangled_label__science:1;/*Statement with no effect generated for label...*/
    if((long long)((unsigned long long)1ull)){

    long long __cbas_local_variable_mangled_name_dgc;
    (__cbas_local_variable_mangled_name_dgc) = ((long long)((unsigned long long)0ull));
    (__cbas_local_variable_mangled_name_v) = ((double)(((double)(__cbas_local_variable_mangled_name_v)) / ((double)__CBAS__pow(((double)((unsigned long long)10ull)),((double)(__cbas_local_variable_mangled_name_pow10))))));
    while((long long)(((long long)(__cbas_local_variable_mangled_name_ndigits)) > ((long long)((unsigned long long)0ull)))){

    long long __cbas_local_variable_mangled_name_temp;
    (__cbas_local_variable_mangled_name_temp) = ((long long)(__cbas_local_variable_mangled_name_v));
    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((long long)(((long long)(__cbas_local_variable_mangled_name_temp)) + ((long long)((unsigned long long)48ull)))));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (__cbas_local_variable_mangled_name_v) = ((double)(((double)(__cbas_local_variable_mangled_name_v)) - ((double)(__cbas_local_variable_mangled_name_temp))));
    (int)((__cbas_local_variable_mangled_name_ndigits)--);
    if((long long)((unsigned long long)(((unsigned long long)((long long)(((long long)(__cbas_local_variable_mangled_name_dgc)) == ((long long)((unsigned long long)0ull))))) && ((unsigned long long)((long long)(((long long)(__cbas_local_variable_mangled_name_ndigits)) != ((long long)((unsigned long long)0ull)))))))){

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)46ull));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    }

    (long long)((__cbas_local_variable_mangled_name_dgc)++);
    (__cbas_local_variable_mangled_name_v) = ((double)(((double)(__cbas_local_variable_mangled_name_v)) * ((double)((unsigned long long)10ull))));
    }

    (((unsigned char*)(__cbas_local_variable_mangled_name_dest))[(long long)((unsigned long long)0ull)]) = ((unsigned char)((unsigned long long)69ull));
    (unsigned char*)((__cbas_local_variable_mangled_name_dest)++);
    (void)__CBAS__itoa(((unsigned char*)(__cbas_local_variable_mangled_name_dest)),((long long)(__cbas_local_variable_mangled_name_pow10)));
    
    return;
    }

    
}
static inline void __CBAS__ftoa(unsigned char* __cbas_local_variable_mangled_name_dest,double __cbas_local_variable_mangled_name_v)
{
    (void)__CBAS__ftoa_n(((unsigned char*)(__cbas_local_variable_mangled_name_dest)),((double)(__cbas_local_variable_mangled_name_v)),((int)((unsigned long long)54ull)));
    
}
static inline void __CBAS__glAccum(unsigned int __cbas_local_variable_mangled_name_op,float __cbas_local_variable_mangled_name_value)
{
    glAccum(__cbas_local_variable_mangled_name_op,__cbas_local_variable_mangled_name_value);
    
}
static inline void __CBAS__glAlphaFunc(unsigned int __cbas_local_variable_mangled_name_funk,float __cbas_local_variable_mangled_name_ref)
{
    glAlphaFunc(__cbas_local_variable_mangled_name_funk,__cbas_local_variable_mangled_name_ref);
    
}
static inline unsigned char __CBAS__glAreTexturesResident(unsigned int __cbas_local_variable_mangled_name_n,unsigned int* __cbas_local_variable_mangled_name_textures,unsigned char* __cbas_local_variable_mangled_name_residences)
{
    unsigned char __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return glAreTexturesResident(
        __cbas_local_variable_mangled_name_n,
        __cbas_local_variable_mangled_name_textures,
        (GLboolean*)__cbas_local_variable_mangled_name_residences
    );
    
}
static inline void __CBAS__glArrayElement(int __cbas_local_variable_mangled_name_i)
{
    glArrayElement(__cbas_local_variable_mangled_name_i);
    
}
static inline void __CBAS__glBegin(unsigned int __cbas_local_variable_mangled_name_mode)
{
    glBegin(__cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glEnd()
{
    glEnd();
    
}
static inline void __CBAS__glBindTexture(unsigned int __cbas_local_variable_mangled_name_target,unsigned int __cbas_local_variable_mangled_name_texture)
{
    glBindTexture(__cbas_local_variable_mangled_name_target, __cbas_local_variable_mangled_name_texture);
    
}
static inline void __CBAS__glBitmap(int __cbas_local_variable_mangled_name_width,int __cbas_local_variable_mangled_name_height,int __cbas_local_variable_mangled_name_xorig,int __cbas_local_variable_mangled_name_yorig,int __cbas_local_variable_mangled_name_xmove,int __cbas_local_variable_mangled_name_ymove,unsigned char* __cbas_local_variable_mangled_name_bitmap)
{
    glBitmap(__cbas_local_variable_mangled_name_width, __cbas_local_variable_mangled_name_height,
    __cbas_local_variable_mangled_name_xorig, __cbas_local_variable_mangled_name_yorig, __cbas_local_variable_mangled_name_xmove, __cbas_local_variable_mangled_name_ymove, __cbas_local_variable_mangled_name_bitmap
    );
    
}
static inline void __CBAS__glBlendFunc(unsigned int __cbas_local_variable_mangled_name_sfactor,unsigned int __cbas_local_variable_mangled_name_dfactor)
{
    glBlendFunc(__cbas_local_variable_mangled_name_sfactor, __cbas_local_variable_mangled_name_dfactor);
    
}
static inline void __CBAS__glCallList(unsigned int __cbas_local_variable_mangled_name_l)
{
    glCallList(__cbas_local_variable_mangled_name_l);
    
}
static inline void __CBAS__glCallLists(unsigned int __cbas_local_variable_mangled_name_n,unsigned int __cbas_local_variable_mangled_name_type,unsigned char* __cbas_local_variable_mangled_name_lists)
{
    glCallLists(__cbas_local_variable_mangled_name_n, __cbas_local_variable_mangled_name_type, __cbas_local_variable_mangled_name_lists);
    
}
static inline void __CBAS__glClear(unsigned int __cbas_local_variable_mangled_name_mask)
{
    glClear(__cbas_local_variable_mangled_name_mask);
    
}
static inline void __CBAS__glClearAccum(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b,float __cbas_local_variable_mangled_name_a)
{
    glClearAccum(__cbas_local_variable_mangled_name_r,__cbas_local_variable_mangled_name_g,__cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glClearColor(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b,float __cbas_local_variable_mangled_name_a)
{
    glClearColor(__cbas_local_variable_mangled_name_r,__cbas_local_variable_mangled_name_g,__cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glClearDepth(double __cbas_local_variable_mangled_name_d)
{
    glClearDepth(__cbas_local_variable_mangled_name_d);
    
}
static inline void __CBAS__glClearIndex(float __cbas_local_variable_mangled_name_d)
{
    glClearIndex(__cbas_local_variable_mangled_name_d);
    
}
static inline void __CBAS__glClearStencil(int __cbas_local_variable_mangled_name_d)
{
    glClearStencil(__cbas_local_variable_mangled_name_d);
    
}
static inline void __CBAS__glClipPlane(unsigned int __cbas_local_variable_mangled_name_p,double* __cbas_local_variable_mangled_name_equa)
{
    glClipPlane(__cbas_local_variable_mangled_name_p, __cbas_local_variable_mangled_name_equa);
    
}
static inline void __CBAS__glColor3b(char __cbas_local_variable_mangled_name_r,char __cbas_local_variable_mangled_name_g,char __cbas_local_variable_mangled_name_b)
{
    glColor3b(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glColor3d(double __cbas_local_variable_mangled_name_r,double __cbas_local_variable_mangled_name_g,double __cbas_local_variable_mangled_name_b)
{
    glColor3d(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glColor3f(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b)
{
    glColor3f(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glColor3i(int __cbas_local_variable_mangled_name_r,int __cbas_local_variable_mangled_name_g,int __cbas_local_variable_mangled_name_b)
{
    glColor3i(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glColor3ub(unsigned char __cbas_local_variable_mangled_name_r,unsigned char __cbas_local_variable_mangled_name_g,unsigned char __cbas_local_variable_mangled_name_b)
{
    glColor3ub(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glColor3ui(unsigned int __cbas_local_variable_mangled_name_r,unsigned int __cbas_local_variable_mangled_name_g,unsigned int __cbas_local_variable_mangled_name_b)
{
    glColor3ui(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glColor3us(unsigned short __cbas_local_variable_mangled_name_r,unsigned short __cbas_local_variable_mangled_name_g,unsigned short __cbas_local_variable_mangled_name_b)
{
    glColor3us(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glColor3s(signed short __cbas_local_variable_mangled_name_r,signed short __cbas_local_variable_mangled_name_g,signed short __cbas_local_variable_mangled_name_b)
{
    glColor3s(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glColor4b(char __cbas_local_variable_mangled_name_r,char __cbas_local_variable_mangled_name_g,char __cbas_local_variable_mangled_name_b,char __cbas_local_variable_mangled_name_a)
{
    glColor4b(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColor4d(double __cbas_local_variable_mangled_name_r,double __cbas_local_variable_mangled_name_g,double __cbas_local_variable_mangled_name_b,double __cbas_local_variable_mangled_name_a)
{
    glColor4d(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColor4f(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b,float __cbas_local_variable_mangled_name_a)
{
    glColor4f(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColor4i(int __cbas_local_variable_mangled_name_r,int __cbas_local_variable_mangled_name_g,int __cbas_local_variable_mangled_name_b,int __cbas_local_variable_mangled_name_a)
{
    glColor4i(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColor4s(signed short __cbas_local_variable_mangled_name_r,signed short __cbas_local_variable_mangled_name_g,signed short __cbas_local_variable_mangled_name_b,signed short __cbas_local_variable_mangled_name_a)
{
    glColor4s(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColor4ub(unsigned char __cbas_local_variable_mangled_name_r,unsigned char __cbas_local_variable_mangled_name_g,unsigned char __cbas_local_variable_mangled_name_b,unsigned char __cbas_local_variable_mangled_name_a)
{
    glColor4ub(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColor4ui(unsigned int __cbas_local_variable_mangled_name_r,unsigned int __cbas_local_variable_mangled_name_g,unsigned int __cbas_local_variable_mangled_name_b,unsigned int __cbas_local_variable_mangled_name_a)
{
    glColor4ui(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColor4us(unsigned short __cbas_local_variable_mangled_name_r,unsigned short __cbas_local_variable_mangled_name_g,unsigned short __cbas_local_variable_mangled_name_b,unsigned short __cbas_local_variable_mangled_name_a)
{
    glColor4us(__cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_g, __cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColor3bv(char* __cbas_local_variable_mangled_name_r)
{
    glColor3bv((GLbyte*)__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor3ubv(unsigned char* __cbas_local_variable_mangled_name_r)
{
    glColor3ubv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor3dv(double* __cbas_local_variable_mangled_name_r)
{
    glColor3dv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor3fv(float* __cbas_local_variable_mangled_name_r)
{
    glColor3fv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor3iv(int* __cbas_local_variable_mangled_name_r)
{
    glColor3iv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor3uiv(unsigned int* __cbas_local_variable_mangled_name_r)
{
    glColor3uiv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor3sv(signed short* __cbas_local_variable_mangled_name_r)
{
    glColor3sv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor3usv(unsigned short* __cbas_local_variable_mangled_name_r)
{
    glColor3usv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor4bv(char* __cbas_local_variable_mangled_name_r)
{
    glColor4bv((GLbyte*)__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor4ubv(unsigned char* __cbas_local_variable_mangled_name_r)
{
    glColor4ubv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor4dv(double* __cbas_local_variable_mangled_name_r)
{
    glColor4dv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor4fv(float* __cbas_local_variable_mangled_name_r)
{
    glColor4fv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor4iv(int* __cbas_local_variable_mangled_name_r)
{
    glColor4iv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor4uiv(unsigned int* __cbas_local_variable_mangled_name_r)
{
    glColor4uiv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor4sv(signed short* __cbas_local_variable_mangled_name_r)
{
    glColor4sv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColor4usv(unsigned short* __cbas_local_variable_mangled_name_r)
{
    glColor4usv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glColorMask(unsigned char __cbas_local_variable_mangled_name_r,unsigned char __cbas_local_variable_mangled_name_g,unsigned char __cbas_local_variable_mangled_name_b,unsigned char __cbas_local_variable_mangled_name_a)
{
    glColorMask(__cbas_local_variable_mangled_name_r,__cbas_local_variable_mangled_name_g,__cbas_local_variable_mangled_name_b,__cbas_local_variable_mangled_name_a);
    
}
static inline void __CBAS__glColorMaterial(unsigned int __cbas_local_variable_mangled_name_face,unsigned int __cbas_local_variable_mangled_name_mode)
{
    glColorMaterial(__cbas_local_variable_mangled_name_face,__cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glColorPointer(int __cbas_local_variable_mangled_name_sz,unsigned int __cbas_local_variable_mangled_name_type,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glColorPointer(__cbas_local_variable_mangled_name_sz, __cbas_local_variable_mangled_name_type, __cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glCopyPixels(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_t)
{
    glCopyPixels(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_w, __cbas_local_variable_mangled_name_h, __cbas_local_variable_mangled_name_t);
    
}
static inline void __CBAS__glCopyTexImage1D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_internalfmt,int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_width,int __cbas_local_variable_mangled_name_border)
{
    glCopyTexImage1D(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_internalfmt,__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_width, __cbas_local_variable_mangled_name_border);
    
}
static inline void __CBAS__glCopyTexSubImage1D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_xoff,int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_width)
{
    glCopyTexSubImage1D(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_xoff, __cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_width);
    
}
static inline void __CBAS__glCopyTexImage2D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_internalfmt,int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_width,unsigned int __cbas_local_variable_mangled_name_height,int __cbas_local_variable_mangled_name_border)
{
    glCopyTexImage2D(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_internalfmt,__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_width, __cbas_local_variable_mangled_name_height, __cbas_local_variable_mangled_name_border);
    
}
static inline void __CBAS__glCopyTexSubImage2D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_xoff,int __cbas_local_variable_mangled_name_yoff,int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_width,unsigned int __cbas_local_variable_mangled_name_height)
{
    glCopyTexSubImage2D(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_xoff, __cbas_local_variable_mangled_name_yoff, __cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_width, __cbas_local_variable_mangled_name_height);
    
}
static inline void __CBAS__glCullFace(unsigned int __cbas_local_variable_mangled_name_mode)
{
    glCullFace(__cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glDeleteLists(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_range)
{
    glDeleteLists(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_range);
    
}
static inline void __CBAS__glDeleteTextures(unsigned int __cbas_local_variable_mangled_name_n,unsigned int* __cbas_local_variable_mangled_name_textures)
{
    glDeleteTextures(__cbas_local_variable_mangled_name_n, __cbas_local_variable_mangled_name_textures);
    
}
static inline void __CBAS__glDepthFunc(unsigned int __cbas_local_variable_mangled_name_funk)
{
    glDepthFunc(__cbas_local_variable_mangled_name_funk);
    
}
static inline void __CBAS__glDepthMask(unsigned char __cbas_local_variable_mangled_name_flag)
{
    glDepthMask(__cbas_local_variable_mangled_name_flag);
    
}
static inline void __CBAS__glDepthRange(double __cbas_local_variable_mangled_name_zNear,double __cbas_local_variable_mangled_name_zFar)
{
    glDepthRange(__cbas_local_variable_mangled_name_zNear, __cbas_local_variable_mangled_name_zFar);
    
}
static inline void __CBAS__glDrawArrays(unsigned int __cbas_local_variable_mangled_name_mode,int __cbas_local_variable_mangled_name_first,unsigned int __cbas_local_variable_mangled_name_count)
{
    glDrawArrays(__cbas_local_variable_mangled_name_mode, __cbas_local_variable_mangled_name_first, __cbas_local_variable_mangled_name_count);
    
}
static inline void __CBAS__glDrawBuffer(unsigned int __cbas_local_variable_mangled_name_mode)
{
    glDrawBuffer(__cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glDrawElements(unsigned int __cbas_local_variable_mangled_name_mode,unsigned int __cbas_local_variable_mangled_name_count,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_indices)
{
    glDrawElements(__cbas_local_variable_mangled_name_mode, __cbas_local_variable_mangled_name_count, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_indices);
    
}
static inline void __CBAS__glDrawPixels(unsigned int __cbas_local_variable_mangled_name_width,unsigned int __cbas_local_variable_mangled_name_height,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glDrawPixels(__cbas_local_variable_mangled_name_width, __cbas_local_variable_mangled_name_height, __cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glEdgeFlag(unsigned char __cbas_local_variable_mangled_name_flag)
{
    glEdgeFlag(__cbas_local_variable_mangled_name_flag);
    
}
static inline void __CBAS__glEdgeFlagv(unsigned char* __cbas_local_variable_mangled_name_flag)
{
    glEdgeFlagv(__cbas_local_variable_mangled_name_flag);
    
}
static inline void __CBAS__glEdgeFlagPointer(unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glEdgeFlagPointer(__cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glEnable(unsigned int __cbas_local_variable_mangled_name_cap)
{
    glEnable(__cbas_local_variable_mangled_name_cap);
    
}
static inline void __CBAS__glDisable(unsigned int __cbas_local_variable_mangled_name_cap)
{
    glDisable(__cbas_local_variable_mangled_name_cap);
    
}
static inline void __CBAS__glEnableClientState(unsigned int __cbas_local_variable_mangled_name_cap)
{
    glEnableClientState(__cbas_local_variable_mangled_name_cap);
    
}
static inline void __CBAS__glDisableClientState(unsigned int __cbas_local_variable_mangled_name_cap)
{
    glDisableClientState(__cbas_local_variable_mangled_name_cap);
    
}
static inline void __CBAS__glEvalCoord1d(double __cbas_local_variable_mangled_name_u)
{
    glEvalCoord1d(__cbas_local_variable_mangled_name_u);
    
}
static inline void __CBAS__glEvalCoord1f(float __cbas_local_variable_mangled_name_u)
{
    glEvalCoord1f(__cbas_local_variable_mangled_name_u);
    
}
static inline void __CBAS__glEvalCoord2d(double __cbas_local_variable_mangled_name_u,double __cbas_local_variable_mangled_name_v)
{
    glEvalCoord2d(__cbas_local_variable_mangled_name_u, __cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glEvalCoord2f(float __cbas_local_variable_mangled_name_u,float __cbas_local_variable_mangled_name_v)
{
    glEvalCoord2f(__cbas_local_variable_mangled_name_u, __cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glEvalCoord1dv(double* __cbas_local_variable_mangled_name_u)
{
    glEvalCoord1dv(__cbas_local_variable_mangled_name_u);
    
}
static inline void __CBAS__glEvalCoord1fv(float* __cbas_local_variable_mangled_name_u)
{
    glEvalCoord1fv(__cbas_local_variable_mangled_name_u);
    
}
static inline void __CBAS__glEvalCoord2dv(double* __cbas_local_variable_mangled_name_u)
{
    glEvalCoord2dv(__cbas_local_variable_mangled_name_u);
    
}
static inline void __CBAS__glEvalCoord2fv(float* __cbas_local_variable_mangled_name_u)
{
    glEvalCoord2fv(__cbas_local_variable_mangled_name_u);
    
}
static inline void __CBAS__glEvalMesh1(unsigned int __cbas_local_variable_mangled_name_mode,int __cbas_local_variable_mangled_name_i1,int __cbas_local_variable_mangled_name_i2)
{
    glEvalMesh1(__cbas_local_variable_mangled_name_mode, __cbas_local_variable_mangled_name_i1, __cbas_local_variable_mangled_name_i2);
    
}
static inline void __CBAS__glEvalMesh2(unsigned int __cbas_local_variable_mangled_name_mode,int __cbas_local_variable_mangled_name_i1,int __cbas_local_variable_mangled_name_i2,int __cbas_local_variable_mangled_name_j1,int __cbas_local_variable_mangled_name_j2)
{
    glEvalMesh2(__cbas_local_variable_mangled_name_mode, __cbas_local_variable_mangled_name_i1, __cbas_local_variable_mangled_name_i2, __cbas_local_variable_mangled_name_j1, __cbas_local_variable_mangled_name_j2);
    
}
static inline void __CBAS__glEvalPoint1(int __cbas_local_variable_mangled_name_i)
{
    glEvalPoint1(__cbas_local_variable_mangled_name_i);
    
}
static inline void __CBAS__glEvalPoint2(int __cbas_local_variable_mangled_name_i,int __cbas_local_variable_mangled_name_j)
{
    glEvalPoint2(__cbas_local_variable_mangled_name_i, __cbas_local_variable_mangled_name_j);
    
}
static inline void __CBAS__glFeedbackBuffer(unsigned int __cbas_local_variable_mangled_name_sz,unsigned int __cbas_local_variable_mangled_name_t,float* __cbas_local_variable_mangled_name_b)
{
    glFeedbackBuffer(__cbas_local_variable_mangled_name_sz, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glFinish()
{
    glFinish();
    
}
static inline void __CBAS__glFlush()
{
    glFlush();
    
}
static inline void __CBAS__glFogf(unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param)
{
    glFogf(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glFogi(unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param)
{
    glFogi(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glFogfv(unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param)
{
    glFogfv(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glFogiv(unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param)
{
    glFogiv(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glFrontFace(unsigned int __cbas_local_variable_mangled_name_mode)
{
    glFrontFace(__cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glFrustum(double __cbas_local_variable_mangled_name_left,double __cbas_local_variable_mangled_name_right,double __cbas_local_variable_mangled_name_bottom,double __cbas_local_variable_mangled_name_top,double __cbas_local_variable_mangled_name_zNear,double __cbas_local_variable_mangled_name_zFar)
{
    glFrustum(__cbas_local_variable_mangled_name_left, __cbas_local_variable_mangled_name_right, __cbas_local_variable_mangled_name_bottom, __cbas_local_variable_mangled_name_top, __cbas_local_variable_mangled_name_zNear, __cbas_local_variable_mangled_name_zFar);
    
}
static inline void __CBAS__glPerspective(double __cbas_local_variable_mangled_name_fov,double __cbas_local_variable_mangled_name_aspect,double __cbas_local_variable_mangled_name_zn,double __cbas_local_variable_mangled_name_zf)
{
    double __cbas_local_variable_mangled_name_fw;
    double __cbas_local_variable_mangled_name_fh;
    (__cbas_local_variable_mangled_name_fh) = ((double)(((double)__CBAS__tan(((double)(((double)(((double)__CBAS__PI()) * ((double)(__cbas_local_variable_mangled_name_fov)))) / ((double)((unsigned long long)360ull)))))) * ((double)(__cbas_local_variable_mangled_name_zn))));
    (__cbas_local_variable_mangled_name_fw) = ((double)(((double)(__cbas_local_variable_mangled_name_aspect)) * ((double)(__cbas_local_variable_mangled_name_fh))));
    (void)__CBAS__glFrustum(((double)(-((double)(__cbas_local_variable_mangled_name_fw)))),((double)(__cbas_local_variable_mangled_name_fw)),((double)(-((double)(__cbas_local_variable_mangled_name_fh)))),((double)(__cbas_local_variable_mangled_name_fh)),((double)(__cbas_local_variable_mangled_name_zn)),((double)(__cbas_local_variable_mangled_name_zf)));
    
}
static inline unsigned int __CBAS__glGenLists(unsigned int __cbas_local_variable_mangled_name_range)
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return glGenLists(__cbas_local_variable_mangled_name_range);
    
}
static inline void __CBAS__glGenTextures(unsigned int __cbas_local_variable_mangled_name_n,unsigned int* __cbas_local_variable_mangled_name_arr)
{
    glGenTextures(__cbas_local_variable_mangled_name_n, __cbas_local_variable_mangled_name_arr);
    
}
static inline void __CBAS__glGetBooleanv(unsigned int __cbas_local_variable_mangled_name_pname,unsigned char* __cbas_local_variable_mangled_name_params)
{
    glGetBooleanv(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetDoublev(unsigned int __cbas_local_variable_mangled_name_pname,double* __cbas_local_variable_mangled_name_params)
{
    glGetDoublev(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetFloatv(unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params)
{
    glGetFloatv(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetIntegerv(unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params)
{
    glGetIntegerv(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetClipPlane(unsigned int __cbas_local_variable_mangled_name_p,double* __cbas_local_variable_mangled_name_equa)
{
    glGetClipPlane(__cbas_local_variable_mangled_name_p, __cbas_local_variable_mangled_name_equa);
    
}
static inline unsigned int __CBAS__glGetError()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return glGetError();
    
}
static inline void __CBAS__glGetLightfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params)
{
    glGetLightfv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetLightiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params)
{
    glGetLightiv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetMapdv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,double* __cbas_local_variable_mangled_name_params)
{
    glGetMapdv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetMapfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params)
{
    glGetMapfv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetMapiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params)
{
    glGetMapiv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetMaterialfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params)
{
    glGetMaterialfv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetMaterialiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params)
{
    glGetMaterialiv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetPixelMapfv(unsigned int __cbas_local_variable_mangled_name_l,float* __cbas_local_variable_mangled_name_params)
{
    glGetPixelMapfv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetPixelMapuiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int* __cbas_local_variable_mangled_name_params)
{
    glGetPixelMapuiv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetPixelMapusv(unsigned int __cbas_local_variable_mangled_name_l,unsigned short* __cbas_local_variable_mangled_name_params)
{
    glGetPixelMapusv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetPointerv(unsigned int __cbas_local_variable_mangled_name_pname,unsigned char* __cbas_local_variable_mangled_name_params)
{
    glGetPointerv(__cbas_local_variable_mangled_name_pname, (void*)__cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetPolygonStipple(unsigned char* __cbas_local_variable_mangled_name_mask)
{
    glGetPolygonStipple(__cbas_local_variable_mangled_name_mask);
    
}
static inline unsigned char* __CBAS__glGetString(unsigned int __cbas_local_variable_mangled_name_nm)
{
    unsigned char* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return (unsigned char*)glGetString(__cbas_local_variable_mangled_name_nm);
    
}
static inline void __CBAS__glGetTexEnvfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params)
{
    glGetTexEnvfv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetTexEnviv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params)
{
    glGetTexEnviv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetTexGendv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,double* __cbas_local_variable_mangled_name_params)
{
    glGetTexGendv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetTexGenfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params)
{
    glGetTexGenfv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetTexGeniv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params)
{
    glGetTexGeniv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetTexImage(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_px)
{
    glGetTexImage(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_px);
    
}
static inline void __CBAS__glGetTexLevelParameterfv(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params)
{
    glGetTexLevelParameterfv(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetTexLevelParameteriv(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params)
{
    glGetTexLevelParameteriv(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetTexParameterfv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_params)
{
    glGetTexParameterfv(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glGetTexParameteriv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_params)
{
    glGetTexParameteriv(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_params);
    
}
static inline void __CBAS__glHint(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_md)
{
    glHint(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_md);
    
}
static inline void __CBAS__glIndexd(double __cbas_local_variable_mangled_name_c)
{
    glIndexd(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexf(float __cbas_local_variable_mangled_name_c)
{
    glIndexf(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexi(int __cbas_local_variable_mangled_name_c)
{
    glIndexi(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexs(signed short __cbas_local_variable_mangled_name_c)
{
    glIndexs(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexub(unsigned char __cbas_local_variable_mangled_name_c)
{
    glIndexub(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexdv(double* __cbas_local_variable_mangled_name_c)
{
    glIndexdv(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexfv(float* __cbas_local_variable_mangled_name_c)
{
    glIndexfv(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexiv(int* __cbas_local_variable_mangled_name_c)
{
    glIndexiv(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexsv(signed short* __cbas_local_variable_mangled_name_c)
{
    glIndexsv(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexubv(unsigned char* __cbas_local_variable_mangled_name_c)
{
    glIndexubv(__cbas_local_variable_mangled_name_c);
    
}
static inline void __CBAS__glIndexMask(unsigned int __cbas_local_variable_mangled_name_m)
{
    glIndexMask(__cbas_local_variable_mangled_name_m);
    
}
static inline void __CBAS__glIndexPointer(unsigned int __cbas_local_variable_mangled_name_t,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glIndexPointer(__cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glInitNames()
{
    glInitNames();
    
}
static inline void __CBAS__glInterleavedArrays(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glInterleavedArrays(__cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_p);
    
}
static inline unsigned char __CBAS__glIsEnabled(unsigned int __cbas_local_variable_mangled_name_cap)
{
    unsigned char __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return glIsEnabled(__cbas_local_variable_mangled_name_cap);
    
}
static inline unsigned char __CBAS__glIsList(unsigned int __cbas_local_variable_mangled_name_cap)
{
    unsigned char __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return glIsList(__cbas_local_variable_mangled_name_cap);
    
}
static inline unsigned char __CBAS__glIsTexture(unsigned int __cbas_local_variable_mangled_name_cap)
{
    unsigned char __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return glIsTexture(__cbas_local_variable_mangled_name_cap);
    
}
static inline void __CBAS__glLightf(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param)
{
    glLightf(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glLighti(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param)
{
    glLighti(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glLightfv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param)
{
    glLightfv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glLightiv(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param)
{
    glLightiv(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glLightModelf(unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param)
{
    glLightModelf(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glLightModeli(unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param)
{
    glLightModeli(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glLightModelfv(unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param)
{
    glLightModelfv(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glLightModeliv(unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param)
{
    glLightModeliv(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glLineStipple(int __cbas_local_variable_mangled_name_factor,unsigned short __cbas_local_variable_mangled_name_pattern)
{
    glLineStipple(__cbas_local_variable_mangled_name_factor, __cbas_local_variable_mangled_name_pattern);
    
}
static inline void __CBAS__glLineWidth(float __cbas_local_variable_mangled_name_w)
{
    glLineWidth(__cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glListBase(unsigned int __cbas_local_variable_mangled_name_b)
{
    glListBase(__cbas_local_variable_mangled_name_b);
    
}
static inline void __CBAS__glLoadIdentity()
{
    glLoadIdentity();
    
}
static inline void __CBAS__glLoadMatrixd(double* __cbas_local_variable_mangled_name_m)
{
    glLoadMatrixd(__cbas_local_variable_mangled_name_m);
    
}
static inline void __CBAS__glLoadMatrixf(float* __cbas_local_variable_mangled_name_m)
{
    glLoadMatrixf(__cbas_local_variable_mangled_name_m);
    
}
static inline void __CBAS__glLoadName(unsigned int __cbas_local_variable_mangled_name_n)
{
    glLoadName(__cbas_local_variable_mangled_name_n);
    
}
static inline void __CBAS__glLogicOp(unsigned int __cbas_local_variable_mangled_name_n)
{
    glLogicOp(__cbas_local_variable_mangled_name_n);
    
}
static inline void __CBAS__glMap1d(unsigned int __cbas_local_variable_mangled_name_tg,double __cbas_local_variable_mangled_name_u1,double __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_stride,int __cbas_local_variable_mangled_name_order,double* __cbas_local_variable_mangled_name_p)
{
    glMap1d(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_u1, __cbas_local_variable_mangled_name_u2, __cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_order, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glMap1f(unsigned int __cbas_local_variable_mangled_name_tg,float __cbas_local_variable_mangled_name_u1,float __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_stride,int __cbas_local_variable_mangled_name_order,float* __cbas_local_variable_mangled_name_p)
{
    glMap1f(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_u1, __cbas_local_variable_mangled_name_u2, __cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_order, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glMap2d(unsigned int __cbas_local_variable_mangled_name_tg,double __cbas_local_variable_mangled_name_u1,double __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_ustride,int __cbas_local_variable_mangled_name_uorder,double __cbas_local_variable_mangled_name_v1,double __cbas_local_variable_mangled_name_v2,int __cbas_local_variable_mangled_name_vstride,int __cbas_local_variable_mangled_name_vorder,double* __cbas_local_variable_mangled_name_p)
{
    glMap2d(__cbas_local_variable_mangled_name_tg, 
    __cbas_local_variable_mangled_name_u1, __cbas_local_variable_mangled_name_u2, __cbas_local_variable_mangled_name_ustride, __cbas_local_variable_mangled_name_uorder, 
    __cbas_local_variable_mangled_name_v1, __cbas_local_variable_mangled_name_v2, __cbas_local_variable_mangled_name_vstride, __cbas_local_variable_mangled_name_vorder, 
    __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glMap2f(unsigned int __cbas_local_variable_mangled_name_tg,float __cbas_local_variable_mangled_name_u1,float __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_ustride,int __cbas_local_variable_mangled_name_uorder,float __cbas_local_variable_mangled_name_v1,float __cbas_local_variable_mangled_name_v2,int __cbas_local_variable_mangled_name_vstride,int __cbas_local_variable_mangled_name_vorder,float* __cbas_local_variable_mangled_name_p)
{
    glMap2f(__cbas_local_variable_mangled_name_tg, 
    __cbas_local_variable_mangled_name_u1, __cbas_local_variable_mangled_name_u2, __cbas_local_variable_mangled_name_ustride, __cbas_local_variable_mangled_name_uorder, 
    __cbas_local_variable_mangled_name_v1, __cbas_local_variable_mangled_name_v2, __cbas_local_variable_mangled_name_vstride, __cbas_local_variable_mangled_name_vorder, 
    __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glMapGrid1d(int __cbas_local_variable_mangled_name_un,double __cbas_local_variable_mangled_name_u1,double __cbas_local_variable_mangled_name_u2)
{
    glMapGrid1d(__cbas_local_variable_mangled_name_un, __cbas_local_variable_mangled_name_u1, __cbas_local_variable_mangled_name_u2);
    
}
static inline void __CBAS__glMapGrid1f(int __cbas_local_variable_mangled_name_un,float __cbas_local_variable_mangled_name_u1,float __cbas_local_variable_mangled_name_u2)
{
    glMapGrid1f(__cbas_local_variable_mangled_name_un, __cbas_local_variable_mangled_name_u1, __cbas_local_variable_mangled_name_u2);
    
}
static inline void __CBAS__glMapGrid2d(int __cbas_local_variable_mangled_name_un,double __cbas_local_variable_mangled_name_u1,double __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_vn,double __cbas_local_variable_mangled_name_v1,double __cbas_local_variable_mangled_name_v2)
{
    glMapGrid2d(__cbas_local_variable_mangled_name_un, __cbas_local_variable_mangled_name_u1, __cbas_local_variable_mangled_name_u2, __cbas_local_variable_mangled_name_vn, __cbas_local_variable_mangled_name_v1, __cbas_local_variable_mangled_name_v2);
    
}
static inline void __CBAS__glMapGrid2f(int __cbas_local_variable_mangled_name_un,float __cbas_local_variable_mangled_name_u1,float __cbas_local_variable_mangled_name_u2,int __cbas_local_variable_mangled_name_vn,float __cbas_local_variable_mangled_name_v1,float __cbas_local_variable_mangled_name_v2)
{
    glMapGrid2f(__cbas_local_variable_mangled_name_un, __cbas_local_variable_mangled_name_u1, __cbas_local_variable_mangled_name_u2, __cbas_local_variable_mangled_name_vn, __cbas_local_variable_mangled_name_v1, __cbas_local_variable_mangled_name_v2);
    
}
static inline void __CBAS__glMaterialf(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param)
{
    glMaterialf(__cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glMateriali(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param)
{
    glMateriali(__cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glMaterialfv(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param)
{
    glMaterialfv(__cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glMaterialiv(unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param)
{
    glMaterialiv(__cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glMatrixMode(unsigned int __cbas_local_variable_mangled_name_n)
{
    glMatrixMode(__cbas_local_variable_mangled_name_n);
    
}
static inline void __CBAS__glMultMatrixd(double* __cbas_local_variable_mangled_name_m)
{
    glMultMatrixd(__cbas_local_variable_mangled_name_m);
    
}
static inline void __CBAS__glMultMatrixf(float* __cbas_local_variable_mangled_name_m)
{
    glMultMatrixf(__cbas_local_variable_mangled_name_m);
    
}
static inline void __CBAS__glNewList(unsigned int __cbas_local_variable_mangled_name_l,unsigned int __cbas_local_variable_mangled_name_m)
{
    glNewList(__cbas_local_variable_mangled_name_l, __cbas_local_variable_mangled_name_m);
    
}
static inline void __CBAS__glEndList()
{
    glEndList();
    
}
static inline void __CBAS__glNormal3bv(char* __cbas_local_variable_mangled_name_r)
{
    glNormal3bv((GLbyte*)__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glNormal3dv(double* __cbas_local_variable_mangled_name_r)
{
    glNormal3dv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glNormal3fv(float* __cbas_local_variable_mangled_name_r)
{
    glNormal3fv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glNormal3iv(int* __cbas_local_variable_mangled_name_r)
{
    glNormal3iv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glNormal3sv(signed short* __cbas_local_variable_mangled_name_r)
{
    glNormal3sv(__cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glNormal3b(char __cbas_local_variable_mangled_name_x,char __cbas_local_variable_mangled_name_y,char __cbas_local_variable_mangled_name_z)
{
    glNormal3b(__cbas_local_variable_mangled_name_x,__cbas_local_variable_mangled_name_y,__cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glNormal3d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z)
{
    glNormal3d(__cbas_local_variable_mangled_name_x,__cbas_local_variable_mangled_name_y,__cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glNormal3f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z)
{
    glNormal3f(__cbas_local_variable_mangled_name_x,__cbas_local_variable_mangled_name_y,__cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glNormal3i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z)
{
    glNormal3i(__cbas_local_variable_mangled_name_x,__cbas_local_variable_mangled_name_y,__cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glNormal3s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z)
{
    glNormal3s(__cbas_local_variable_mangled_name_x,__cbas_local_variable_mangled_name_y,__cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glNormalPointer(unsigned int __cbas_local_variable_mangled_name_t,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glNormalPointer(__cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glOrtho(double __cbas_local_variable_mangled_name_left,double __cbas_local_variable_mangled_name_right,double __cbas_local_variable_mangled_name_bottom,double __cbas_local_variable_mangled_name_top,double __cbas_local_variable_mangled_name_zNear,double __cbas_local_variable_mangled_name_zFar)
{
    glOrtho(__cbas_local_variable_mangled_name_left, __cbas_local_variable_mangled_name_right, __cbas_local_variable_mangled_name_bottom, __cbas_local_variable_mangled_name_top, __cbas_local_variable_mangled_name_zNear, __cbas_local_variable_mangled_name_zFar);
    
}
static inline void __CBAS__glPixelMapfv(unsigned int __cbas_local_variable_mangled_name_m,unsigned int __cbas_local_variable_mangled_name_msz,float* __cbas_local_variable_mangled_name_v)
{
    glPixelMapfv(__cbas_local_variable_mangled_name_m, __cbas_local_variable_mangled_name_msz, __cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glPixelMapuiv(unsigned int __cbas_local_variable_mangled_name_m,unsigned int __cbas_local_variable_mangled_name_msz,unsigned int* __cbas_local_variable_mangled_name_v)
{
    glPixelMapuiv(__cbas_local_variable_mangled_name_m, __cbas_local_variable_mangled_name_msz, __cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glPixelMapusv(unsigned int __cbas_local_variable_mangled_name_m,unsigned int __cbas_local_variable_mangled_name_msz,unsigned short* __cbas_local_variable_mangled_name_v)
{
    glPixelMapusv(__cbas_local_variable_mangled_name_m, __cbas_local_variable_mangled_name_msz, __cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glPixelStoref(unsigned int __cbas_local_variable_mangled_name_m,float __cbas_local_variable_mangled_name_v)
{
    glPixelStoref(__cbas_local_variable_mangled_name_m, __cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glPixelStorei(unsigned int __cbas_local_variable_mangled_name_m,int __cbas_local_variable_mangled_name_v)
{
    glPixelStorei(__cbas_local_variable_mangled_name_m, __cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glPixelTransferf(unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param)
{
    glPixelTransferf(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glPixelTransferi(unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param)
{
    glPixelTransferi(__cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glPixelZoom(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y)
{
    glPixelZoom(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glPointSize(float __cbas_local_variable_mangled_name_sz)
{
    glPointSize(__cbas_local_variable_mangled_name_sz);
    
}
static inline void __CBAS__glPolygonMode(unsigned int __cbas_local_variable_mangled_name_face,unsigned int __cbas_local_variable_mangled_name_mode)
{
    glPolygonMode(__cbas_local_variable_mangled_name_face, __cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glPolygonOffset(float __cbas_local_variable_mangled_name_factor,float __cbas_local_variable_mangled_name_units)
{
    glPolygonOffset(__cbas_local_variable_mangled_name_factor, __cbas_local_variable_mangled_name_units);
    
}
static inline void __CBAS__glPolygonStipple(unsigned char* __cbas_local_variable_mangled_name_mask)
{
    glPolygonStipple(__cbas_local_variable_mangled_name_mask);
    
}
static inline void __CBAS__glPrioritizeTextures(unsigned int __cbas_local_variable_mangled_name_n,unsigned int* __cbas_local_variable_mangled_name_textures,float* __cbas_local_variable_mangled_name_p)
{
    glPrioritizeTextures(__cbas_local_variable_mangled_name_n, __cbas_local_variable_mangled_name_textures, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glPushAttrib(unsigned int __cbas_local_variable_mangled_name_mask)
{
    glPushAttrib(__cbas_local_variable_mangled_name_mask);
    
}
static inline void __CBAS__glPopAttrib()
{
    glPopAttrib();
    
}
static inline void __CBAS__glPushClientAttrib(unsigned int __cbas_local_variable_mangled_name_mask)
{
    glPushClientAttrib(__cbas_local_variable_mangled_name_mask);
    
}
static inline void __CBAS__glPopClientAttrib()
{
    glPopClientAttrib();
    
}
static inline void __CBAS__glPushMatrix()
{
    glPushMatrix();
    
}
static inline void __CBAS__glPopMatrix()
{
    glPopMatrix();
    
}
static inline void __CBAS__glPushName(unsigned int __cbas_local_variable_mangled_name_name)
{
    glPushName(__cbas_local_variable_mangled_name_name);
    
}
static inline void __CBAS__glPopName()
{
    glPopName();
    
}
static inline void __CBAS__glRasterPos2d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y)
{
    glRasterPos2d(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glRasterPos2f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y)
{
    glRasterPos2f(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glRasterPos2i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y)
{
    glRasterPos2i(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glRasterPos2s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y)
{
    glRasterPos2s(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glRasterPos3d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z)
{
    glRasterPos3d(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glRasterPos3f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z)
{
    glRasterPos3f(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glRasterPos3i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z)
{
    glRasterPos3i(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glRasterPos3s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z)
{
    glRasterPos3s(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glRasterPos4d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z,double __cbas_local_variable_mangled_name_w)
{
    glRasterPos4d(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z, __cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glRasterPos4f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z,float __cbas_local_variable_mangled_name_w)
{
    glRasterPos4f(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z, __cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glRasterPos4i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z,int __cbas_local_variable_mangled_name_w)
{
    glRasterPos4i(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z, __cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glRasterPos4s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z,signed short __cbas_local_variable_mangled_name_w)
{
    glRasterPos4s(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z, __cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glRasterPos2dv(double* __cbas_local_variable_mangled_name_x)
{
    glRasterPos2dv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos2fv(float* __cbas_local_variable_mangled_name_x)
{
    glRasterPos2fv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos2iv(int* __cbas_local_variable_mangled_name_x)
{
    glRasterPos2iv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos2sv(signed short* __cbas_local_variable_mangled_name_x)
{
    glRasterPos2sv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos3dv(double* __cbas_local_variable_mangled_name_x)
{
    glRasterPos3dv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos3fv(float* __cbas_local_variable_mangled_name_x)
{
    glRasterPos3fv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos3iv(int* __cbas_local_variable_mangled_name_x)
{
    glRasterPos3iv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos3sv(signed short* __cbas_local_variable_mangled_name_x)
{
    glRasterPos3sv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos4dv(double* __cbas_local_variable_mangled_name_x)
{
    glRasterPos4dv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos4fv(float* __cbas_local_variable_mangled_name_x)
{
    glRasterPos4fv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos4iv(int* __cbas_local_variable_mangled_name_x)
{
    glRasterPos4iv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glRasterPos4sv(signed short* __cbas_local_variable_mangled_name_x)
{
    glRasterPos4sv(__cbas_local_variable_mangled_name_x);
    
}
static inline void __CBAS__glReadBuffer(unsigned int __cbas_local_variable_mangled_name_mode)
{
    glReadBuffer(__cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glReadPixels(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_px)
{
    glReadPixels(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_w, __cbas_local_variable_mangled_name_h, __cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_px);
    
}
static inline void __CBAS__glRectd(double __cbas_local_variable_mangled_name_x1,double __cbas_local_variable_mangled_name_y1,double __cbas_local_variable_mangled_name_x2,double __cbas_local_variable_mangled_name_y2)
{
    glRectd(__cbas_local_variable_mangled_name_x1, __cbas_local_variable_mangled_name_y1, __cbas_local_variable_mangled_name_x2, __cbas_local_variable_mangled_name_y2);
    
}
static inline void __CBAS__glRectf(float __cbas_local_variable_mangled_name_x1,float __cbas_local_variable_mangled_name_y1,float __cbas_local_variable_mangled_name_x2,float __cbas_local_variable_mangled_name_y2)
{
    glRectf(__cbas_local_variable_mangled_name_x1, __cbas_local_variable_mangled_name_y1, __cbas_local_variable_mangled_name_x2, __cbas_local_variable_mangled_name_y2);
    
}
static inline void __CBAS__glRecti(int __cbas_local_variable_mangled_name_x1,int __cbas_local_variable_mangled_name_y1,int __cbas_local_variable_mangled_name_x2,int __cbas_local_variable_mangled_name_y2)
{
    glRecti(__cbas_local_variable_mangled_name_x1, __cbas_local_variable_mangled_name_y1, __cbas_local_variable_mangled_name_x2, __cbas_local_variable_mangled_name_y2);
    
}
static inline void __CBAS__glRects(signed short __cbas_local_variable_mangled_name_x1,signed short __cbas_local_variable_mangled_name_y1,signed short __cbas_local_variable_mangled_name_x2,signed short __cbas_local_variable_mangled_name_y2)
{
    glRects(__cbas_local_variable_mangled_name_x1, __cbas_local_variable_mangled_name_y1, __cbas_local_variable_mangled_name_x2, __cbas_local_variable_mangled_name_y2);
    
}
static inline void __CBAS__glRectdv(double* __cbas_local_variable_mangled_name_v1,double* __cbas_local_variable_mangled_name_v2)
{
    glRectdv(__cbas_local_variable_mangled_name_v1, __cbas_local_variable_mangled_name_v2);
    
}
static inline void __CBAS__glRectfv(float* __cbas_local_variable_mangled_name_v1,float* __cbas_local_variable_mangled_name_v2)
{
    glRectfv(__cbas_local_variable_mangled_name_v1, __cbas_local_variable_mangled_name_v2);
    
}
static inline void __CBAS__glRectiv(int* __cbas_local_variable_mangled_name_v1,int* __cbas_local_variable_mangled_name_v2)
{
    glRectiv(__cbas_local_variable_mangled_name_v1, __cbas_local_variable_mangled_name_v2);
    
}
static inline void __CBAS__glRectsv(signed short* __cbas_local_variable_mangled_name_v1,signed short* __cbas_local_variable_mangled_name_v2)
{
    glRectsv(__cbas_local_variable_mangled_name_v1, __cbas_local_variable_mangled_name_v2);
    
}
static inline void __CBAS__glRenderMode(unsigned int __cbas_local_variable_mangled_name_mode)
{
    glRenderMode(__cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glRotated(double __cbas_local_variable_mangled_name_angle,double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z)
{
    glRotated(__cbas_local_variable_mangled_name_angle, __cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glRotatef(float __cbas_local_variable_mangled_name_angle,float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z)
{
    glRotatef(__cbas_local_variable_mangled_name_angle, __cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glScaled(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z)
{
    glScaled(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glScalef(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z)
{
    glScalef(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glScissor(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h)
{
    glScissor(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_w, __cbas_local_variable_mangled_name_h);
    
}
static inline void __CBAS__glSelectBuffer(unsigned int __cbas_local_variable_mangled_name_sz,unsigned int* __cbas_local_variable_mangled_name_buf)
{
    glSelectBuffer(__cbas_local_variable_mangled_name_sz, __cbas_local_variable_mangled_name_buf);
    
}
static inline void __CBAS__glShadeModel(unsigned int __cbas_local_variable_mangled_name_mode)
{
    glShadeModel(__cbas_local_variable_mangled_name_mode);
    
}
static inline void __CBAS__glStencilFunc(unsigned int __cbas_local_variable_mangled_name_funk,int __cbas_local_variable_mangled_name_ref,unsigned int __cbas_local_variable_mangled_name_mask)
{
    glStencilFunc(__cbas_local_variable_mangled_name_funk, __cbas_local_variable_mangled_name_ref, __cbas_local_variable_mangled_name_mask);
    
}
static inline void __CBAS__glStencilMask(unsigned int __cbas_local_variable_mangled_name_mask)
{
    glStencilMask(__cbas_local_variable_mangled_name_mask);
    
}
static inline void __CBAS__glStencilOp(unsigned int __cbas_local_variable_mangled_name_fail,unsigned int __cbas_local_variable_mangled_name_zfail,unsigned int __cbas_local_variable_mangled_name_zpass)
{
    glStencilOp(__cbas_local_variable_mangled_name_fail, __cbas_local_variable_mangled_name_zfail, __cbas_local_variable_mangled_name_zpass);
    
}
static inline void __CBAS__glTexCoord1d(double __cbas_local_variable_mangled_name_s)
{
    glTexCoord1d(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord1f(float __cbas_local_variable_mangled_name_s)
{
    glTexCoord1f(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord1i(int __cbas_local_variable_mangled_name_s)
{
    glTexCoord1i(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord1s(signed short __cbas_local_variable_mangled_name_s)
{
    glTexCoord1s(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord2d(double __cbas_local_variable_mangled_name_s,double __cbas_local_variable_mangled_name_t)
{
    glTexCoord2d(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t);
    
}
static inline void __CBAS__glTexCoord2f(float __cbas_local_variable_mangled_name_s,float __cbas_local_variable_mangled_name_t)
{
    glTexCoord2f(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t);
    
}
static inline void __CBAS__glTexCoord2i(int __cbas_local_variable_mangled_name_s,int __cbas_local_variable_mangled_name_t)
{
    glTexCoord2i(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t);
    
}
static inline void __CBAS__glTexCoord2s(signed short __cbas_local_variable_mangled_name_s,signed short __cbas_local_variable_mangled_name_t)
{
    glTexCoord2s(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t);
    
}
static inline void __CBAS__glTexCoord3d(double __cbas_local_variable_mangled_name_s,double __cbas_local_variable_mangled_name_t,double __cbas_local_variable_mangled_name_r)
{
    glTexCoord3d(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glTexCoord3f(float __cbas_local_variable_mangled_name_s,float __cbas_local_variable_mangled_name_t,float __cbas_local_variable_mangled_name_r)
{
    glTexCoord3f(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glTexCoord3i(int __cbas_local_variable_mangled_name_s,int __cbas_local_variable_mangled_name_t,int __cbas_local_variable_mangled_name_r)
{
    glTexCoord3i(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glTexCoord3s(signed short __cbas_local_variable_mangled_name_s,signed short __cbas_local_variable_mangled_name_t,signed short __cbas_local_variable_mangled_name_r)
{
    glTexCoord3s(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_r);
    
}
static inline void __CBAS__glTexCoord4d(double __cbas_local_variable_mangled_name_s,double __cbas_local_variable_mangled_name_t,double __cbas_local_variable_mangled_name_r,double __cbas_local_variable_mangled_name_q)
{
    glTexCoord4d(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_q);
    
}
static inline void __CBAS__glTexCoord4f(float __cbas_local_variable_mangled_name_s,float __cbas_local_variable_mangled_name_t,float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_q)
{
    glTexCoord4f(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_q);
    
}
static inline void __CBAS__glTexCoord4i(int __cbas_local_variable_mangled_name_s,int __cbas_local_variable_mangled_name_t,int __cbas_local_variable_mangled_name_r,int __cbas_local_variable_mangled_name_q)
{
    glTexCoord4i(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_q);
    
}
static inline void __CBAS__glTexCoord4s(signed short __cbas_local_variable_mangled_name_s,signed short __cbas_local_variable_mangled_name_t,signed short __cbas_local_variable_mangled_name_r,signed short __cbas_local_variable_mangled_name_q)
{
    glTexCoord4s(__cbas_local_variable_mangled_name_s, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_r, __cbas_local_variable_mangled_name_q);
    
}
static inline void __CBAS__glTexCoord1dv(double* __cbas_local_variable_mangled_name_s)
{
    glTexCoord1dv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord1fv(float* __cbas_local_variable_mangled_name_s)
{
    glTexCoord1fv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord1iv(int* __cbas_local_variable_mangled_name_s)
{
    glTexCoord1iv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord1sv(signed short* __cbas_local_variable_mangled_name_s)
{
    glTexCoord1sv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord2dv(double* __cbas_local_variable_mangled_name_s)
{
    glTexCoord2dv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord2fv(float* __cbas_local_variable_mangled_name_s)
{
    glTexCoord2fv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord2iv(int* __cbas_local_variable_mangled_name_s)
{
    glTexCoord2iv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord2sv(signed short* __cbas_local_variable_mangled_name_s)
{
    glTexCoord2sv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord3dv(double* __cbas_local_variable_mangled_name_s)
{
    glTexCoord3dv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord3fv(float* __cbas_local_variable_mangled_name_s)
{
    glTexCoord3fv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord3iv(int* __cbas_local_variable_mangled_name_s)
{
    glTexCoord3iv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord3sv(signed short* __cbas_local_variable_mangled_name_s)
{
    glTexCoord3sv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord4dv(double* __cbas_local_variable_mangled_name_s)
{
    glTexCoord4dv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord4fv(float* __cbas_local_variable_mangled_name_s)
{
    glTexCoord4fv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord4iv(int* __cbas_local_variable_mangled_name_s)
{
    glTexCoord4iv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoord4sv(signed short* __cbas_local_variable_mangled_name_s)
{
    glTexCoord4sv(__cbas_local_variable_mangled_name_s);
    
}
static inline void __CBAS__glTexCoordPointer(int __cbas_local_variable_mangled_name_sz,unsigned int __cbas_local_variable_mangled_name_t,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glTexCoordPointer(__cbas_local_variable_mangled_name_sz, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glTexEnvf(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param)
{
    glTexEnvf(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexEnvi(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param)
{
    glTexEnvi(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexEnvfv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param)
{
    glTexEnvfv(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexEnviv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param)
{
    glTexEnviv(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexGend(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,double __cbas_local_variable_mangled_name_param)
{
    glTexGend(__cbas_local_variable_mangled_name_coord, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexGenf(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param)
{
    glTexGenf(__cbas_local_variable_mangled_name_coord, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexGeni(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param)
{
    glTexGeni(__cbas_local_variable_mangled_name_coord, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexGendv(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,double* __cbas_local_variable_mangled_name_param)
{
    glTexGendv(__cbas_local_variable_mangled_name_coord, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexGenfv(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param)
{
    glTexGenfv(__cbas_local_variable_mangled_name_coord, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexGeniv(unsigned int __cbas_local_variable_mangled_name_coord,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param)
{
    glTexGeniv(__cbas_local_variable_mangled_name_coord, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexImage1D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_internalfmt,unsigned int __cbas_local_variable_mangled_name_width,int __cbas_local_variable_mangled_name_border,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_px)
{
    glTexImage1D(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_internalfmt, __cbas_local_variable_mangled_name_width, __cbas_local_variable_mangled_name_border, __cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_px);
    
}
static inline void __CBAS__glTexImage2D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_internalfmt,unsigned int __cbas_local_variable_mangled_name_width,unsigned int __cbas_local_variable_mangled_name_height,int __cbas_local_variable_mangled_name_border,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_px)
{
    glTexImage2D(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_internalfmt, __cbas_local_variable_mangled_name_width, __cbas_local_variable_mangled_name_height, __cbas_local_variable_mangled_name_border, __cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_px);
    
}
static inline void __CBAS__glTexParameterf(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float __cbas_local_variable_mangled_name_param)
{
    glTexParameterf(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexParameteri(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int __cbas_local_variable_mangled_name_param)
{
    glTexParameteri(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexParameterfv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,float* __cbas_local_variable_mangled_name_param)
{
    glTexParameterfv(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexParameteriv(unsigned int __cbas_local_variable_mangled_name_tg,unsigned int __cbas_local_variable_mangled_name_pname,int* __cbas_local_variable_mangled_name_param)
{
    glTexParameteriv(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_pname, __cbas_local_variable_mangled_name_param);
    
}
static inline void __CBAS__glTexSubImage1D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_xoff,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glTexSubImage1D(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_xoff, __cbas_local_variable_mangled_name_w, __cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glTexSubImage2D(unsigned int __cbas_local_variable_mangled_name_tg,int __cbas_local_variable_mangled_name_lvl,int __cbas_local_variable_mangled_name_xoff,int __cbas_local_variable_mangled_name_yoff,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_f,unsigned int __cbas_local_variable_mangled_name_t,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glTexSubImage2D(__cbas_local_variable_mangled_name_tg, __cbas_local_variable_mangled_name_lvl, __cbas_local_variable_mangled_name_xoff, __cbas_local_variable_mangled_name_yoff, __cbas_local_variable_mangled_name_w, __cbas_local_variable_mangled_name_h, __cbas_local_variable_mangled_name_f, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glTranslated(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z)
{
    glTranslated(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glTranslatef(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z)
{
    glTranslatef(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glVertex2d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y)
{
    glVertex2d(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glVertex2f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y)
{
    glVertex2f(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glVertex2i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y)
{
    glVertex2i(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glVertex2s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y)
{
    glVertex2s(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y);
    
}
static inline void __CBAS__glVertex3d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z)
{
    glVertex3d(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glVertex3f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z)
{
    glVertex3f(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glVertex3i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z)
{
    glVertex3i(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glVertex3s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z)
{
    glVertex3s(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z);
    
}
static inline void __CBAS__glVertex4d(double __cbas_local_variable_mangled_name_x,double __cbas_local_variable_mangled_name_y,double __cbas_local_variable_mangled_name_z,double __cbas_local_variable_mangled_name_w)
{
    glVertex4d(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z, __cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glVertex4f(float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z,float __cbas_local_variable_mangled_name_w)
{
    glVertex4f(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z, __cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glVertex4i(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_z,int __cbas_local_variable_mangled_name_w)
{
    glVertex4i(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z, __cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glVertex4s(signed short __cbas_local_variable_mangled_name_x,signed short __cbas_local_variable_mangled_name_y,signed short __cbas_local_variable_mangled_name_z,signed short __cbas_local_variable_mangled_name_w)
{
    glVertex4s(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_z, __cbas_local_variable_mangled_name_w);
    
}
static inline void __CBAS__glVertex2dv(double* __cbas_local_variable_mangled_name_v)
{
    glVertex2dv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex2fv(float* __cbas_local_variable_mangled_name_v)
{
    glVertex2fv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex2iv(int* __cbas_local_variable_mangled_name_v)
{
    glVertex2iv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex2sv(signed short* __cbas_local_variable_mangled_name_v)
{
    glVertex2sv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex3dv(double* __cbas_local_variable_mangled_name_v)
{
    glVertex3dv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex3fv(float* __cbas_local_variable_mangled_name_v)
{
    glVertex3fv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex3iv(int* __cbas_local_variable_mangled_name_v)
{
    glVertex3iv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex3sv(signed short* __cbas_local_variable_mangled_name_v)
{
    glVertex3sv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex4dv(double* __cbas_local_variable_mangled_name_v)
{
    glVertex4dv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex4fv(float* __cbas_local_variable_mangled_name_v)
{
    glVertex4fv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex4iv(int* __cbas_local_variable_mangled_name_v)
{
    glVertex4iv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertex4sv(signed short* __cbas_local_variable_mangled_name_v)
{
    glVertex4sv(__cbas_local_variable_mangled_name_v);
    
}
static inline void __CBAS__glVertexPointer(int __cbas_local_variable_mangled_name_sz,unsigned int __cbas_local_variable_mangled_name_t,unsigned int __cbas_local_variable_mangled_name_stride,unsigned char* __cbas_local_variable_mangled_name_p)
{
    glVertexPointer(__cbas_local_variable_mangled_name_sz, __cbas_local_variable_mangled_name_t, __cbas_local_variable_mangled_name_stride, __cbas_local_variable_mangled_name_p);
    
}
static inline void __CBAS__glViewport(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h)
{
    glViewport(__cbas_local_variable_mangled_name_x, __cbas_local_variable_mangled_name_y, __cbas_local_variable_mangled_name_w, __cbas_local_variable_mangled_name_h);
    
}
static inline unsigned int __CBAS__GL_2D()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_2D;
    
}
static inline unsigned int __CBAS__GL_3D()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_3D;
    
}
static inline unsigned int __CBAS__GL_3D_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_3D_COLOR;
    
}
static inline unsigned int __CBAS__GL_3D_COLOR_TEXTURE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_3D_COLOR_TEXTURE;
    
}
static inline unsigned int __CBAS__GL_4D_COLOR_TEXTURE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_4D_COLOR_TEXTURE;
    
}
static inline unsigned int __CBAS__GL_2_BYTES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_2_BYTES;
    
}
static inline unsigned int __CBAS__GL_3_BYTES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_3_BYTES;
    
}
static inline unsigned int __CBAS__GL_4_BYTES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_4_BYTES;
    
}
static inline unsigned int __CBAS__GL_ACCUM()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ACCUM;
    
}
static inline unsigned int __CBAS__GL_ACCUM_ALPHA_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ACCUM_ALPHA_BITS;
    
}
static inline unsigned int __CBAS__GL_ACCUM_BLUE_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ACCUM_BLUE_BITS;
    
}
static inline unsigned int __CBAS__GL_ACCUM_BUFFER_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ACCUM_BUFFER_BIT;
    
}
static inline unsigned int __CBAS__GL_ACCUM_CLEAR_VALUE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ACCUM_CLEAR_VALUE;
    
}
static inline unsigned int __CBAS__GL_ACCUM_GREEN_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ACCUM_GREEN_BITS;
    
}
static inline unsigned int __CBAS__GL_ACCUM_RED_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ACCUM_RED_BITS;
    
}
static inline unsigned int __CBAS__GL_ADD()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ADD;
    
}
static inline unsigned int __CBAS__GL_ALL_ATTRIB_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALL_ATTRIB_BITS;
    
}
static inline unsigned int __CBAS__GL_ALPHA()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALPHA;
    
}
static inline unsigned int __CBAS__GL_ALPHA_BIAS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALPHA_BIAS;
    
}
static inline unsigned int __CBAS__GL_ALPHA_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALPHA_BITS;
    
}
static inline unsigned int __CBAS__GL_ALPHA_SCALE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALPHA_SCALE;
    
}
static inline unsigned int __CBAS__GL_ALPHA_TEST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALPHA_TEST;
    
}
static inline unsigned int __CBAS__GL_ALPHA_TEST_FUNC()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALPHA_TEST_FUNC;
    
}
static inline unsigned int __CBAS__GL_ALPHA_TEST_REF()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALPHA_TEST_REF;
    
}
static inline unsigned int __CBAS__GL_ALWAYS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ALWAYS;
    
}
static inline unsigned int __CBAS__GL_AMBIENT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AMBIENT;
    
}
static inline unsigned int __CBAS__GL_AMBIENT_AND_DIFFUSE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AMBIENT_AND_DIFFUSE;
    
}
static inline unsigned int __CBAS__GL_AND()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AND;
    
}
static inline unsigned int __CBAS__GL_AND_INVERTED()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AND_INVERTED;
    
}
static inline unsigned int __CBAS__GL_AND_REVERSE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AND_REVERSE;
    
}
static inline unsigned int __CBAS__GL_ATTRIB_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ATTRIB_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_AUTO_NORMAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AUTO_NORMAL;
    
}
static inline unsigned int __CBAS__GL_AUX0()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AUX0;
    
}
static inline unsigned int __CBAS__GL_AUX1()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AUX1;
    
}
static inline unsigned int __CBAS__GL_AUX2()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AUX2;
    
}
static inline unsigned int __CBAS__GL_AUX3()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AUX3;
    
}
static inline unsigned int __CBAS__GL_AUX_BUFFERS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_AUX_BUFFERS;
    
}
static inline unsigned int __CBAS__GL_BACK()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BACK;
    
}
static inline unsigned int __CBAS__GL_BACK_LEFT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BACK_LEFT;
    
}
static inline unsigned int __CBAS__GL_BACK_RIGHT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BACK_RIGHT;
    
}
static inline unsigned int __CBAS__GL_BITMAP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BITMAP;
    
}
static inline unsigned int __CBAS__GL_BITMAP_TOKEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BITMAP_TOKEN;
    
}
static inline unsigned int __CBAS__GL_BLEND()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BLEND;
    
}
static inline unsigned int __CBAS__GL_BLEND_DST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BLEND_DST;
    
}
static inline unsigned int __CBAS__GL_BLEND_SRC()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BLEND_SRC;
    
}
static inline unsigned int __CBAS__GL_BLUE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BLUE;
    
}
static inline unsigned int __CBAS__GL_BLUE_BIAS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BLUE_BIAS;
    
}
static inline unsigned int __CBAS__GL_BLUE_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BLUE_BITS;
    
}
static inline unsigned int __CBAS__GL_BLUE_SCALE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BLUE_SCALE;
    
}
static inline unsigned int __CBAS__GL_BYTE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_BYTE;
    
}
static inline unsigned int __CBAS__GL_CCW()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CCW;
    
}
static inline unsigned int __CBAS__GL_CLAMP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CLAMP;
    
}
static inline unsigned int __CBAS__GL_CLEAR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CLEAR;
    
}
static inline unsigned int __CBAS__GL_CLIP_PLANE0()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CLIP_PLANE0;
    
}
static inline unsigned int __CBAS__GL_CLIP_PLANE1()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CLIP_PLANE1;
    
}
static inline unsigned int __CBAS__GL_CLIP_PLANE2()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CLIP_PLANE2;
    
}
static inline unsigned int __CBAS__GL_CLIP_PLANE3()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CLIP_PLANE3;
    
}
static inline unsigned int __CBAS__GL_CLIP_PLANE4()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CLIP_PLANE4;
    
}
static inline unsigned int __CBAS__GL_CLIP_PLANE5()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CLIP_PLANE5;
    
}
static inline unsigned int __CBAS__GL_COEFF()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COEFF;
    
}
static inline unsigned int __CBAS__GL_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR;
    
}
static inline unsigned int __CBAS__GL_COLOR_BUFFER_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR_BUFFER_BIT;
    
}
static inline unsigned int __CBAS__GL_COLOR_CLEAR_VALUE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR_CLEAR_VALUE;
    
}
static inline unsigned int __CBAS__GL_COLOR_INDEX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR_INDEX;
    
}
static inline unsigned int __CBAS__GL_COLOR_INDEXES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR_INDEXES;
    
}
static inline unsigned int __CBAS__GL_COLOR_MATERIAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR_MATERIAL;
    
}
static inline unsigned int __CBAS__GL_COLOR_MATERIAL_FACE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR_MATERIAL_FACE;
    
}
static inline unsigned int __CBAS__GL_COLOR_MATERIAL_PARAMETER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR_MATERIAL_PARAMETER;
    
}
static inline unsigned int __CBAS__GL_COLOR_WRITEMASK()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COLOR_WRITEMASK;
    
}
static inline unsigned int __CBAS__GL_COMPILE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COMPILE;
    
}
static inline unsigned int __CBAS__GL_COMPILE_AND_EXECUTE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COMPILE_AND_EXECUTE;
    
}
static inline unsigned int __CBAS__GL_CONSTANT_ATTENUATION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CONSTANT_ATTENUATION;
    
}
static inline unsigned int __CBAS__GL_COPY()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COPY;
    
}
static inline unsigned int __CBAS__GL_COPY_INVERTED()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COPY_INVERTED;
    
}
static inline unsigned int __CBAS__GL_COPY_PIXEL_TOKEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_COPY_PIXEL_TOKEN;
    
}
static inline unsigned int __CBAS__GL_CULL_FACE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CULL_FACE;
    
}
static inline unsigned int __CBAS__GL_CULL_FACE_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CULL_FACE_MODE;
    
}
static inline unsigned int __CBAS__GL_CURRENT_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_BIT;
    
}
static inline unsigned int __CBAS__GL_CURRENT_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_COLOR;
    
}
static inline unsigned int __CBAS__GL_CURRENT_INDEX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_INDEX;
    
}
static inline unsigned int __CBAS__GL_CURRENT_NORMAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_NORMAL;
    
}
static inline unsigned int __CBAS__GL_CURRENT_RASTER_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_RASTER_COLOR;
    
}
static inline unsigned int __CBAS__GL_CURRENT_RASTER_INDEX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_RASTER_INDEX;
    
}
static inline unsigned int __CBAS__GL_CURRENT_RASTER_POSITION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_RASTER_POSITION;
    
}
static inline unsigned int __CBAS__GL_CURRENT_RASTER_POSITION_VALID()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_RASTER_POSITION_VALID;
    
}
static inline unsigned int __CBAS__GL_CURRENT_RASTER_TEXTURE_COORDS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_RASTER_TEXTURE_COORDS;
    
}
static inline unsigned int __CBAS__GL_CURRENT_TEXTURE_COORDS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CURRENT_TEXTURE_COORDS;
    
}
static inline unsigned int __CBAS__GL_CW()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_CW;
    
}
static inline unsigned int __CBAS__GL_DECAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DECAL;
    
}
static inline unsigned int __CBAS__GL_DECR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DECR;
    
}
static inline unsigned int __CBAS__GL_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH;
    
}
static inline unsigned int __CBAS__GL_DEPTH_BIAS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_BIAS;
    
}
static inline unsigned int __CBAS__GL_DEPTH_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_BITS;
    
}
static inline unsigned int __CBAS__GL_DEPTH_BUFFER_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_BUFFER_BIT;
    
}
static inline unsigned int __CBAS__GL_DEPTH_CLEAR_VALUE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_CLEAR_VALUE;
    
}
static inline unsigned int __CBAS__GL_DEPTH_COMPONENT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_COMPONENT;
    
}
static inline unsigned int __CBAS__GL_DEPTH_FUNC()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_FUNC;
    
}
static inline unsigned int __CBAS__GL_DEPTH_RANGE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_RANGE;
    
}
static inline unsigned int __CBAS__GL_DEPTH_SCALE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_SCALE;
    
}
static inline unsigned int __CBAS__GL_DEPTH_TEST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_TEST;
    
}
static inline unsigned int __CBAS__GL_DEPTH_WRITEMASK()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DEPTH_WRITEMASK;
    
}
static inline unsigned int __CBAS__GL_DIFFUSE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DIFFUSE;
    
}
static inline unsigned int __CBAS__GL_DITHER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DITHER;
    
}
static inline unsigned int __CBAS__GL_DOMAIN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DOMAIN;
    
}
static inline unsigned int __CBAS__GL_DONT_CARE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DONT_CARE;
    
}
static inline unsigned int __CBAS__GL_DOUBLEBUFFER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DOUBLEBUFFER;
    
}
static inline unsigned int __CBAS__GL_DRAW_BUFFER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DRAW_BUFFER;
    
}
static inline unsigned int __CBAS__GL_DRAW_PIXEL_TOKEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DRAW_PIXEL_TOKEN;
    
}
static inline unsigned int __CBAS__GL_DST_ALPHA()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DST_ALPHA;
    
}
static inline unsigned int __CBAS__GL_DST_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_DST_COLOR;
    
}
static inline unsigned int __CBAS__GL_EDGE_FLAG()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EDGE_FLAG;
    
}
static inline unsigned int __CBAS__GL_EMISSION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EMISSION;
    
}
static inline unsigned int __CBAS__GL_ENABLE_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ENABLE_BIT;
    
}
static inline unsigned int __CBAS__GL_EQUAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EQUAL;
    
}
static inline unsigned int __CBAS__GL_EQUIV()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EQUIV;
    
}
static inline unsigned int __CBAS__GL_EVAL_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EVAL_BIT;
    
}
static inline unsigned int __CBAS__GL_EXP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EXP;
    
}
static inline unsigned int __CBAS__GL_EXP2()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EXP2;
    
}
static inline unsigned int __CBAS__GL_EXTENSIONS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EXTENSIONS;
    
}
static inline unsigned int __CBAS__GL_EYE_LINEAR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EYE_LINEAR;
    
}
static inline unsigned int __CBAS__GL_EYE_PLANE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_EYE_PLANE;
    
}
static inline unsigned int __CBAS__GL_FALSE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FALSE;
    
}
static inline unsigned int __CBAS__GL_FASTEST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FASTEST;
    
}
static inline unsigned int __CBAS__GL_FEEDBACK()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FEEDBACK;
    
}
static inline unsigned int __CBAS__GL_FILL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FILL;
    
}
static inline unsigned int __CBAS__GL_FLAT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FLAT;
    
}
static inline unsigned int __CBAS__GL_FLOAT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FLOAT;
    
}
static inline unsigned int __CBAS__GL_FOG()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG;
    
}
static inline unsigned int __CBAS__GL_FOG_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG_BIT;
    
}
static inline unsigned int __CBAS__GL_FOG_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG_COLOR;
    
}
static inline unsigned int __CBAS__GL_FOG_DENSITY()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG_DENSITY;
    
}
static inline unsigned int __CBAS__GL_FOG_END()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG_END;
    
}
static inline unsigned int __CBAS__GL_FOG_HINT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG_HINT;
    
}
static inline unsigned int __CBAS__GL_FOG_INDEX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG_INDEX;
    
}
static inline unsigned int __CBAS__GL_FOG_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG_MODE;
    
}
static inline unsigned int __CBAS__GL_FOG_START()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FOG_START;
    
}
static inline unsigned int __CBAS__GL_FRONT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FRONT;
    
}
static inline unsigned int __CBAS__GL_FRONT_AND_BACK()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FRONT_AND_BACK;
    
}
static inline unsigned int __CBAS__GL_FRONT_FACE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FRONT_FACE;
    
}
static inline unsigned int __CBAS__GL_FRONT_LEFT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FRONT_LEFT;
    
}
static inline unsigned int __CBAS__GL_FRONT_RIGHT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_FRONT_RIGHT;
    
}
static inline unsigned int __CBAS__GL_GEQUAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_GEQUAL;
    
}
static inline unsigned int __CBAS__GL_GREATER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_GREATER;
    
}
static inline unsigned int __CBAS__GL_GREEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_GREEN;
    
}
static inline unsigned int __CBAS__GL_GREEN_BIAS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_GREEN_BIAS;
    
}
static inline unsigned int __CBAS__GL_GREEN_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_GREEN_BITS;
    
}
static inline unsigned int __CBAS__GL_GREEN_SCALE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_GREEN_SCALE;
    
}
static inline unsigned int __CBAS__GL_HINT_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_HINT_BIT;
    
}
static inline unsigned int __CBAS__GL_INCR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INCR;
    
}
static inline unsigned int __CBAS__GL_INDEX_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INDEX_BITS;
    
}
static inline unsigned int __CBAS__GL_INDEX_CLEAR_VALUE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INDEX_CLEAR_VALUE;
    
}
static inline unsigned int __CBAS__GL_INDEX_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INDEX_MODE;
    
}
static inline unsigned int __CBAS__GL_INDEX_OFFSET()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INDEX_OFFSET;
    
}
static inline unsigned int __CBAS__GL_INDEX_SHIFT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INDEX_SHIFT;
    
}
static inline unsigned int __CBAS__GL_INDEX_WRITEMASK()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INDEX_WRITEMASK;
    
}
static inline unsigned int __CBAS__GL_INT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INT;
    
}
static inline unsigned int __CBAS__GL_INVALID_ENUM()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INVALID_ENUM;
    
}
static inline unsigned int __CBAS__GL_INVALID_OPERATION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INVALID_OPERATION;
    
}
static inline unsigned int __CBAS__GL_INVALID_VALUE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INVALID_VALUE;
    
}
static inline unsigned int __CBAS__GL_INVERT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_INVERT;
    
}
static inline unsigned int __CBAS__GL_KEEP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_KEEP;
    
}
static inline unsigned int __CBAS__GL_LEFT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LEFT;
    
}
static inline unsigned int __CBAS__GL_LEQUAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LEQUAL;
    
}
static inline unsigned int __CBAS__GL_LESS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LESS;
    
}
static inline unsigned int __CBAS__GL_LIGHT0()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT0;
    
}
static inline unsigned int __CBAS__GL_LIGHT1()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT1;
    
}
static inline unsigned int __CBAS__GL_LIGHT2()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT2;
    
}
static inline unsigned int __CBAS__GL_LIGHT3()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT3;
    
}
static inline unsigned int __CBAS__GL_LIGHT4()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT4;
    
}
static inline unsigned int __CBAS__GL_LIGHT5()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT5;
    
}
static inline unsigned int __CBAS__GL_LIGHT6()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT6;
    
}
static inline unsigned int __CBAS__GL_LIGHT7()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT7;
    
}
static inline unsigned int __CBAS__GL_LIGHTING()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHTING;
    
}
static inline unsigned int __CBAS__GL_LIGHTING_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHTING_BIT;
    
}
static inline unsigned int __CBAS__GL_LIGHT_MODEL_AMBIENT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT_MODEL_AMBIENT;
    
}
static inline unsigned int __CBAS__GL_LIGHT_MODEL_LOCAL_VIEWER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT_MODEL_LOCAL_VIEWER;
    
}
static inline unsigned int __CBAS__GL_LIGHT_MODEL_TWO_SIDE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIGHT_MODEL_TWO_SIDE;
    
}
static inline unsigned int __CBAS__GL_LINE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE;
    
}
static inline unsigned int __CBAS__GL_LINEAR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINEAR;
    
}
static inline unsigned int __CBAS__GL_LINEAR_ATTENUATION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINEAR_ATTENUATION;
    
}
static inline unsigned int __CBAS__GL_LINEAR_MIPMAP_LINEAR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINEAR_MIPMAP_LINEAR;
    
}
static inline unsigned int __CBAS__GL_LINEAR_MIPMAP_NEAREST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINEAR_MIPMAP_NEAREST;
    
}
static inline unsigned int __CBAS__GL_LINES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINES;
    
}
static inline unsigned int __CBAS__GL_LINE_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_BIT;
    
}
static inline unsigned int __CBAS__GL_LINE_LOOP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_LOOP;
    
}
static inline unsigned int __CBAS__GL_LINE_RESET_TOKEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_RESET_TOKEN;
    
}
static inline unsigned int __CBAS__GL_LINE_SMOOTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_SMOOTH;
    
}
static inline unsigned int __CBAS__GL_LINE_SMOOTH_HINT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_SMOOTH_HINT;
    
}
static inline unsigned int __CBAS__GL_LINE_STIPPLE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_STIPPLE;
    
}
static inline unsigned int __CBAS__GL_LINE_STIPPLE_PATTERN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_STIPPLE_PATTERN;
    
}
static inline unsigned int __CBAS__GL_LINE_STIPPLE_REPEAT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_STIPPLE_REPEAT;
    
}
static inline unsigned int __CBAS__GL_LINE_STRIP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_STRIP;
    
}
static inline unsigned int __CBAS__GL_LINE_TOKEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_TOKEN;
    
}
static inline unsigned int __CBAS__GL_LINE_WIDTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_WIDTH;
    
}
static inline unsigned int __CBAS__GL_LINE_WIDTH_GRANULARITY()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_WIDTH_GRANULARITY;
    
}
static inline unsigned int __CBAS__GL_LINE_WIDTH_RANGE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LINE_WIDTH_RANGE;
    
}
static inline unsigned int __CBAS__GL_LIST_BASE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIST_BASE;
    
}
static inline unsigned int __CBAS__GL_LIST_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIST_BIT;
    
}
static inline unsigned int __CBAS__GL_LIST_INDEX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIST_INDEX;
    
}
static inline unsigned int __CBAS__GL_LIST_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LIST_MODE;
    
}
static inline unsigned int __CBAS__GL_LOAD()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LOAD;
    
}
static inline unsigned int __CBAS__GL_LOGIC_OP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LOGIC_OP;
    
}
static inline unsigned int __CBAS__GL_LOGIC_OP_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LOGIC_OP_MODE;
    
}
static inline unsigned int __CBAS__GL_LUMINANCE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LUMINANCE;
    
}
static inline unsigned int __CBAS__GL_LUMINANCE_ALPHA()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_LUMINANCE_ALPHA;
    
}
static inline unsigned int __CBAS__GL_MAP1_COLOR_4()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_COLOR_4;
    
}
static inline unsigned int __CBAS__GL_MAP1_GRID_DOMAIN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_GRID_DOMAIN;
    
}
static inline unsigned int __CBAS__GL_MAP1_GRID_SEGMENTS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_GRID_SEGMENTS;
    
}
static inline unsigned int __CBAS__GL_MAP1_INDEX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_INDEX;
    
}
static inline unsigned int __CBAS__GL_MAP1_NORMAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_NORMAL;
    
}
static inline unsigned int __CBAS__GL_MAP1_TEXTURE_COORD_1()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_TEXTURE_COORD_1;
    
}
static inline unsigned int __CBAS__GL_MAP1_TEXTURE_COORD_2()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_TEXTURE_COORD_2;
    
}
static inline unsigned int __CBAS__GL_MAP1_TEXTURE_COORD_3()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_TEXTURE_COORD_3;
    
}
static inline unsigned int __CBAS__GL_MAP1_TEXTURE_COORD_4()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_TEXTURE_COORD_4;
    
}
static inline unsigned int __CBAS__GL_MAP1_VERTEX_3()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_VERTEX_3;
    
}
static inline unsigned int __CBAS__GL_MAP1_VERTEX_4()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP1_VERTEX_4;
    
}
static inline unsigned int __CBAS__GL_MAP2_COLOR_4()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_COLOR_4;
    
}
static inline unsigned int __CBAS__GL_MAP2_GRID_DOMAIN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_GRID_DOMAIN;
    
}
static inline unsigned int __CBAS__GL_MAP2_GRID_SEGMENTS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_GRID_SEGMENTS;
    
}
static inline unsigned int __CBAS__GL_MAP2_INDEX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_INDEX;
    
}
static inline unsigned int __CBAS__GL_MAP2_NORMAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_NORMAL;
    
}
static inline unsigned int __CBAS__GL_MAP2_TEXTURE_COORD_1()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_TEXTURE_COORD_1;
    
}
static inline unsigned int __CBAS__GL_MAP2_TEXTURE_COORD_2()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_TEXTURE_COORD_2;
    
}
static inline unsigned int __CBAS__GL_MAP2_TEXTURE_COORD_3()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_TEXTURE_COORD_3;
    
}
static inline unsigned int __CBAS__GL_MAP2_TEXTURE_COORD_4()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP2_TEXTURE_COORD_4;
    
}
static inline unsigned int __CBAS__GL_MAP_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP_COLOR;
    
}
static inline unsigned int __CBAS__GL_MAP_STENCIL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAP_STENCIL;
    
}
static inline unsigned int __CBAS__GL_MATRIX_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MATRIX_MODE;
    
}
static inline unsigned int __CBAS__GL_MAX_ATTRIB_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_ATTRIB_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_MAX_CLIP_PLANES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_CLIP_PLANES;
    
}
static inline unsigned int __CBAS__GL_MAX_EVAL_ORDER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_EVAL_ORDER;
    
}
static inline unsigned int __CBAS__GL_MAX_LIGHTS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_LIGHTS;
    
}
static inline unsigned int __CBAS__GL_MAX_LIST_NESTING()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_LIST_NESTING;
    
}
static inline unsigned int __CBAS__GL_MAX_MODELVIEW_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_MODELVIEW_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_MAX_NAME_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_NAME_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_MAX_PIXEL_MAP_TABLE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_PIXEL_MAP_TABLE;
    
}
static inline unsigned int __CBAS__GL_MAX_PROJECTION_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_PROJECTION_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_MAX_TEXTURE_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_TEXTURE_SIZE;
    
}
static inline unsigned int __CBAS__GL_MAX_TEXTURE_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_TEXTURE_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_MAX_VIEWPORT_DIMS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MAX_VIEWPORT_DIMS;
    
}
static inline unsigned int __CBAS__GL_MODELVIEW()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MODELVIEW;
    
}
static inline unsigned int __CBAS__GL_MODELVIEW_MATRIX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MODELVIEW_MATRIX;
    
}
static inline unsigned int __CBAS__GL_MODELVIEW_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MODELVIEW_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_MODULATE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MODULATE;
    
}
static inline unsigned int __CBAS__GL_MULT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_MULT;
    
}
static inline unsigned int __CBAS__GL_NAME_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NAME_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_NAND()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NAND;
    
}
static inline unsigned int __CBAS__GL_NEAREST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NEAREST;
    
}
static inline unsigned int __CBAS__GL_NEAREST_MIPMAP_LINEAR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NEAREST_MIPMAP_LINEAR;
    
}
static inline unsigned int __CBAS__GL_NEAREST_MIPMAP_NEAREST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NEAREST_MIPMAP_NEAREST;
    
}
static inline unsigned int __CBAS__GL_NEVER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NEVER;
    
}
static inline unsigned int __CBAS__GL_NICEST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NICEST;
    
}
static inline unsigned int __CBAS__GL_NONE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NONE;
    
}
static inline unsigned int __CBAS__GL_NOOP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NOOP;
    
}
static inline unsigned int __CBAS__GL_NOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NOR;
    
}
static inline unsigned int __CBAS__GL_NORMALIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NORMALIZE;
    
}
static inline unsigned int __CBAS__GL_NOTEQUAL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NOTEQUAL;
    
}
static inline unsigned int __CBAS__GL_NO_ERROR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_NO_ERROR;
    
}
static inline unsigned int __CBAS__GL_OBJECT_LINEAR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_OBJECT_LINEAR;
    
}
static inline unsigned int __CBAS__GL_OBJECT_PLANE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_OBJECT_PLANE;
    
}
static inline unsigned int __CBAS__GL_ONE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ONE;
    
}
static inline unsigned int __CBAS__GL_ONE_MINUS_DST_ALPHA()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ONE_MINUS_DST_ALPHA;
    
}
static inline unsigned int __CBAS__GL_ONE_MINUS_DST_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ONE_MINUS_DST_COLOR;
    
}
static inline unsigned int __CBAS__GL_ONE_MINUS_SRC_ALPHA()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ONE_MINUS_SRC_ALPHA;
    
}
static inline unsigned int __CBAS__GL_ONE_MINUS_SRC_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ONE_MINUS_SRC_COLOR;
    
}
static inline unsigned int __CBAS__GL_OR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_OR;
    
}
static inline unsigned int __CBAS__GL_OR_INVERTED()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_OR_INVERTED;
    
}
static inline unsigned int __CBAS__GL_OR_REVERSE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_OR_REVERSE;
    
}
static inline unsigned int __CBAS__GL_ORDER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ORDER;
    
}
static inline unsigned int __CBAS__GL_OUT_OF_MEMORY()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_OUT_OF_MEMORY;
    
}
static inline unsigned int __CBAS__GL_PACK_ALIGNMENT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PACK_ALIGNMENT;
    
}
static inline unsigned int __CBAS__GL_PACK_LSB_FIRST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PACK_LSB_FIRST;
    
}
static inline unsigned int __CBAS__GL_PACK_ROW_LENGTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PACK_ROW_LENGTH;
    
}
static inline unsigned int __CBAS__GL_PACK_SKIP_PIXELS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PACK_SKIP_PIXELS;
    
}
static inline unsigned int __CBAS__GL_PACK_SKIP_ROWS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PACK_SKIP_ROWS;
    
}
static inline unsigned int __CBAS__GL_PACK_SWAP_BYTES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PACK_SWAP_BYTES;
    
}
static inline unsigned int __CBAS__GL_PASS_THROUGH_TOKEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PASS_THROUGH_TOKEN;
    
}
static inline unsigned int __CBAS__GL_PERSPECTIVE_CORRECTION_HINT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PERSPECTIVE_CORRECTION_HINT;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_I()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_I;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_S_TO_S()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_S_TO_S;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_R()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_R;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_G()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_G;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_B()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_B;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_A()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_A;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_R_TO_R()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_R_TO_R;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_G_TO_G()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_G_TO_G;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_B_TO_B()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_B_TO_B;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_A_TO_A()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_A_TO_A;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_I_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_I_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_S_TO_S_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_S_TO_S_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_R_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_R_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_G_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_G_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_B_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_B_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_I_TO_A_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_I_TO_A_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_R_TO_R_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_R_TO_R_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_G_TO_G_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_G_TO_G_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_B_TO_B_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_B_TO_B_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MAP_A_TO_A_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MAP_A_TO_A_SIZE;
    
}
static inline unsigned int __CBAS__GL_PIXEL_MODE_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PIXEL_MODE_BIT;
    
}
static inline unsigned int __CBAS__GL_POINT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINT;
    
}
static inline unsigned int __CBAS__GL_POINTS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINTS;
    
}
static inline unsigned int __CBAS__GL_POINT_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINT_BIT;
    
}
static inline unsigned int __CBAS__GL_POINT_SIZE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINT_SIZE;
    
}
static inline unsigned int __CBAS__GL_POINT_SIZE_GRANULARITY()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINT_SIZE_GRANULARITY;
    
}
static inline unsigned int __CBAS__GL_POINT_SIZE_RANGE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINT_SIZE_RANGE;
    
}
static inline unsigned int __CBAS__GL_POINT_SMOOTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINT_SMOOTH;
    
}
static inline unsigned int __CBAS__GL_POINT_SMOOTH_HINT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINT_SMOOTH_HINT;
    
}
static inline unsigned int __CBAS__GL_POINT_TOKEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POINT_TOKEN;
    
}
static inline unsigned int __CBAS__GL_POLYGON()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POLYGON;
    
}
static inline unsigned int __CBAS__GL_POLYGON_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POLYGON_BIT;
    
}
static inline unsigned int __CBAS__GL_POLYGON_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POLYGON_MODE;
    
}
static inline unsigned int __CBAS__GL_POLYGON_SMOOTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POLYGON_SMOOTH;
    
}
static inline unsigned int __CBAS__GL_POLYGON_SMOOTH_HINT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POLYGON_SMOOTH_HINT;
    
}
static inline unsigned int __CBAS__GL_POLYGON_STIPPLE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POLYGON_STIPPLE;
    
}
static inline unsigned int __CBAS__GL_POLYGON_STIPPLE_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POLYGON_STIPPLE_BIT;
    
}
static inline unsigned int __CBAS__GL_POLYGON_TOKEN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POLYGON_TOKEN;
    
}
static inline unsigned int __CBAS__GL_POSITION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_POSITION;
    
}
static inline unsigned int __CBAS__GL_PROJECTION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PROJECTION;
    
}
static inline unsigned int __CBAS__GL_PROJECTION_MATRIX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PROJECTION_MATRIX;
    
}
static inline unsigned int __CBAS__GL_PROJECTION_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_PROJECTION_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_Q()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_Q;
    
}
static inline unsigned int __CBAS__GL_QUADRATIC_ATTENUATION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_QUADRATIC_ATTENUATION;
    
}
static inline unsigned int __CBAS__GL_QUADS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_QUADS;
    
}
static inline unsigned int __CBAS__GL_QUAD_STRIP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_QUAD_STRIP;
    
}
static inline unsigned int __CBAS__GL_R()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_R;
    
}
static inline unsigned int __CBAS__GL_READ_BUFFER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_READ_BUFFER;
    
}
static inline unsigned int __CBAS__GL_RED()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RED;
    
}
static inline unsigned int __CBAS__GL_RED_BIAS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RED_BIAS;
    
}
static inline unsigned int __CBAS__GL_RED_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RED_BITS;
    
}
static inline unsigned int __CBAS__GL_RED_SCALE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RED_SCALE;
    
}
static inline unsigned int __CBAS__GL_RENDER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RENDER;
    
}
static inline unsigned int __CBAS__GL_RENDERER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RENDERER;
    
}
static inline unsigned int __CBAS__GL_RENDER_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RENDER_MODE;
    
}
static inline unsigned int __CBAS__GL_REPEAT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_REPEAT;
    
}
static inline unsigned int __CBAS__GL_REPLACE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_REPLACE;
    
}
static inline unsigned int __CBAS__GL_RETURN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RETURN;
    
}
static inline unsigned int __CBAS__GL_RGB()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RGB;
    
}
static inline unsigned int __CBAS__GL_RGBA()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RGBA;
    
}
static inline unsigned int __CBAS__GL_RGBA_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RGBA_MODE;
    
}
static inline unsigned int __CBAS__GL_RIGHT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_RIGHT;
    
}
static inline unsigned int __CBAS__GL_S()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_S;
    
}
static inline unsigned int __CBAS__GL_SCISSOR_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SCISSOR_BIT;
    
}
static inline unsigned int __CBAS__GL_SCISSOR_BOX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SCISSOR_BOX;
    
}
static inline unsigned int __CBAS__GL_SCISSOR_TEST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SCISSOR_TEST;
    
}
static inline unsigned int __CBAS__GL_SELECT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SELECT;
    
}
static inline unsigned int __CBAS__GL_SET()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SET;
    
}
static inline unsigned int __CBAS__GL_SHININESS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SHININESS;
    
}
static inline unsigned int __CBAS__GL_SHADE_MODEL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SHADE_MODEL;
    
}
static inline unsigned int __CBAS__GL_SHORT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SHORT;
    
}
static inline unsigned int __CBAS__GL_SMOOTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SMOOTH;
    
}
static inline unsigned int __CBAS__GL_SPECULAR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SPECULAR;
    
}
static inline unsigned int __CBAS__GL_SPHERE_MAP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SPHERE_MAP;
    
}
static inline unsigned int __CBAS__GL_SPOT_CUTOFF()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SPOT_CUTOFF;
    
}
static inline unsigned int __CBAS__GL_SPOT_DIRECTION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SPOT_DIRECTION;
    
}
static inline unsigned int __CBAS__GL_SPOT_EXPONENT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SPOT_EXPONENT;
    
}
static inline unsigned int __CBAS__GL_SRC_ALPHA()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SRC_ALPHA;
    
}
static inline unsigned int __CBAS__GL_SRC_ALPHA_SATURATE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SRC_ALPHA_SATURATE;
    
}
static inline unsigned int __CBAS__GL_SRC_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SRC_COLOR;
    
}
static inline unsigned int __CBAS__GL_STACK_OVERFLOW()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STACK_OVERFLOW;
    
}
static inline unsigned int __CBAS__GL_STACK_UNDERFLOW()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STACK_UNDERFLOW;
    
}
static inline unsigned int __CBAS__GL_STENCIL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL;
    
}
static inline unsigned int __CBAS__GL_STENCIL_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_BITS;
    
}
static inline unsigned int __CBAS__GL_STENCIL_BUFFER_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_BUFFER_BIT;
    
}
static inline unsigned int __CBAS__GL_STENCIL_INDEX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_INDEX;
    
}
static inline unsigned int __CBAS__GL_STENCIL_CLEAR_VALUE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_CLEAR_VALUE;
    
}
static inline unsigned int __CBAS__GL_STENCIL_FAIL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_FAIL;
    
}
static inline unsigned int __CBAS__GL_STENCIL_FUNC()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_FUNC;
    
}
static inline unsigned int __CBAS__GL_STENCIL_PASS_DEPTH_FAIL()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_PASS_DEPTH_FAIL;
    
}
static inline unsigned int __CBAS__GL_STENCIL_PASS_DEPTH_PASS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_PASS_DEPTH_PASS;
    
}
static inline unsigned int __CBAS__GL_STENCIL_REF()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_REF;
    
}
static inline unsigned int __CBAS__GL_STENCIL_TEST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_TEST;
    
}
static inline unsigned int __CBAS__GL_STENCIL_VALUE_MASK()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_VALUE_MASK;
    
}
static inline unsigned int __CBAS__GL_STENCIL_WRITEMASK()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STENCIL_WRITEMASK;
    
}
static inline unsigned int __CBAS__GL_STEREO()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_STEREO;
    
}
static inline unsigned int __CBAS__GL_SUBPIXEL_BITS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_SUBPIXEL_BITS;
    
}
static inline unsigned int __CBAS__GL_T()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_T;
    
}
static inline unsigned int __CBAS__GL_TEXTURE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_1D()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_1D;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_2D()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_2D;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_BIT;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_BORDER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_BORDER;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_BORDER_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_BORDER_COLOR;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_COMPONENTS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_COMPONENTS;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_ENV()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_ENV;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_ENV_COLOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_ENV_COLOR;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_ENV_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_ENV_MODE;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_GEN_MODE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_GEN_MODE;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_GEN_Q()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_GEN_Q;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_GEN_R()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_GEN_R;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_GEN_S()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_GEN_S;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_GEN_T()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_GEN_T;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_HEIGHT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_HEIGHT;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_MAG_FILTER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_MAG_FILTER;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_MATRIX()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_MATRIX;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_MIN_FILTER()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_MIN_FILTER;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_STACK_DEPTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_STACK_DEPTH;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_WIDTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_WIDTH;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_WRAP_S()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_WRAP_S;
    
}
static inline unsigned int __CBAS__GL_TEXTURE_WRAP_T()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TEXTURE_WRAP_T;
    
}
static inline unsigned int __CBAS__GL_TRANSFORM_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TRANSFORM_BIT;
    
}
static inline unsigned int __CBAS__GL_TRIANGLES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TRIANGLES;
    
}
static inline unsigned int __CBAS__GL_TRIANGLE_FAN()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TRIANGLE_FAN;
    
}
static inline unsigned int __CBAS__GL_TRIANGLE_STRIP()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TRIANGLE_STRIP;
    
}
static inline unsigned int __CBAS__GL_TRUE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_TRUE;
    
}
static inline unsigned int __CBAS__GL_UNPACK_ALIGNMENT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNPACK_ALIGNMENT;
    
}
static inline unsigned int __CBAS__GL_UNPACK_LSB_FIRST()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNPACK_LSB_FIRST;
    
}
static inline unsigned int __CBAS__GL_UNPACK_ROW_LENGTH()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNPACK_ROW_LENGTH;
    
}
static inline unsigned int __CBAS__GL_UNPACK_SKIP_PIXELS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNPACK_SKIP_PIXELS;
    
}
static inline unsigned int __CBAS__GL_UNPACK_SKIP_ROWS()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNPACK_SKIP_ROWS;
    
}
static inline unsigned int __CBAS__GL_UNPACK_SWAP_BYTES()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNPACK_SWAP_BYTES;
    
}
static inline unsigned int __CBAS__GL_UNSIGNED_BYTE()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNSIGNED_BYTE;
    
}
static inline unsigned int __CBAS__GL_UNSIGNED_INT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNSIGNED_INT;
    
}
static inline unsigned int __CBAS__GL_UNSIGNED_SHORT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_UNSIGNED_SHORT;
    
}
static inline unsigned int __CBAS__GL_VENDOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_VENDOR;
    
}
static inline unsigned int __CBAS__GL_VERSION()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_VERSION;
    
}
static inline unsigned int __CBAS__GL_VIEWPORT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_VIEWPORT;
    
}
static inline unsigned int __CBAS__GL_VIEWPORT_BIT()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_VIEWPORT_BIT;
    
}
static inline unsigned int __CBAS__GL_XOR()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_XOR;
    
}
static inline unsigned int __CBAS__GL_ZERO()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ZERO;
    
}
static inline unsigned int __CBAS__GL_ZOOM_X()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ZOOM_X;
    
}
static inline unsigned int __CBAS__GL_ZOOM_Y()
{
    unsigned int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return GL_ZOOM_Y;
    
}
static inline void __CBAS__openWindow(unsigned char* __cbas_local_variable_mangled_name_text,unsigned int __cbas_local_variable_mangled_name_w,unsigned int __cbas_local_variable_mangled_name_h)
{
    
        __CBAS__width = __cbas_local_variable_mangled_name_w;
        __CBAS__height = __cbas_local_variable_mangled_name_h;
        sdl_win = SDL_CreateWindow(
            (char*)__cbas_local_variable_mangled_name_text,
    		SDL_WINDOWPOS_UNDEFINED,
    		SDL_WINDOWPOS_UNDEFINED,
    		__CBAS__width, 
    		__CBAS__height,
    		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    	);
    	if(!sdl_win)
    	{
    		printf("SDL2 window creation failed.\n"
    			"SDL_Error: %s\n", SDL_GetError());
    		exit(1);
    	}
    	sdl_glcontext = SDL_GL_CreateContext(sdl_win);
        if(!sdl_glcontext){
    		printf("SDL2 GL Context creation failed.\n"
    		"SDL_Error: %s\n", SDL_GetError());
    		exit(1);
    	}
    	SDL_GL_MakeCurrent(sdl_win, sdl_glcontext);
	
    (void)__CBAS__glViewport(((int)((unsigned long long)0ull)),((int)((unsigned long long)0ull)),((unsigned int)(__CBAS__width)),((unsigned int)(__CBAS__height)));
    (void)__CBAS__glEnable(((unsigned int)__CBAS__GL_CULL_FACE()));
    (void)__CBAS__glCullFace(((unsigned int)__CBAS__GL_BACK()));
    (void)__CBAS__glDisable(((unsigned int)__CBAS__GL_TEXTURE_2D()));
    (void)__CBAS__glDisable(((unsigned int)__CBAS__GL_LIGHTING()));
    (void)__CBAS__glEnable(((unsigned int)__CBAS__GL_DEPTH_TEST()));
    (void)__CBAS__glShadeModel(((unsigned int)__CBAS__GL_SMOOTH()));
    (void)__CBAS__glEnable(((unsigned int)__CBAS__GL_NORMALIZE()));
    (void)__CBAS__glClearColor(((float)((double)0)),((float)((double)0)),((float)((double)0)),((float)((double)0)));
    (void)__CBAS__glMatrixMode(((unsigned int)__CBAS__GL_PROJECTION()));
    (void)__CBAS__glLoadIdentity();
    (void)__CBAS__glMatrixMode(((unsigned int)__CBAS__GL_MODELVIEW()));
    (void)__CBAS__glLoadIdentity();
    
}
static inline void __CBAS__closeWindow()
{
        
        if(sdl_glcontext)
            SDL_GL_DeleteContext(sdl_glcontext);
        if(sdl_win)
   	        SDL_DestroyWindow(sdl_win);
   	    sdl_win = NULL;
   	    sdl_glcontext = NULL;
    
    
}
static inline void __CBAS__clearScreen(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_g,float __cbas_local_variable_mangled_name_b)
{
    (void)__CBAS__glClearColor(((float)(__cbas_local_variable_mangled_name_r)),((float)(__cbas_local_variable_mangled_name_g)),((float)(__cbas_local_variable_mangled_name_b)),((float)((double)0)));
    (void)__CBAS__glClear(((unsigned int)((unsigned long long)(((unsigned long long)((unsigned int)__CBAS__GL_COLOR_BUFFER_BIT())) | ((unsigned long long)((unsigned int)__CBAS__GL_DEPTH_BUFFER_BIT()))))));
    
}
static inline void __CBAS__displayModel(unsigned int __cbas_local_variable_mangled_name_dl,float __cbas_local_variable_mangled_name_x,float __cbas_local_variable_mangled_name_y,float __cbas_local_variable_mangled_name_z,float __cbas_local_variable_mangled_name_rx,float __cbas_local_variable_mangled_name_ry,float __cbas_local_variable_mangled_name_rz)
{
    (void)__CBAS__glMatrixMode(((unsigned int)__CBAS__GL_MODELVIEW()));
    (void)__CBAS__glPushMatrix();
    (void)__CBAS__glRotatef(((float)(__cbas_local_variable_mangled_name_ry)),((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glRotatef(((float)(__cbas_local_variable_mangled_name_rz)),((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glRotatef(((float)(__cbas_local_variable_mangled_name_rx)),((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glTranslatef(((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_y)),((float)(__cbas_local_variable_mangled_name_z)));
    (void)__CBAS__glCallList(((unsigned int)(__cbas_local_variable_mangled_name_dl)));
    (void)__CBAS__glPopMatrix();
    
}
static inline int __CBAS__setSwapInterval(int __cbas_local_variable_mangled_name_a)
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_GL_SetSwapInterval(__cbas_local_variable_mangled_name_a);
    
}
static inline int __CBAS__lockCursor(int __cbas_local_variable_mangled_name_state)
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_SetRelativeMouseMode(__cbas_local_variable_mangled_name_state);
    
}
static inline void __CBAS__freeMusic(unsigned char* __cbas_local_variable_mangled_name_p)
{
    
        Mix_Music* m = (Mix_Music*)__cbas_local_variable_mangled_name_p;
        Mix_FreeMusic(m);
    
    
}
static inline void __CBAS__beginUI()
{
    (void)__CBAS__glDisable(((unsigned int)__CBAS__GL_CULL_FACE()));
    (void)__CBAS__glDisable(((unsigned int)__CBAS__GL_DEPTH_TEST()));
    (void)__CBAS__glMatrixMode(((unsigned int)__CBAS__GL_PROJECTION()));
    (void)__CBAS__glPushMatrix();
    (void)__CBAS__glLoadIdentity();
    (void)__CBAS__glOrtho(((double)((unsigned long long)0ull)),((double)(__CBAS__width)),((double)(__CBAS__height)),((double)((unsigned long long)0ull)),((double)((long long)(-((long long)((unsigned long long)1ull))))),((double)((unsigned long long)1ull)));
    (void)__CBAS__glMatrixMode(((unsigned int)__CBAS__GL_MODELVIEW()));
    (void)__CBAS__glPushMatrix();
    (void)__CBAS__glLoadIdentity();
    (void)__CBAS__glEnable(((unsigned int)__CBAS__GL_TEXTURE_2D()));
    
}
static inline void __CBAS__endUI()
{
    (void)__CBAS__glDisable(((unsigned int)__CBAS__GL_BLEND()));
    (void)__CBAS__glEnable(((unsigned int)__CBAS__GL_CULL_FACE()));
    (void)__CBAS__glEnable(((unsigned int)__CBAS__GL_DEPTH_TEST()));
    (void)__CBAS__glDisable(((unsigned int)__CBAS__GL_TEXTURE_2D()));
    (void)__CBAS__glMatrixMode(((unsigned int)__CBAS__GL_MODELVIEW()));
    (void)__CBAS__glPopMatrix();
    (void)__CBAS__glMatrixMode(((unsigned int)__CBAS__GL_PROJECTION()));
    (void)__CBAS__glPopMatrix();
    
}
static inline void __CBAS__swapDisplay()
{
    SDL_GL_SwapWindow(sdl_win);
    
}
static inline int __CBAS__numJoysticks()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_NumJoysticks();
    
}
static inline unsigned char __CBAS__isGameController(int __cbas_local_variable_mangled_name_ind)
{
    unsigned char __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_IsGameController(__cbas_local_variable_mangled_name_ind);
    
}
static inline unsigned char* __CBAS__openGameController(int __cbas_local_variable_mangled_name_ind)
{
    unsigned char* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return (unsigned char*)SDL_GameControllerOpen(__cbas_local_variable_mangled_name_ind);
    
}
static inline void __CBAS__closeGameController(unsigned char* __cbas_local_variable_mangled_name_h)
{
    SDL_GameControllerClose((SDL_GameController*)__cbas_local_variable_mangled_name_h);
    
}
static inline signed short __CBAS__gameControllerGetAxis(unsigned char* __cbas_local_variable_mangled_name_h,int __cbas_local_variable_mangled_name_which)
{
    signed short __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    
        SDL_GameController* gc = (SDL_GameController*)__cbas_local_variable_mangled_name_h;
        return SDL_GameControllerGetAxis(gc, __cbas_local_variable_mangled_name_which);
    
    
}
static inline unsigned char __CBAS__gameControllerGetButton(unsigned char* __cbas_local_variable_mangled_name_h,int __cbas_local_variable_mangled_name_which)
{
    unsigned char __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    
        SDL_GameController* gc = (SDL_GameController*)__cbas_local_variable_mangled_name_h;
        return SDL_GameControllerGetButton(gc, __cbas_local_variable_mangled_name_which);
    
    
}
static inline int __CBAS__CONTROLLER_BUTTON_A()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_A;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_B()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_B;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_X()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_X;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_Y()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_Y;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_BACK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_BACK;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_GUIDE()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_GUIDE;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_START()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_START;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_LEFTSTICK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_LEFTSTICK;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_RIGHTSTICK()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_LEFTSHOULDER()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_RIGHTSHOULDER()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_DPAD_UP()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_DPAD_UP;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_DPAD_DOWN()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_DPAD_LEFT()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_DPAD_RIGHT()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_MISC1()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_MISC1;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_PADDLE1()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_PADDLE1;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_PADDLE2()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_PADDLE2;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_PADDLE3()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_PADDLE3;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_PADDLE4()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_PADDLE4;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_TOUCHPAD()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_TOUCHPAD;
    
}
static inline int __CBAS__CONTROLLER_BUTTON_MAX()
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_CONTROLLER_BUTTON_MAX;
    
}
static inline void __CBAS__setWindowResizable(unsigned char __cbas_local_variable_mangled_name_toggle)
{
    SDL_SetWindowResizable(sdl_win, __cbas_local_variable_mangled_name_toggle);
    
}
static inline unsigned char* __CBAS__getClipboardText()
{
    unsigned char* __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return (unsigned char*)SDL_GetClipboardText();
    
}
static inline int __CBAS__setClipboardText(unsigned char* __cbas_local_variable_mangled_name_txt)
{
    int __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_SetClipboardText((char*)__cbas_local_variable_mangled_name_txt);
    
}
static inline unsigned char __CBAS__hasClipboardText()
{
    unsigned char __cbas_local_variable_mangled_name___SEABASS_____RETVAL;
    return SDL_HasClipboardText();
    
}
static inline void __CBAS__startTextInput()
{
    SDL_StartTextInput();
    
}
static inline void __CBAS__stopTextInput()
{
    SDL_StopTextInput();
    
}
static inline void __CBAS__setTextInputRect(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y,int __cbas_local_variable_mangled_name_w,int __cbas_local_variable_mangled_name_h)
{
    
        SDL_Rect rr;
        rr.x = __cbas_local_variable_mangled_name_x;
        rr.y = __cbas_local_variable_mangled_name_y;
        rr.w = __cbas_local_variable_mangled_name_w;
        rr.h = __cbas_local_variable_mangled_name_h;
        SDL_SetTextInputRect(&rr);
    
    
}
static inline void __CBAS__drawBox(float __cbas_local_variable_mangled_name_xdim,float __cbas_local_variable_mangled_name_ydim,float __cbas_local_variable_mangled_name_zdim)
{
    float __cbas_local_variable_mangled_name_x;
    float __cbas_local_variable_mangled_name_y;
    (__cbas_local_variable_mangled_name_x) = ((float)(__cbas_local_variable_mangled_name_xdim));
    (__cbas_local_variable_mangled_name_y) = ((float)(__cbas_local_variable_mangled_name_zdim));
    (void)__CBAS__glNormal3f(((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(__cbas_local_variable_mangled_name_ydim)),((float)(-((float)(__cbas_local_variable_mangled_name_y)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_ydim)),((float)(-((float)(__cbas_local_variable_mangled_name_y)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_ydim)),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(__cbas_local_variable_mangled_name_ydim)),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glNormal3f(((float)((unsigned long long)0ull)),((float)((long long)(-((long long)((unsigned long long)1ull))))),((float)((unsigned long long)0ull)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(-((float)(__cbas_local_variable_mangled_name_ydim)))),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_x)),((float)(-((float)(__cbas_local_variable_mangled_name_ydim)))),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_x)),((float)(-((float)(__cbas_local_variable_mangled_name_ydim)))),((float)(-((float)(__cbas_local_variable_mangled_name_y)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(-((float)(__cbas_local_variable_mangled_name_ydim)))),((float)(-((float)(__cbas_local_variable_mangled_name_y)))));
    (__cbas_local_variable_mangled_name_x) = ((float)(__cbas_local_variable_mangled_name_zdim));
    (__cbas_local_variable_mangled_name_y) = ((float)(__cbas_local_variable_mangled_name_ydim));
    (void)__CBAS__glNormal3f(((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_xdim)),((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(-((float)(__cbas_local_variable_mangled_name_y)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_xdim)),((float)(__cbas_local_variable_mangled_name_x)),((float)(-((float)(__cbas_local_variable_mangled_name_y)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_xdim)),((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_xdim)),((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glNormal3f(((float)((long long)(-((long long)((unsigned long long)1ull))))),((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_xdim)))),((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_xdim)))),((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_xdim)))),((float)(__cbas_local_variable_mangled_name_x)),((float)(-((float)(__cbas_local_variable_mangled_name_y)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_xdim)))),((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(-((float)(__cbas_local_variable_mangled_name_y)))));
    (__cbas_local_variable_mangled_name_x) = ((float)(__cbas_local_variable_mangled_name_xdim));
    (__cbas_local_variable_mangled_name_y) = ((float)(__cbas_local_variable_mangled_name_ydim));
    (void)__CBAS__glNormal3f(((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(-((float)(__cbas_local_variable_mangled_name_y)))),((float)(__cbas_local_variable_mangled_name_zdim)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_x)),((float)(-((float)(__cbas_local_variable_mangled_name_y)))),((float)(__cbas_local_variable_mangled_name_zdim)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_y)),((float)(__cbas_local_variable_mangled_name_zdim)));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(__cbas_local_variable_mangled_name_y)),((float)(__cbas_local_variable_mangled_name_zdim)));
    (void)__CBAS__glNormal3f(((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)),((float)((long long)(-((long long)((unsigned long long)1ull))))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(__cbas_local_variable_mangled_name_y)),((float)(-((float)(__cbas_local_variable_mangled_name_zdim)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)1ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_y)),((float)(-((float)(__cbas_local_variable_mangled_name_zdim)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)1ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(__cbas_local_variable_mangled_name_x)),((float)(-((float)(__cbas_local_variable_mangled_name_y)))),((float)(-((float)(__cbas_local_variable_mangled_name_zdim)))));
    (void)__CBAS__glTexCoord2f(((float)((unsigned long long)0ull)),((float)((unsigned long long)0ull)));
    (void)__CBAS__glVertex3f(((float)(-((float)(__cbas_local_variable_mangled_name_x)))),((float)(-((float)(__cbas_local_variable_mangled_name_y)))),((float)(-((float)(__cbas_local_variable_mangled_name_zdim)))));
    
}
static inline void __CBAS__drawSphere(float __cbas_local_variable_mangled_name_r,unsigned int __cbas_local_variable_mangled_name_midrings,unsigned int __cbas_local_variable_mangled_name_vrings)
{
    
    
}
static inline void __CBAS__drawCone(float __cbas_local_variable_mangled_name_rbase,float __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_vrings)
{
    
    
}
static inline void __CBAS__drawCylinder(float __cbas_local_variable_mangled_name_r,float __cbas_local_variable_mangled_name_h,unsigned int __cbas_local_variable_mangled_name_vrings)
{
    
    
}
static void __CBAS__randomizeFood()
{
    unsigned long long __cbas_local_variable_mangled_name_i;
    ((((&__CBAS__food))->__CBAS__member__x)) = ((int)((unsigned int)(((unsigned int)((int)__CBAS__rand())) % ((unsigned int)((long long)64ll)))));
    ((((&__CBAS__food))->__CBAS__member__y)) = ((int)((unsigned int)(((unsigned int)((int)__CBAS__rand())) % ((unsigned int)((long long)36ll)))));
    for((__cbas_local_variable_mangled_name_i) = ((unsigned long long)0ull);(long long)(((unsigned long long)(__cbas_local_variable_mangled_name_i)) < ((unsigned long long)(__CBAS__snake_length)));(unsigned long long)((__cbas_local_variable_mangled_name_i)++)){

    if((long long)((unsigned long long)(((unsigned long long)((long long)(((int)((((&__CBAS__food))->__CBAS__member__x))) == ((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)(__cbas_local_variable_mangled_name_i))))->__CBAS__member__x)))))) && ((unsigned long long)((long long)(((int)((((&__CBAS__food))->__CBAS__member__y))) == ((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)(__cbas_local_variable_mangled_name_i))))->__CBAS__member__y))))))))){

    
    __CBAS__randomizeFood();return;
    }

    }

    
}
static void __CBAS__advanceSnake()
{
    int __cbas_local_variable_mangled_name_nx;
    int __cbas_local_variable_mangled_name_ny;
    long long __cbas_local_variable_mangled_name_i;
    (__cbas_local_variable_mangled_name_nx) = ((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__x)));
    (__cbas_local_variable_mangled_name_ny) = ((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__y)));
    if((long long)(((long long)(__CBAS__direction)) == ((long long)((unsigned long long)3ull)))){

    (int)((__cbas_local_variable_mangled_name_nx)++);
    }

    else if((long long)(((long long)(__CBAS__direction)) == ((long long)((unsigned long long)2ull)))){

    (int)((__cbas_local_variable_mangled_name_nx)--);
    }

    else if((long long)(((long long)(__CBAS__direction)) == ((long long)((unsigned long long)1ull)))){

    (int)((__cbas_local_variable_mangled_name_ny)--);
    }

    else if((long long)(((long long)(__CBAS__direction)) == ((long long)((unsigned long long)4ull)))){

    (int)((__cbas_local_variable_mangled_name_ny)++);
    }

    if((long long)(((long long)(__cbas_local_variable_mangled_name_nx)) < ((long long)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name_nx) = ((int)((long long)(((long long)(__cbas_local_variable_mangled_name_nx)) + ((long long)64ll))));
    }

    else if((long long)(((long long)(__cbas_local_variable_mangled_name_ny)) < ((long long)((unsigned long long)0ull)))){

    (__cbas_local_variable_mangled_name_ny) = ((int)((long long)(((long long)(__cbas_local_variable_mangled_name_ny)) + ((long long)36ll))));
    }

    (__cbas_local_variable_mangled_name_nx) = ((int)((long long)(((long long)(__cbas_local_variable_mangled_name_nx)) % ((long long)64ll))));
    (__cbas_local_variable_mangled_name_ny) = ((int)((long long)(((long long)(__cbas_local_variable_mangled_name_ny)) % ((long long)36ll))));
    for((__cbas_local_variable_mangled_name_i) = ((long long)((unsigned long long)(((unsigned long long)(__CBAS__snake_length)) - ((unsigned long long)1ull))));(long long)(((long long)(__cbas_local_variable_mangled_name_i)) > ((long long)((unsigned long long)0ull)));(long long)((__cbas_local_variable_mangled_name_i)--)){

    (*((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)(__cbas_local_variable_mangled_name_i)))) = *((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)(((long long)(__cbas_local_variable_mangled_name_i)) - ((long long)((unsigned long long)1ull)))))));
    }

    ((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__x)) = ((int)(__cbas_local_variable_mangled_name_nx));
    ((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__y)) = ((int)(__cbas_local_variable_mangled_name_ny));
    
}
static void __CBAS__renderSnake()
{
    float __cbas_local_variable_mangled_name_seg_width;
    float __cbas_local_variable_mangled_name_seg_height;
    long long __cbas_local_variable_mangled_name_i;
    (__cbas_local_variable_mangled_name_seg_width) = ((float)((double)(((double)((float)(__CBAS__width))) / ((double)64.0))));
    (__cbas_local_variable_mangled_name_seg_height) = ((float)((double)(((double)((float)(__CBAS__height))) / ((double)36.0))));
    (void)__CBAS__glColor3f(((float)((double)0.5000000000000000000000000000000000000000000000000000000000000000)),((float)((unsigned long long)1ull)),((float)((double)0.5000000000000000000000000000000000000000000000000000000000000000)));
    (void)__CBAS__glBegin(((unsigned int)__CBAS__GL_QUADS()));
    for((__cbas_local_variable_mangled_name_i) = ((long long)((unsigned long long)0ull));(long long)(((long long)(__cbas_local_variable_mangled_name_i)) < ((long long)(__CBAS__snake_length)));(long long)((__cbas_local_variable_mangled_name_i)++)){

    float __cbas_local_variable_mangled_name_x;
    float __cbas_local_variable_mangled_name_y;
    (__cbas_local_variable_mangled_name_x) = ((float)(((float)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)(__cbas_local_variable_mangled_name_i))))->__CBAS__member__x))) * ((float)(__cbas_local_variable_mangled_name_seg_width))));
    (__cbas_local_variable_mangled_name_y) = ((float)(((float)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)(__cbas_local_variable_mangled_name_i))))->__CBAS__member__y))) * ((float)(__cbas_local_variable_mangled_name_seg_height))));
    (void)__CBAS__glVertex2f(((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glVertex2f(((float)(((float)(__cbas_local_variable_mangled_name_x)) + ((float)(__cbas_local_variable_mangled_name_seg_width)))),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glVertex2f(((float)(((float)(__cbas_local_variable_mangled_name_x)) + ((float)(__cbas_local_variable_mangled_name_seg_width)))),((float)(((float)(__cbas_local_variable_mangled_name_y)) + ((float)(__cbas_local_variable_mangled_name_seg_height)))));
    (void)__CBAS__glVertex2f(((float)(__cbas_local_variable_mangled_name_x)),((float)(((float)(__cbas_local_variable_mangled_name_y)) + ((float)(__cbas_local_variable_mangled_name_seg_height)))));
    }

    (void)__CBAS__glEnd();
    
}
static void __CBAS__renderFood()
{
    float __cbas_local_variable_mangled_name_seg_width;
    float __cbas_local_variable_mangled_name_seg_height;
    float __cbas_local_variable_mangled_name_x;
    float __cbas_local_variable_mangled_name_y;
    (__cbas_local_variable_mangled_name_seg_width) = ((float)((double)(((double)((float)(__CBAS__width))) / ((double)64.0))));
    (__cbas_local_variable_mangled_name_seg_height) = ((float)((double)(((double)((float)(__CBAS__height))) / ((double)36.0))));
    (void)__CBAS__glBegin(((unsigned int)__CBAS__GL_QUADS()));
    (__cbas_local_variable_mangled_name_x) = ((float)(((float)((((&__CBAS__food))->__CBAS__member__x))) * ((float)(__cbas_local_variable_mangled_name_seg_width))));
    (__cbas_local_variable_mangled_name_y) = ((float)(((float)((((&__CBAS__food))->__CBAS__member__y))) * ((float)(__cbas_local_variable_mangled_name_seg_height))));
    (void)__CBAS__glColor3f(((float)((unsigned long long)1ull)),((float)((double)0.5000000000000000000000000000000000000000000000000000000000000000)),((float)((double)0.5000000000000000000000000000000000000000000000000000000000000000)));
    (void)__CBAS__glVertex2f(((float)(__cbas_local_variable_mangled_name_x)),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glVertex2f(((float)(((float)(__cbas_local_variable_mangled_name_x)) + ((float)(__cbas_local_variable_mangled_name_seg_width)))),((float)(__cbas_local_variable_mangled_name_y)));
    (void)__CBAS__glVertex2f(((float)(((float)(__cbas_local_variable_mangled_name_x)) + ((float)(__cbas_local_variable_mangled_name_seg_width)))),((float)(((float)(__cbas_local_variable_mangled_name_y)) + ((float)(__cbas_local_variable_mangled_name_seg_height)))));
    (void)__CBAS__glVertex2f(((float)(__cbas_local_variable_mangled_name_x)),((float)(((float)(__cbas_local_variable_mangled_name_y)) + ((float)(__cbas_local_variable_mangled_name_seg_height)))));
    (void)__CBAS__glEnd();
    
}
static void __CBAS__checkEatFood()
{
    if((long long)((unsigned long long)(((unsigned long long)((long long)(((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__x))) != ((int)((((&__CBAS__food))->__CBAS__member__x)))))) || ((unsigned long long)((long long)(((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__y))) != ((int)((((&__CBAS__food))->__CBAS__member__y))))))))){

    
    return;
    }

    (int)__CBAS__playSample(((unsigned char*)(__CBAS__eatSound)),((int)((unsigned long long)0ull)));
    (void)__CBAS__randomizeFood();
    (unsigned int)((__CBAS__snake_length)++);
    if((long long)(((long long)(__CBAS__snake_length)) == ((long long)20ll))){

    (unsigned int)((__CBAS__snake_length)--);
    (__CBAS__mode) = ((int)((unsigned long long)1ull));
    
    return;
    }

    (*((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)(((unsigned long long)(__CBAS__snake_length)) - ((unsigned long long)1ull)))))) = *((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)(((unsigned long long)(__CBAS__snake_length)) - ((unsigned long long)2ull)))))));
    
}
static void __CBAS__checkCollide()
{
    int __cbas_local_variable_mangled_name_x;
    int __cbas_local_variable_mangled_name_y;
    long long __cbas_local_variable_mangled_name_i;
    (__cbas_local_variable_mangled_name_x) = ((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__x)));
    (__cbas_local_variable_mangled_name_y) = ((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__y)));
    for((__cbas_local_variable_mangled_name_i) = ((long long)((unsigned long long)1ull));(long long)(((long long)(__cbas_local_variable_mangled_name_i)) < ((long long)(__CBAS__snake_length)));(long long)((__cbas_local_variable_mangled_name_i)++)){

    if((long long)(((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)(__cbas_local_variable_mangled_name_i))))->__CBAS__member__x))) != ((int)(__cbas_local_variable_mangled_name_x)))){

    continue;
    }

    if((long long)(((int)((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)(__cbas_local_variable_mangled_name_i))))->__CBAS__member__y))) != ((int)(__cbas_local_variable_mangled_name_y)))){

    continue;
    }

    (__CBAS__mode) = ((int)((unsigned long long)2ull));
    
    return;
    }

    
}
static void __CBAS__appInit()
{
    (void)__CBAS__openWindow(((unsigned char*)("Joy!")),((unsigned int)((unsigned long long)1280ull)),((unsigned int)((unsigned long long)720ull)));
    (int)__CBAS__setSwapInterval(((int)((unsigned long long)1ull)));
    (__CBAS__ourFont) = ((unsigned char*)__CBAS__loadFont(((unsigned char*)("fonts/jupiteroid/Bold.ttf")),((float)((unsigned long long)128ull))));
    (__CBAS__eatSound) = ((unsigned char*)__CBAS__loadSample(((unsigned char*)("soundfx/Suction Hit 1 Zube Tube.wav"))));
    (__CBAS__music) = ((unsigned char*)__CBAS__loadMusic(((unsigned char*)("music/Study and Relax.mp3"))));
    (void)__CBAS__srand(((unsigned int)((unsigned long long)__CBAS__unixtime())));
    (void)__CBAS__randomizeFood();
    ((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__x)) = ((int)((unsigned long long)10ull));
    ((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)0ull))))->__CBAS__member__y)) = ((int)((unsigned long long)10ull));
    ((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)1ull))))->__CBAS__member__x)) = ((int)((unsigned long long)9ull));
    ((((struct __CBAS__type__snake_segment*)((__CBAS__segs) + ((long long)((unsigned long long)1ull))))->__CBAS__member__y)) = ((int)((unsigned long long)10ull));
    
}
static void __CBAS__updateSnakeDir()
{
    if((long long)((unsigned long long)(!((long long)(__CBAS__stored_direction_change))))){

    
    return;
    }

    if((long long)((unsigned long long)(((unsigned long long)((long long)(((long long)(__CBAS__stored_direction_change)) == ((long long)((unsigned long long)1ull))))) && ((unsigned long long)((long long)(((long long)(__CBAS__direction)) != ((long long)((unsigned long long)4ull)))))))){

    (__CBAS__direction) = ((int)(__CBAS__stored_direction_change));
    }

    else if((long long)((unsigned long long)(((unsigned long long)((long long)(((long long)(__CBAS__stored_direction_change)) == ((long long)((unsigned long long)4ull))))) && ((unsigned long long)((long long)(((long long)(__CBAS__direction)) != ((long long)((unsigned long long)1ull)))))))){

    (__CBAS__direction) = ((int)(__CBAS__stored_direction_change));
    }

    else if((long long)((unsigned long long)(((unsigned long long)((long long)(((long long)(__CBAS__stored_direction_change)) == ((long long)((unsigned long long)2ull))))) && ((unsigned long long)((long long)(((long long)(__CBAS__direction)) != ((long long)((unsigned long long)3ull)))))))){

    (__CBAS__direction) = ((int)(__CBAS__stored_direction_change));
    }

    else if((long long)((unsigned long long)(((unsigned long long)((long long)(((long long)(__CBAS__stored_direction_change)) == ((long long)((unsigned long long)3ull))))) && ((unsigned long long)((long long)(((long long)(__CBAS__direction)) != ((long long)((unsigned long long)2ull)))))))){

    (__CBAS__direction) = ((int)(__CBAS__stored_direction_change));
    }

    (__CBAS__stored_direction_change) = ((int)((unsigned long long)0ull));
    
}
static void __CBAS__appUpdate()
{
    (void)__CBAS__clearScreen(((float)((double)0.1000000000000000000000000000000000000000000000000000000000000000)),((float)((double)0.1000000000000000000000000000000000000000000000000000000000000000)),((float)((double)0.3000000000000000444089209850062616169452667236328125000000000000)));
    (void)__CBAS__beginUI();
    if((long long)(((long long)(__CBAS__mode)) == ((long long)(-((long long)((unsigned long long)1ull)))))){

    (void)__CBAS__glColor3f(((float)((double)0.8000000000000000000000000000000000000000000000000000000000000000)),((float)((double)0.8000000000000000000000000000000000000000000000000000000000000000)),((float)((unsigned long long)1ull)));
    (void)__CBAS__renderText(((unsigned char*)("Press any key")),((unsigned char*)(__CBAS__ourFont)),((float)((unsigned long long)100ull)),((float)((unsigned long long)128ull)),((float)((unsigned long long)128ull)));
    }

    else if((long long)(((long long)(__CBAS__mode)) == ((long long)((unsigned long long)0ull)))){

    if((long long)(((long long)(__CBAS__stall)) > ((long long)((unsigned long long)0ull)))){

    (int)((__CBAS__stall)--);
    }

    else {
    (__CBAS__stall) = ((int)((unsigned long long)5ull));
    }

    if((long long)(((long long)(__CBAS__stall)) == ((long long)((unsigned long long)0ull)))){

    (void)__CBAS__updateSnakeDir();
    (void)__CBAS__advanceSnake();
    (void)__CBAS__checkEatFood();
    (void)__CBAS__checkCollide();
    }

    (void)__CBAS__renderFood();
    (void)__CBAS__renderSnake();
    }

    else if((long long)(((long long)(__CBAS__mode)) == ((long long)((unsigned long long)1ull)))){

    (void)__CBAS__glColor3f(((float)((double)0.8000000000000000000000000000000000000000000000000000000000000000)),((float)((double)0.8000000000000000000000000000000000000000000000000000000000000000)),((float)((unsigned long long)1ull)));
    (void)__CBAS__renderText(((unsigned char*)("You Won!")),((unsigned char*)(__CBAS__ourFont)),((float)((unsigned long long)100ull)),((float)((unsigned long long)128ull)),((float)((unsigned long long)128ull)));
    (void)__CBAS__glDisable(((unsigned int)__CBAS__GL_BLEND()));
    (void)__CBAS__renderFood();
    (void)__CBAS__renderSnake();
    }

    else if((long long)(((long long)(__CBAS__mode)) == ((long long)((unsigned long long)2ull)))){

    (void)__CBAS__glColor3f(((float)((double)0.8000000000000000000000000000000000000000000000000000000000000000)),((float)((double)0.8000000000000000000000000000000000000000000000000000000000000000)),((float)((unsigned long long)1ull)));
    (void)__CBAS__renderText(((unsigned char*)("You Lost!")),((unsigned char*)(__CBAS__ourFont)),((float)((unsigned long long)100ull)),((float)((unsigned long long)128ull)),((float)((unsigned long long)128ull)));
    (void)__CBAS__glDisable(((unsigned int)__CBAS__GL_BLEND()));
    (void)__CBAS__renderFood();
    (void)__CBAS__renderSnake();
    }

    (void)__CBAS__endUI();
    (void)__CBAS__swapDisplay();
    
}
static void __CBAS__appClose()
{
    (void)__CBAS__haltMusic();
    (void)__CBAS__freeMusic(((unsigned char*)(__CBAS__music)));
    (void)__CBAS__freeSample(((unsigned char*)(__CBAS__eatSound)));
    (void)__CBAS__free(((unsigned char*)(__CBAS__ourFont)));
    (void)__CBAS__closeWindow();
    
}
static void __CBAS__onClick(int __cbas_local_variable_mangled_name_btn,int __cbas_local_variable_mangled_name_state)
{
    
    
}
static void __CBAS__onTextInput(unsigned char* __cbas_local_variable_mangled_name_text)
{
    
    
}
static void __CBAS__onTextEdit(unsigned char* __cbas_local_variable_mangled_name_text,int __cbas_local_variable_mangled_name_start,int __cbas_local_variable_mangled_name_length)
{
    
    
}
static void __CBAS__onKey(int __cbas_local_variable_mangled_name_kc,unsigned char __cbas_local_variable_mangled_name_state)
{
    if((long long)((unsigned long long)(((unsigned long long)((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_state)) == ((unsigned long long)0ull)))) && ((unsigned long long)((long long)(((int)(__cbas_local_variable_mangled_name_kc)) == ((int)((unsigned int)__CBAS__KEY_ESCAPE())))))))){

    (void)__CBAS__appClose();
    (void)__CBAS__sys_exit(((int)((unsigned long long)0ull)));
    }

    if((long long)((unsigned long long)(((unsigned long long)((long long)(((long long)(__CBAS__mode)) == ((long long)(-((long long)((unsigned long long)1ull))))))) && ((unsigned long long)((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_state)) == ((unsigned long long)0ull))))))){

    (__CBAS__mode) = ((int)((unsigned long long)0ull));
    (int)__CBAS__playMusic(((unsigned char*)(__CBAS__music)),((int)((long long)(-((long long)((unsigned long long)1ull))))),((int)((unsigned long long)500ull)));
    }

    else if((long long)((unsigned long long)(((unsigned long long)((long long)(((long long)(__CBAS__mode)) == ((long long)((unsigned long long)0ull))))) && ((unsigned long long)((long long)(((unsigned long long)(__cbas_local_variable_mangled_name_state)) == ((unsigned long long)1ull))))))){

    if((long long)(((int)(__cbas_local_variable_mangled_name_kc)) == ((int)((unsigned int)__CBAS__KEY_UP())))){

    (__CBAS__stored_direction_change) = ((int)((unsigned long long)1ull));
    }

    else if((long long)(((int)(__cbas_local_variable_mangled_name_kc)) == ((int)((unsigned int)__CBAS__KEY_LEFT())))){

    (__CBAS__stored_direction_change) = ((int)((unsigned long long)2ull));
    }

    else if((long long)(((int)(__cbas_local_variable_mangled_name_kc)) == ((int)((unsigned int)__CBAS__KEY_RIGHT())))){

    (__CBAS__stored_direction_change) = ((int)((unsigned long long)3ull));
    }

    else if((long long)(((int)(__cbas_local_variable_mangled_name_kc)) == ((int)((unsigned int)__CBAS__KEY_DOWN())))){

    (__CBAS__stored_direction_change) = ((int)((unsigned long long)4ull));
    }

    }

    
}
static void __CBAS__onKeyRepeat(int __cbas_local_variable_mangled_name_kc,unsigned char __cbas_local_variable_mangled_name_state)
{
    
    
}
static void __CBAS__onResize(int __cbas_local_variable_mangled_name_w,int __cbas_local_variable_mangled_name_h)
{
    (void)__CBAS__glViewport(((int)((unsigned long long)0ull)),((int)((unsigned long long)0ull)),((unsigned int)(__cbas_local_variable_mangled_name_w)),((unsigned int)(__cbas_local_variable_mangled_name_h)));
    
}
static void __CBAS__onScroll(int __cbas_local_variable_mangled_name_x,int __cbas_local_variable_mangled_name_y)
{
    
    
}
/*
Blessed be the Lord Jesus Christ of Nazareth, forever. Amen.
*/
