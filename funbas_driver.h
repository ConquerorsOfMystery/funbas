
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "c_includes/stb_image.h"
#include "c_includes/stb_truetype.h"

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//the degree to which this is stateful...
static SDL_Window *sdl_win = NULL;
static SDL_GLContext* sdl_glcontext = NULL;
static int shouldquit = 0;
static int __CBAS__width = 640, __CBAS__height = 480, __CBAS__display_scale = 1;
static int __CBAS__mousex = 0, __CBAS__mousey = 0, __CBAS__mousexrel=0, __CBAS__mouseyrel=0;
static int is_window_open = 0;
static int __CBAS__argc = 0;
static unsigned char** __CBAS__argv = NULL;



//Taken from stb_truetype.h
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


/*
static int __CBAS__lockCursor(int state){
	return SDL_SetRelativeMouseMode(state);
}
*/
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
		} else if (ev.type == SDL_KEYUP){
		    if(ev.key.repeat == 0)
		        __CBAS__onKey(SDL_GetKeyFromScancode(ev.key.keysym.scancode), 0);
		} else if(ev.type == SDL_WINDOWEVENT){
		    if(ev.window.event == SDL_WINDOWEVENT_RESIZED){
		        __CBAS__width = ev.window.data1;
		        __CBAS__height = ev.window.data2;
		        __CBAS__onResize(__CBAS__width, __CBAS__height);
		    }
		}
	}
}


/*

static void ainit(int needsSDLINIT){
	if(needsSDLINIT)
		if (SDL_Init(SDL_INIT_AUDIO)!=0)  
			{fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());exit(0);}
	Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
	if(-1 == Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)) {printf("\nAudio can't init :(");exit(2);}
}
static void acleanup(){
	Mix_CloseAudio();	
	Mix_Quit(); 
	SDL_Quit();
}
*/
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
const unsigned __CBAS__KEY_PGDN = SDLK_PAGEDOWN;
const unsigned __CBAS__KEY_PGUP = SDLK_PAGEUP;
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
const unsigned __CBAS__KEY_GRAVE = SDLK_BACKQUOTE;
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
const unsigned __CBAS__KEY_KP_STAR = SDLK_KP_MULTIPLY;
MKCONST(KP_DIVIDE)
const unsigned __CBAS__KEY_KP_SLASH = SDLK_KP_DIVIDE;
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



