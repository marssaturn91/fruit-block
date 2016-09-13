/**
 * block4.c
 *  - Fourth Test of FruitBlock
 */

// ~~ Includes ~~
#include <gb/gb.h>
#include <rand.h>
#include <stdio.h>
#include "util.h"
#include "leveltiles.h"
#include "levelmap.h"

// ~~ Defines ~~
#define BLK_BUF_WIDTH          6
#define BLK_BUF_HEIGHT        13

// ~~ Macros ~~
#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)
#define TRANS_BLK_BUF1_X(b) ((b - 2))
#define TRANS_BLK_BUF1_Y(b) ((b + 1))
#define REV_TRANS_BLK_BUF1_X(b) ((b + 2))
#define REV_TRANS_BLK_BUF1_Y(b) ((b - 1))

// ~~ Enums ~~
typedef enum _game_mode {
    INTRO,
    TITLE,
    MENU,
    EXERCISE_1P,
    EXERCISE_2P,
    VS_2P,
    VS_CPU
} game_mode;

// ~~ Structs ~~
typedef struct _player_sprite {
    //INT8 abs_x, abs_y;
    INT8 diff_x, diff_y;
    UINT8 sub_x, sub_y;
    INT8 tile_x, tile_y;
    UINT8 oam_index;
    UINT8 fruit_num;
} player_sprite;

// ~~ Function Prototypes ~~
void apply_gravity(void);
void detect_ground();
void draw(void);
void draw_block_buffer(void);
void get_1p_input(void);
void get_input(void);
void init(void);
void init_draw(void);
void init_sprites(void);
void reset_player_sprite(int num);
void update(void);
void update_exercise(void);

// ~~ Global Vars ~~
//INT8 block_buffer1[BLK_BUF_WIDTH][BLK_BUF_HEIGHT];      // deprecated note: negative values indicate newly added blocks that haven't been added to the map yet
//INT8 block_buffer2[BLK_BUF_WIDTH][BLK_BUF_HEIGHT];
//INT8 block_buffer1[BLK_BUF_WIDTH * BLK_BUF_HEIGHT];     // deprecated note: runs by columns and then rows (each column is length 13)
//INT8 block_buffer2[BLK_BUF_WIDTH * BLK_BUF_HEIGHT];
UINT8 block_buffer1[BLK_BUF_HEIGHT * BLK_BUF_WIDTH];
UINT8 block_buffer2[BLK_BUF_HEIGHT * BLK_BUF_WIDTH];
UINT8 column_buffer[BLK_BUF_HEIGHT];
player_sprite plyr_sp1;
player_sprite plyr_sp2;
game_mode gm_mode;
UINT16 rnd_seed;
BOOLEAN map_init;
UINT8 cycle_num;
INT8 gravity;

// debugging Vars
UINT16 *debug_var_1 = 0xD100;
UINT16 *debug_var_2 = 0xD102;

int main(void)
{
    init();

    while (TRUE) {
        if (cycle_num == 64) {
            cycle_num = 0;
        }

        get_input();
        update();

        delay(5);
        cycle_num++;
    }

    reset();
}

void apply_gravity(void)
{
    if (gm_mode == EXERCISE_1P) {
        if (cycle_num % 16 == 0) {
            plyr_sp1.diff_y += gravity;
            plyr_sp1.sub_y += gravity;
            if (plyr_sp1.sub_y > 7) {
                plyr_sp1.sub_y = 0;
                plyr_sp1.tile_y++;
            }
        }
    }
}

void detect_ground()
{
    BOOLEAN grounded;
    grounded = FALSE;

    if (plyr_sp1.tile_y >= 11 && plyr_sp1.sub_y >= 6) {
        grounded = TRUE;
    }

    //if (block_buffer1[plyr_sp1.tile_x - 2][plyr_sp1.tile_y + 2] != 0) {
    //if (block_buffer1[TRANS_BLK_BUF1_X(plyr_sp1.tile_x)][TRANS_BLK_BUF1_Y(plyr_sp1.tile_y + 1)] != 0 && plyr_sp1.sub_y >= 6) {
    //if (block_buffer1[BLK_BUF_HEIGHT * TRANS_BLK_BUF1_X(plyr_sp1.tile_x) + TRANS_BLK_BUF1_Y(plyr_sp1.tile_y + 1)] != 0 && plyr_sp1.sub_y >= 6) {
    if (block_buffer1[BLK_BUF_WIDTH * TRANS_BLK_BUF1_Y(plyr_sp1.tile_y + 1) + TRANS_BLK_BUF1_X(plyr_sp1.tile_x)] != 0 && plyr_sp1.sub_y >= 6) {
        grounded = TRUE;
    }

    if (grounded) {
        //block_buffer1[plyr_sp1.tile_x - 2][plyr_sp1.tile_y + 1] = -plyr_sp1.fruit_num;
        //printf("blkbuf coord: %d, %d\n", TRANS_BLK_BUF1_X(plyr_sp1.tile_x), TRANS_BLK_BUF1_Y(plyr_sp1.tile_y));
        //printf("blkbuf coord: %u, %u\n", (UWORD) plyr_sp1.tile_x, (UWORD) plyr_sp1.tile_y);
        //*debug_var_1 = &plyr_sp1.tile_x;
        //*debug_var_2 = &plyr_sp1.tile_y;
        //*debug_var_1 = plyr_sp1.tile_x;
        //*debug_var_2 = plyr_sp1.tile_y;
        //*debug_var_1 = TRANS_BLK_BUF1_X(plyr_sp1.tile_x);
        //*debug_var_2 = TRANS_BLK_BUF1_Y(plyr_sp1.tile_y);
        *debug_var_1 = &block_buffer1;
        //*debug_var_2 = &grounded;
        //debug_var_1 = -plyr_sp1.fruit_num;
        //block_buffer1[TRANS_BLK_BUF1_X(plyr_sp1.tile_x)][TRANS_BLK_BUF1_Y(plyr_sp1.tile_y)] = -plyr_sp1.fruit_num;
        //block_buffer1[TRANS_BLK_BUF1_X(plyr_sp1.tile_x)][TRANS_BLK_BUF1_Y(plyr_sp1.tile_y)] = 0xF;
        //block_buffer1[BLK_BUF_HEIGHT * TRANS_BLK_BUF1_X(plyr_sp1.tile_x) + TRANS_BLK_BUF1_Y(plyr_sp1.tile_y)] = -plyr_sp1.fruit_num;
        block_buffer1[BLK_BUF_WIDTH * TRANS_BLK_BUF1_Y(plyr_sp1.tile_y) + TRANS_BLK_BUF1_X(plyr_sp1.tile_x)] = plyr_sp1.fruit_num;
        reset_player_sprite(1);
    }
}

void draw(void)
{
    disable_interrupts();
    if (!map_init) {
        map_init = TRUE;
        init_draw();
    }
    draw_block_buffer();
    enable_interrupts();
}

void draw_block_buffer(void)
{
    //UINT8 i, j;
    //UINT8 block_array[1];
    UINT8 *block_buffer_vis;

    /*
    for (j = 0; j < BLK_BUF_WIDTH; j++) {
        for (i = 1; i < BLK_BUF_HEIGHT; i++) {      // starts at 1 to hide top-most row, which is invisible
            if (block_buffer1[j][i] < 0) {
                block_buffer1[j][i] = abs(block_buffer1[j][i]);
                block_array[0] = block_buffer1[j][i];
                //set_bkg_tiles(i + 1, j, 1, 1, block_array);
                set_bkg_tiles(REV_TRANS_BLK_BUF1_X(j), REV_TRANS_BLK_BUF1_Y(i), 1, 1, block_array);
            }
            if (block_buffer2[j][i] < 0) {

            }
        }
    }
    */

    /*
    for (j = 0; j < BLK_BUF_WIDTH; j++) {
        for (i = 1; i < BLK_BUF_HEIGHT; i++) {      // starts at 1 to hide top-most row, which is invisible
            if (block_buffer1[BLK_BUF_HEIGHT * j + i] < 0) {
                block_buffer1[BLK_BUF_HEIGHT * j + i] = abs((INT16) block_buffer1[BLK_BUF_HEIGHT * j + i]);
                block_array[0] = block_buffer1[BLK_BUF_HEIGHT * j + i];
                set_bkg_tiles(REV_TRANS_BLK_BUF1_X(j), REV_TRANS_BLK_BUF1_Y(i), 1, 1, block_array);
            }
            // second buffer
        }
    }
    */

    // an alternative method would be to use a boolean to track when updated
    //  and blit the whole block to the map
    //set_bkg_tiles()
    block_buffer_vis = block_buffer1 + BLK_BUF_WIDTH;
    set_bkg_tiles(2, 0, BLK_BUF_WIDTH, BLK_BUF_HEIGHT - 1, block_buffer_vis);
}

void get_1p_input(void)
{
    if (cycle_num % 12 == 0) {
        UINT8 key = joypad();
        if (key & J_DOWN) {
            //printf("DOWN\n");
            plyr_sp1.sub_y += 2;
            plyr_sp1.diff_y += 2;
            if (plyr_sp1.sub_y > 7) {
                plyr_sp1.sub_y = 0;
                plyr_sp1.tile_y++;
            }
        }

        if (key & J_LEFT) {
            //printf("LEFT\n");
            //if (plyr_sp1.tile_x > 2 && block_buffer1[plyr_sp1.tile_y + 1][plyr_sp1.tile_x - 3] == 0) {
            //if (plyr_sp1.tile_x > 2 && block_buffer1[TRANS_BLK_BUF1_X(plyr_sp1.tile_x) - 1][TRANS_BLK_BUF1_Y(plyr_sp1.tile_y)] == 0) {
            if (plyr_sp1.tile_x > 2 && block_buffer1[BLK_BUF_WIDTH * TRANS_BLK_BUF1_Y(plyr_sp1.tile_y) + TRANS_BLK_BUF1_X(plyr_sp1.tile_x - 1)] == 0) {
                plyr_sp1.tile_x--;
                plyr_sp1.diff_x -= 8;
            }
        }

        if (key & J_RIGHT) {
            //printf("RIGHT\n");
            //if (plyr_sp1.tile_x < 7 && block_buffer1[plyr_sp1.tile_y + 1][plyr_sp1.tile_x - 1] == 0) {
            //if (plyr_sp1.tile_x < 7 && block_buffer1[TRANS_BLK_BUF1_X(plyr_sp1.tile_x) + 1][TRANS_BLK_BUF1_Y(plyr_sp1.tile_y)] == 0) {
            if (plyr_sp1.tile_x < 7 && block_buffer1[BLK_BUF_WIDTH * TRANS_BLK_BUF1_Y(plyr_sp1.tile_y) + TRANS_BLK_BUF1_X(plyr_sp1.tile_x + 1)] == 0) {
                plyr_sp1.tile_x++;
                plyr_sp1.diff_x += 8;
            }
        }
    }
}

void get_input(void)
{
    switch (gm_mode) {
        case INTRO: TITLE: MENU:
            break;
        default:
            get_1p_input();
    }
}

void init(void)
{
    UINT8 i, j;

    disable_interrupts();
    DISPLAY_OFF;                        // Routine checks for V-Blank automatically

    // set default values
    gm_mode = EXERCISE_1P;
    map_init = FALSE;
    cycle_num = 0;
    gravity = 2;
    rnd_seed = 7;

    // clear the block buffers
    //for (j = 0; j < BLK_BUF_WIDTH; j++) {
    //    for (i = 0; i < BLK_BUF_HEIGHT; i++) {
    for (j = 0; j < BLK_BUF_HEIGHT; j++) {
        for (i = 0; i < BLK_BUF_WIDTH; i++) {
            //block_buffer1[j][i] = 15;
            //block_buffer2[j][i] = 15;
            //block_buffer1[BLK_BUF_HEIGHT * j + i] = 0;
            block_buffer1[BLK_BUF_WIDTH * j + i] = 0;
        }
    }

    initrand(rnd_seed);
    init_sprites();

    add_VBL(draw);

    enable_interrupts();
    DISPLAY_ON;
}

void init_draw(void)
{
    DISPLAY_OFF;

    set_sprite_data(0, 9, leveltiles);
    set_bkg_data(0, 37, leveltiles);
    set_bkg_tiles(1, 0, 18, 18, levelmap);

    SHOW_BKG;
    SHOW_SPRITES;

    DISPLAY_ON;
}

void init_sprites(void)
{
    switch (gm_mode) {
        case EXERCISE_1P:
            reset_player_sprite(1);
            break;
        case EXERCISE_2P: VS_2P: VS_CPU:
            reset_player_sprite(1);
            reset_player_sprite(2);
            break;
        default:
            break;
    }
}

void reset_player_sprite(int num)
{
    if (num < 2) {
        plyr_sp1.tile_x = 4;
        plyr_sp1.tile_y = -2;
        //plyr_sp1.tile_y = 2;
        plyr_sp1.sub_x = 0;
        plyr_sp1.sub_y = 0;
        plyr_sp1.diff_x = 0;
        plyr_sp1.diff_y = 0;
        plyr_sp1.oam_index = 0;
        //plyr_sp1.fruit_num = (rand() % 7) + 1;
        do {
            plyr_sp1.fruit_num = (rand() & 7) + 1;
        } while (plyr_sp1.fruit_num > 6);

        //plyr_sp1.fruit_num &= 7;
        //plyr_sp1.fruit_num++;

        /*
        printf("tile coord: %u, %u\n", plyr_sp1.tile_x, plyr_sp1.tile_y);
        printf("sub coord: %u, %u\n", plyr_sp1.tile_x, plyr_sp1.tile_y);
        printf("oam index: %u\n", plyr_sp1.oam_index);
        printf("fruit num: %u\n", plyr_sp1.fruit_num);
        */

        set_sprite_tile(plyr_sp1.oam_index, plyr_sp1.fruit_num);
        move_sprite(plyr_sp1.oam_index, (plyr_sp1.tile_x + 1) * 8 + plyr_sp1.sub_x, (plyr_sp1.tile_y + 1) * 8 + plyr_sp1.sub_y);
    } else {
        plyr_sp2.tile_x = 14;
        plyr_sp2.tile_y = -2;
        plyr_sp2.sub_x = 0;
        plyr_sp2.sub_y = 0;
        plyr_sp2.diff_x = 0;
        plyr_sp2.diff_y = 0;
        plyr_sp2.oam_index = 2;
        plyr_sp2.fruit_num = rand() % 7 + 1;
    }
}

void update(void)
{
    switch (gm_mode) {
        case EXERCISE_1P:
            apply_gravity();
            detect_ground();
            update_exercise();
            break;
    }
}

void update_exercise(void)
{
    if (plyr_sp1.diff_x != 0 || plyr_sp1.diff_y != 0) {
        scroll_sprite(plyr_sp1.oam_index, plyr_sp1.diff_x, plyr_sp1.diff_y);
        plyr_sp1.diff_x = 0;
        plyr_sp1.diff_y = 0;
    }
}
