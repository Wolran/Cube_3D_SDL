#define SDL_MAIN_HANDLED
#include "console_engine.h"

Sprite* tex;

int map[10][10];
float hmap[10][10];

float playerx;
float playery;
float theta;

float balek;

void maprand()
{
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            if (rand() % 8 <= 1)    map[x][y] = 1;
            else                    map[x][y] = 0;
                    
            hmap[x][y] = (float)rand() / RAND_MAX * 2;
        }
    }
}

int getmap(int x, int y)
{
    if (x < 0 || x >= 10 || y < 0 || y >= 10)
        return -1;

    return map[x][y];
}
int onGameCreate(GameEngine* ge)
{  
    tex = spriteF("test1.png");

    maprand();
    playerx = 5;
    playery = 5;
    theta = 0;
    
    balek = 0.0f;

    return 1;
}

int onGameUpdate(GameEngine* ge, float elapsed_time)
{
    float x2 = 0;
    float y2 = 0;
    float dx = 0;
    float dy = 0;

    balek += elapsed_time;

    dx = cos(theta);
    dy = sin(theta);

    x2 = dx * 20 + playerx;
    y2 = dy * 20 + playery;


    if (getKey(ge, SDL_SCANCODE_W).held) {
        playerx = playerx + (elapsed_time * dx * 2);
        playery = playery + (elapsed_time * dy * 2);
    }
    if (getKey(ge, SDL_SCANCODE_S).held) {

        playerx = playerx - (elapsed_time * dx * 2);
        playery = playery - (elapsed_time * dy * 2);
    }
    if (getKey(ge, SDL_SCANCODE_A).held) theta = theta - (elapsed_time);
    if (getKey(ge, SDL_SCANCODE_D).held) theta = theta + (elapsed_time);
    if (getKey(ge, SDL_SCANCODE_SPACE).held) maprand();

    Clear(ge, colorD(0x000000FF));

    float x3 = 0;
    float y3 = 0;
    for (int x = 0; x < ge->winWidth; x++) {

        x3 = playerx;
        y3 = playery;
        int flag = 0;
        float dist = 0;

        float fov = 3.14159 * 0.5;


        float half_screen = ((float)ge->winWidth / 2.0);

        float adj = tan(fov / 2) / half_screen;

        //Vec2 p1 = (Vec2){ (float)ge->winWidth / 2.0, adj };
        //p1 = rot_v2(p1, theta - fov / 2);

        //Vec2 p2 = (Vec2){ (float)ge->winWidth / 2.0, 0 };
        //p2 = rot_v2(p2, theta + 3.14159 / 2);

        float ar = atan(adj * ((float)x - half_screen));

        float rx = cos(ar + theta);
        float ry = sin(ar + theta);


        while (flag == 0) {
            dist = dist + 1.0 / 30;
            x3 = x3 + rx / 30;
            y3 = y3 + ry / 30;

            flag = getmap((int)(x3 + 1) - 1, (int)(y3 + 1) - 1);
            //printf("oue:%d\n", flag);
        }

        float ratio = (float)ge->winWidth / (float)ge->winHeight;

        float wall = (ge->winHeight * ratio / 2) / (dist * cos(ar));
        float end = (float)ge->winHeight / 2 + wall/2;
        float start = end - wall * hmap[(int)x3][(int)y3];
        if (flag == -1)
            start = end - wall;

        float px = x3 - (int)x3;
        float py = y3 - (int)y3;

        int dir = 0;
        if ((int)x3 == (int)(x3 - rx / 30))
            dir = 1;


        //drawLine(ge, x, start, x, end, col);
        for (int y = start; y < end; y++)
        {
            int ty = (float)(y - start);

            Color col = colorD(0x10FF65FF);
            if (flag > 0)
            {
                if (dir == 1)
                    col = getPixel(tex, px * tex->size_x, (int)((float)ty / wall * tex->size_y) % tex->size_y);
                else
                    col = getPixel(tex, py * tex->size_x, (int)((float)ty / wall * tex->size_y) % tex->size_y);
            }
            Draw(ge, x, y, col);
        }
		
        drawLine(ge, playerx * 5, playery * 5, x3 * 5, y3 * 5, colorD(0x00ff00ff));
    }

    drawRect(ge, playerx * 5 - 4, playery * 5 - 4, 8, 8, colorD(0x00ff00ff));

    setPixelMode(ge, M_ALPHA);
    fillRect(ge, 0, 0, 50, 50, colorD(0x60606060));
    setPixelMode(ge, M_NORMAL);
    Draw(ge, playerx * 5, playery * 5, colorD(0xff0000ff));
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            if (map[x][y] == 1)
                fillRect(ge, x * 5, y * 5, 5, 5, colorD(0xff0000ff));
        }

    }
    return 1;
}

void onGameDestroy(GameEngine* ge)
{
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
      GameEngine game_engine;
      if (createGameEngine(&game_engine, "3D !", 600, 400, 2, 2))
          startGameEngine(&game_engine);
      return 0;
}