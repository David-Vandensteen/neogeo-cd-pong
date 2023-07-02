#include <neocore.h>
#include "externs.h"

#define SCREEN_Y_MIN 0
#define SCREEN_Y_MAX 256

#define RACQUET_SPEED 3
#define IA_STATE_TIMEOUT 10

static enum state { IDLE, UP, DOWN };

static GFX_Picture_Physic racquet[2];
static paletteInfo racquet2_asset_Palettes;

static enum state ia_state = IDLE;
static int ia_state_timeout = IA_STATE_TIMEOUT;


// ----------------------------------------patch neocore
static void update_joypad(BYTE id) {
  if (id == 0) update_joypad_p1();
}

static int get_random(int range) {
  return RAND(range);
}
//--------------------------------------------------------




static void init_racquet(GFX_Picture_Physic *racquet, paletteInfo *palette) {
  init_gpp(racquet, &racquet_asset, palette, 0, 0, 0, 0, AUTOBOX);
}

static void init() {
  BYTE i;
  for (i = 0; i < (racquet_asset_Palettes.palCount MULT16); i++) { racquet2_asset_Palettes.data[i] = get_random(0xFFFF); }
  init_gpu();
  init_racquet(&racquet[0], &racquet_asset_Palettes);
  init_racquet(&racquet[1], &racquet2_asset_Palettes);
}

static void display() {
  display_gpp(&racquet[0], 16, 16);
  display_gpp(&racquet[1], 320 - 32, 16);
}

static void update_player() {
  update_joypad(0);
  if (joypad_is_up(0)) move_gpp(&racquet[0], 0, -RACQUET_SPEED);
  if (joypad_is_down(0)) move_gpp(&racquet[0], 0, RACQUET_SPEED);
}

static void update_ia() {
  if (ia_state_timeout <= 0) {
    ia_state = get_random(3);
    ia_state_timeout = IA_STATE_TIMEOUT;
  }
  if (ia_state == UP && get_y_gpp(racquet[1]) > SCREEN_Y_MIN) move_gpp(&racquet[1], 0, -RACQUET_SPEED);
  if (ia_state == DOWN && get_y_gpp(racquet[1]) < SCREEN_Y_MAX) move_gpp(&racquet[1], 0, RACQUET_SPEED);
  ia_state_timeout -= 1;
}

static void update() {
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
