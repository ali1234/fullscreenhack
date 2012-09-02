// fullscreenhack.c
// Alistair Buxton <a.j.buxton@gmail.com>
// extended by Steve Purchase <steve@t220.com>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>

typedef Status (*xgg_func)(Display *, Drawable, Window *,
				int *, int *, unsigned int *,
				unsigned int *, unsigned int *,
				unsigned int *);

typedef int (*xselectinput_func)(Display*, Window, long);

typedef XineramaScreenInfo * (*xsi_func)(Display*,int*);

typedef int (*xfree_func) (void *);

typedef Bool (*xqp_func) (Display *, Window, Window *,
              Window *, int *, int *,
              int *, int *, unsigned int *);


void __attribute__ ((constructor)) load(void);

// Called when the library is loaded and before dlopen() returns
void load(void)
{
    fprintf(stderr, "fullscreen hack loaded...\n");
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
    fprintf(stderr, "pointer: %dx%d\n", x, y);
    for(n=0;n<n_screens;n++) {
        fprintf(stderr, "screen %d, %dx%d+%d+%d\n", n, screens[n].width,
                screens[n].height, screens[n].x_org, screens[n].y_org);
        if (x >= screens[n].x_org &&
            x < (screens[n].x_org + screens[n].width) &&
            y >= screens[n].y_org &&
            y < (screens[n].y_org + screens[n].height)) {
            fprintf(stderr, "match found\n");
            return n;
        }
    }
    return 0;
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

    if (d == RootWindow(display, DefaultScreen(display))) {
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

        XFree(screens);
    }
    return s;

}

int XSelectInput(Display* display, Window window, long event_mask)
{
    void *xlib_handle;
    xlib_handle = dlopen("libX11.so", RTLD_LAZY);
    xselectinput_func fn = dlsym(xlib_handle, "XSelectInput");

    // Mask out PropertyChange events so Flash doesn't realise
    // the pointer/focus has left the window.
    event_mask &= ~PropertyChangeMask;

    return fn(display, window, event_mask);
}
