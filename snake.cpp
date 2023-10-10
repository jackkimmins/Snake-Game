#include <SDL.h>
#include <emscripten.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#define WIDTH 800
#define HEIGHT 600
#define BOX_SIZE 40
#define SPEED 2

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static int directionX = 0;
static int directionY = -BOX_SIZE;
std::vector<SDL_Rect> snake = { {WIDTH / 2, HEIGHT / 2, BOX_SIZE, BOX_SIZE} };
SDL_Rect fruit;

inline bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}

void PlaceRandomFruit() {
    fruit = {(rand() % (WIDTH / BOX_SIZE)) * BOX_SIZE, (rand() % (HEIGHT / BOX_SIZE)) * BOX_SIZE, BOX_SIZE, BOX_SIZE};
}

void UpdateSnake() {
    snake[0].x += directionX;
    snake[0].y += directionY;

    snake[0].x = (snake[0].x + WIDTH) % WIDTH;
    snake[0].y = (snake[0].y + HEIGHT) % HEIGHT;

    for (size_t i = 1; i < snake.size(); i++) {
        if (CheckCollision(snake[0], snake[i])) {
            emscripten_cancel_main_loop();
            return;
        }
    }

    if (CheckCollision(snake[0], fruit)) {
        snake.push_back({0, 0, BOX_SIZE, BOX_SIZE});
        PlaceRandomFruit();
    }

    for (int i = snake.size() - 1; i > 0; i--) {
        snake[i] = snake[i-1];
    }
}

void Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto &segment : snake) {
        SDL_RenderFillRect(renderer, &segment);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fruit);

    SDL_RenderPresent(renderer);
}

const Uint32 FRAME_DURATION = 100;

void mainloop() {
    static Uint32 lastUpdate = 0;
    Uint32 currentTime = SDL_GetTicks();
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            emscripten_cancel_main_loop();
            return;
        } else if (e.type == SDL_KEYDOWN && ((e.key.keysym.sym == SDLK_UP && directionY == 0) ||
                                             (e.key.keysym.sym == SDLK_DOWN && directionY == 0) ||
                                             (e.key.keysym.sym == SDLK_LEFT && directionX == 0) ||
                                             (e.key.keysym.sym == SDLK_RIGHT && directionX == 0))) {
            directionX = (e.key.keysym.sym == SDLK_RIGHT ? BOX_SIZE : (e.key.keysym.sym == SDLK_LEFT ? -BOX_SIZE : 0));
            directionY = (e.key.keysym.sym == SDLK_DOWN ? BOX_SIZE : (e.key.keysym.sym == SDLK_UP ? -BOX_SIZE : 0));
        }
    }

    if (currentTime - lastUpdate >= FRAME_DURATION) {
        UpdateSnake();
        lastUpdate = currentTime;
    }

    Render();
}

int main() {
    srand((unsigned int)time(NULL));

    PlaceRandomFruit();

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    emscripten_set_main_loop(mainloop, 0, 1);

    return 0;
}