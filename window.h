
struct event
{
   int type, x, y, press;
};

struct event cairo_check_event(cairo_surface_t *sfc, int block);
cairo_surface_t *cairo_create_x11_surface(int *x, int *y);
void cairo_close_x11_surface(cairo_surface_t *sfc);
