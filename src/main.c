#include <neocore.h>
#include "externs.h"

#define SCREEN_Y_MIN 0
#define SCREEN_Y_MAX 256

#define RACQUET_SPEED 3
#define IA_STATE_TIMEOUT 10

static enum state { IDLE, UP, DOWN };

static GFX_Picture_Physic racquet[2];

static enum state ia_state = IDLE;
static int ia_state_timeout = IA_STATE_TIMEOUT;

static void init_racquet(GFX_Picture_Physic *racquet) {
  init_gpp(racquet, &racquet_asset, &racquet_asset_Palettes, 0, 0, 0, 0, AUTOBOX);
}

static void init() {
  init_gpu();
  init_racquet(&racquet[0]);
  init_racquet(&racquet[1]);
}

static void display() {
  display_gpp(&racquet[0], 16, 16);
  display_gpp(&racquet[1], 320 - 32, 16);
}

static void player_update() {
  update_joypad(0);
  if (joypad_is_up(0) && get_y_gpp(racquet[0]) > SCREEN_Y_MIN) move_gpp(&racquet[0], 0, -RACQUET_SPEED);

  if (joypad_is_down(0) && get_y_gpp(racquet[0]) < SCREEN_Y_MAX - racquet[0].gfx_picture.pixel_height) {
    move_gpp(&racquet[0], 0, RACQUET_SPEED);
  }
}

static void ia_update() {
  if (ia_state_timeout <= 0) {
    ia_state = get_random(3);
    ia_state_timeout = IA_STATE_TIMEOUT;
  }
  if (ia_state == UP && get_y_gpp(racquet[1]) > SCREEN_Y_MIN) move_gpp(&racquet[1], 0, -RACQUET_SPEED);

  if (ia_state == DOWN && get_y_gpp(racquet[1]) < SCREEN_Y_MAX - racquet[1].gfx_picture.pixel_height) {
    move_gpp(&racquet[1], 0, RACQUET_SPEED);
  }
  ia_state_timeout -= 1;
}

static void update() {
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
