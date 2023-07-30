#include <neocore.h>
#include "externs.h"

// TODO : mak serve:raine neocore
// TODO : bug mame-profile in standalone mode
// TODO : mak raine
// todo : mak mame
// TODO : neocore refactor vec2... constructor
// TODO : neocore gpp to gfx_picture_physic ect ...
// TODO : neocore unit test

#define SCREEN_X_MIN 0
#define SCREEN_X_MAX 320
#define SCREEN_Y_MIN 0
#define SCREEN_Y_MAX 224

#define RACQUET_SPEED 4
#define BALL_SPEED 3
#define IA_DIRECTION_MAX_TIMEOUT 25

// todo : neocore expose direction ?
enum direction { NONE, UP, DOWN, LEFT, RIGHT };

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

static enum direction ia_direction = NONE;
static int ia_direction_timeout = IA_DIRECTION_MAX_TIMEOUT;

// patch neocore
// TODO get_negative_short
// TODO get_invert_short
short get_positive_short(short num) {
  if (num < 0) num = ~num + 1;
  return num;
}

BOOL each_frame(DWORD frame) {
  return (get_frame_counter() % frame == 0) ? true : false;
}

Vec2short get_position_gfx_picture_physic(GFX_Picture_Physic gfx_picture_physic) {
  Vec2short position = {get_x_gpp(gfx_picture_physic), get_y_gpp(gfx_picture_physic)};
  return position;
}

void set_position_gfx_picture_physic(GFX_Picture_Physic *gfx_picture_physic, Vec2short position) {
  set_pos_gpp(gfx_picture_physic, position.x, position.y);
}

Vec2short get_next_ball_position(Vec2short current_position, short slope, enum direction direction, int speed) {
  Vec2short next_position = {current_position.x, current_position.y};
  if (slope != 0 && current_position.x % slope == 0) next_position.y = (slope > 0) ? next_position.y + speed : next_position.y - speed;
  next_position.x = (direction == RIGHT) ? next_position.x + speed : next_position.x - speed;
  return next_position;
}

void pause() {
  update_joypad(0);
  while( !joypad_is_a(0)) {
    update_joypad(0);
    wait_vbl();
  }
}

void sleep(DWORD frame) {
  wait_vbl_max(frame);
}
//

int main();

static Vec2short get_relative_position(Box box, Vec2short world_coord) {
  Vec2short coord = {
    world_coord.x - box.p0.x,
    get_positive_short(world_coord.y - box.p3.y)
  };
  return coord;
}

static void init_racquet(GFX_Picture_Physic *racquet) {
  init_gpp(racquet, &racquet_asset, &racquet_asset_Palettes, 16, 64, 0, 0, AUTOBOX);

  // --------------------------------------------------------neocore patch
  racquet->gfx_picture.pixel_height = racquet->gfx_picture.pixel_height - 64;
  // --------------width patch ?
}

static void init_ball_state(BallState *ball_state) {
  ball_state->slope = -2;
  ball_state->direction = RIGHT;
  ball_state->position = get_position_gfx_picture_physic(ball);
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
  if (ball_state.position.x >= SCREEN_X_MAX || ball_state.position.x <= SCREEN_X_MIN) {
    init_log();
    set_pos_log(10, 10);
    if (ball_state.position.x >= SCREEN_X_MAX) log_info("PLAYER WINS");
    if (ball_state.position.x <= SCREEN_X_MIN) log_info("COMPUTER WINS");
    log_info("");
    log_info("PRESS A TO CONTINUE");
    pause();
    main();
  }
}

static void slope_ball(BallState *ball_state ,GFX_Picture_Physic racquet) {
  Vec2short ball_local_coord = get_relative_position(racquet.box, ball_state->position);
  if (ball_local_coord.y >= 0 && ball_local_coord.y < 22) ball_state->slope = 2;
  if (ball_local_coord.y >= 22 && ball_local_coord.y < 44) ball_state->slope = 0;
  if (ball_local_coord.y >= 44) ball_state->slope = -2;
}

static void update_ball() {
  BOOL collide_player = collide_box(&ball.box, &racquet[0].box);
  BOOL collide_ia = collide_box(&ball.box, &racquet[1].box);
  BOOL collide_upper_wall = collide_box(&upper_wall, &ball.box);
  BOOL collide_lower_wall = collide_box(&lower_wall, &ball.box);

  ball_state.position = get_position_gfx_picture_physic(ball);

  if (
      (collide_upper_wall && ball_state.slope < 0)
      || (collide_lower_wall && ball_state.slope > 0)
    ) ball_state.slope = ball_state.slope * -1;

  init_log();

  if (collide_player || ball_state.position.x <= SCREEN_X_MIN) {
    slope_ball(&ball_state, racquet[0]);
    ball_state.direction = RIGHT;
  }

  if (collide_ia || ball_state.position.x >= SCREEN_X_MAX) {
    slope_ball(&ball_state, racquet[1]);
    ball_state.direction = LEFT;
  }

  set_position_gfx_picture_physic(
    &ball,
    get_next_ball_position(
      ball_state.position,
      ball_state.slope,
      ball_state.direction,
      BALL_SPEED
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
    Vec2short ball_position = get_position_gfx_picture_physic(ball);
    enum direction ball_direction = NONE;
    if (ball_position.y < get_y_gpp(racquet[1])) ball_direction = UP;
    if (ball_position.y > get_y_gpp(racquet[1])) ball_direction = DOWN;
    ia_direction = ball_direction;
    ia_direction_timeout = get_random(IA_DIRECTION_MAX_TIMEOUT);
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
