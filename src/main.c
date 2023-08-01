#include <neocore.h>
#include "externs.h"

/* -----------------------------------------------

    Pong sample for Neo Geo CD over neocore kit

    @2023
    David Vandensteen

* ----------------------------------------------- */

// TODO : mak serve:raine neocore
// TODO : bug mame-profile in standalone mode
// TODO : mak raine
// todo : mak mame
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

int main();

/*------------------------

  Function to get the next position of the ball based on its slope and direction

-------------------------*/
Vec2short get_next_ball_position(Vec2short current_position, short slope, enum direction direction, int speed) {
  Vec2short next_position = {current_position.x, current_position.y};
  if (slope != 0 && current_position.x % slope == 0) next_position.y = (slope > 0) ? next_position.y + speed : next_position.y - speed;
  next_position.x = (direction == RIGHT) ? next_position.x + speed : next_position.x - speed;
  return next_position;
}

/*------------------------

  Function to initialize a racquet

-------------------------*/

static void init_racquet(GFX_Picture_Physic *racquet) {
  init_gfx_picture_physic(racquet, &racquet_asset, &racquet_asset_Palettes, 16, 64, 0, 0, AUTOBOX);

  // --------------------------------------------------------neocore patch
  racquet->gfx_picture.pixel_height = racquet->gfx_picture.pixel_height - 64;
  // --------------width patch ?
}

/*------------------------

  Function to initialize the state of the ball

-------------------------*/

static void init_ball_state(BallState *ball_state) {
  ball_state->slope = -2;
  ball_state->direction = RIGHT;
  ball_state->position = get_position_gfx_picture_physic(ball);
}

/*------------------------

  Function to initialize the game

-------------------------*/

static void init() {
  init_gpu();
  init_racquet(&racquet[0]);
  init_racquet(&racquet[1]);
  init_gfx_picture_physic(&ball, &ball_asset, &ball_asset_Palettes, 16, 16, 0, 0, AUTOBOX); // TODO : neocore : AUTOBOX not working

  init_box(&upper_wall, 320, 16, 0, 0);
  update_box(&upper_wall, 0, -16);

  init_box(&lower_wall, 320, 16, 0, 0);
  update_box(&lower_wall, 0, 224);
}

/*------------------------

  Function to display the game

-------------------------*/

static void display() {
  display_gfx_picture_physic(&racquet[0], 16, 16);
  display_gfx_picture_physic(&racquet[1], 320 - 32, 16);
  display_gfx_picture_physic(&ball, 50, 100);
  init_ball_state(&ball_state);
}

/*------------------------

  Function to update game logic

-------------------------*/

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

/*------------------------

  Function to determine the ball's slope based on its position relative to the racquet

-------------------------*/

static void slope_ball(BallState *ball_state ,GFX_Picture_Physic racquet) {
  Vec2short ball_local_coord = get_relative_position(racquet.box, ball_state->position);
  if (ball_local_coord.y >= 0 && ball_local_coord.y < 22) {
    ball_state->slope = 2;
  } else if (ball_local_coord.y >= 22 && ball_local_coord.y < 44) {
    ball_state->slope = 0;
  } else if (ball_local_coord.y >= 44) ball_state->slope = -2;
}

/*------------------------

  Function to update the ball's position and handle collisions

-------------------------*/

/*
static void update_ball() {
  Vec2short next_position = get_next_ball_position(
    ball_state.position,
    ball_state.slope,
    ball_state.direction,
    BALL_SPEED
  );

  BOOL collide_player = collide_box(&ball.box, &racquet[0].box);
  BOOL collide_ia = collide_box(&ball.box, &racquet[1].box);
  BOOL collide_upper_wall = collide_box(&upper_wall, &ball.box);
  BOOL collide_lower_wall = collide_box(&lower_wall, &ball.box);

  if (collide_upper_wall || collide_lower_wall) {
    ball_state.slope = -ball_state.slope;
  }

  if (collide_player || collide_ia) {
    ball_state.slope = -ball_state.slope;
    ball_state.direction = (collide_player) ? RIGHT : LEFT;
  }

  // set_position_gfx_picture_physic(&ball, next_position);
  init_log();
  log_short("CUR X", ball_state.position.x);
  log_short("CUR Y", ball_state.position.y);
  log_short("NEXT X", next_position.x);
  log_short("NEXT Y", next_position.y);
  set_position_gfx_picture_physic_patched(&ball, next_position);
}
*/

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

/*------------------------

  Function to update player-controlled racquet

-------------------------*/

static void update_player() {
  update_joypad(0);
  if (joypad_is_up(0) && get_y_gpp(racquet[0]) > SCREEN_Y_MIN) move_gpp(&racquet[0], 0, -RACQUET_SPEED);

  if (joypad_is_down(0) && get_y_gpp(racquet[0]) < SCREEN_Y_MAX - racquet[0].gfx_picture.pixel_height) {
    move_gpp(&racquet[0], 0, RACQUET_SPEED);
  }
}

/*------------------------

  Function to update AI-controlled racquet

-------------------------*/

static void update_ia() {
  if (ia_direction_timeout <= 0) {
    Vec2short ball_position = get_position_gfx_picture_physic(ball);
    enum direction ball_direction = NONE;
    if (ball_position.y < get_y_gfx_picture_physic(racquet[1])) ball_direction = UP;
    if (ball_position.y > get_y_gfx_picture_physic(racquet[1])) ball_direction = DOWN;
    ia_direction = ball_direction;
    ia_direction_timeout = get_random(IA_DIRECTION_MAX_TIMEOUT);
  }

  if (ia_direction == UP && get_y_gfx_picture_physic(racquet[1]) > SCREEN_Y_MIN) move_gfx_picture_physic(&racquet[1], 0, -RACQUET_SPEED);

  if (ia_direction == DOWN && get_y_gfx_picture_physic(racquet[1]) < SCREEN_Y_MAX - racquet[1].gfx_picture.pixel_height) {
    move_gfx_picture_physic(&racquet[1], 0, RACQUET_SPEED);
  }

  ia_direction_timeout -= 1;
}

/*------------------------

  Function to update the game state

-------------------------*/

static void update() {
  update_logic();
  update_ball();
  update_player();
  update_ia();
}

/*------------------------

  Main function of the game

-------------------------*/

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
