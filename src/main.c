#include <neocore.h>
#include "externs.h"

#define RACQUET_SPEED 3

GFX_Picture_Physic racquet[];
paletteInfo racquet2_asset_Palettes;

static void init_racquet(GFX_Picture_Physic *racquet, paletteInfo *palette) {
  init_gpp(racquet, &racquet_asset, palette, 0, 0, 0, 0, AUTOBOX);
}

static void init() {
  BYTE i;
  for (i = 0; i < (racquet_asset_Palettes.palCount MULT16); i++) { racquet2_asset_Palettes.data[i] = RAND(0xFFFF); }
  init_gpu();
  init_racquet(&racquet[0], &racquet_asset_Palettes);
  init_racquet(&racquet[1], &racquet2_asset_Palettes);
}

static void display() {
  display_gpp(&racquet[0], 16, 16);
  display_gpp(&racquet[1], 320 - 32, 16);
}

static void update() {
  BOOL randomBool = RAND(2);
  BYTE y;
  update_joypad_p1();
  if (joypad_is_up(0)) move_gpp(&racquet[0], 0, -RACQUET_SPEED);
  if (joypad_is_down(0)) move_gpp(&racquet[0], 0, RACQUET_SPEED);

  if (get_frame_counter() % 5 == 0) {
    if (randomBool == 0) {
      move_gpp(&racquet[1], 0, RACQUET_SPEED);
    } else {
      move_gpp(&racquet[1], 0, -RACQUET_SPEED);
    }
  }
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
