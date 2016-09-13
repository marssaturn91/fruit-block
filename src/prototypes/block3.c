/**
 * block3.c
 *  - Third Test of FruitBlock's Fruit Tiles
 */

// ~~ Includes ~~
#include <gb/gb.h>
//#include <gb/console.h>
#include <stdio.h>
#include <types.h>
#include "fruit.h"
#include "leveltiles.h"
#include "levelmap.h"

// ~~ Macros ~~
#define HI_BYTE(b)  (((b) >> 8) & 0x00FF)
#define LO_BYTE(b)  ((b) & 0x00FF)
#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)

// ~~ Structs ~~
typedef struct _oam_sprite {
    UINT8 y;
    UINT8 x;
    UINT8 tile_index;
    UINT8 sprite_flags;
} oam_sprite;

typedef struct _player_sprite {
    //int abs_x, abs_y;               // absolute pixel coords of player sprite
    int sub_x, sub_y;
    int fruit_num;                  // index num of fruit to display
    int tile_x, tile_y;             // tile block coords of player sprite
    int sprite_num;                 // OAM entry num of sprite tile
    int sprite_moved;               // flag to indicate when to update sprite position
    int diff_x, diff_y;
    int shifted_cycle;
} player_sprite;

// ~~ Function Prototypes ~~
int abs(int num);
void init(void);
void get_input(void);
void update(void);
void init_draw(void);
void draw(void);
void draw_fruit_tile();
void detect_ground();
//void dma_copy(void);

// ~~ Global Vars ~~
//oam_sprite oam_sprites[40];
oam_sprite *oam_sprites = 0xC000;
player_sprite plyr_sp;
int cycle_num;
int map_init;
int gravity;
int init_count;
//uint8 dungeon[13][6];

int main(void)
{

    init();

    while (TRUE) {
        if (cycle_num >= 64) {      // This should always be a num that's 2^n so it will work evenly w/ bitwise operators
            cycle_num = 0;
        }

        get_input();
        update();

        //wait_vbl_done();
        //draw();
        cycle_num++;
        delay(5);
    }

    reset();
}

int abs(int num)
{
  if(num < 0)
    return -num;
  else
    return num;
}

void init(void)
{
    int i;

    // clear OAM table
    disable_interrupts();
    DISPLAY_OFF;
    /*
    for (i = 0; i < 40; i++) {
        //delay(1);                                   // needed to complete loop for unknown reason
        oam_sprites[i].y = 0;
        oam_sprites[i].x = 0;
        oam_sprites[i].tile_index = 0;
        oam_sprites[i].sprite_flags = 0;
    }
    */

    //printf("OAM init complete\n");
    init_count = 0;
    cycle_num = 0;
    map_init = FALSE;
    gravity = 2;

    // initialize sprite
    plyr_sp.tile_x = 4;
    plyr_sp.tile_y = -2;
    plyr_sp.sub_x = 0;
    plyr_sp.sub_y = 0;
    plyr_sp.sprite_num = 0;
    plyr_sp.sprite_moved = FALSE;
    plyr_sp.diff_x = 0;
    plyr_sp.diff_y = 0;
    plyr_sp.shifted_cycle = -1;
    plyr_sp.fruit_num = 1;

    oam_sprites[plyr_sp.sprite_num].y = (plyr_sp.tile_y + 1) * 8;
    oam_sprites[plyr_sp.sprite_num].x = (plyr_sp.tile_x + 1) * 8;
    oam_sprites[plyr_sp.sprite_num].tile_index = 1;
    oam_sprites[plyr_sp.sprite_num].sprite_flags = 0;

    //set_sprite_data(1, 9, fruit);
    //set_bkg_data(1, 9, fruit);
    //printf("Sprite init complete\n");
    add_VBL(draw);
    // draw first frame
    //set_interrupts(VBL_IFLAG);
    enable_interrupts();
    DISPLAY_ON;
    //wait_vbl_done();
    //wait_vbl_done();

    //disable_interrupts();
    //DISPLAY_OFF;
    //remove_VBL(init_draw);
    //set_interrupts(VBL_IFLAG);

    //add_VBL(draw);
    //enable_interrupts();

    //SHOW_BKG;
    //SHOW_SPRITES;
    //draw();
}

void get_input(void)
{
    UINT8 tile_array[1];
    tile_array[0] = 0;

    if (cycle_num % 12 == 0 && !plyr_sp.sprite_moved) {
        int key = joypad();
        if (key & J_UP) {
            plyr_sp.sprite_moved = TRUE;
            plyr_sp.diff_y -= 2;
            plyr_sp.sub_y -= 2;
            if (plyr_sp.sub_y < 0) {
                plyr_sp.sub_y += 8;
                plyr_sp.tile_y--;
            }
        }

        if (key & J_DOWN) {
            plyr_sp.sprite_moved = TRUE;
            plyr_sp.diff_y += 2;
            plyr_sp.sub_y += 2;
            if (plyr_sp.sub_y > 7) {
                plyr_sp.sub_y -= 8;
                plyr_sp.tile_y++;
            }
        }

        if (key & J_LEFT) {
            get_bkg_tiles(plyr_sp.tile_x - 1, plyr_sp.tile_y, 1, 1, tile_array);
            if (plyr_sp.tile_x > 2 && tile_array[0] == 0) {
                plyr_sp.sprite_moved = TRUE;
                plyr_sp.tile_x--;
            }
        }

        if (key & J_RIGHT) {
            get_bkg_tiles(plyr_sp.tile_x + 1, plyr_sp.tile_y, 1, 1, tile_array);
            if (plyr_sp.tile_x < 7 && tile_array[0] == 0) {
                plyr_sp.sprite_moved = TRUE;
                plyr_sp.tile_x++;
            }
        }

        if (key & J_B) {
            if (plyr_sp.shifted_cycle == -1) {
                plyr_sp.fruit_num++;
                if (plyr_sp.fruit_num >= 7) {
                    plyr_sp.fruit_num = 1;
                }
                plyr_sp.shifted_cycle = cycle_num;
            }
        }

        if (key & J_A) {
            draw_fruit_tile();
        }
        /*
        if (key & J_A) {
            DISPLAY_OFF;
            disable_interrupts();
            add_VBL(draw);
            remove_VBL(init_draw);
            enable_interrupts();
            DISPLAY_ON;
        }
        */
    }
}

void update(void)
{
    detect_ground();

    if (plyr_sp.sprite_moved) {
        oam_sprites[plyr_sp.sprite_num].y = (plyr_sp.tile_y + 1) * 8 + plyr_sp.sub_y;
        oam_sprites[plyr_sp.sprite_num].x = (plyr_sp.tile_x + 1) * 8 + plyr_sp.sub_x;
        plyr_sp.sprite_moved = FALSE;
    }

    if (plyr_sp.shifted_cycle >= 0 && abs(plyr_sp.shifted_cycle - cycle_num) > 8) {
        oam_sprites[plyr_sp.sprite_num].tile_index = plyr_sp.fruit_num;
        plyr_sp.shifted_cycle = -1;
    }

    if (cycle_num % 16 == 0) {
        plyr_sp.diff_y += gravity;
        plyr_sp.sub_y += gravity;
        if (plyr_sp.sub_y > 7) {
            plyr_sp.sub_y -= 8;
            plyr_sp.tile_y++;
        }
        oam_sprites[plyr_sp.sprite_num].y = (plyr_sp.tile_y + 1) * 8 + plyr_sp.sub_y;
    }
}

void init_draw(void)
{
    //disable_interrupts();
    //set_sprite_data(1, 9, fruit);
    DISPLAY_OFF;
    set_sprite_data(0, 9, leveltiles);
    set_bkg_data(0, 37, leveltiles);
    //set_win_tiles(0, 0, 18, 18, levelmap);
    set_bkg_tiles(1, 0, 18, 18, levelmap);
    SHOW_BKG;
    SHOW_SPRITES;
    //SHOW_WIN;
    DISPLAY_ON;
    //enable_interrupts();
    //HIDE_WIN;
}

void draw(void)
{
    //dma_copy();
    disable_interrupts();
    if (!map_init) {
        map_init = TRUE;
        init_draw();
    }
    enable_interrupts();
}

void draw_fruit_tile()
{
    unsigned char fruit_array[1];
    fruit_array[0] = plyr_sp.fruit_num;
    set_bkg_tiles(plyr_sp.tile_x, plyr_sp.tile_y, 1, 1, fruit_array);
}

void detect_ground()
{
    int grounded;
    UINT8 tile_array[1];
    grounded = FALSE;

    if (plyr_sp.tile_y >= 11 && plyr_sp.sub_y >= 6) {
        grounded = TRUE;
    }

    get_bkg_tiles(plyr_sp.tile_x, plyr_sp.tile_y + 1, 1, 1, tile_array);
    if (tile_array[0] > 0 && tile_array[0] < 8 && plyr_sp.sub_y >= 6) {
        grounded = TRUE;
    }

    if (grounded) {
        draw_fruit_tile();
        plyr_sp.tile_x = 4;
        plyr_sp.tile_y = -2;
        plyr_sp.sub_x = 0;
        plyr_sp.sub_y = 0;
    }
}

/*
void dma_copy()
{
    disable_interrupts();
    //DMA_REG = (unsigned char *) HI_BYTE((short) oam_sprites);
    //printf("OAM Table %x\n", oam_sprites);
    //printf("DMA: %x\n", HI_BYTE((UINT16) oam_sprites));
    //for (int i = 0x28; i > 0; i--);
    delay(160);
    enable_interrupts();
}
*/
