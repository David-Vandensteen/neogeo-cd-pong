#include <neocore.h>
#include "externs.h"

// TODO : mak serve:raine neocore
// TODO : bug mame-profile in standalone mode
// TODO : mak raine
// todo : mak mame
// TODO : neocore refactor vec2... constructor

#define SCREEN_X_MIN 0
#define SCREEN_X_MAX 320
#define SCREEN_Y_MIN 0
#define SCREEN_Y_MAX 224

#define RACQUET_SPEED 4
#define BALL_SPEED 2
#define IA_DIRECTION_TIMEOUT 10

// todo : neocore expose direction ?
enum direction { IDLE, UP, DOWN, LEFT, RIGHT };

typedef struct BallState {
  Vec2short position;
  short slope;
  enum direction direction;
} BallState;

static Box upper_wall;
static Box lower_wall;

static GFX_Picture_Physic racquet[2];
static GFX_Picture_Physic ball;
static BallState ball_state;

static enum direction ia_direction = IDLE;
static int ia_direction_timeout = IA_DIRECTION_TIMEOUT;

// patch neocore
BOOL each_frame(DWORD frame) {
  return (get_frame_counter() % frame == 0) ? true : false;
}

Vec2short get_vec2_pos_gpp (GFX_Picture_Physic gfx_picture_physic) {
  Vec2short pos = {get_x_gpp(gfx_picture_physic), get_y_gpp(gfx_picture_physic)};
  return pos;
}

void set_vec2_pos_gpp(GFX_Picture_Physic *gfx_picture_physic, Vec2short pos) {
  set_pos_gpp(gfx_picture_physic, pos.x, pos.y);
}

Vec2short get_next_pos_slope(Vec2short current_pos, short slope, enum direction direction) {
  Vec2short next_pos = {current_pos.x, current_pos.y};
  if (slope != 0 && current_pos.x % slope == 0) next_pos.y = (slope > 0) ? next_pos.y + 1 : next_pos.y - 1;
  next_pos.x = (direction == RIGHT) ? next_pos.x + 1 : next_pos.x - 1;
  return next_pos;
}
//

int main();

static void init_racquet(GFX_Picture_Physic *racquet) {
  init_gpp(racquet, &racquet_asset, &racquet_asset_Palettes, 16, 64, 0, 0, AUTOBOX);

  // --------------------------------------------------------neocore patch
  racquet->gfx_picture.pixel_height = racquet->gfx_picture.pixel_height - 64;
  // --------------width patch ?
}

static void init_ball_state(BallState *ball_state) {
  ball_state->slope = -5;
  ball_state->direction = RIGHT;
  ball_state->position = get_vec2_pos_gpp(ball);
}

static void init() {
  init_gpu();
  init_racquet(&racquet[0]);
  init_racquet(&racquet[1]);
  init_gpp(&ball, &ball_asset, &ball_asset_Palettes, 16, 16, 0, 0, AUTOBOX); // TODO : neocore : AUTOBOX not working

  init_box(&upper_wall, 320, 16, 0, 0);
  update_box(&upper_wall, 0, -16);

  init_box(&lower_wall, 320, 16, 0, 0);
  update_box(&lower_wall, 0, 224);
}

static void display() {
  display_gpp(&racquet[0], 16, 16);
  display_gpp(&racquet[1], 320 - 32, 16);
  display_gpp(&ball, 10, 100);
  init_ball_state(&ball_state);
}

static void update_logic() {
  // init_log();
  // log_short("X", ball_state.position.x);
  // log_short("Y", ball_state.position.y);

  if (ball_state.position.x >= SCREEN_X_MAX || ball_state.position.y <= SCREEN_X_MIN) {
    init_log();
    if (ball_state.position.x >= SCREEN_X_MAX) log_info("PLAYER WINS");
    if (ball_state.position.x <= SCREEN_X_MIN) log_info("COMPUTER WINS");
    wait_vbl_max(500);
    main();
  }
}

static void update_ball() {
  BOOL collide_player = collide_box(&ball.box, &racquet[0].box);
  BOOL collide_ia = collide_box(&ball.box, &racquet[1].box);
  BOOL collide_upper_wall = collide_box(&upper_wall, &ball.box);
  BOOL collide_lower_wall = collide_box(&lower_wall, &ball.box);

  ball_state.position = get_vec2_pos_gpp(ball);

  if (
      (collide_upper_wall && ball_state.slope < 0)
      || (collide_lower_wall && ball_state.slope > 0)
    ) ball_state.slope = ball_state.slope * -1;

  if (collide_player || ball_state.position.x <= SCREEN_X_MIN) {
    ball_state.direction = RIGHT;
    ball_state.slope = (short)get_random(10);
  }

  if (collide_ia || ball_state.position.x >= SCREEN_X_MAX) {
    ball_state.direction = LEFT;
  }

  set_vec2_pos_gpp(
    &ball,
    get_next_pos_slope(
      ball_state.position,
      ball_state.slope,
      ball_state.direction
    )
  );
}

static void update_player() {
  update_joypad(0);
  if (joypad_is_up(0) && get_y_gpp(racquet[0]) > SCREEN_Y_MIN) move_gpp(&racquet[0], 0, -RACQUET_SPEED);

  if (joypad_is_down(0) && get_y_gpp(racquet[0]) < SCREEN_Y_MAX - racquet[0].gfx_picture.pixel_height) {
    move_gpp(&racquet[0], 0, RACQUET_SPEED);
  }
}

static void update_ia() {
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
  update_logic();
  update_ball();
  update_player();
  update_ia();
}

int main() {
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
