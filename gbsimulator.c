//#include "sidlib.h"
#include "lcdc.h"
#include "gameboy.h"
#include "error.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

// Variables globales
gameboy_t gameboy;
struct timeval start;
struct timeval paused;

// Key press bits
#define MY_KEY_UP_BIT     0x01
#define MY_KEY_DOWN_BIT   0x02
#define MY_KEY_RIGHT_BIT  0x04
#define MY_KEY_LEFT_BIT   0x08
#define MY_KEY_A_BIT      0x10
#define MY_KEY_B_BIT      0x
#define MY_KEY_SELECT_BIT 0x
#define MY_KEY_START_BIT  0x

// ======================================================================
static void set_grey(guchar* pixels, int row, int col, int width, guchar grey)
{
    const size_t i = (size_t) (3 * (row * width + col)); // 3 = RGB
    pixels[i+2] = pixels[i+1] = pixels[i] = grey;
}

// ======================================================================
static uint64_t get_time_in_GB_cycles_since(struct timeval* from)
{
    struct timeval now;
    int err = gettimeofday(&now, NULL);
    if (err != 0){
        fprintf(stderr, "error reading time from system!");
        return 0;
    }
    int cmp = timercmp(&now, from);
    if (cmp != 0){
        struct timeval delta;
        timersub(&now, from, &delta);
        return delta.tv_sec * GB_CYCLES_PER_S +
                (delta.tv_usec * GB_CYCLES_PER_S) / 1000000;
    } else {
        return 0;
    }
}

// ======================================================================
static void generate_image(guchar* pixels, int height, int width)
{
    M_REQUIRE_NO_ERR(gameboy_run_until(&gameboy,
            get_time_in_GB_cycles_since(&start)));
    for (size_t y = 0; y < height; ++y){
        for (size_t x = 0; x < width; ++x){
            uint8_t pixel = 0;
            int err = image_get_pixel(pixel, &(gameboy.screen.display),
                    x, y); // changer facteur?
            if (err == 0){
            fprintf(stderr, "error generatin image!\n");
            return;
            }
            set_grey(pixels, x, y, width, 255 - 85 * pixel);
        }
    }
}

// ======================================================================
#define do_key(X) \
    do { \
        if (! (psd->key_status & MY_KEY_ ## X ##_BIT)) { \
            psd->key_status |= MY_KEY_ ## X ##_BIT; \
            joypad_key_pressed(&gameboy.pad, X ## _KEY); \
        } \
    } while(0) // error codes?

static gboolean keypress_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        do_key(UP);
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        return TRUE;

    case 'S':
    case 's':
        do_key(B);
        return TRUE;

    case GDK_KEY_Page_Up:
        do_key(SELECT);
        return TRUE;

    case GDK_KEY_Page_Down:
        do_key(START);
        return TRUE;

    case GDK_KEY_space:
        if (psd -> timeout_id > 0){
            gettimeofday(&paused, NULL);
        } else {
            struct timeval now;
            gettimeofday(&now, NULL);
            timersub(&now, &paused, &paused);
            timeradd(&start, &paused, &start);
            timerclear(&paused);
        }

    }

    return ds_simple_key_handler(keyval, data);
}
#undef do_key

// ======================================================================
#define do_key(X) \
    do { \
        if (psd->key_status & MY_KEY_ ## X ##_BIT) { \
          psd->key_status &= (unsigned char) ~MY_KEY_ ## X ##_BIT; \
            joypad_key_released(&gameboy.pad, X ## _KEY); \
        } \
    } while(0)

static gboolean keyrelease_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        do_key(UP);
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        return TRUE;

    case 'S':
    case 's':
        do_key(B);
        return TRUE;

    case GDK_KEY_Page_Up:
        do_key(SELECT);
        return TRUE;

    case GDK_KEY_Page_Down:
        do_key(START);
        return TRUE;

    }

    return FALSE;
}
#undef do_key

// ======================================================================
int main(int argc, char *argv[])
{
    int err = gettimeofday(&start, NULL);
    if (err != 0){
        fprintf(stderr, "error reading time from system!");
        return 1;// error code?
    }
    timerclear(&paused);

    char cr_name[25];
    printf("Cartridge name (max length 25):\n");
    int err = 0;
    while (err == 0){
        err = fgets(cr_name, 25, stdin);
        if (err == 0){
            printf("Invalid name, insert again:\n");
        }
    }
    err = 0;
    memset(&gameboy, 0, sizeof(gameboy_t));
    err = gameboy_create(&gameboy, cr_name);
    if (err != ERR_NONE) {
        gameboy_free(&gameboy);
        return err;
    }
    sd_launch(&argc, &argv, sd_init("Simulateur GameBoy", LCD_WIDTH * 2,
            LCD_HEIGHT * 2, 40, generate_image, keypress_handler, keyrelease_handler));
    gameboy_free(&gameboy);
    return 0;
}
