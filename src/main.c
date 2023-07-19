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
#define SCREEN_Y_MAX 256

#define RACQUET_SPEED 4
#define BALL_SPEED 2
#define IA_DIRECTION_TIMEOUT 10

// patch neocore
typedef struct QuadShort {
  Vec2short vector0;
  Vec2short vector1;
} QuadShort;
//

// todo : neocore expose direction ?
enum direction { IDLE, UP, DOWN, LEFT, RIGHT };

typedef struct BallState {
  QuadShort border_limit;
  Vec2short destination;
  Vec2short position;
  short slope;
  enum direction direction;
} BallState;


static GFX_Picture_Physic racquet[2];
static GFX_Picture_Physic ball;
static BallState ball_state;

static enum direction ia_direction = IDLE;
static int ia_direction_timeout = IA_DIRECTION_TIMEOUT;

// patch neocore
BOOL each_frame(DWORD frame) {
  return (get_frame_counter() % frame == 0) ? true : false;
}

// todo : deprecated neocore box ?
QuadShort quad_short(short p0, short p1, short p2, short p3) {
  Vec2short v0 = {p0, p1};
  Vec2short v1 = {p2, p3};
  QuadShort quad;
  quad.vector0 = v0;
  quad.vector1 = v1;
  return quad;
}

Vec2short get_vec2_pos_gpp (GFX_Picture_Physic gfx_picture_physic) {
  Vec2short pos = {get_x_gpp(gfx_picture_physic), get_y_gpp(gfx_picture_physic)};
  return pos;
}

void set_vec2_pos_gpp(GFX_Picture_Physic *gfx_picture_physic, Vec2short pos) {
  set_pos_gpp(gfx_picture_physic, pos.x, pos.y);
}

Vec2short get_next_pos_slope(Vec2short current_pos, Vec2short destination_limit_pos, short slope, enum direction direction) {
  Vec2short next_pos = {current_pos.x, current_pos.y};

  if (direction == RIGHT) {
    if (current_pos.x >= destination_limit_pos.x || current_pos.y >= destination_limit_pos.y) return current_pos;
  } else if (direction == LEFT) {
    if (current_pos.x <= destination_limit_pos.x || current_pos.y <= destination_limit_pos.y) return current_pos;
  }

  if (slope != 0 && current_pos.x % slope == 0) next_pos.y = (slope > 0) ? next_pos.y + 1 : next_pos.y - 1;
  next_pos.x = (direction == RIGHT) ? next_pos.x + 1 : next_pos.x - 1;
  return next_pos;
}
//

static void init_racquet(GFX_Picture_Physic *racquet) {
  init_gpp(racquet, &racquet_asset, &racquet_asset_Palettes, 16, 64, 0, 0, AUTOBOX);

  // --------------------------------------------------------neocore patch
  racquet->gfx_picture.pixel_height = racquet->gfx_picture.pixel_height - 28;
  // --------------width patch ?
}

static void init_ball_state(BallState *ball_state) {
  ball_state->slope = 0;
  ball_state->direction = RIGHT;
  ball_state->border_limit = quad_short(SCREEN_X_MIN, SCREEN_Y_MIN, SCREEN_X_MAX - 32, SCREEN_Y_MAX);
}

static void init() {
  init_gpu();
  init_racquet(&racquet[0]);
  init_racquet(&racquet[1]);
  init_gpp(&ball, &ball_asset, &ball_asset_Palettes, 16, 16, 0, 0, AUTOBOX); // TODO : neocore : AUTOBOX not working
  init_ball_state(&ball_state);
}

static void display() {
  display_gpp(&racquet[0], 16, 16);
  display_gpp(&racquet[1], 320 - 32, 16);
  display_gpp(&ball, 0, 10);
}

static void update_ball() {
  ball_state.position = get_vec2_pos_gpp(ball);

  if (ball_state.direction == RIGHT) ball_state.destination = ball_state.border_limit.vector1;
  if (ball_state.direction == LEFT) ball_state.destination = ball_state.border_limit.vector0;

  init_log();

  if (ball_state.position.x <= ball_state.border_limit.vector0.x) {
    ball_state.direction = RIGHT;
  }

  if (ball_state.position.x >= ball_state.border_limit.vector1.x) {
    ball_state.direction = LEFT;
  }

  set_vec2_pos_gpp(
    &ball,
    get_next_pos_slope(
      ball_state.position,
      ball_state.destination,
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
  update_ball();
  update_player();
  update_ia();
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
