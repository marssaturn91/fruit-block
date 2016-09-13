/**
 * block1.c
 *  - First Test of FruitBlock's Fruit Tiles
 */

#include <gb/gb.h>
#include "fruit.h"

void detect_ground(int *x_coord, int *y_coord, int fruit_num);

int main(void)
{
    int x, y, st, key, z, i, j;
    x = 32;
    y = -8;
    st = 0;
    z = 1;
    i = 0;
    j = 0;

    set_sprite_data(1, 9, fruit);
    set_bkg_data(1, 9, fruit);
    wait_vbl_done();
    set_sprite_tile(0, 1);
    //set_sprite_tile(1, 1);

    SHOW_BKG;

    move_sprite(st, x, y);

    //move_sprite(1, 200, 200);
    SHOW_SPRITES;

    key = 0;

    while (!0) {
        i++;
        j++;

        detect_ground(&x, &y, z);

        if (i > 10) {
            y++;
            move_sprite(st, x, y);
            i = 0;
        }
        /*

        key = joypad();
        if (key == J_UP) {
            y--;
            move_sprite(st, x, y);
        }
        */

        key = joypad();
        if (key == J_DOWN) {
            y++;
            move_sprite(st, x, y);
        }

        key = joypad();
        if (key == J_LEFT) {
            //x--;
            if (j > 15) {
                x -= 8;
                move_sprite(st, x, y);
                j = 0;
            }
        }

        key = joypad();
        if (key == J_RIGHT) {
            //x++;
            if (j > 15) {
                x += 8;
                move_sprite(st, x, y);
                j = 0;
            }
        }

        key = joypad();
        if (key == J_A) {
            if (i > 8) {
                z++;
                if (z > 7) {
                    z = 1;
                }
                set_sprite_tile(0, z);
            }
        }

        delay(20);
    }

    reset();
}

void detect_ground(int *x_coord, int *y_coord, int fruit_num)
{
    char grounded = 0;
    unsigned char fruit_array[1];
    fruit_array[0] = fruit_num;
    // Check for bottom of column
    if (*y_coord >= 144) {
        grounded = 1;
    }


    // Check for tiles beneath

    if (grounded) {
        set_bkg_tiles((*x_coord) >> 3, (*y_coord) >> 3, 1, 1, fruit_array);
        *x_coord = 32;
        *y_coord = -8;
        move_sprite(0, *x_coord, *y_coord);
    }

}
