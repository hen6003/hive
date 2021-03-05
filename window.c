#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo/cairo-xlib.h>

#include <stdlib.h>

#include "window.h"

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
