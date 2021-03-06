#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo/cairo-xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "render.h"
#include "window.h"

enum tile_types
{
   none,
   bee,
   spider,
   beetle,
   grasshopper,
   ant,
};

enum states
{
   menu_titlescreen,
   selecting_tile,
   menu_tile_type,
   menu_quit,
};

#define MAX_SIZE 20

int get_tile(int x, int y, enum tile_types tiles[MAX_SIZE][MAX_SIZE])
{
   printf("%d x %d\n", x, y);
   return tiles[x+MAX_SIZE/2][y+MAX_SIZE/2];
}
// returns amount of surrounding hexs of type none
int check_surrounding_hexs(cairo_t *ctx, int x, int y, enum tile_types tiles[MAX_SIZE][MAX_SIZE])
{
   int num_hexs_filled = 0;

   if (get_tile(x, y+1, tiles))
      num_hexs_filled++;
   if (get_tile(x, y-1, tiles))
      num_hexs_filled++;
   if (get_tile(x+1, y, tiles))
      num_hexs_filled++;
   if (get_tile(x-1, y, tiles))
      num_hexs_filled++;
   
   if (abs(x) % 2 == 1)
   {
      if (get_tile(x+1, y+1, tiles))
         num_hexs_filled++;
      if (get_tile(x-1, y+1, tiles))
         num_hexs_filled++;
   }                               
   else                            
   {                               
      if (get_tile(x+1, y-1, tiles))
         num_hexs_filled++;
      if (get_tile(x-1, y-1, tiles))
         num_hexs_filled++;
   }

   printf("%d\n", num_hexs_filled);
   
   return num_hexs_filled;
}

void draw_hex_with_colour(cairo_t *ctx, int x, int y, struct color c)
{
   render_hex_by_index(ctx, x, y);
   cairo_set_source_rgb(ctx, 10, 10, 10);
   cairo_fill_preserve(ctx);

   char debug_text[20];
   sprintf(debug_text, "%d x %d", x, y);
   render_text_to_screen(ctx, debug_text);
}

// here for debug purposes
int draw_surrounding_hexs(cairo_t *ctx,int x, int y, enum tile_types tiles[MAX_SIZE][MAX_SIZE])
{
   int num_hexs_filled = 0;

   struct color c;
   c.r = c.g = c.b = 0.1;

   draw_hex_with_colour(ctx, x, y+1, c);
   draw_hex_with_colour(ctx, x, y-1, c);

   draw_hex_with_colour(ctx, x+1, y, c);
   draw_hex_with_colour(ctx, x-1, y, c);
   
   if (abs(x) % 2 == 1)
   {
      draw_hex_with_colour(ctx, x+1, y+1, c);
      draw_hex_with_colour(ctx, x-1, y+1, c);
   }
   else
   {
      draw_hex_with_colour(ctx, x+1, y-1, c);
      draw_hex_with_colour(ctx, x-1, y-1, c);
   }
   
   return num_hexs_filled;
}

int main(int argc, char **argv)
{
   cairo_surface_t *sfc;
   cairo_t *ctx;
   int x, y;
   struct timespec ts = {0, 5000000};
   enum tile_types tiles[MAX_SIZE][MAX_SIZE];
   enum states cur_state = menu_tile_type;
   int running;
   int cur_X, cur_Y;
   int mouse_X, mouse_Y;
   int moving_camera;
   char debug_text[100];
   camera_X = camera_Y = mouse_X = mouse_Y = moving_camera = 0;
   unsigned int cur_menu_pos, max_menu_pos;
   radius = 100;
   struct color menu_hex_colors[5];
   struct color color_none;
   struct color color_bee;
   struct color color_spider;
   struct color color_beetle;
   struct color color_grasshopper;
   struct color color_ant;
   struct color color_current_hex;

   color_none.r = color_none.g = color_none.b = 0.9;
   color_bee.r = color_bee.g = 1;
   color_bee.b = 0;
   color_spider.r = 0.7;
   color_spider.g = 0.5;
   color_spider.b = 0;
   color_beetle.r = color_beetle.b = 1;
   color_beetle.g = 0;
   color_grasshopper.r = color_grasshopper.b = 0;
   color_grasshopper.g = 1;
   color_ant.r = color_ant.g = 0;
   color_ant.b = 1;
   
   cur_menu_pos = 2;
   max_menu_pos = 5;
   menu_hex_colors[0] = color_bee;
   menu_hex_colors[1] = color_spider;
   menu_hex_colors[2] = color_beetle;
   menu_hex_colors[3] = color_grasshopper;
   menu_hex_colors[4] = color_ant;

   cur_X = cur_Y = 0;
   x = y = 0;
   sfc = cairo_create_x11_surface(&x, &y);
   ctx = cairo_create(sfc);
   width = x;
   height = y;

   cairo_select_font_face (ctx,
      "monospace",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_NORMAL);

   for (int x = 0; x < MAX_SIZE; ++x)
      for (int y = 0; y < MAX_SIZE; ++y)
         tiles[x][y] = none;

   for (running = 1; running;)
   {
      cairo_push_group(ctx);
      cairo_set_line_width(ctx, 5);
      cairo_set_source_rgb(ctx, 0.2, 0.2, 0.2);
      cairo_paint(ctx);
      text_output_y = 10;

      for (int x = 0; x < MAX_SIZE; ++x)
         for (int y = 0; y < MAX_SIZE; ++y)
         {
            switch (tiles[x][y])
            {
               case none:
                  color_current_hex = color_none;
                  break;
               case bee:
                  color_current_hex = color_bee;
                  break;
               case spider:
                  color_current_hex = color_spider;
                  break;
               case beetle:
                  color_current_hex = color_beetle;
                  break;
               case grasshopper:
                  color_current_hex = color_grasshopper;
                  break;
               case ant:
                  color_current_hex = color_ant;
                  break;
            }

            render_hex_by_index(ctx, x-MAX_SIZE/2, y-MAX_SIZE/2);
            cairo_set_source_rgb(ctx, color_current_hex.r, color_current_hex.g, color_current_hex.b);
            cairo_fill_preserve(ctx);
            cairo_set_source_rgb(ctx, 0.2, 0.2, 0.2);
            cairo_stroke(ctx);
         }
      render_hex_by_index(ctx, cur_X, cur_Y);
      cairo_set_source_rgb(ctx, 0, 0, 0.7);
      cairo_set_line_width(ctx, 7);
      cairo_stroke(ctx);
      cairo_set_line_width(ctx, 5);

      if (cur_state != selecting_tile)
      {
         render_menu_bg(ctx);
         render_menu_hex(ctx, max_menu_pos, cur_menu_pos, menu_hex_colors);
      }

      sprintf(debug_text, "%d x %d", cur_X, cur_Y);
      render_text_to_screen(ctx, debug_text);

      // draw_surrounding_hexs(ctx, cur_X, cur_Y, tiles);

      cairo_pop_group_to_source(ctx);
      cairo_paint(ctx);
      cairo_surface_flush(sfc);

      struct event e = cairo_check_event(sfc, 0);
      switch (e.type)
      {
         case 0xff51:
            if (cur_state != selecting_tile)
            {
               if (cur_menu_pos-1 == -1)
                  break;
               cur_menu_pos--;
            }
            else
               cur_X--;
            break;

         case 0xff52:
            if (cur_state != selecting_tile)
               break;
            cur_Y--;
            break;

         case 0xff53:
            if (cur_state != selecting_tile)
            {
               if (cur_menu_pos+1 == max_menu_pos)
                  break;
               cur_menu_pos++;
            }
            else
               cur_X++;
            break;

         case 0xff54:
            if (cur_state != selecting_tile)
               break;
            cur_Y++;
            break;

         case 0xff0d: // Return
            if (cur_state == menu_tile_type)
            {
               tiles[cur_X+MAX_SIZE/2][cur_Y+MAX_SIZE/2] = cur_menu_pos+1;
               cur_state = selecting_tile;
            }
            else if (cur_state == menu_quit)
            {
               if (cur_menu_pos)
                  running = 0;
               else
                  cur_state = selecting_tile;
            }
            else
            {
               if (get_tile(cur_X, cur_Y, tiles) != none)
                  break;
               if (!check_surrounding_hexs(ctx, cur_X, cur_Y, tiles))
                  break;
                  
               cur_menu_pos = 2;
               max_menu_pos = 5;
               menu_hex_colors[0] = color_bee;
               menu_hex_colors[1] = color_spider;
               menu_hex_colors[2] = color_beetle;
               menu_hex_colors[3] = color_grasshopper;
               menu_hex_colors[4] = color_ant;
               cur_state = menu_tile_type;
            }
            break;

         case 0xff1b:   // Esc
            if (cur_state == menu_tile_type)
            {
               if (cur_X != 0 || cur_Y != 0)
                  cur_state = selecting_tile;
            }
            else if (cur_state == menu_quit)
               cur_state = selecting_tile;
            else
            {
               cur_state = menu_quit;
               cur_menu_pos = 0;
               max_menu_pos = 2;
               menu_hex_colors[0].r = menu_hex_colors[0].g = menu_hex_colors[0].b = 0.7;
               menu_hex_colors[1].r = 1;
               menu_hex_colors[1].g = menu_hex_colors[1].g = 0;
            }
            break;

         case -3:   // right mouse button
            moving_camera = e.press;
            mouse_X = e.x;
            mouse_Y = e.y;
            break;
         
         case 0x72:
            camera_X = camera_Y = moving_camera = 0;
            break;
      }

      if (moving_camera)
      {
         int change_X = mouse_X - e.x;
         int change_Y = mouse_Y - e.y;
         camera_X += change_X / 50;
         camera_Y += change_Y / 50;
      }
   
      nanosleep(&ts, NULL);
   }

   cairo_destroy(ctx);
   cairo_close_x11_surface(sfc);

   return 0;
}
