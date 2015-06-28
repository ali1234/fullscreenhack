// fullscreenhack.c
// Alistair Buxton <a.j.buxton@gmail.com>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <gdk/gdk.h>
#include "common.h"

typedef Status (*xgg_func)(Display *, Drawable, Window *, 
				int *, int *, unsigned int *, 
				unsigned int *, unsigned int *, 
				unsigned int *);

typedef XineramaScreenInfo * (*xsi_func)(Display*,int*);

typedef int (*xfree_func) (void *);

typedef Bool (*xqp_func) (Display *, Window, Window *,
              Window *, int *, int *,
              int *, int *, unsigned int *);

typedef void (*gsgmg_func) (GdkScreen *,  gint ,  GdkRectangle *);

typedef gint (*gsgmatw_func) (GdkScreen *, GdkWindow* );

typedef GdkWindow* (*gsgaw_func) (GdkScreen *);

void __attribute__ ((constructor)) load(void);

// Called when the library is loaded and before dlopen() returns
void load(void)
{
    fshack_init_running_under_flash();
    fprintf(stderr, "fullscreen hack loaded... running under flash = %d\n", _running_under_flash);
}

int choose_screen(Display *display, XineramaScreenInfo *screens, 
                    int n_screens, void *xlib_handle)
{
    const char *s = getenv("FLASH_FULLSCREEN_DISPLAY");
    char *e;
    Window a, b;
    Window root = RootWindow(display, DefaultScreen(display));
    int x, y, wx, wy, n;
    unsigned int mask;
    if (s != NULL) {
        int w = strtol(s, &e, 0);
        if (e != NULL && *e == 0 && w >= 0 && w < n_screens)
            return w;
	}
    xqp_func xqp = dlsym(xlib_handle, "XQueryPointer");
    xqp(display, root, &a, &b, &x, &y, &wx, &wy, &mask);
    fprintf(stderr, "\nCursor location: %dx%d\n", x, y);
    for(n=0;n<n_screens;n++) {
        fprintf(stderr, "\nscreen[%d]: %dx%d+%d+%d\n", n, screens[n].width, 
                screens[n].height, screens[n].x_org, screens[n].y_org);
        if (x >= screens[n].x_org && 
            x < (screens[n].x_org + screens[n].width) &&
            y >= screens[n].y_org && 
            y < (screens[n].y_org + screens[n].height)) {
            //fprintf(stderr, "match found\n"); // unnecessary with 'Using: ' below
            return n;
        }
    }
    fprintf(stderr, "\n----\nNo matching screen found!\n----\n\n");
    return 0;
}
void gdk_screen_get_monitor_geometry (GdkScreen *screen,
                                 gint monitor_num,
                                 GdkRectangle *dest)
{
    void* gdk_handle;

    gdk_handle = dlopen("libgdk-x11-2.0.so.0", RTLD_LAZY);
    gsgmg_func gsgmg = dlsym(gdk_handle, "gdk_screen_get_monitor_geometry");

    if (_running_under_flash) {
     GdkWindow *window;
     gsgaw_func gsgaw = dlsym(gdk_handle, "gdk_screen_get_active_window");
     window = gsgaw(screen);
     gsgmatw_func gsgmatw = dlsym(gdk_handle, "gdk_screen_get_monitor_at_window");

      //dest->width = 2560;
      //dest->height = 1440;

     monitor_num = gsgmatw(screen, window);
   }

   gsgmg(screen, monitor_num, dest);

   printf("monitor %u size - %ux%u\n",monitor_num, dest->width, dest->height);
   dlclose(gdk_handle);
}

Status XGetGeometry(Display *display, Drawable d, Window *root_return, 
            int *x_return, int *y_return, 
            unsigned int *width_return, 
            unsigned int *height_return, 
            unsigned int *border_width_return, 
            unsigned int *depth_return)
{
    void *xlib_handle;
    Status s;
    xlib_handle = dlopen("libX11.so", RTLD_LAZY);
    xgg_func xgg = dlsym(xlib_handle, "XGetGeometry");
    s = xgg(display, d, root_return, 
                x_return, y_return, 
                width_return, height_return, 
                border_width_return, depth_return);

    if (_running_under_flash && d == RootWindow(display, DefaultScreen(display))) {
        xfree_func XFree = dlsym(xlib_handle, "XFree");
        void *xin_handle;
        XineramaScreenInfo *screens;
        int n_screens, n;
        xin_handle = dlopen("libXinerama.so", RTLD_LAZY);
        xsi_func qs = dlsym(xin_handle, "XineramaQueryScreens");
        screens = qs(display, &n_screens);

        n = choose_screen(display, screens, n_screens, xlib_handle);

        *width_return = screens[n].width;
        *height_return = screens[n].height;
	fprintf(stderr, "\nUsing: screen[%d] %dx%d+%d+%d\n\n", n, screens[n].width, // output selected resolution to stderr
               screens[n].height, screens[n].x_org, screens[n].y_org);

        XFree(screens);
        //dlclose(xin_handle); // this is making  firefox to crash, why?...
    }
    dlclose(xlib_handle);
    return s;

}


