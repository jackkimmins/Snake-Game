#include <SDL.h>
#include <emscripten.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#define WIDTH 800
#define HEIGHT 600
#define BOX_SIZE 40
#define SPEED 2
const int GAP_SIZE = 4;

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static int directionX = 0;
static int directionY = -BOX_SIZE;
std::vector<SDL_Rect> snake = { { .x = WIDTH / 2, .y = HEIGHT / 2, .w = BOX_SIZE, .h = BOX_SIZE } };
SDL_Rect fruit = { .x = (rand() % (WIDTH / BOX_SIZE)) * BOX_SIZE, .y = (rand() % (HEIGHT / BOX_SIZE)) * BOX_SIZE, .w = BOX_SIZE, .h = BOX_SIZE };

bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return a.x < b.x + b.w &&
           a.x + a.w > b.x &&
           a.y < b.y + b.h &&
           a.y + a.h > b.y;
}

void UpdateSnake() {
    // Move the snake's head.
    snake[0].x += directionX;
    snake[0].y += directionY;

    // Wrap-around if the snake goes out of the viewport.
    if (snake[0].x < 0) {
        snake[0].x = WIDTH - BOX_SIZE;
    } else if (snake[0].x + BOX_SIZE > WIDTH) {
        snake[0].x = 0;
    }

    if (snake[0].y < 0) {
        snake[0].y = HEIGHT - BOX_SIZE;
    } else if (snake[0].y + BOX_SIZE > HEIGHT) {
        snake[0].y = 0;
    }

    // Check for self-collision.
    for (size_t i = 1; i < snake.size(); i++) {
        if (CheckCollision(snake[0], snake[i])) {
            emscripten_cancel_main_loop();  // End game if snake collides with itself.
            return;
        }
    }

    // Check for collision with the fruit.
    if (CheckCollision(snake[0], fruit)) {
        // Grow snake and move fruit to new random position.
        snake.push_back({ 0, 0, BOX_SIZE, BOX_SIZE });
        fruit.x = (rand() % (WIDTH / BOX_SIZE)) * BOX_SIZE;
        fruit.y = (rand() % (HEIGHT / BOX_SIZE)) * BOX_SIZE;
    }

    // Move the snake's body.
    for (int i = snake.size() - 1; i > 0; i--) {
        snake[i] = snake[i-1];
    }
}


void Render() {
    // Clear screen.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw snake.
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (auto &segment : snake) {
        SDL_Rect gapSegment = segment;
        gapSegment.x += GAP_SIZE / 2;
        gapSegment.y += GAP_SIZE / 2;
        gapSegment.w -= GAP_SIZE;
        gapSegment.h -= GAP_SIZE;
        SDL_RenderFillRect(renderer, &gapSegment);
    }

    // Draw fruit.
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fruit);

    // Update screen.
    SDL_RenderPresent(renderer);
}

const Uint32 FRAME_DURATION = 100;  // Update game every 100ms (10 FPS for game logic).

void mainloop() {
    static Uint32 lastUpdate = 0;
    Uint32 currentTime = SDL_GetTicks();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            emscripten_cancel_main_loop();
            return;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (directionY == 0) {
                        directionX = 0;
                        directionY = -BOX_SIZE;
                    }
                    break;
                case SDLK_DOWN:
                    if (directionY == 0) {
                        directionX = 0;
                        directionY = BOX_SIZE;
                    }
                    break;
                case SDLK_LEFT:
                    if (directionX == 0) {
                        directionX = -BOX_SIZE;
                        directionY = 0;
                    }
                    break;
                case SDLK_RIGHT:
                    if (directionX == 0) {
                        directionX = BOX_SIZE;
                        directionY = 0;
                    }
                    break;
            }
        }
    }

    // Only update the game if enough time has passed.
    if (currentTime - lastUpdate >= FRAME_DURATION) {
        UpdateSnake();
        lastUpdate = currentTime;
    }

    Render();
}

int main() {
    // Seed the random number generator.
    srand((unsigned int)time(NULL));

    // Initialise SDL.
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    emscripten_set_main_loop(mainloop, 0, 1);

    return 0;
}
