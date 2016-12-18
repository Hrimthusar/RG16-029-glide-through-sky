#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

//==============================================================================

#include "glut_functions.h"

#include "player.h"
#include "player_externs.h"

#include "wall.h"
#include "wall_externs.h"

#include "world.h"
#include "world_externs.h"

#include "mana.h"
#include "mana_externs.h"

#include "enemy.h"
#include "enemy_externs.h"

#include "bullet.h"
#include "bullet_externs.h"

//==============================================================================

int main(int argc, char* argv[]){

  glutInit(&argc,argv);

  glutInitWindowSize(window_width,window_height);
  glutInitWindowPosition(100,100);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Glide Through Sky");

  glClearColor(0.01, 0.01, 0.02, 0);
  glEnable(GL_DEPTH_TEST);

  glutKeyboardFunc(on_keyboard);
  glutReshapeFunc(on_reshape);
  glutDisplayFunc(on_display);

  dt = glutGet(GLUT_ELAPSED_TIME);

  srand(time(NULL));

  bullets_init();
  stars_init();
  score_init();
  wall_init();

  // TODO player init //

  // int global_colors_number = 8;
  // Color3f global_colors[] = {{1,0,1},{0,1,0},{1,1,0},{1,.2,.2},{.5,0,1},{0,1,.6},{0,0,1},{0,1,1}};

  player.colors = global_colors[(int)(rand()/(float)RAND_MAX * global_colors_number)];
  //////////////////////

  // TODO trails init ////////////////////
  trails[0].pos_x = player.curr_x;
  trails[0].pos_y = 0;
  trails[0].pos_z = -.4;
  trails[0].size = player.size * .3;
  trails[0].colors = trail_color_alpha;

  int i;
  for(i=1; i<TRAIL_MAX; i++){
    trails[i].pos_x = trails[i-1].pos_x - trail_x_move;
    // printf("%f\n", trails[i].pos_x);
    trails[i].pos_y = 0;
    trails[i].pos_z = trails[i-1].pos_z + .001;
    trails[i].colors = trails[i-1].colors - trail_color_alpha/TRAIL_MAX;
  }
  //////////////////////////////////////////


  glutMainLoop();

  return 0;
}

static void on_keyboard(unsigned char key, int x, int y) {
    switch (key) {
      case 27:
        exit(0);
        break;

      case 'g':
      case 'G':
        if (!world.animation_ongoing) {
            glutTimerFunc(PLAYER_REFRESH_TIMER_INTERVAL, on_timer, PLAYER_REFRESH_TIMER_ID);
            glutTimerFunc(TRAIL_TIMER_INTERVAL, on_timer, TRAIL_TIMER_ID);
            world.animation_ongoing = 1;
        }
        break;

      case 's':
      case 'S':
        world.animation_ongoing = 0;
        break;

      case 'q':
      case 'Q':
        if(!player.dashing)
          player.v_y = world.jump;
        break;

      // TODO vidi da ubacis i glutTimerFunc u dash_start
      case 'w':
      case 'W':
        if(!player.dashing && player.mana > 0){
          glutTimerFunc(DASH_TIMER_INTERVAL, on_timer, DASH_TIMER_ID);
          dash_start();
        }
        break;
      ////////////////////////////////////////////////////////

      case 'e':
      case 'E':
        
        if(player.mana >0 && player.hp < 100){
          player.mana--;
          if(player.hp + 30 > 100)
            player.hp = 100;
          else
            player.hp += 30; 
        }
        break;
    }
}

static void on_reshape(int width, int height){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  window_height = height;
  window_width = width;

  int left = (width - height) / 2;
  glViewport(left, 0, height, height);
  gluPerspective(60.0, 1, 0.1, 100.0);
  glutPostRedisplay();
}

static void on_timer(int value){
    int i,j;
    // TODO mozda stavi preko distance, nema potrebe za tajmerom
    if(value == TRAIL_TIMER_ID){
      summon_trail();
      if (world.animation_ongoing) {
        glutTimerFunc(TRAIL_TIMER_INTERVAL, on_timer, TRAIL_TIMER_ID);
      }
    }
    //////////////////////////////////////////////////


    if(value == PLAYER_REFRESH_TIMER_ID){

      // Correction for different performances
      updateDeltaTime();

      speed_correction = dt / (float)PLAYER_REFRESH_TIMER_INTERVAL;
      float ms = wall_speed * speed_correction;
      
      world.time += ms;
      if(mana_init<3){
        mana_init_time+=ms;
        if(mana_init_time > 30*ms){
          mana_init++;
          player.mana++;
          mana_init_time-= 30*ms;
        }
      }
      if(world.distance>=1.5){
        
        world.distance-=1.3;
        summon_wall();
        summon_enemy(); 
        for(i=0;i<WALL_COUNT;i++){
         fire(i);
         
        }
        if((walls_passed_counter = (walls_passed_counter+1)%4) == 0)
          summon_mana();
      }


      player_move();
      world.distance += ms;
      
      for(i=0;i<STAR_X_NUMBER;i++){
        for(j=0;j<STAR_Y_NUMBER;j++){
          stars[i][j].curr_x -= stars[i][j].speed;
          if(stars[i][j].curr_x<-2)
            stars[i][j].curr_x=2;

        }  
      }
      mana_collision();
      bullets_player_collision();
      bullets_walls_turrets_collision();
      for(i=0;i<WALL_COUNT;i++){
        wall_collision(i);
        enemy_collision(i);
        if(enemies[i].alive == DYING)
          enemies[i].dying_time += .007*speed_correction;
        walls[i].curr_x -= ms;
        enemies[i].curr_x -= ms;
        check_score(i);
      }


      mana_crystal_rotation += speed_correction * 1.8;
      if (world.animation_ongoing) {
        crystal.curr_x -= ms;
      }
      crystal.curr_y = sin(mana_crystal_rotation*.02)*.08;

      aim();

      for(i=0;i<WALL_COUNT;i++){
        for(j=0;j<BULLET_COUNT;j++){
          if(!bullets[i][j].alive)
            continue;
          bullets[i][j].curr_x-=ms;
          bullets[i][j].curr_x+=bullets[i][j].v_x*.025;
          bullets[i][j].curr_y+=bullets[i][j].v_y*.025;
        }
      }

      glutPostRedisplay();

      if (world.animation_ongoing) {
          glutTimerFunc(PLAYER_REFRESH_TIMER_INTERVAL, on_timer, PLAYER_REFRESH_TIMER_ID);
      }
    }

    if(value == DASH_TIMER_ID){
      if(player.dashing){
        player.dashing = 0;
      }
      dash();
    }
}

// float global_transform_matrix[16];

void on_display(){
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  // fps(1);

  /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
  GLfloat light_position[] = { 0, 0, 2.45, 1 };

  GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };
  GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
  GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

  GLfloat ambient_coeffs[] = { 0.3, 0.3, 0.3, 1 };
  GLfloat specular_coeffs[] = { 0.5, 0.5, 0.5, 1 };

  /* Koeficijent glatkosti materijala. */
  GLfloat shininess = 100;

  /* Podesava se vidna tacka. */
  gluLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0);

  glEnable(GL_NORMALIZE);
  /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);


  /* Podesavaju se parametri materijala. */
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);

  glMatrixMode(GL_MODELVIEW);

  // glGetFloatv(GL_MODELVIEW_MATRIX, global_transform_matrix);

  glLoadIdentity();

  draw_world();

  int i,j;
  for(i=0;i<WALL_COUNT;i++){ // sigurno je manje kristala od zidova (ili jednako)
    draw_mana_crystal(i);

    draw_wall(walls[i].curr_x,i);
    draw_enemy(i);
    for(j=0;j<BULLET_COUNT;j++)
      draw_bullet(i,j);
  }

  teleport();

  if(player.dashing){
    dash();
  }

  draw_mana_circle(player.mana);
  draw_player(player.curr_x, player.curr_y, player.colors.R, player.colors.G, player.colors.B); // FIXME jos konstanti

  for(i=0;i<trail_count;i++){
    draw_trail(trails[i].pos_x, trails[i].pos_y, // TODO crtaj trouglove, opacity, itd
      player.colors.R * trails[i].colors,
      player.colors.G * trails[i].colors,
      player.colors.B * trails[i].colors,
      trails[i].size);
  }

// TODO Svasta sa ispisom teksta, pocevsi od plate-a pa na dalje
  float text_x = -.93;
  float text_y = .82;
  float text_z = .3;
  RenderString(text_x, text_y, GLUT_BITMAP_HELVETICA_18, gui.score_text, 0.6, 0.6, 0.6);

  glFlush();
  glutSwapBuffers();
}

void RenderString(float x, float y, void *font, const char* string, float r, float g, float b) {
  GLfloat emission_coeffs[] = { r, g, b, 1 };
  glMaterialfv(GL_FRONT, GL_EMISSION, emission_coeffs);
  glRasterPos3f(x, y, .3);
  glutBitmapString(font, string);
  emission_coeffs[0] = 0;
  emission_coeffs[1] = 0;
  emission_coeffs[2] = 0;
  emission_coeffs[3] = 1;
  glMaterialfv(GL_FRONT, GL_EMISSION, emission_coeffs);
}

#define DT_MAX 60
static int newTime;
static int oldTime = 0;
static int timeSum = 0;

void updateDeltaTime() {
    newTime = glutGet(GLUT_ELAPSED_TIME);
    // printf("TIME: %f\n",newTime);
    dt = newTime - oldTime;
    // printf("dt: %f\n");
    oldTime = newTime;
    timeSum += dt;
    if (dt>DT_MAX)
        dt=DT_MAX;
}

void fps(int print) {
    static int frame = 0;
    frame++;
    if (print && timeSum >= 1000){
        printf("fps:%f\n",frame*1000/(float)timeSum);
        timeSum = 0;
        frame = 0;
    }
}
