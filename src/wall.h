#ifndef WALL_H
  #define WALL_H

  void draw_wall(float x, int index);
  void summon_wall();
  void wall_init();

  #define WALL_SUMMON_TIMER_ID 1
  #define WALL_SUMMON_INTERVAL 1000

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
