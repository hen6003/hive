#ifndef RENDER_H
#define RENDER_H

#include <cairo/cairo.h>

extern int width, height, radius;
extern int camera_X, camera_Y;

struct colour 
{
   double r,g,b;
};

void render_hex(cairo_t *ctx, int centerX, int centerY, int radius);
void render_hex_by_index(cairo_t *ctx, int indexX, int indexY);
void render_menu_hex(cairo_t *ctx, int amount, unsigned int selected, struct colour *colours);
void render_menu_bg(cairo_t *ctx);
#endif