#ifndef RENDER_H
#define RENDER_H

#include <cairo/cairo.h>

extern int width, height, radius;
extern int camera_X, camera_Y;
extern int text_output_y;

struct color 
{
   double r,g,b;
};

void render_text_to_screen(cairo_t *ctx, char *str);
void render_hex(cairo_t *ctx, int centerX, int centerY, int radius);
void render_hex_by_index(cairo_t *ctx, int indexX, int indexY);
void render_menu_hex(cairo_t *ctx, int amount, unsigned int selected, struct color *colors);
void render_menu_bg(cairo_t *ctx);
#endif
