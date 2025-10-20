#include "raylib.h"
#include "raymath.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {
  printf("starting sequencer_frontend\n");
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "k-means");
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GetColor(0x181818AA));
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
