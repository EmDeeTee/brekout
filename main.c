#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define BALL_SIZE 25
#define PAD_WIDTH 100
#define PAD_HEIGHT 25
#define FPS 60
#define BALL_SPEED 200
#define PAD_SPEED 5
#define TARGET_CAP 20
#define TARGET_WIDTH 50
// I'm pretty sure that this is not how delta time works
#define DELTA_TIME 1.0/FPS

TTF_Font* font = NULL;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
float ball_x = SCREEN_WIDTH/2;
float ball_y = SCREEN_HEIGHT/2;
float pad_x = SCREEN_WIDTH/2-PAD_WIDTH/2;
float pad_y = SCREEN_HEIGHT - 100;
int ball_dx = 1;
int ball_dy = 1;
int score = 0;

typedef struct Target {
  SDL_Rect rect;
  bool isDestroyed;
} Target;

Target targets[TARGET_CAP] = {0};

// This is seriously not OK... Or is it?
void InitTargets() {
  int row = 1;
  int ii = 0;
  for (int i = 0; i < TARGET_CAP; i++) {
    SDL_Rect* rect = &targets[i].rect;
    rect->x = (TARGET_WIDTH + 10)*ii;
    rect->y = (40)*row;
    if (rect->x > SCREEN_WIDTH) {
      row++;
      ii = 0;
    }
    ii++;
  }
}

void RenderTargets() {
  for (int i = 0; i < TARGET_CAP; i++) {
    Target* t = &targets[i];
    if (t->isDestroyed)
      continue;
    t->rect = (SDL_Rect){.x = t->rect.x, .y = t->rect.y, .w = TARGET_WIDTH, .h = PAD_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 255, 0, 100, 255);
    SDL_RenderFillRect(renderer, &t->rect);
  }
}

void RenderScore(void) {
  SDL_Surface* text;
  SDL_Color color = { 255, 255, 255 };
  char msg[3];
  sprintf(msg,"%i",score);
  
  text = TTF_RenderText_Solid(font, msg, color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
  SDL_Rect texture_rect = {0,0,35,50};
  SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
  SDL_FreeSurface(text);
  SDL_DestroyTexture(texture);
}

void CloseAndExit(void) {
  SDL_DestroyWindow(window);
  SDL_Quit();
  exit(0);
}

void HandleInput(void) {
  SDL_PumpEvents();
  const Uint8* keys = SDL_GetKeyboardState(NULL);

  if (keys[SDL_SCANCODE_Q]) {
    CloseAndExit();
  }
  if (keys[SDL_SCANCODE_A]) {
    pad_x -= PAD_SPEED;
  }
  if (keys[SDL_SCANCODE_D]) {
    pad_x += PAD_SPEED;
  }
}

int main(void) {
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  font = TTF_OpenFont("Arial.ttf", 24);
  if (font == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Cannot load font!\n");
    CloseAndExit();
  }
  window = SDL_CreateWindow(
			    "Brekout",
			    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			    SCREEN_WIDTH, SCREEN_HEIGHT,
			    SDL_WINDOW_SHOWN
			    );
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  InitTargets();
  
  bool isPaused = false;

  while (!isPaused) {
    // Nice ball btw
    SDL_Rect ball = (SDL_Rect){.x = ball_x, .y = ball_y, .w = BALL_SIZE, .h = BALL_SIZE};
    SDL_Rect pad = (SDL_Rect){.x = pad_x, .y = pad_y, .w = PAD_WIDTH, .h = PAD_HEIGHT};
    
    SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &ball);

    SDL_SetRenderDrawColor(renderer, 90, 0, 100, 255);
    SDL_RenderFillRect(renderer, &pad);

    RenderTargets();
    RenderScore();
    
    SDL_RenderPresent(renderer);

    float nx = ball_x + ball_dx*BALL_SPEED*DELTA_TIME;
    float ny = ball_y + ball_dy*BALL_SPEED*DELTA_TIME;
    if (nx < 0 || nx + BALL_SIZE > SCREEN_WIDTH) {
      ball_dx *= -1;
    } else if (ny < 0 || ny + BALL_SIZE > SCREEN_HEIGHT) {
      ball_dy *= -1;
    }

    if (pad_x < 0) {
      pad_x = 1;
    } else if (pad_x + PAD_WIDTH > SCREEN_WIDTH) {
      pad_x = SCREEN_WIDTH - PAD_WIDTH- 1;
    }

    ball_x = nx;
    ball_y = ny;

    if (SDL_HasIntersection(&pad, &ball) == SDL_TRUE) {
      ball_dy *= -1;
      // Michau problems require Michau solutions
      ball_y -= 20;
    }
    for (int i = 0; i < TARGET_CAP; i++) {
      Target* t = &targets[i];
	if (SDL_HasIntersection(&ball, &t->rect) == SDL_TRUE) {
	  t->isDestroyed = true;
	  // The score system def. working as intended
	  score++;
	}
    }
    
    HandleInput();
    // Framerate dependent movement KEKW
    SDL_Delay(1000/FPS);
  }
  

  CloseAndExit();
  return 0;
}
