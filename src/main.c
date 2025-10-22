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

#define SQUARES_COUNT 16
#define SQUARE_SIZE 50.0f
#define SQUARE_PAD (SQUARE_SIZE*0.2f)
#define FONT_SIZE 68

// ENV PART
typedef struct Env {
  float delta_time;
  float screen_width;
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
  Task *tasks;
  size_t task_idx;
} Plug;

Vector2 grid_2_world(size_t row, size_t col) {
  Vector2 world;
  world.x = col * (SQUARE_SIZE + SQUARE_PAD);
  world.y = row * (SQUARE_SIZE + SQUARE_PAD);
  return world;
}

int main(int argc, char **argv) {
  // Initialization
  Env env = {0};
  env.screen_width = 800;
  env.screen_height = 600;
  Plug p = {0};
  p.font =
      LoadFontEx("./assets/fonts/Vollkorn-Regular.ttf", FONT_SIZE, NULL, 0);
  for (size_t i = 0; i < SQUARES_COUNT; i++) {
    p.squares[i].x = (i % 8) * (SQUARE_SIZE + SQUARE_PAD);
    p.squares[i].y = (int)(i / 8) * (SQUARE_SIZE + SQUARE_PAD);
    p.squares[i].width = SQUARE_SIZE;
    p.squares[i].height = SQUARE_SIZE;
  }
  p.task_idx = 0;
  // add first task
  arrput(p.tasks, ((Task){
      .kind = TASK_MOVE, .as = {.move = grid_2_world(1, 1)}, .square_id = 0}));
  printf("starting sequencer_frontend\n");
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(env.screen_width, env.screen_height, "sequencer_frontend");
  while (!WindowShouldClose()) {
    BeginDrawing();
    Color background_color = ColorFromHSV(0, 0, 0.05);
    Color foreground_color = ColorFromHSV(0, 0, 0.95);

    ClearBackground(background_color);
    memset(&p.camera, 0 , sizeof(p.camera));
    p.camera.zoom = 1.0f;
    p.camera.target = CLITERAL(Vector2) {
      // TODO: adjust to amount of squares
      -3.0 * SQUARE_SIZE + SQUARE_PAD*0.5,
      -env.screen_height/2 + SQUARE_SIZE + SQUARE_PAD*0.5,
    };
    BeginMode2D(p.camera);
    for(size_t i = 0; i < SQUARES_COUNT; i++) {
      DrawRectangleRec(p.squares[i], foreground_color);
    }
    EndDrawing();
  }
  CloseWindow();
  UnloadFont(p.font);
  return 0;
}
