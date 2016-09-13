/**
 * block2.c
 *  - Second Test of FruitBlock's Fruit Tiles
 */

// ~~ Includes ~~
#include <gb/gb.h>
#include <gb/console.h>
#include <stdio.h>
#include <types.h>
#include "fruit.h"

// ~~ Structs ~~
typedef struct _player_sprite {
    int abs_x, abs_y;               // absolute pixel coords of player sprite
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
void init_isr();
void joypad_int();
void draw_fruit_tile(player_sprite *plyr_sp);
void detect_ground(player_sprite *plyr_sp);


// ~~ global vars ~~
int cycle_num = 0;
player_sprite *plyr_sp_ptr;

int main(void)
{
    player_sprite plyr_sp;
    plyr_sp_ptr = &plyr_sp;

    // init
    plyr_sp.tile_x = 4;
    //plyr_sp.tile_y = -2;
    plyr_sp.tile_y = 4;
    plyr_sp.fruit_num = 1;
    plyr_sp.abs_x = (4 + 1) * 8;
    plyr_sp.abs_y = (4 + 1) * 8;
    plyr_sp.sub_x = 0;
    plyr_sp.sub_y = 0;
    plyr_sp.sprite_num = 0;
    plyr_sp.sprite_moved = FALSE;
    plyr_sp.diff_x = 0;
    plyr_sp.diff_y = 0;
    plyr_sp.shifted_cycle = -1;

    set_sprite_data(1, 9, fruit);
    set_bkg_data(1, 9, fruit);
    wait_vbl_done();
    set_sprite_tile(plyr_sp.sprite_num, 1);

    SHOW_BKG;

    move_sprite(plyr_sp.sprite_num, plyr_sp.abs_x, plyr_sp.abs_y);

    SHOW_SPRITES;

    init_isr();

    while (TRUE) {
        if (cycle_num >= 64) {      // This should always be a num that's 2^n so it will work evenly w/ bitwise operators
            cycle_num = 0;
        }

        if (plyr_sp.sprite_moved) {
            move_sprite(plyr_sp.sprite_num, plyr_sp.abs_x, plyr_sp.abs_y);
            //gotoxy(0, 0);
            //printf("X:%d Y:%d            ", plyr_sp.abs_x, plyr_sp.abs_y);
            //printf("X:%d  Y:%d             ", plyr_sp.tile_x, plyr_sp.tile_y);
            plyr_sp.sprite_moved = FALSE;
        }

        if (plyr_sp.shifted_cycle >= 0 && abs(plyr_sp.shifted_cycle - cycle_num) > 8) {
            plyr_sp.shifted_cycle = -1;
        }

        cycle_num++;
        delay(20);
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

void init_isr()
{
    disable_interrupts();
    add_JOY(joypad_int);
    enable_interrupts();
    set_interrupts(VBL_IFLAG | JOY_IFLAG);
}

void joypad_int()
{
    disable_interrupts();

    //if (cycle_num & 0x03 == 0) {     // Bitwise And with 3 is the functional equivalent of cycle_num MOD 4
    if (cycle_num % 8 == 0 && !plyr_sp_ptr->sprite_moved) {
        int key = joypad();

        //TODO: Remove This up key
        if (key & J_UP) {
            plyr_sp_ptr->abs_y -= 2;
            plyr_sp_ptr->sprite_moved = TRUE;
            plyr_sp_ptr->diff_y -= 2;
            plyr_sp_ptr->sub_y -= 2;
            if (plyr_sp_ptr->sub_y < 0) {
                plyr_sp_ptr->sub_y += 8;
                plyr_sp_ptr->tile_y--;
            }
            /*
            if (plyr_sp_ptr->abs_y % 8 == 0) {
                plyr_sp_ptr->tile_y--;
            }
            */
        }

        if (key & J_DOWN) {
            plyr_sp_ptr->abs_y += 2;
            plyr_sp_ptr->diff_y += 2;
            plyr_sp_ptr->sprite_moved = TRUE;
            plyr_sp_ptr->sub_y += 2;
            if (plyr_sp_ptr->sub_y > 7) {
                plyr_sp_ptr->sub_y -= 8;
                plyr_sp_ptr->tile_y++;
            }
            /*
            if (plyr_sp_ptr->abs_y % 8 == 0) {
                plyr_sp_ptr->tile_y++;
            }
            */
        }

        if (key & J_LEFT) {
            plyr_sp_ptr->abs_x -= 8;
            plyr_sp_ptr->tile_x--;
            plyr_sp_ptr->sprite_moved = TRUE;
        }

        if (key & J_RIGHT) {
            plyr_sp_ptr->abs_x += 8;
            plyr_sp_ptr->tile_x++;
            plyr_sp_ptr->sprite_moved = TRUE;
        }

        if (key & J_B) {
            if (plyr_sp_ptr->shifted_cycle == -1) {
                plyr_sp_ptr->fruit_num++;
                if (plyr_sp_ptr->fruit_num >= 7) {
                    plyr_sp_ptr->fruit_num = 1;
                }
                set_sprite_tile(plyr_sp_ptr->sprite_num, plyr_sp_ptr->fruit_num);
                plyr_sp_ptr->shifted_cycle = cycle_num;
            }
        }

        if (key & J_A) {
            draw_fruit_tile(plyr_sp_ptr);
        }
    }

    enable_interrupts();
}

void draw_fruit_tile(player_sprite *plyr_sp)
{
    unsigned char fruit_array[1];
    fruit_array[0] = plyr_sp->fruit_num;
    set_bkg_tiles(plyr_sp->tile_x, plyr_sp->tile_y, 1, 1, fruit_array);
}

void detect_ground(player_sprite *plyr_sp)
{

}
