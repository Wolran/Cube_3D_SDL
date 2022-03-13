#include "console_engine.h"

///### GameEngine ###
int createGameEngine(GameEngine *ge, char *title,uint16_t win_size_x, uint16_t win_size_y, uint8_t pix_size_x, uint8_t pix_size_y)
{
    ge->winWidth    = win_size_x;
    ge->winHeight   = win_size_y;
    ge->pixWidth    = pix_size_x;
    ge->pixHeight   = pix_size_y;
    ge->title       = title;
    memset(ge->keyboard, 0, 256 * sizeof(Keys));
    memset(ge->mouse, 0, 5 * sizeof(Keys));

    uint16_t real_size_x = win_size_x * pix_size_x;
    uint16_t real_size_y = win_size_y * pix_size_y;
    
    SDL_Init(SDL_INIT_VIDEO);

    ge->window = SDL_CreateWindow(title, 100, 100, real_size_x, real_size_y, 0); 
    if (ge->window == NULL)
    {   
        printf("Could not create window: %s\n", SDL_GetError());
        return -1;
    }
    printf("Created window.\n");
    
    ge->renderer = SDL_CreateRenderer(ge->window, -1, SDL_RENDERER_ACCELERATED);
    if (ge->renderer == NULL)
    {   
        printf("Could not create renderer: %s\n", SDL_GetError());
        return -1;
    }
    printf("Created renderer.\n");
    
    ge->texture = SDL_CreateTexture(ge->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, win_size_x, win_size_y);
    if (ge->texture == NULL)
    {   
        printf("Could not create texture: %s\n", SDL_GetError());
        return -1;
    }
    printf("Created texture.\n");
    
    ge->default_sprite = sprite(win_size_x, win_size_y);
    if (ge->default_sprite == NULL)
    {   
        printf("Could not create pixels data: (malloc fail)\n");
        return -1;
    }
    printf("Created buffer.\n");

    ge->selected_sprite = ge->default_sprite;
    ge->draw_mode = M_NORMAL;
    ge->custom_draw_mode = NULL;

    return 1;
}

int startGameEngine(GameEngine *ge)
{
    int is_running = onGameCreate(ge);
    
    uint32_t startTime = 0;
    uint32_t currTime = 0;
    double elapsed_time = 0.0;

    while (is_running)
    {
        startTime = SDL_GetTicks();

        is_running &= processEvents(ge, &startTime);
        is_running &= processKeyboard(ge);
        is_running &= processMouse(ge);
        is_running &= onGameUpdate(ge, elapsed_time);
        is_running &= updateGraph(ge);
        
        currTime = SDL_GetTicks();
        elapsed_time = (currTime - startTime) / 1000.0;
    }
    
    onGameDestroy(ge);
    destroyGameEngine(ge);

    return 1;
}

void destroyGameEngine(GameEngine *ge)
{
    destroySprite(ge->default_sprite);
    SDL_DestroyTexture(ge->texture);
    SDL_DestroyRenderer(ge->renderer);
    SDL_DestroyWindow(ge->window);

    SDL_Quit();
}

int updateGraph(GameEngine *ge)
{
    int flag = 0;
    flag |= SDL_UpdateTexture(ge->texture, NULL, ge->default_sprite->data, ge->winWidth * sizeof(Color));
    flag |= SDL_RenderClear(ge->renderer);
    flag |= SDL_RenderCopy(ge->renderer, ge->texture, NULL, NULL);
    SDL_RenderPresent(ge->renderer);

    return !flag;
}

int processEvents(GameEngine *ge, uint32_t *startTime)
{   
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_WINDOWEVENT:
                if ( event.window.event == SDL_WINDOWEVENT_CLOSE )
                {
                    return 0;
                }
				if (event.window.event == SDL_WINDOWEVENT_MOVED)
				{
					*startTime = SDL_GetTicks();
				}
            break;
        }
    }

    return 1;
}

int processKeyboard(GameEngine *ge)
{
    uint8_t *new_keys = (uint8_t*)SDL_GetKeyboardState(NULL);

    for (uint32_t i = 0; i < 256; i++)
	{
		ge->keyboard[i].pressed = 0;
		ge->keyboard[i].released = 0;
		if (new_keys[i] != ge->old_keyboard_keys[i])
		{
			if (new_keys[i])
			{
				ge->keyboard[i].pressed = !ge->keyboard[i].held;
				ge->keyboard[i].held = 1;
			}
			else
			{
				ge->keyboard[i].released = 1;
				ge->keyboard[i].held = 0;
			}
		}
		ge->old_keyboard_keys[i] = new_keys[i];
	}

    return 1;
}

int processMouse(GameEngine *ge)
{
    uint32_t new_keys = SDL_GetMouseState(&ge->mouseX, &ge->mouseY);
    ge->mouseX /= ge->pixWidth;
    ge->mouseY /= ge->pixHeight;

    for (uint16_t i = 0; i < 5; i++)
	{
		ge->mouse[i].pressed = 0;
		ge->mouse[i].released = 0;
		if ((new_keys & (1 << i)) != (ge->old_mouse_keys & (1 << i)))
		{
			if (new_keys & (1 << i))
			{
				ge->mouse[i].pressed = !ge->mouse[i].held;
				ge->mouse[i].held = 1;
			}
			else
			{
				ge->mouse[i].released = 1;
				ge->mouse[i].held = 0;
			}
		}
	}
    ge->old_mouse_keys = new_keys;

    return 1; 
}

Keys getKey(GameEngine *ge, SDL_Scancode sc)
{
    return ge->keyboard[sc];
}

Keys getMouse(GameEngine *ge, uint8_t sc)
{
    return ge->mouse[sc];
}


int getMouseX(GameEngine *ge)
{
    return ge->mouseX;
}

int getMouseY(GameEngine *ge)
{
    return ge->mouseY;
}

int setCursorMode(int mode)
{
    return SDL_ShowCursor(mode);
}

Color getPixel(Sprite *spr, int x, int y)
{
    if (x < 0 || y < 0 || x >= spr->size_x || y >= spr->size_y || spr == NULL)
        return colorD(0x00000000);

    return spr->data[x + y * spr->size_x];
}

void setPixel(Sprite *spr, int x, int y, Color col)
{
    if (x < 0 || y < 0 || x >= spr->size_x || y >= spr->size_y || spr == NULL)
        return;

    spr->data[x + y * spr->size_x] = col;
}

void setPixelMode(GameEngine* ge, DRAWMODE mode)
{
    ge->draw_mode = mode;
}

void setCustomPixelFunc(GameEngine* ge, void(*func)(GameEngine* ge, int x, int y, Color col))
{
    ge->custom_draw_mode = func;
}

DRAWMODE getPixelMode(GameEngine* ge)
{
    return ge->draw_mode;
}

void setDrawTarget(GameEngine* ge, Sprite* spr)
{
    if (spr == NULL)
    {
        ge->selected_sprite = ge->default_sprite;
        return;
    }

    ge->selected_sprite = spr;
}

Sprite* getDrawTarget(GameEngine* ge)
{
    return ge->selected_sprite;
}

void Clear(GameEngine *ge, Color col)
{ 
    if (ge->selected_sprite == NULL) return;
    //memset(ge->pixels, col.d, ge->winWidth * ge->winHeight * sizeof(Color));
    for (uint32_t i = 0; i < ge->selected_sprite->size_x * ge->selected_sprite->size_y; i++)
        ge->selected_sprite->data[i] = col;
}

void Draw(GameEngine* ge, int x, int y, Color col)
{
    if (ge->selected_sprite == NULL) return;

    if (ge->draw_mode == M_NORMAL)
    {
        setPixel(ge->selected_sprite, x, y, col);
    }
    else if (ge->draw_mode == M_MASK)
    {
        if (col.a == 255)
            setPixel(ge->selected_sprite, x, y, col);
    }
    else if (ge->draw_mode == M_ALPHA)
    {
        Color pix = getPixel(ge->selected_sprite, x, y);

        float a = (float)(col.a / 255.0f) * 1.0f;//fBlendFactor;
        float c = 1.0f - a;
        float r = a * (float)col.r + c * (float)pix.r;
        float g = a * (float)col.g + c * (float)pix.g;
        float b = a * (float)col.b + c * (float)pix.b;

        setPixel(ge->selected_sprite, x, y, color(r, g, b, 255));
    }
    else if (ge->draw_mode == M_CUSTOM)
    {
        (*ge->custom_draw_mode)(ge, x, y, col);
    }
}

void drawLine(GameEngine* ge, int x1, int y1, int x2, int y2, Color col)
{
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = fabs(dx);
    dy1 = fabs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;
    if (dy1 <= dx1)
    {
        if (dx >= 0)
        {
            x = x1;
            y = y1;
            xe = x2;
        }
        else
        {
            x = x2;
            y = y2;
            xe = x1;
        }
        Draw(ge, x, y, col);
        for (i = 0; x < xe; i++)
        {
            x = x + 1;
            if (px < 0)
                px = px + 2 * dy1;
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                    y = y + 1;
                else
                    y = y - 1;
                px = px + 2 * (dy1 - dx1);
            }
            Draw(ge, x, y, col);
        }
    }
    else
    {
        if (dy >= 0)
        {
            x = x1;
            y = y1;
            ye = y2;
        }
        else
        {
            x = x2;
            y = y2;
            ye = y1;
        }
        Draw(ge, x, y, col);
        for (i = 0; y < ye; i++)
        {
            y = y + 1;
            if (py <= 0)
                py = py + 2 * dx1;
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                    x = x + 1;
                else
                    x = x - 1;
                py = py + 2 * (dx1 - dy1);
            }
            Draw(ge, x, y, col);
        }
    }
}

void drawRect(GameEngine *ge, int x, int y, uint16_t dx, uint16_t dy, Color col)
{
    drawLine(ge, x, y, x + dx, y, col);
    drawLine(ge, x, y + dy, x + dx, y + dy, col);
    drawLine(ge, x, y, x, y + dy, col);
    drawLine(ge, x + dx, y, x + dx, y + dy, col);
}

void fillRect(GameEngine *ge, int x, int y, uint16_t dx, uint16_t dy, Color col)
{
    for (int iy = 0; iy < dy; iy++)
        for (int ix = 0; ix < dx; ix++)
            Draw(ge, x + ix, y + iy, col);
}

void drawSprite(GameEngine *ge, Sprite *spr, int x, int y, uint8_t size)
{
    if (spr->data == NULL)
        return;

    for (int dy = 0; dy < spr->size_y; dy++)
    {
        for (int dx = 0; dx < spr->size_x; dx++)
        {
            Color col = getPixel(spr, dx, dy);
            Draw(ge, x + dx, y + dy, col);
        }
    }
}

void drawRotateSprite(GameEngine* ge, Sprite* spr, float cx, float cy, float angle, uint16_t x, uint16_t y)
{
    if (spr->data == NULL)
        return;

    //NEED TO ADD A WAY TO CALCULATE THE MINIMUM SPACE NEEDED

    for (int dy = -spr->size_y; dy < spr->size_y; dy++)
    {
        for (int dx = -spr->size_x; dx < spr->size_x; dx++)
        {
            float ex = cx + cos(-angle) * dx + sin(-angle) * dy;
            float ey = cy - sin(-angle) * dx + cos(-angle) * dy;

            if (ex >= 0 && ex < spr->size_x && ey >= 0 && ey < spr->size_y)
            {
                Color col = getPixel(spr, ex, ey);
                Draw(ge, x + dx, y + dy, col);
            }
        }
    }
}

void drawPartialSprite(GameEngine* ge, Sprite* spr, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, int x, int y, uint8_t size)
{
    if (spr->data == NULL)
        return;

    for (int dy = 0; dy < ey; dy++)
    {
        for (int dx = 0; dx < ex; dx++)
        {
            Color col = getPixel(spr, sx + dx, sy + dy);
            Draw(ge, x + dx, y + dy, col);
        }
    }
}

void drawPartialRotatedSprite(GameEngine* ge, Sprite* spr, float cx, float cy, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, float angle, int x, int y, uint8_t size)
{
    if (spr->data == NULL)
        return;

    //NEED TO ADD A WAY TO CALCULATE THE MINIMUM SPACE NEEDED

    for (int dy = -spr->size_y; dy < spr->size_y; dy++)
    {
        for (int dx = -spr->size_x; dx < spr->size_x; dx++)
        {
            float hx = (cx + sx) + cos(-angle) * dx + sin(-angle) * dy;
            float hy = (cy + sy) - sin(-angle) * dx + cos(-angle) * dy;

            if (hx >= sx && hx < sx + ex && hy >= sy && hy < sy + ey)
            {
                Color col = getPixel(spr, hx, hy);
                Draw(ge, x + dx, y + dy, col);
            }
        }
    }
}



///### Color ###
Color color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    Color col;
    col.r = r;
    col.g = g;
    col.b = b;
    col.a = a;
    return col;
}

Color colorD(uint32_t d)
{
    Color col;
    col.d = d;
    return col;
}

Color colorF(float r, float g, float b, float a)
{
    Color col;
    col.r = r * 255;
    col.g = g * 255;
    col.b = b * 255;
    col.a = a * 255;
    return col;
}

///### Vector ###
Vec2 add_v2(Vec2 a, Vec2 b)
{
    return (Vec2) { a.x + b.x, a.y + b.y};
}

Vec2 rem_v2(Vec2 a, Vec2 b)
{
    return (Vec2) { a.x - b.x, a.y - b.y };
}

Vec2 mul_v2(Vec2 a, float b)
{
    return (Vec2) { a.x * b, a.y * b };
}

Vec2 div_v2(Vec2 a, float b)
{
    return (Vec2) { a.x / b, a.y / b };
}

float dist_v2(Vec2 a)
{
    return sqrtf(fabsf(a.x * a.x + a.y * a.y));
}

Vec2 norm_v2(Vec2 a)
{
    float l = dist_v2(a);
    if (l == 0.0f) return (Vec2) { 0, 0 };
    return div_v2(a, l);
}

Vec2 rot_v2(Vec2 a, float r)
{
    return (Vec2) { cosf(r)* a.x + sinf(r) * a.y, -sinf(r) * a.x + cosf(r) * a.y };
}

///### Sprite ###
Sprite* sprite(uint16_t x, uint16_t y)
{
    Sprite *spr = (Sprite*)malloc(sizeof(Sprite));
    
    spr->data = (Color*)malloc(x * y * sizeof(Color));
    spr->size_x = x;
    spr->size_y = y;

    return spr;
}

Sprite* spriteF(const char* path)
{
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path);
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
    }
    else
    {
        Sprite* tex = sprite(loadedSurface->w, loadedSurface->h);

        //Convert surface to sprite
        for (uint32_t i = 0; i < loadedSurface->w * loadedSurface->h; i++)
        {
            uint32_t pixel = *((uint32_t*)loadedSurface->pixels + i);
            
            tex->data[i].r = pixel;
            tex->data[i].g = pixel >> 8;
            tex->data[i].b = pixel >> 16;
            tex->data[i].a = pixel >> 24;
        }

        printf("Could load %s !\n", path);

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface);

        return tex;
    }
    return NULL;
}

int destroySprite(Sprite* spr)
{
    if (spr == NULL)
        return 0;
    
    free(spr->data);

    return 1;
}








