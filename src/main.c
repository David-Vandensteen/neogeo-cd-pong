/* -----------------------------------------------

    Pong sample for Neo Geo CD with neocore kit

    @2023
    David Vandensteen
    Visuals by Grass

* ----------------------------------------------- */

#include <neocore.h>
#include "externs.h"
#include "sound.h"

#define SCREEN_X_MIN 0
#define SCREEN_X_MAX 320
#define SCREEN_Y_MIN 0
#define SCREEN_Y_MAX 224

#define RACQUET_SPEED 4
#define BALL_SPEED 3
#define IA_DIRECTION_MAX_TIMEOUT 25

typedef struct BallState {
  Vec2short position;
  short slope;
  enum Direction direction;
} BallState;

static Box upper_wall;
static Box lower_wall;

static GFX_Picture playfield;
static GFX_Picture_Physic racquet1;
static GFX_Picture_Physic racquet2;
static GFX_Picture_Physic ball;
static BallState ball_state;

static enum Direction ia_direction = NONE;
static int ia_direction_timeout = IA_DIRECTION_MAX_TIMEOUT;

static enum Game_state { GAME_WAITING, GAME_PLAYING } game_state = GAME_WAITING;

static BOOL joypad_0_is_start() {
  if (nc_joypad_is_start(0)) return true;
  return false;
}

static BOOL joypad_0_is_a() {
  if (nc_joypad_is_a(0)) return true;
  return false;
}

static void wait_game_start() {
  if (game_state == GAME_WAITING) {
    nc_init_log();
    nc_set_position_log(3, 5);
    nc_log("PRESS START");
    nc_pause(&joypad_0_is_start);
    game_state = GAME_PLAYING;
    nc_init_log();
  }
}

int main();

void play_sound() {
  if (nc_get_adpcm_player()->state == IDLE) {
    send_sound_command(ADPCM_STOP);
    send_sound_command(ADPCM_MIXKIT_GAME_CLICK_1114);
    nc_push_remaining_frame_adpcm_player(nc_second_to_frame(1));
  }
}

/*------------------------
  Function to get the next position of the ball based on its slope and direction.
    - Calculate the next position of the ball based on its current position, slope, direction, and speed,
    enabling realistic movement in the game.

    - It returns the updated position for the ball in the next step.
-------------------------*/

Vec2short get_next_ball_position(Vec2short current_position, short slope, enum Direction direction, int speed) {
  Vec2short next_position = {current_position.x, current_position.y};
  if (slope != 0 && current_position.x % slope == 0) next_position.y = (slope > 0) ? next_position.y + speed : next_position.y - speed;
  next_position.x = (direction == RIGHT) ? next_position.x + speed : next_position.x - speed;
  return next_position;
}

/*------------------------
  Function to initialize the state of the ball
    - Init the ball's state with an initial slope, direction, and position obtained from the "ball" object.
-------------------------*/

static void init_ball_state(BallState *ball_state) {
  ball_state->slope = -2;
  ball_state->direction = RIGHT;
  ball_state->position.x = nc_get_position_gfx_picture_physic(ball).x;
  ball_state->position.y = nc_get_position_gfx_picture_physic(ball).y;
}

/*------------------------
  Function to initialize the game
    - Init the game environment and elements, including the racquets, ball, and boundary walls, to prepare for the start of the game.
-------------------------*/

static void init() {
  nc_init_gfx_picture(&playfield, &playfield_asset, &playfield_asset_Palettes);
  nc_init_gfx_picture_physic(&racquet1, &racquet1_asset, &racquet1_asset_Palettes, 16, 64, 0, 0, AUTOBOX);
  nc_init_gfx_picture_physic(&racquet2, &racquet2_asset, &racquet2_asset_Palettes, 16, 64, 0, 0, AUTOBOX);
  nc_init_gfx_picture_physic(&ball, &ball_asset, &ball_asset_Palettes, 16, 16, 0, 0, AUTOBOX);

  nc_init_box(&upper_wall, 320, 16, 0, 0);
  nc_update_box(&upper_wall, 0, -16);

  nc_init_box(&lower_wall, 320, 16, 0, 0);
  nc_update_box(&lower_wall, 0, 224);

  nc_play_cdda(2);
}

/*------------------------
  Function to display the game
    - Show the game elements on the screen by using the "display_gfx_picture_physic" function to display the racquets and ball at specific positions.
    It also initializes the ball's state using the "init_ball_state" function.
-------------------------*/

static void display() {
  nc_display_gfx_picture(&playfield, 0, 0);
  nc_display_gfx_picture_physic(&racquet1, 16, 16);
  nc_display_gfx_picture_physic(&racquet2, 320 - 32, 16);
  nc_display_gfx_picture_physic(&ball, 50, 100);
  init_ball_state(&ball_state);
}

/*------------------------
  Function to update game logic
    - Check if the ball has hit the left or right edge of the game screen.
      If so, it displays the winner (either "PLAYER WINS" or "COMPUTER WINS")
        and prompts the player to press "A" to continue.
      The game then restarts by calling the "main" function.
-------------------------*/

static void update_logic() {
  if (ball_state.position.x >= SCREEN_X_MAX || ball_state.position.x <= SCREEN_X_MIN) {
    nc_init_log();
    nc_set_position_log(10, 10);
    if (ball_state.position.x >= SCREEN_X_MAX) nc_log_info("PLAYER WINS");
    if (ball_state.position.x <= SCREEN_X_MIN) nc_log_info("COMPUTER WINS");
    nc_log_info("");
    nc_log_info("PRESS A TO CONTINUE");
    nc_pause(&joypad_0_is_a);
    nc_init_log();
    main();
  }
}

/*------------------------
  Function to determine the ball's slope based on its position relative to the racquet
    - Calculate the slope of the ball's movement based on its position relative to a given racquet.
      It then adjusts the vertical movement of the ball using the calculated slope.
-------------------------*/

static void slope_ball(BallState *ball_state ,GFX_Picture_Physic racquet) {
  Vec2short ball_local_coord = nc_get_relative_position(racquet.box, ball_state->position);
  if (ball_local_coord.y >= 0 && ball_local_coord.y < 22) {
    ball_state->slope = 2;
  } else if (ball_local_coord.y >= 22 && ball_local_coord.y < 44) {
    switch (nc_random(3)) {
      case 0:
        ball_state->slope = -10;
        break;

      default:
      case 1:
        ball_state->slope = 0;
        break;

      case 3:
        ball_state->slope = 10;
        break;
    }
  } else if (ball_local_coord.y >= 44) ball_state->slope = -2;
}

/*------------------------
  Function to update the ball's position and handle collisions
    - Manage the ball's movement, collision detection, and position updates based on game logic and interactions with the game boundaries and racquets.
      It handles collisions with the upper and lower walls to reverse the vertical movement of the ball when necessary.
      Additionally, it adjusts the slope and direction of the ball if it collides with the player's racquet or the AI's racquet,
        and then calculates the next position of the ball
        using the updated slope and direction.
-------------------------*/

static void update_ball() {
  BOOL collide_player = nc_collide_box(&ball.box, &racquet1.box);
  BOOL collide_ia = nc_collide_box(&ball.box, &racquet2.box);
  BOOL collide_upper_wall = nc_collide_box(&upper_wall, &ball.box);
  BOOL collide_lower_wall = nc_collide_box(&lower_wall, &ball.box);
  Vec2short next_ball_position;

  ball_state.position = nc_get_position_gfx_picture_physic(ball);

  if (
      (collide_upper_wall && ball_state.slope < 0)
      || (collide_lower_wall && ball_state.slope > 0)
    ) ball_state.slope = -ball_state.slope;

  if (collide_player || collide_ia) {
    if (collide_player) {
      slope_ball(&ball_state, racquet1);
    } else {
      slope_ball(&ball_state, racquet2);
    }
    ball_state.direction = (collide_player) ? RIGHT : LEFT;
    play_sound();
  }

  next_ball_position = get_next_ball_position(ball_state.position, ball_state.slope, ball_state.direction, BALL_SPEED);
  nc_set_position_gfx_picture_physic(&ball, next_ball_position.x, next_ball_position.y);
}

/*------------------------
  Function to update player-controlled racquet
    - The "update_player" function allows the player to control their racquet using the joypad.
      It moves the racquet up or down based on the player's input,
        ensuring that the racquet stays within the vertical boundaries of the game area.
-------------------------*/

static void update_player() {
  if (nc_joypad_is_up(0) && nc_get_position_gfx_picture_physic(racquet1).y > SCREEN_Y_MIN) nc_move_gfx_picture_physic(&racquet1, 0, -RACQUET_SPEED);

  if (nc_joypad_is_down(0) && nc_get_position_gfx_picture_physic(racquet1).y + 64 < SCREEN_Y_MAX) {
    nc_move_gfx_picture_physic(&racquet1, 0, RACQUET_SPEED);
  }
}

/*------------------------
  Function to update AI-controlled racquet
    - Control the AI-controlled racquet's movement.
      It periodically observes the ball's position and adjusts its own position
        (up or down) to try to intercept the ball during the game.
-------------------------*/

static void update_ia() {
  if (ia_direction_timeout <= 0) {
    Vec2short ball_position = nc_get_position_gfx_picture_physic(ball);
    enum Direction ball_direction = NONE;
    if (ball_position.y < nc_get_position_gfx_picture_physic(racquet2).y) ball_direction = UP;
    if (ball_position.y > nc_get_position_gfx_picture_physic(racquet2).y) ball_direction = DOWN;
    ia_direction = ball_direction;
    ia_direction_timeout = nc_random(IA_DIRECTION_MAX_TIMEOUT);
  }

  if (ia_direction == UP && nc_get_position_gfx_picture_physic(racquet2).y > SCREEN_Y_MIN) nc_move_gfx_picture_physic(&racquet2, 0, -RACQUET_SPEED);

  if (ia_direction == DOWN && nc_get_position_gfx_picture_physic(racquet2).y < SCREEN_Y_MAX - racquet2.gfx_picture.pixel_height) {
    nc_move_gfx_picture_physic(&racquet2, 0, RACQUET_SPEED);
  }

  ia_direction_timeout -= 1;
}

/*------------------------
  Function to update the game state
    - This function is the main update loop of the game.
      It calls four sub-functions in sequence to handle different aspects of the game:
      - "update_logic": Handles game logic, such as checking for win conditions when the ball reaches the left or right edges of the screen.
      - "update_ball": Manages the movement and collisions of the ball during the game.
      - "update_player": Controls the player's racquet movement based on their input.
      - "update_ia": Controls the AI-controlled racquet's movement to intercept the ball.
    - In summary, the "update" function orchestrates the game's mechanics by updating
        game logic, ball movement, player racquet,
        and AI-controlled racquet positions in each game loop iteration
-------------------------*/

static void update() {
  update_logic();
  update_ball();
  update_player();
  update_ia();
}

/*------------------------
  Main function of the game
    - The "main" function is the entry point of the program and serves as the game loop. Here's a concise explanation of the function:
      - It first initializes the game environment and elements by calling the "init" function.
      - Then, it displays the initial game state by calling the "display" function.
      - The program enters a continuous loop (while loop) that represents the main game loop.
      - After waiting for the vertical blank, it updates the game state
        by calling the "update" function, which manages game logic and element movements.

      - The loop continues indefinitely due to the "while(1)" condition,
        keeping the game running.

  - In summary, the "main" function
    initializes the game, displays the initial state,
    and enters an infinite game loop where
    it updates the game state and keeps the game running until termination.
-------------------------*/

int main() {
  init();
  display();
  while(1) {
    nc_update();
    update();
    if (game_state != GAME_PLAYING) wait_game_start();
  };
  return 0;
}
