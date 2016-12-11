#ifndef WALL_H
  #define WALL_H

  void draw_wall(float x, int index);
  void summon_wall();
  void wall_init();

  #define WALL_COUNT 3

  typedef struct {
    float x_curr;
    float y_bot;
    float y_top;
    int alive;
    int pass;
    float colorR;
    float colorG;
    float colorB;
  } Wall;

#endif
