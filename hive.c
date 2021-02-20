#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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

struct colour 
{
   double r,g,b;
};

struct event
{
   int type, x, y, press;
};

int width, height, radius;
int camera_X, camera_Y;

struct event cairo_check_event(cairo_surface_t *sfc, int block)
{
   char keybuf[8];
   KeySym key;
   XEvent e;
   Display *dsp;
   struct event r_e;
   int mouse_X, mouse_Y;
   
   unsigned m;
   Window w, r;
   int i;

   dsp = cairo_xlib_surface_get_display(sfc);

   for (;;)
   {
      if (block || XPending(dsp))
         XNextEvent(dsp, &e);
      else 
         r_e.type = 0;

      switch (e.type)
      {
         case ButtonPress:
            r_e.type = -e.xbutton.button;
            r_e.x = e.xbutton.x;
            r_e.y = e.xbutton.y;
            r_e.press = 1;
            // printf("%d\n", -e.xbutton.button);
            break;
         
         case ButtonRelease:
            r_e.type = -e.xbutton.button;
            r_e.x = e.xbutton.x;
            r_e.y = e.xbutton.y;
            r_e.press = 0;
            break;

         case KeyPress:
            XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
            r_e.type = key;
            r_e.press = 1;
         default:
            XQueryPointer(dsp, cairo_xlib_surface_get_drawable(sfc), &r,
               &w, &i, &i, &mouse_X, &mouse_Y, &m);

            // printf("%d x %d\n", mouse_X, mouse_Y);
            
            r_e.x = mouse_X;
            r_e.y = mouse_Y;
      }

      return r_e;
   }
}

// static void fullscreen(Display* dpy, Window win)
// {
//    Atom atoms[2] = { XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False), None };
//    XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_STATE", False),
//                   XA_ATOM, 32, PropModeReplace, (unsigned char*) atoms, 1);
// }

cairo_surface_t *cairo_create_x11_surface(int *x, int *y)
{
   Display *dsp;
   Drawable da;
   Screen *scr;
   int screen;
   cairo_surface_t *sfc;

   if ((dsp = XOpenDisplay(NULL)) == NULL)
      exit(1);
   screen = DefaultScreen(dsp);
   scr = DefaultScreenOfDisplay(dsp);
   if (!*x || !*y)
   {
      *x = WidthOfScreen(scr), *y = HeightOfScreen(scr);
      *x = 1500;
      *y = 1000;
      da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, *x, *y, 0, 0, 0);
      // fullscreen (dsp, da);
   }
   else
      da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, *x, *y, 0, 0, 0);
   XSelectInput(dsp, da, ButtonPressMask | ButtonReleaseMask | KeyPressMask);
   XMapWindow(dsp, da);

   sfc = cairo_xlib_surface_create(dsp, da, DefaultVisual(dsp, screen), *x, *y);
   cairo_xlib_surface_set_size(sfc, *x, *y);

   return sfc;
}

void cairo_close_x11_surface(cairo_surface_t *sfc)
{
   Display *dsp = cairo_xlib_surface_get_display(sfc);

   cairo_surface_destroy(sfc);
   XCloseDisplay(dsp);
}

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

void render_menu_hex(cairo_t *ctx, int amount, unsigned int selected, struct colour *colours)
{
   int gap = radius + 10;
   int center_screen = (width / 2) - ((amount-1) * gap);

   for (int i = 0; i < amount; i++)
   {
      render_hex(ctx, center_screen + (i * gap * 2), height - height / 8, radius);

      struct colour current_colour = colours[i];
      cairo_set_source_rgb(ctx, current_colour.r, current_colour.g, current_colour.b);
      cairo_fill_preserve(ctx);

      if (i == selected)
         cairo_set_source_rgb(ctx, 0.3, 0.3, 0.3);
      else
         cairo_set_source_rgb(ctx, 0.2, 0.2, 0.2);
      cairo_stroke(ctx);
   }
}

int main(int argc, char **argv)
{
   cairo_surface_t *sfc;
   cairo_t *ctx;
   int x, y;
   struct timespec ts = {0, 5000000};
   int MAX_SIZE = 20;
   enum tile_types tiles[MAX_SIZE][MAX_SIZE];
   enum states cur_state = selecting_tile;
   int running;
   int cur_X, cur_Y;
   int mouse_X, mouse_Y;
   int moving_camera;
   char debug_text[100];
   camera_X = camera_Y = mouse_X = mouse_Y = moving_camera = 0;
   unsigned int cur_menu_pos, max_menu_pos;
   radius = 100;
   struct colour menu_hex_colours[5];
   struct colour colour_none;
   struct colour colour_bee;
   struct colour colour_spider;
   struct colour colour_beetle;
   struct colour colour_grasshopper;
   struct colour colour_ant;
   struct colour colour_current_hex;

   colour_none.r = colour_none.g = colour_none.b = 0.9;
   colour_bee.r = colour_bee.g = 1;
   colour_bee.b = 0;
   colour_spider.r = 0.7;
   colour_spider.g = 0.5;
   colour_spider.b = 0;
   colour_beetle.r = colour_beetle.b = 1;
   colour_beetle.g = 0;
   colour_grasshopper.r = colour_grasshopper.b = 0;
   colour_grasshopper.g = 1;
   colour_ant.r = colour_ant.g = 0;
   colour_ant.b = 1;

   cur_X = cur_Y = 0;
   x = y = 0;
   sfc = cairo_create_x11_surface(&x, &y);
   ctx = cairo_create(sfc);
   width = x;
   height = y;

   cairo_select_font_face(ctx,
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

      for (int x = 0; x < MAX_SIZE; ++x)
         for (int y = 0; y < MAX_SIZE; ++y)
         {
            switch (tiles[x][y])
            {
               case none:
                  colour_current_hex = colour_none;
                  break;
               case bee:
                  colour_current_hex = colour_bee;
                  break;
               case spider:
                  colour_current_hex = colour_spider;
                  break;
               case beetle:
                  colour_current_hex = colour_beetle;
                  break;
               case grasshopper:
                  colour_current_hex = colour_grasshopper;
                  break;
               case ant:
                  colour_current_hex = colour_ant;
                  break;
            }

            render_hex_by_index(ctx, x-MAX_SIZE/2, y-MAX_SIZE/2);
            cairo_set_source_rgb(ctx, colour_current_hex.r, colour_current_hex.g, colour_current_hex.b);
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
         render_menu_hex(ctx, max_menu_pos, cur_menu_pos, menu_hex_colours);
      }

      sprintf(debug_text, "%d x %d", camera_X, camera_Y);
      cairo_move_to(ctx, 2, 10);
      cairo_set_source_rgb(ctx, 1, 0, 0);
      cairo_show_text(ctx, debug_text);

      cairo_pop_group_to_source(ctx);
      cairo_paint(ctx);
      cairo_surface_flush(sfc);

      // end of render code, now key/mouse events

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
            cur_Y++;
            break;

         case 0xff0d:
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
               cur_menu_pos = 2;
               max_menu_pos = 5;
               menu_hex_colours[0] = colour_bee;
               menu_hex_colours[1] = colour_spider;
               menu_hex_colours[2] = colour_beetle;
               menu_hex_colours[3] = colour_grasshopper;
               menu_hex_colours[4] = colour_ant;
               cur_state = menu_tile_type;
            }
            break;

         case 0xff1b:   // Esc
            cur_menu_pos = 0;
            max_menu_pos = 2;
            if (cur_state == menu_tile_type || cur_state == menu_quit)
               cur_state = selecting_tile;
            else
            {
               cur_state = menu_quit;
               menu_hex_colours[0].r = menu_hex_colours[0].g = menu_hex_colours[0].b = 0.7;
               menu_hex_colours[1].r = 1;
               menu_hex_colours[1].g = menu_hex_colours[1].g = 0;
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