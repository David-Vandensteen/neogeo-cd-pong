#include <neocore.h>
#include "externs.h"

// TODO : mak serve:raine neocore

#define SCREEN_Y_MIN 0
#define SCREEN_Y_MAX 256

#define RACQUET_SPEED 4
#define BALL_SPEED 2
#define IA_DIRECTION_TIMEOUT 10

enum direction { IDLE, UP, DOWN };

static GFX_Picture_Physic racquet[2];
static GFX_Picture_Physic ball;

static enum direction ia_direction = IDLE;
static int ia_direction_timeout = IA_DIRECTION_TIMEOUT;

static int ball_direction = 0;
static short ball_x_move = -1;
static short ball_y_move = 0;
static int ball_y_move_frequency = 10;

static void init_racquet(GFX_Picture_Physic *racquet) {
  init_gpp(racquet, &racquet_asset, &racquet_asset_Palettes, 16, 64, 0, 0, AUTOBOX);

  // --------------------------------------------------------neocore patch
  racquet->gfx_picture.pixel_height = racquet->gfx_picture.pixel_height - 28;
  // --------------------------------------------------width patch ?
}

static void init() {
  init_gpu();
  init_racquet(&racquet[0]);
  init_racquet(&racquet[1]);
  init_gpp(&ball, &ball_asset, &ball_asset_Palettes, 16, 16, 0, 0, AUTOBOX); // TODO : neocore : AUTOBOX not working
}

static void display() {
  display_gpp(&racquet[0], 16, 16);
  display_gpp(&racquet[1], 320 - 32, 16);
  display_gpp(&ball, 250, 100);
}

static void ball_update() {
  if (collide_box(&racquet[0].box, &ball.box)) {
    ball_direction = get_random(3);
    ball_y_move_frequency = get_random(10);
    ball_x_move = BALL_SPEED;
    if (ball_direction == 0) ball_y_move = -1;
    if (ball_direction == 1) ball_y_move = 0;
    if (ball_direction == 2) ball_y_move = 1;
  }
  if (collide_box(&racquet[1].box, &ball.box)) ball_x_move = -BALL_SPEED;
  if (get_frame_counter() % ball_y_move_frequency == 0) {
    move_gpp(&ball, ball_x_move, ball_y_move);
  } else {
    move_gpp(&ball, ball_x_move, 0);
  }
}

static void player_update() {
  update_joypad(0);
  if (joypad_is_up(0) && get_y_gpp(racquet[0]) > SCREEN_Y_MIN) move_gpp(&racquet[0], 0, -RACQUET_SPEED);

  if (joypad_is_down(0) && get_y_gpp(racquet[0]) < SCREEN_Y_MAX - racquet[0].gfx_picture.pixel_height) {
    move_gpp(&racquet[0], 0, RACQUET_SPEED);
  }
}

static void ia_update() {
  if (ia_direction_timeout <= 0) {
    ia_direction = get_random(3);
    ia_direction_timeout = IA_DIRECTION_TIMEOUT;
  }
  if (ia_direction == UP && get_y_gpp(racquet[1]) > SCREEN_Y_MIN) move_gpp(&racquet[1], 0, -RACQUET_SPEED);

  if (ia_direction == DOWN && get_y_gpp(racquet[1]) < SCREEN_Y_MAX - racquet[1].gfx_picture.pixel_height) {
    move_gpp(&racquet[1], 0, RACQUET_SPEED);
  }
  ia_direction_timeout -= 1;
}

static void update() {
  ball_update();
  player_update();
  ia_update();
}

int main(void) {
  init();
  display();
  while(1) {
    wait_vbl();
    update();
    close_vbl();
  };
  close_vbl();
  return 0;
}
