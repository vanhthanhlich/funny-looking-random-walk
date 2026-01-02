#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600
#define AGENT_SIZE 2
#define STEP 7

typedef struct {
    int x, y;
} Velocity;

typedef struct {
    int x, y;
    Uint32 color;
} Agent;

static void hueToRgb(unsigned int *c, float p, float q, float t) {
    if (t < 0.0f)
        t += 1.0f;
    if (t > 1.0f)
        t -= 1.0f;
    if (t < 0.166666)
        *c = (unsigned int) ((q + (p - q) * 6 * t) * 100);
    else if (t < 0.5)
        *c = (unsigned int) (p * 100);
    else if (t < 0.666666)
        *c = (unsigned int) ((q + (p - q) * (.666666 - t) * 6) * 100);
    else
        *c = (unsigned int) (q * 100);
}

void HSLToRGB(float H, float S, float L, int rgb[]) {

    unsigned int red, green, blue;
    float p, q;

    if (S == 0) {
        red = green = blue = (unsigned int) (L * 100);
    } else {
        p = L < .50 ? L * (1 + S) : L + S - (L * S);
        q = 2 * L - p;


        hueToRgb(&red, p, q, H + .33333f);

        hueToRgb(&green, p, q, H);

        hueToRgb(&blue, p, q, H - .33333f);


    }
    red = (unsigned int) (((((float) red) / 100) * 255) + 0.5);
    green = (unsigned int) (((((float) green) / 100) * 255) + 0.5);
    blue = (unsigned int) (((((float) blue) / 100) * 255) + 0.5);

    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;
}

int getRandomColor() {
    int rgb[3];
    float h = (float)rand() / (float)RAND_MAX ;

    HSLToRGB(h, 1, 0.5, rgb);
    return (rgb[0] * 255 + rgb[1]) * 255 + rgb[2];
}

Velocity getRandomVelocity() {
    const int dx[] = {0, 0, -1, 1};
    const int dy[] = {-1, 1, 0, 0};
    
    int idx = rand() % 4;
    return (Velocity) { dx[idx], dy[idx] };
}

void moveAgent(SDL_Surface *psurface, Agent *agent) {
    SDL_Rect rect = {agent->x, agent->y, AGENT_SIZE, AGENT_SIZE};
    Velocity v = getRandomVelocity();
    
    for(int i = 0; i < STEP; i ++) {
        rect.x += v.x, rect.y += v.y;
        SDL_FillRect(psurface, &rect, agent->color); 
    }
 
    agent->x = rect.x, agent->y = rect.y;
}

Agent Init_agent() {
    int posx = WIDTH  / 2 + (2 * (rand() % 2) - 1) * (rand() % 30);
    int posy = HEIGHT / 2 + (2 * (rand() % 2) - 1) * (rand() % 30);
    Uint32 color = getRandomColor();

    return (Agent) {posx, posy, color};
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); 
    
    int num_agent = 0;
    if(argc > 2) {
        printf("How to use: %s <num-of-agents>\n", argv[0]);
    }
    else if(argc == 2) {
        num_agent = atoi(argv[1]);
    }
    else if(argc == 1) {
        num_agent = 5;
    }

    SDL_Window *pwindow = SDL_CreateWindow("random-walk-noanim", 
                                      SDL_WINDOWPOS_CENTERED, 
                                      SDL_WINDOWPOS_CENTERED, 
                                      WIDTH, 
                                      HEIGHT,  
                                      0);
    
    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);
    SDL_Rect rect = {WIDTH / 2, HEIGHT / 2, 1, 1};

    Agent *pagents = (Agent*) calloc(num_agent, sizeof(Agent));
    for(int i = 0; i < num_agent; i ++) {
        pagents[i] = Init_agent();
    }

    int running = 1;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
        }
       
        for(int i = 0; i < num_agent; i++) moveAgent(psurface, &pagents[i]);
        SDL_UpdateWindowSurface(pwindow);
        
        SDL_Delay(1000 / 60);
    }

    return 0;
}