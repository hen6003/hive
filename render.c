#include <math.h>
#include <stdlib.h>
#include "render.h"

int width, height, radius;
int camera_X, camera_Y;

void render_hex(cairo_t *ctx, int centerX, int centerY, int radius)
{
   const int nsides = 6;
   float angle = 0.0f;
   float incr = 2.0 * M_PI / nsides;

   int newX = radius * cos(angle) + centerX;
   int newY = radius * sin(angle) + centerY;

   cairo_move_to(ctx, newX, newY);

   for (int i = 0; i < nsides+1; i++) {
      angle += incr;
      newX = radius * cos(angle) + centerX;
      newY = radius * sin(angle) + centerY;
     
      cairo_line_to(ctx, newX, newY);
   }
}

void render_menu_bg(cairo_t *ctx)
{
   cairo_rectangle(ctx, 0, height - height / 4, width, height);
   cairo_set_source_rgb(ctx, 0.8, 0.8, 0.8);
   cairo_fill_preserve(ctx);
   cairo_stroke(ctx);

   cairo_set_source_rgb(ctx, 0.2, 0.2, 0.2);
   cairo_move_to(ctx, 0, height - height / 4);
   cairo_line_to(ctx, width, height - height / 4);
   cairo_stroke(ctx);
   
   cairo_move_to(ctx, 0, height - height / 4);
}

void render_menu_hex(cairo_t *ctx, int amount, unsigned int selected, struct color *colors)
{
   int gap = radius + 10;
   int center_screen = (width / 2) - ((amount-1) * gap);

   for (int i = 0; i < amount; i++)
   {
      render_hex(ctx, center_screen + (i * gap * 2), height - height / 8, radius);

      struct color current_color = colors[i];
      cairo_set_source_rgb(ctx, current_color.r, current_color.g, current_color.b);
      cairo_fill_preserve(ctx);

      if (i == selected)
         cairo_set_source_rgb(ctx, 0.3, 0.3, 0.3);
      else
         cairo_set_source_rgb(ctx, 0.2, 0.2, 0.2);
      cairo_stroke(ctx);
   }
}

void render_hex_by_index(cairo_t *ctx, int indexX, int indexY)
{
   int offsetX = indexX * 150;
   int offsetY = indexY * 173;
   // int offsetX = indexX * (radius + radius/2);
   // int offsetY = indexY * (radius/4*3-radius/(radius/2));
   
   if ( abs(indexX) % 2 == 1 )
      offsetY += 86;
   
   render_hex(ctx, width / 2 + offsetX + camera_X, height / 2 + offsetY + camera_Y, radius);
}
