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
#define SQUARE_MOVE_DURATION 0.3

typedef struct Task_Move_Data {
  Vector2 move;
  size_t square_id;
  float t; // time
  float t1;
} Task_Move_Data;

// TODO fix forward deklaration
typedef struct Env Env;

typedef struct Task {
  void (*setup)(Env, void*);
  bool (*update)(Env, void*);
  void (*close)(Env, void*);
  void* data;
} Task;

typedef struct Square {
  Vector2 position;
  Color color;
  Rectangle boundary;
  Vector2 offset;
} Square;

// not really needed, no plugin concept yet
typedef struct Plug {
  size_t size;
  Font font;
  Square squares[SQUARES_COUNT];
  // Rectangle squares[SQUARES_COUNT];
  Camera2D camera;
  Task *tasks;
  size_t task_idx;
} Plug;

// ENV PART
typedef struct Env {
  float delta_time;
  float screen_width;
  float screen_height;
  bool rendering;
  void *params;
  float time;
  Plug* p;
} Env;

Vector2 grid_2_world(size_t row, size_t col) {
  Vector2 world;
  world.x = col * (SQUARE_SIZE + SQUARE_PAD);
  world.y = row * (SQUARE_SIZE + SQUARE_PAD);
  return world;
}

void task_move_setup(Env env, void* data) {
  // niy
  Task_Move_Data *task_data = (Task_Move_Data *)data;
  task_data->t = 0.0;
  task_data->t1 = env.time;
}
bool task_move_update(Env env, void* data) {
  Task_Move_Data *task_data = (Task_Move_Data *)data;
  if (task_data->t >= 1.0) {
    return true;
  };
  task_data->t = (task_data->t*SQUARE_MOVE_DURATION + env.delta_time)/SQUARE_MOVE_DURATION;
  Square* square = &env.p->squares[task_data->square_id];
  if (task_data->square_id < SQUARES_COUNT) {
    Vector2 position = {
      square->boundary.x,
      square->boundary.y,
    };
    square->offset = Vector2Subtract(task_data->move, position);
    square->offset = Vector2Scale(square->offset, task_data->t);
  }
  if (task_data->t >= 1.0) {
    return true;
  }
  return false;
}
void task_move_close(Env env, void* data) {
  Task_Move_Data *task_data = (Task_Move_Data *)data;
  Square* square = &env.p->squares[task_data->square_id];
  square->boundary.x += square->offset.x;
  square->boundary.y += square->offset.y;
  square->offset.x = 0;
  square->offset.y = 0;
}

void task_move_add(Task **tasks, Task_Move_Data **data_arr, Task_Move_Data new_data) {
  arrput(*data_arr, new_data);
  size_t data_array_len = arrlen(*data_arr) - 1;
  arrput(*tasks, ((Task){.data = &((*data_arr)[data_array_len]),
                         .setup = task_move_setup,
                         .update = task_move_update,
                         .close = task_move_close}));
  size_t len = arrlen(*tasks)-1;
}

int main(int argc, char **argv) {
  // Initialization
  Color background_color = ColorFromHSV(0, 0, 0.05);
  Color foreground_color = ColorFromHSV(0, 0, 0.95);
  Plug p = {0};
  // TODO put into plug
  Task_Move_Data *data_arr = NULL;
  //p.font =
  //    LoadFontEx("./assets/fonts/Vollkorn-Regular.ttf", FONT_SIZE, NULL, 0);
  for (size_t i = 0; i < SQUARES_COUNT; i++) {
    Vector2 world = grid_2_world((int)(i / 8), ( i % 8));
    p.squares[i].boundary.x = world.x;
    p.squares[i].boundary.y = world.y;
    p.squares[i].boundary.width = SQUARE_SIZE;
    p.squares[i].boundary.height = SQUARE_SIZE;
    p.squares[i].color = foreground_color;
    p.squares[i].offset = CLITERAL(Vector2){0};
  }
  p.task_idx = 0;
  Env env = {0};
  env.screen_width = 800;
  env.screen_height = 600;
  env.p = &p;
  env.time = 0.0f;

  // add first task
  task_move_add(&p.tasks, &data_arr, ((Task_Move_Data){.move = grid_2_world(0, 1), .square_id = 0}));
  task_move_add(&p.tasks, &data_arr, ((Task_Move_Data){.move = grid_2_world(0, 2), .square_id = 1}));
  task_move_add(&p.tasks, &data_arr, ((Task_Move_Data){.move = grid_2_world(0, 3), .square_id = 2}));
  task_move_add(&p.tasks, &data_arr, ((Task_Move_Data){.move = grid_2_world(3, 3), .square_id = 0}));

  printf("starting sequencer_frontend\n");
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(env.screen_width, env.screen_height, "sequencer_frontend");
  while (!WindowShouldClose()) {
    BeginDrawing();

    // iterate through tasks
    if(p.task_idx < arrlen(p.tasks)) {
      Task task = p.tasks[p.task_idx];
      if(task.update(env, task.data), env.time){
        p.task_idx +=1;
      }
    }
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
      Rectangle boundary = p.squares[i].boundary;
      boundary.x += p.squares[i].offset.x;
      boundary.y += p.squares[i].offset.y;
      DrawRectangleRec(boundary, p.squares[i].color);
    }
    EndDrawing();
    env.time += 0.01;
  }
  CloseWindow();
  UnloadFont(p.font);
  return 0;
}
