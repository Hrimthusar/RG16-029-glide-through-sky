#ifndef WORLD_H
  #define WORLD_H

  #define STAR_X_NUMBER 7
  #define STAR_Y_NUMBER 8

  void check_score(int index);
  void draw_world();
  void score_init();
  void stars_init();

  int window_width;
  int window_height;
  float speed_correction;

  typedef struct {
    // Global z-index of gui
    float z;
    // Plate
    float plate_width;
    float plate_height;
    // HP
    float hp_x;
    float hp_y;
    // Mana
    float mana_x;
    float mana_y;
    // Score
    float score_x;
    float score_y;
    char score_text[15];
  } GUI;

  typedef struct {
    float gravity;
    float jump;
    int score;
    float distance;
    int animation_ongoing;
    float time;
  } World;

  typedef struct {
    float x_curr;
    float y_curr;
    float speed;
  } Star;

#endif
