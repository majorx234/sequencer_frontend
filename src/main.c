#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include <time.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

#define SQUARES_COUNT 8
#define SQUARE_SIZE 100.0f
#define SQUARE_PAD (SQUARE_SIZE*0.2f)
#define FONT_SIZE 68

// ENV PART
typedef struct Env {
  float delta_time;
  float screen_wdith;
  float screen_height;
  bool rendering;
  void *params;
} Env;
/*
typedef struct Action_Kind {
  ACTION_BLINK;
  ACTION_MOVE;
} Action_Kind;
*/
// typedef union

typedef enum {
  TASK_MOVE,
  TASK_COLOR,
} Task_Kind;

typedef union Task_As {
  Vector2 move;
  Color color;
} Task_As;

typedef struct Task {
  Task_Kind kind;
  Task_As as;
  size_t square_id;
} Task;

/*
typedef struct {
  Task *items;
  size_t count;
  size_t capacity;
} Tasks;
*/

// not really needed, no plugin concept yet
typedef struct Plug {
  size_t size;
  Font font;
  Rectangle squares[SQUARES_COUNT];
  Camera2D camera;
  Task* tasks;
} Plug;

Vector2 grid_2_world(size_t row, size_t col) {
  Vector2 world;
  world.x = col * (SQUARE_SIZE + SQUARE_PAD);
  world.y = row * (SQUARE_SIZE + SQUARE_PAD);
  return world;
}

int main(int argc, char **argv) {
  // Initialization
  Plug p = {0};
  p.font =
      LoadFontEx("./assets/fonts/Vollkorn-Regular.ttf", FONT_SIZE, NULL, 0);
  for (size_t i = 0; i < SQUARES_COUNT; i++) {
    p.squares[i].x = (i % 8) * SQUARE_SIZE + SQUARE_PAD;
    p.squares[i].y = (int)(i / 8) * SQUARE_SIZE + SQUARE_PAD;
    p.squares[i].width = SQUARE_SIZE;
    p.squares[i].height = SQUARE_SIZE;
  }

  // add first task
  arrput(p.tasks, ((Task){
      .kind = TASK_MOVE, .as = {.move = grid_2_world(1, 1)}, .square_id = 0}));
  printf("starting sequencer_frontend\n");
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "k-means");
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GetColor(0x181818AA));
    EndDrawing();
  }
  CloseWindow();
  UnloadFont(p.font);
  return 0;
}
