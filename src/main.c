#include <neocore.h>
#include "externs.h"

// TODO : mak serve:raine neocore
// TODO : bug mame-profile in standalone mode
// TODO : mak raine
// todo : mak mame

#define SCREEN_X_MIN 0
#define SCREEN_X_MAX 320
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

// patch neocore
BOOL each_frame(DWORD frame) {
  return (get_frame_counter() % frame == 0) ? true : false;
  // if (get_frame_counter() % frame == 0) return true;
  // return false;
}

Vec2short get_vec2_pos_gpp (GFX_Picture_Physic gfx_picture_physic) {
  Vec2short pos = {get_x_gpp(gfx_picture_physic), get_y_gpp(gfx_picture_physic)};
  return pos;
}

void set_vec2_pos_gpp(GFX_Picture_Physic *gfx_picture_physic, Vec2short pos) {
  set_pos_gpp(gfx_picture_physic, pos.x, pos.y);
}

Vec2short get_next_pos_slope(Vec2short current_pos, Vec2short destination_limit_pos, short slope) {
  Vec2short next_pos = {current_pos.x, current_pos.y};
  short normalized_slope = slope;
  if (current_pos.x >= destination_limit_pos.x || current_pos.y >= destination_limit_pos.y) return current_pos;
  if (slope == 0) normalized_slope = 1;
  if (current_pos.x % normalized_slope == 0) {
    if (normalized_slope > 0) {
      next_pos.y += 1;
    } else {
      next_pos.y -= 1;
    }
  }
  next_pos.x += 1;
  return next_pos;
}
//

static void init_racquet(GFX_Picture_Physic *racquet) {
  init_gpp(racquet, &racquet_asset, &racquet_asset_Palettes, 16, 64, 0, 0, AUTOBOX);

  // --------------------------------------------------------neocore patch
  racquet->gfx_picture.pixel_height = racquet->gfx_picture.pixel_height - 28;
  // --------------width patch ?
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
  display_gpp(&ball, 10, 10);
}

static void ball_update() {
  Vec2short ball_next_position;
  Vec2short ball_destination_limit = {SCREEN_X_MAX, SCREEN_Y_MAX};

  init_log();

  log_short("x", ball_next_position.x);
  log_short("y", ball_next_position.y);

  set_vec2_pos_gpp(
    &ball,
    get_next_pos_slope(get_vec2_pos_gpp(ball), ball_destination_limit, -10)
  );
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
