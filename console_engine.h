/*
//### EXEMPLE ###
#define SDL_MAIN_HANDLED
#include "console_engine.h"

int onGameCreate(GameEngine *ge)
{
    Clear(ge, colorD(0x000000FF));

    return 1;
}

int onGameUpdate(GameEngine *ge, float elapsed_time)
{
    for (int y = 0; y < ge->winHeight; y++)
        for (int x = 0; x < ge->winWidth; x++)
            Draw(ge, x, y, color(rand() & 255, rand() & 255, rand() & 255, 255));


    return 1;
}

void onGameDestroy(GameEngine *ge)
{
}

int main( int argc, char *argv[] ) 
{
    GameEngine game_engine;
    if (createGameEngine(&game_engine, "PUT TITLE HERE !", 320,  240, 4, 4))
        startGameEngine(&game_engine);
    
    return 0;   
}

*/

#ifndef _CONSOLE_ENGINE
#define _CONSOLE_ENGINE

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct  GameEngine_s    GameEngine;
typedef struct  Keys_s          Keys;
typedef union   Color_u         Color;
typedef struct  Sprite_s        Sprite;
typedef struct  Vec2_s          Vec2;       //Still not defined
typedef struct  Vec3_s          Vec3;       //Still not defined

typedef enum    DRAWMODE_E      DRAWMODE;

///### GameEngine ###
int     	createGameEngine(GameEngine *ge, char *title, uint16_t win_size_x, uint16_t win_size_y, uint8_t pix_size_x, uint8_t pix_size_y);
int     	startGameEngine(GameEngine *ge);
void    	destroyGameEngine(GameEngine *ge);

int			updateGraph(GameEngine *ge);
int			processEvents(GameEngine *ge, uint32_t *startTime);	//pointer to start time is here to fix window freezing when moved on win32
int			processKeyboard(GameEngine *ge);
int			processMouse(GameEngine *ge);

int			onGameCreate(GameEngine *ge);						//undefined - need to be defined by the user
int			onGameUpdate(GameEngine *ge, float elapsed_time);	//undefined - need to be defined by the user
void		onGameDestroy(GameEngine *ge);						//undefined - need to be defined by the user

// inputs/outputs
Keys    	getKey(GameEngine *ge, SDL_Scancode sc);

Keys    	getMouse(GameEngine *ge, uint8_t sc);
int     	getMouseX(GameEngine *ge);
int     	getMouseY(GameEngine *ge);
int     	setCursorMode(int mode);

Color   	getPixel(Sprite *spr, int x, int y);
void    	setPixel(Sprite *spr, int x, int y, Color col);

void        setPixelMode(GameEngine* ge, DRAWMODE mode);
void        setCustomPixelFunc(GameEngine* ge, void(*func)(GameEngine* ge, int x, int y, Color col));
DRAWMODE    getPixelMode(GameEngine* ge);

void    	setDrawTarget(GameEngine* ge, Sprite* spr);
Sprite* 	getDrawTarget(GameEngine* ge);

void    	Clear(GameEngine *ge, Color col);
void    	Draw(GameEngine *ge, int x, int y, Color col);
void    	drawRect(GameEngine *ge, int x, int y, uint16_t dx, uint16_t dy, Color col);
void    	drawLine(GameEngine* ge, int x0, int y0, int x1, int y1, Color col);
void    	fillRect(GameEngine* ge, int x, int y, uint16_t dx, uint16_t dy, Color col);
void    	drawSprite(GameEngine *ge, Sprite *spr, int x, int y, uint8_t size);
void    	drawRotateSprite(GameEngine* ge, Sprite* spr, float cx, float cy, float angle, uint16_t x, uint16_t y);
void    	drawPartialSprite(GameEngine* ge, Sprite* spr, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, int x, int y, uint8_t size);
void    	drawPartialRotatedSprite(GameEngine* ge, Sprite* spr, float cx, float cy, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, float angle, int x, int y, uint8_t size);


///### Color ###
Color		color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
Color		colorD(uint32_t d);
Color		colorF(float r, float g, float b, float a);

///### Vector ###
Vec2		add_v2(Vec2 a, Vec2 b);
Vec2		rem_v2(Vec2 a, Vec2 b);
Vec2		mul_v2(Vec2 a, float b);
Vec2		div_v2(Vec2 a, float b);
float		dist_v2(Vec2 a);
Vec2		norm_v2(Vec2 a);
Vec2		rot_v2(Vec2 a, float r);

///### Sprite ###
Sprite*		sprite(uint16_t x, uint16_t y);
Sprite*		spriteF(const char* path);
int			destroySprite(Sprite* spr);

struct	Keys_s
{
    uint8_t	pressed;
    uint8_t	held;
    uint8_t	released;
};


enum	DRAWMODE_E {
    M_NORMAL 	= 0,
    M_MASK		= 1,
    M_ALPHA		= 2,
    M_CUSTOM	= 3
};

struct	GameEngine_s
{
    uint16_t		winWidth;
    uint16_t		winHeight;
    uint8_t			pixWidth;
    uint8_t			pixHeight;
    char			*title;

    SDL_Window		*window;
    SDL_Renderer	*renderer;
    SDL_Texture		*texture;

    DRAWMODE		draw_mode;
    void			(*custom_draw_mode)(GameEngine* ge, int x, int y, Color col);

    Sprite			*default_sprite;
    Sprite			*selected_sprite;

    uint8_t			old_keyboard_keys[256];
    uint32_t		old_mouse_keys;
    Keys			keyboard[256];
    Keys			mouse[5];

    int				mouseX;
    int				mouseY;
};

union	Color_u
{
    struct { uint8_t a; uint8_t b; uint8_t g; uint8_t r; };
    uint32_t d;
};

struct	Vec2_s
{
    float x;
    float y;
};

struct	Vec3_s
{
    float x;
    float y;
    float z;
};

struct	Sprite_s
{
    Color*      data;
    uint16_t    size_x;
    uint16_t    size_y;
};

#endif
